#pragma once

/*
#include "../eterLib/NetDatagramReceiver.h"
#include "../eterLib/NetDatagramSender.h"
#include "Packet.h"

class CPythonNetworkDatagram : public CSingleton<CPythonNetworkDatagram>
{
	public:
		CPythonNetworkDatagram();
		virtual ~CPythonNetworkDatagram();
		
		void Destroy();

		// With Server
		void SetRecvBufferSize(DWORD dwSize);
		void SetConnection(const char * c_szIP, WORD wPortIndex);
		void SendToServer(const void * c_pBuffer, DWORD dwSize);
		void Bind(DWORD dwAddress, WORD wPortIndex);

		// With UDP Senders
		void RegisterSender(DWORD dwID, DWORD dwAddress, WORD wPortIndex);
		void DeleteSender(DWORD dwID);

		void Select(DWORD dwID);
		void SendToSenders(const void * c_pBuffer, DWORD dwSize);

		// Regulary update function
		void Process();
		void SendCharacterStatePacket(DWORD dwVID, DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg);

	protected:
		BOOL CheckPacket(TPacketHeader * piRetHeader);
		BOOL GetSenderPointer(DWORD dwID, CNetDatagramSender ** ppSender);
		BOOL RecvStateWalkingPacket();

	protected:
		// Sender Map
		typedef std::map<DWORD, CNetDatagramSender*> TNetSenderMap;
		typedef TNetSenderMap::iterator TNetSenderMapIterator;
		// Sender List
		typedef std::list<CNetDatagramSender*> TNetSenderList;
		typedef TNetSenderList::iterator TNetSenderListIterator;

	protected:
		// Sender
		TNetSenderMap m_NetSenderMap;
		TNetSenderList m_NetSenderList;

		// Connection with server
		CNetDatagramSender m_NetSender;
		CNetDatagramReceiver m_NetReceiver;

	private:
		CDynamicPool<CNetDatagramSender> m_NetSenderPool;
};
*/