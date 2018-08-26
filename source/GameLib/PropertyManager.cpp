#include "StdAfx.h"
#include "../eterPack/EterPackManager.h"

#include "PropertyManager.h"
#include "Property.h"

CPropertyManager::CPropertyManager() : m_isFileMode(true)
{
}

CPropertyManager::~CPropertyManager()
{
	Clear();
}

bool CPropertyManager::Initialize(const char * c_pszPackFileName)
{
	if (c_pszPackFileName)
	{
		if (!m_pack.Create(m_fileDict, c_pszPackFileName, "", true))
		{
			LogBoxf("Cannot open property pack file (filename %s)", c_pszPackFileName);
			return false;
		}

		m_isFileMode = false;

		TDataPositionMap & indexMap = m_pack.GetIndexMap();

		TDataPositionMap::iterator itor = indexMap.begin();

		typedef std::map<DWORD, TEterPackIndex *> TDataPositionMap;

		int i = 0;

		while (indexMap.end() != itor)
		{
			TEterPackIndex * pIndex = itor->second;
			++itor;

			if (!stricmp("property/reserve", pIndex->filename))
			{
				LoadReservedCRC(pIndex->filename);
				continue;
			}

			if (!Register(pIndex->filename))
				continue;

			++i;
		}
	}
	else
	{
		m_isFileMode = true;
		// NOTE : 여기서 Property를 등록시키면 WorldEditor에서 이상이 생김 ;
		//        또한, Property Tree List에도 등록을 시켜야 되기 때문에 바깥쪽에서.. - [levites]
	}

	return true;
}

bool CPropertyManager::BuildPack()
{
	if (!m_pack.Create(m_fileDict, "property", ""))
		return false;

	WIN32_FIND_DATA fdata;
	HANDLE hFind = FindFirstFile("property\\*", &fdata);

	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		char szSourceFileName[256 + 1];
		_snprintf(szSourceFileName, sizeof(szSourceFileName), "property\\%s", fdata.cFileName);

		m_pack.Put(fdata.cFileName, szSourceFileName,COMPRESSED_TYPE_NONE,"");
	}
	while (FindNextFile(hFind, &fdata));

	FindClose(hFind);
	return true;
}

bool CPropertyManager::LoadReservedCRC(const char * c_pszFileName)
{
	CMappedFile file;
	LPCVOID c_pvData;

	if (!CEterPackManager::Instance().Get(file, c_pszFileName, &c_pvData))
		return false;
	
	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(file.Size(), c_pvData);

	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		const char * pszLine = textFileLoader.GetLineString(i).c_str();

		if (!pszLine || !*pszLine)
			continue;

		ReserveCRC(atoi(pszLine));
	}

	return true;
}

void CPropertyManager::ReserveCRC(DWORD dwCRC)
{
	m_ReservedCRCSet.insert(dwCRC);
}

DWORD CPropertyManager::GetUniqueCRC(const char * c_szSeed)
{
	std::string stTmp = c_szSeed;

	while (1)
	{
		DWORD dwCRC = GetCRC32(stTmp.c_str(), stTmp.length());

		if (m_ReservedCRCSet.find(dwCRC) == m_ReservedCRCSet.end() &&
			m_PropertyByCRCMap.find(dwCRC) == m_PropertyByCRCMap.end())
			return dwCRC;

		char szAdd[2];
		_snprintf(szAdd, sizeof(szAdd), "%d", random() % 10);
		stTmp += szAdd;
	}
}

bool CPropertyManager::Register(const char * c_pszFileName, CProperty ** ppProperty)
{
	CMappedFile file;
	LPCVOID		c_pvData;

	if (!CEterPackManager::Instance().Get(file, c_pszFileName, &c_pvData))
		return false;

	CProperty * pProperty = new CProperty(c_pszFileName);

	if (!pProperty->ReadFromMemory(c_pvData, file.Size(), c_pszFileName))
	{
		delete pProperty;
		return false;
	}

	DWORD dwCRC = pProperty->GetCRC();

	TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find(dwCRC);

	if (m_PropertyByCRCMap.end() != itor)
	{
		Tracef("Property already registered, replace %s to %s\n",
				itor->second->GetFileName(),
				c_pszFileName);

		delete itor->second;
		itor->second = pProperty;
	}
	else
		m_PropertyByCRCMap.insert(TPropertyCRCMap::value_type(dwCRC, pProperty));

	if (ppProperty)
		*ppProperty = pProperty;

	return true;
}

bool CPropertyManager::Get(const char * c_pszFileName, CProperty ** ppProperty)
{
	return Register(c_pszFileName, ppProperty);
}

bool CPropertyManager::Get(DWORD dwCRC, CProperty ** ppProperty)
{
	TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find(dwCRC);

	if (m_PropertyByCRCMap.end() == itor)
		return false;

	*ppProperty = itor->second;
	return true;
}

bool CPropertyManager::Put(const char * c_pszFileName, const char * c_pszSourceFileName)
{
	if (!CopyFile(c_pszSourceFileName, c_pszFileName, FALSE))
		return false;

	if (!m_isFileMode)	// 팩 파일에도 넣음
	{
		if (!m_pack.Put(c_pszFileName, NULL, COMPRESSED_TYPE_NONE,""))
		{
			assert(!"CPropertyManager::Put cannot write to pack file");
			return false;
		}
	}

	Register(c_pszFileName);
	return true;
}

bool CPropertyManager::Erase(DWORD dwCRC)
{
	TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find(dwCRC);

	if (m_PropertyByCRCMap.end() == itor)
		return false;

	CProperty * pProperty = itor->second;
	m_PropertyByCRCMap.erase(itor);

	DeleteFile(pProperty->GetFileName());
	ReserveCRC(pProperty->GetCRC());

	if (!m_isFileMode)	// 파일 모드가 아니면 팩에서도 지움
		m_pack.Delete(pProperty->GetFileName());

	FILE * fp = fopen("property/reserve", "a+");

	if (!fp)
		LogBox("예약 CRC 파일을 열 수 없습니다.");
	else
	{
		char szCRC[64 + 1];
		_snprintf(szCRC, sizeof(szCRC), "%u\r\n", pProperty->GetCRC());

		fputs(szCRC, fp);
		fclose(fp);
	}

	delete pProperty;
	return true;
}

bool CPropertyManager::Erase(const char * c_pszFileName)
{
	CProperty * pProperty = NULL;

	if (Get(c_pszFileName, &pProperty))
		return Erase(pProperty->GetCRC());
	
	return false;
}

void CPropertyManager::Clear()
{
	stl_wipe_second(m_PropertyByCRCMap);
}
