#include "StdAfx.h"
#include "MappedFile.h"
#include "Debug.h"

CMappedFile::CMappedFile() :
m_hFM(NULL),
m_lpMapData(NULL),
m_dataOffset(0),
m_mapSize(0),
m_seekPosition(0),
m_pLZObj(NULL),
m_pbBufLinkData(NULL),
m_dwBufLinkSize(0),
m_pbAppendResultDataBlock(NULL),
m_dwAppendResultDataSize(0)
{
}

CMappedFile::~CMappedFile()
{
	Destroy();
}

BOOL CMappedFile::Create(const char * filename)
{
	Destroy();
	return CFileBase::Create(filename, FILEMODE_READ);
}

BOOL CMappedFile::Create(const char * filename, const void** dest, int offset, int size)
{
	if (!CMappedFile::Create(filename))
		return NULL;

	int ret = Map(dest, offset, size);
	return (ret) > 0;
}

LPCVOID CMappedFile::Get()
{
	return m_lpData;
}

void CMappedFile::Link(DWORD dwBufSize, const void* c_pvBufData)
{
	m_dwBufLinkSize=dwBufSize;
	m_pbBufLinkData=(BYTE*)c_pvBufData;
}

void CMappedFile::BindLZObject(CLZObject * pLZObj)
{
	assert(m_pLZObj == NULL);
	m_pLZObj = pLZObj;

	Link(m_pLZObj->GetSize(), m_pLZObj->GetBuffer());
}

void CMappedFile::BindLZObjectWithBufferedSize(CLZObject * pLZObj)
{
	assert(m_pLZObj == NULL);
	m_pLZObj = pLZObj;

	Link(m_pLZObj->GetBufferSize(), m_pLZObj->GetBuffer());
}

BYTE* CMappedFile::AppendDataBlock( const void* pBlock, DWORD dwBlockSize )
{
	if( m_pbAppendResultDataBlock )
	{
		delete []m_pbAppendResultDataBlock;
	}

	//realloc
	m_dwAppendResultDataSize  = m_dwBufLinkSize+dwBlockSize;
	m_pbAppendResultDataBlock = new BYTE[m_dwAppendResultDataSize];

	memcpy(m_pbAppendResultDataBlock, m_pbBufLinkData, m_dwBufLinkSize );
	memcpy(m_pbAppendResultDataBlock + m_dwBufLinkSize, pBlock, dwBlockSize );

	//redirect
	Link(m_dwAppendResultDataSize, m_pbAppendResultDataBlock);

	return m_pbAppendResultDataBlock;
}

void CMappedFile::Destroy()
{
	if (m_pLZObj)	// 압축된 데이터가 이 포인터로 연결 된다
	{
		delete m_pLZObj;
		m_pLZObj = NULL;
	}

	if (NULL != m_lpMapData)
	{		
		Unmap(m_lpMapData);
		m_lpMapData = NULL;
	}

	if (NULL != m_hFM)
	{
		CloseHandle(m_hFM);
		m_hFM = NULL;
	}

	if( m_pbAppendResultDataBlock )
	{
		delete []m_pbAppendResultDataBlock;
		m_pbAppendResultDataBlock = NULL;
	}

	m_dwAppendResultDataSize = 0;

	m_pbBufLinkData = NULL;
	m_dwBufLinkSize = 0;

	m_seekPosition = 0;
	m_dataOffset = 0;
	m_mapSize = 0;

	CFileBase::Destroy();
}

int CMappedFile::Seek(DWORD offset, int iSeekType)
{
	switch (iSeekType)
	{
		case SEEK_TYPE_BEGIN:
			if (offset > m_dwSize)
				offset = m_dwSize;

			m_seekPosition = offset;
			break;
			
		case SEEK_TYPE_CURRENT:
			m_seekPosition = min(m_seekPosition + offset, Size());
			break;

		case SEEK_TYPE_END:
			m_seekPosition = max(0, Size() - offset);
			break;
	}

	return m_seekPosition;
}

// 2004.09.16.myevan.MemoryMappedFile 98/ME 개수 제한 문제 체크
//DWORD g_dwCount=0;

int CMappedFile::Map(const void **dest, int offset, int size)
{
	m_dataOffset = offset;

	if (size == 0)
		m_mapSize = m_dwSize;
	else
		m_mapSize = size;

	if (m_dataOffset + m_mapSize > m_dwSize)
		return NULL;

	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwSysGran = SysInfo.dwAllocationGranularity;
	DWORD dwFileMapStart = (m_dataOffset / dwSysGran) * dwSysGran;
	DWORD dwMapViewSize = (m_dataOffset % dwSysGran) + m_mapSize;
	INT iViewDelta = m_dataOffset - dwFileMapStart;
	

	m_hFM = CreateFileMapping(m_hFile,				// handle
							  NULL,					// security
							  PAGE_READONLY,		// flProtect
							  0,					// high
							  m_dataOffset + m_mapSize,	// low
							  NULL);				// name

	if (!m_hFM)
	{
		OutputDebugString("CMappedFile::Map !m_hFM\n");
		return NULL;
	}	

	m_lpMapData = MapViewOfFile(m_hFM,
								FILE_MAP_READ,
								0,
								dwFileMapStart,
								dwMapViewSize);

	if (!m_lpMapData) // Success
	{
		TraceError("CMappedFile::Map !m_lpMapData %lu", GetLastError());
		return 0;
	}
	
	// 2004.09.16.myevan.MemoryMappedFile 98/ME 개수 제한 문제 체크
	//g_dwCount++;
	//Tracenf("MAPFILE %d", g_dwCount);
	
	m_lpData = (char*) m_lpMapData + iViewDelta;
	*dest = (char*) m_lpData;
	m_seekPosition = 0;

	Link(m_mapSize, m_lpData);

	return (m_mapSize);
}

BYTE * CMappedFile::GetCurrentSeekPoint()
{
	return m_pbBufLinkData+m_seekPosition;
	//return m_pLZObj ? m_pLZObj->GetBuffer() + m_seekPosition : (BYTE *) m_lpData + m_seekPosition;
}


DWORD CMappedFile::Size()
{
	return m_dwBufLinkSize;
	/*
	if (m_pLZObj)
		return m_pLZObj->GetSize();

	return (m_mapSize);
	*/
}

DWORD CMappedFile::GetPosition()
{
	return m_dataOffset;
}

BOOL CMappedFile::Read(void * dest, int bytes)
{
	if (m_seekPosition + bytes > Size())
		return FALSE;

	memcpy(dest, GetCurrentSeekPoint(), bytes);
	m_seekPosition += bytes;
	return TRUE;
}

DWORD CMappedFile::GetSeekPosition(void)
{
	return m_seekPosition;
}

void CMappedFile::Unmap(LPCVOID data)
{	
	if (UnmapViewOfFile(data))
	{
		// 2004.09.16.myevan.MemoryMappedFile 98/ME 개수 제한 문제 체크
		//g_dwCount--;
		//Tracenf("UNMAPFILE %d", g_dwCount);
	}
	else
	{
		TraceError("CMappedFile::Unmap - Error");
	}
	m_lpData = NULL;
}
