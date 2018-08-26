#include "StdAfx.h"
#include "NetDatagramReceiver.h"

BOOL CNetDatagramReceiver::Process()
{
	m_recvBufCurrentPos = 0;
	m_recvBufCurrentSize = 0;

	int irecvAddrLength = sizeof(SOCKADDR_IN);
	m_recvBufCurrentSize = recvfrom(m_Socket, (char *)m_recvBuf, m_recvBufSize, 0, (PSOCKADDR)&m_SockAddr, &irecvAddrLength);

	if (m_recvBufCurrentSize <= 0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CNetDatagramReceiver::Recv(void * pBuffer, int iSize)
{
	if (!Peek(pBuffer, iSize))
		return FALSE;

	m_recvBufCurrentPos += iSize;
	return TRUE;
}

BOOL CNetDatagramReceiver::Peek(void * pBuffer, int iSize)
{
	if (m_recvBufCurrentSize < m_recvBufCurrentPos+iSize)
		return FALSE;

	memcpy(pBuffer, m_recvBuf + m_recvBufCurrentPos, iSize);
	return TRUE;
}

BOOL CNetDatagramReceiver::isBind()
{
	return m_isBind;
}

BOOL CNetDatagramReceiver::Bind(DWORD /*dwAddress*/, WORD wPortIndex)
{
	m_Socket = socket(AF_INET, SOCK_DGRAM, 0);

	DWORD arg = 1;
	ioctlsocket(m_Socket, FIONBIO, &arg);	// Non-blocking mode

	memset(&m_SockAddr, 0, sizeof(SOCKADDR_IN));
	m_SockAddr.sin_family = AF_INET;
//	m_SockAddr.sin_addr.s_addr = dwAddress;
	m_SockAddr.sin_addr.s_addr = INADDR_ANY;
	m_SockAddr.sin_port = htons(wPortIndex);

	if (bind(m_Socket, (PSOCKADDR)&m_SockAddr, sizeof(SOCKADDR_IN)) < 0)
	{
		Tracef("Failed binding socket\n");
		return FALSE;
	}

	m_isBind = TRUE;

	return TRUE;
}

void CNetDatagramReceiver::SetRecvBufferSize(int recvBufSize)
{
	if (m_recvBuf)
	{
		if (m_recvBufSize>recvBufSize)
			return;

		delete [] m_recvBuf;
	}
	m_recvBufSize=recvBufSize;
	m_recvBuf=new char[m_recvBufSize];
}

void CNetDatagramReceiver::Clear()
{
	
	m_isBind = FALSE;

	m_dwPortIndex = 1000;

	m_Socket = 0;
	memset(&m_SockAddr, 0, sizeof(SOCKADDR_IN));

	m_recvBufCurrentPos = 0;
	m_recvBufCurrentSize = 0;	
}

CNetDatagramReceiver::CNetDatagramReceiver()
{
	m_recvBuf = NULL;
	m_recvBufSize = 0;

	Clear();
}
CNetDatagramReceiver::~CNetDatagramReceiver()
{
	if (m_recvBuf)
		delete [] m_recvBuf;
}