#include "StdAfx.h"
#include "NetDatagram.h"

CNetworkDatagram::CNetworkDatagram()
{
	__Initialize();
}

CNetworkDatagram::~CNetworkDatagram()
{
	Destroy();
}

void CNetworkDatagram::Destroy()
{
	if (INVALID_SOCKET==m_sock)
		return;

	closesocket(m_sock);
	
	__Initialize();
}

bool CNetworkDatagram::Create(UINT uPort)
{
	assert(INVALID_SOCKET==m_sock);
	m_sock = socket(AF_INET, SOCK_DGRAM, 0);

	DWORD arg = 1;
	ioctlsocket(m_sock, FIONBIO, &arg);	// Non-blocking mode

	SOCKADDR_IN sockAddrIn;
	memset(&sockAddrIn, 0, sizeof(SOCKADDR_IN));
	sockAddrIn.sin_family = AF_INET;
	sockAddrIn.sin_addr.s_addr = INADDR_ANY;
	sockAddrIn.sin_port = htons(uPort);

	if (SOCKET_ERROR == bind(m_sock, (PSOCKADDR)&sockAddrIn, sizeof(SOCKADDR_IN)))
	{
		return false;
	}

	return true;
}

#pragma warning(push)
#pragma warning(disable:4127)
void CNetworkDatagram::Update()
{
	if (m_sock == INVALID_SOCKET)
		return;

	FD_ZERO(&m_fdsRecv);
	FD_ZERO(&m_fdsSend);

	FD_SET(m_sock, &m_fdsRecv);
	FD_SET(m_sock, &m_fdsSend);

	TIMEVAL delay;

	delay.tv_sec = 0;
	delay.tv_usec = 0;
	
	if (select(0, &m_fdsRecv, &m_fdsSend, NULL, &delay) == SOCKET_ERROR)
		return;
}
#pragma warning(pop)

bool CNetworkDatagram::CanRecv()
{
	if (FD_ISSET(m_sock, &m_fdsRecv))
		return true;

	return false;
}

		
int CNetworkDatagram::PeekRecvFrom(UINT uBufLen, void* pvBuf, SOCKADDR_IN* pkSockAddrIn)
{
	int nSockAddrInLen=sizeof(SOCKADDR_IN);
	return recvfrom(m_sock, (char*)pvBuf, uBufLen, MSG_PEEK, (PSOCKADDR)pkSockAddrIn, &nSockAddrInLen);
}

int CNetworkDatagram::RecvFrom(UINT uBufLen, void* pvBuf, SOCKADDR_IN* pkSockAddrIn)
{
	int nSockAddrInLen=sizeof(SOCKADDR_IN);
	return recvfrom(m_sock, (char*)pvBuf, uBufLen, 0, (PSOCKADDR)pkSockAddrIn, &nSockAddrInLen);
}

int CNetworkDatagram::SendTo(UINT uBufLen, const void* c_pvBuf, const SOCKADDR_IN& c_rkSockAddrIn)
{
	return sendto(m_sock, (const char *)c_pvBuf, uBufLen, 0, (PSOCKADDR)&c_rkSockAddrIn, sizeof(SOCKADDR_IN));
}


void CNetworkDatagram::__Initialize()
{
	m_sock=INVALID_SOCKET;
}