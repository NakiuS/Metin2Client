#include "StdAfx.h"

/*
#include "PythonNetworkDatagram.h"
#include "PythonNetworkStream.h"

class CDatagramPacketHeaderMap : public CNetworkPacketHeaderMap
{
	public:
		CDatagramPacketHeaderMap()
		{
			Set(HEADER_CC_STATE_WALKING,			CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketCCState), false));
		}
};

void CPythonNetworkDatagram::Destroy()
{
	m_NetSenderPool.Clear();
}

BOOL CPythonNetworkDatagram::CheckPacket(TPacketHeader * piRetHeader)
{
	static CDatagramPacketHeaderMap s_packetHeaderMap;

	if (!m_NetReceiver.isBind())
		return FALSE;

	*piRetHeader = 0;

	TPacketHeader header;

	if (!m_NetReceiver.Peek(&header, sizeof(TPacketHeader)))
		return false;

	CNetworkPacketHeaderMap::TPacketType PacketType;

	if (!s_packetHeaderMap.Get(header, &PacketType))
	{
		Tracef("Unknown UDP packet header");
		assert(!"Unknown UDP packet header");
		return FALSE;
	}

	if (!header)
		return FALSE;

	*piRetHeader = header;
	return TRUE;
}

void CPythonNetworkDatagram::Process()
{
	while (m_NetReceiver.Process())
	{
		TPacketHeader iHeader;

		if (!CheckPacket(&iHeader))
			continue;

		switch(iHeader)
		{
			case HEADER_CC_STATE_WALKING:
				RecvStateWalkingPacket();
				break;

			case HEADER_CC_STATE_WAITING:
			case HEADER_CC_STATE_GOING:
			case HEADER_CC_EVENT_NORMAL_ATTACKING:
			case HEADER_CC_EVENT_COMBO_ATTACKING:
			case HEADER_CC_EVENT_HIT:
				break;
		}
	}
}

void CPythonNetworkDatagram::SetConnection(const char * c_szIP, WORD wPortIndex)
{
	m_NetSender.SetSocket(c_szIP, wPortIndex);
}

void CPythonNetworkDatagram::SetRecvBufferSize(DWORD dwSize)
{
	m_NetReceiver.SetRecvBufferSize(dwSize);
}

void CPythonNetworkDatagram::SendToServer(const void * c_pBuffer, DWORD dwSize)
{
	if (!m_NetSender.isSocket())
	{
		assert(!"UDP Socket has not set!");
		return;
	}

	m_NetSender.Send(c_pBuffer, dwSize);
}

void CPythonNetworkDatagram::Bind(DWORD dwAddress, WORD wPortIndex)
{
	m_NetReceiver.Bind(dwAddress, wPortIndex);
}

void CPythonNetworkDatagram::RegisterSender(DWORD dwID, DWORD dwAddress, WORD wPortIndex)
{
	CNetDatagramSender * pSender = m_NetSenderPool.Alloc();

	pSender->SetSocket(dwAddress, wPortIndex);

	m_NetSenderMap.insert(TNetSenderMap::value_type(dwID, pSender));
}
void CPythonNetworkDatagram::DeleteSender(DWORD dwID)
{
	CNetDatagramSender * pSender;
	if (!GetSenderPointer(dwID, &pSender))
		return;

	m_NetSenderPool.Free(pSender);

	m_NetSenderMap.erase(dwID);
}

void CPythonNetworkDatagram::Select(DWORD dwID)
{
	CNetDatagramSender * pSender;
	if (!GetSenderPointer(dwID, &pSender))
		return;

	m_NetSenderList.push_back(pSender);
}
void CPythonNetworkDatagram::SendToSenders(const void * c_pBuffer, DWORD dwSize)
{
	// NOTE : Temporary Code
	//        Now, Send to every around client.
	for (TNetSenderMapIterator itorMap = m_NetSenderMap.begin(); itorMap != m_NetSenderMap.end(); ++itorMap)
	{
		CNetDatagramSender * pSender = itorMap->second;
		m_NetSenderList.push_back(pSender);
	}
	// NOTE : Temporary Code

	for (TNetSenderListIterator itor = m_NetSenderList.begin(); itor != m_NetSenderList.end(); ++itor)
	{
		CNetDatagramSender * pSender = *itor;

		pSender->Send(c_pBuffer, dwSize);
	}

	m_NetSenderList.clear();
}

BOOL CPythonNetworkDatagram::GetSenderPointer(DWORD dwID, CNetDatagramSender ** ppSender)
{
	TNetSenderMapIterator itor = m_NetSenderMap.find(dwID);

	if (m_NetSenderMap.end() == itor)
		return FALSE;

	*ppSender = itor->second;

	return TRUE;
}

//////////////////////////////////
// Walking

void CPythonNetworkDatagram::SendCharacterStatePacket(DWORD dwVID, DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{	
	fDstRot=fmod(fDstRot, 360.0f);

	if (fDstRot<0)
		fDstRot=360.0f-fDstRot;
	
	TPacketCCState kStatePacket;
	kStatePacket.bHeader=HEADER_CC_STATE_WALKING;
	kStatePacket.dwVID=dwVID;
	kStatePacket.bFunc=eFunc;
	kStatePacket.bArg=uArg;
	kStatePacket.bRot=fDstRot/5.0f;
	kStatePacket.dwTime=dwCmdTime;
	kStatePacket.kPPos=c_rkPPosDst;
	//SendToSenders(&kStatePacket, sizeof(kStatePacket));
}

BOOL CPythonNetworkDatagram::RecvStateWalkingPacket()
{
	TPacketCCState kStatePacket;
	if (!m_NetReceiver.Recv(&kStatePacket, sizeof(kStatePacket)))
	{
		assert(!"CPythonNetworkDatagram::RecvStatePacket - PAKCET READ ERROR");
		Tracenf("CPythonNetworkDatagram::RecvStatePacket - PAKCET READ ERROR");
		return FALSE;
	}

	CInstanceBase * pkChrInst = CPythonCharacterManager::Instance().GetInstancePtr(kStatePacket.dwVID);
	
	if (!pkChrInst)
	{
		//Tracenf("CPythonNetworkDatagram::RecvStatePacket - NOT EXIST VID(kStateWaitingPacket.vid = %d)", kStatePacket.m_dwVID);
		return TRUE;
	}

	pkChrInst->PushUDPState(kStatePacket.dwTime, kStatePacket.kPPos, kStatePacket.bRot*5.0f, kStatePacket.bFunc, kStatePacket.bArg);
	return TRUE;
}

CPythonNetworkDatagram::CPythonNetworkDatagram()
{	
}

CPythonNetworkDatagram::~CPythonNetworkDatagram()
{
}

  */