#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "Packet.h"

extern DWORD g_adwEncryptKey[4];
extern DWORD g_adwDecryptKey[4];

// Select Character ---------------------------------------------------------------------------
void CPythonNetworkStream::SetSelectPhase()
{
	if ("Select" != m_strPhase)
		m_phaseLeaveFunc.Run();

	Tracen("");
	Tracen("## Network - Select Phase ##");
	Tracen("");

	m_strPhase = "Select";	

#ifndef _IMPROVED_PACKET_ENCRYPTION_
	SetSecurityMode(true, (const char *) g_adwEncryptKey, (const char *) g_adwDecryptKey);
#endif

	m_dwChangingPhaseTime = ELTimer_GetMSec();
	m_phaseProcessFunc.Set(this, &CPythonNetworkStream::SelectPhase);
	m_phaseLeaveFunc.Set(this, &CPythonNetworkStream::__LeaveSelectPhase);

	if (__DirectEnterMode_IsSet())
	{
		PyCallClassMemberFunc(m_poHandler, "SetLoadingPhase", Py_BuildValue("()"));	
	}
	else
	{
		if (IsSelectedEmpire())
			PyCallClassMemberFunc(m_poHandler, "SetSelectCharacterPhase", Py_BuildValue("()"));
		else
			PyCallClassMemberFunc(m_poHandler, "SetSelectEmpirePhase", Py_BuildValue("()"));
	}
}

void CPythonNetworkStream::SelectPhase()
{
	TPacketHeader header;

	if (!CheckPacket(&header))
		return;

	switch (header)
	{
		case HEADER_GC_PHASE:
			if (RecvPhasePacket())
				return;	
			break;

		case HEADER_GC_EMPIRE:
			if (__RecvEmpirePacket())
				return;
			break;

		case HEADER_GC_LOGIN_SUCCESS3:
			if (__RecvLoginSuccessPacket3())
				return;
			break;

		case HEADER_GC_LOGIN_SUCCESS4:
			if (__RecvLoginSuccessPacket4())
				return;
			break;


		case HEADER_GC_PLAYER_CREATE_SUCCESS:
			if (__RecvPlayerCreateSuccessPacket())
				return;
			break;

		case HEADER_GC_PLAYER_CREATE_FAILURE:
			if (__RecvPlayerCreateFailurePacket())
				return;
			break;

		case HEADER_GC_PLAYER_DELETE_WRONG_SOCIAL_ID:
			if (__RecvPlayerDestroyFailurePacket())
				return;
			break;

		case HEADER_GC_PLAYER_DELETE_SUCCESS:
			if (__RecvPlayerDestroySuccessPacket())
				return;
			break;

		case HEADER_GC_CHANGE_NAME:
			if (__RecvChangeName())
				return;
			break;

		case HEADER_GC_HANDSHAKE:
			RecvHandshakePacket();
			return;
			break;

		case HEADER_GC_HANDSHAKE_OK:
			RecvHandshakeOKPacket();
			return;
			break;

		case HEADER_GC_HYBRIDCRYPT_KEYS:
			RecvHybridCryptKeyPacket();
			return;
			break;

		case HEADER_GC_HYBRIDCRYPT_SDB:
			RecvHybridCryptSDBPacket();
			return;
			break;


#ifdef _IMPROVED_PACKET_ENCRYPTION_
		case HEADER_GC_KEY_AGREEMENT:
			RecvKeyAgreementPacket();
			return;
			break;

		case HEADER_GC_KEY_AGREEMENT_COMPLETED:
			RecvKeyAgreementCompletedPacket();
			return;
			break;
#endif

		case HEADER_GC_PLAYER_POINT_CHANGE:
			TPacketGCPointChange PointChange;
			Recv(sizeof(TPacketGCPointChange), &PointChange);
			return;
			break;

		///////////////////////////////////////////////////////////////////////////////////////////
		case HEADER_GC_PING:
			if (RecvPingPacket())
				return;
			break;
	}

	RecvErrorPacket(header);
}

bool CPythonNetworkStream::SendSelectEmpirePacket(DWORD dwEmpireID)
{
	TPacketCGEmpire kPacketEmpire;
	kPacketEmpire.bHeader=HEADER_CG_EMPIRE;
	kPacketEmpire.bEmpire=dwEmpireID;

	if (!Send(sizeof(kPacketEmpire), &kPacketEmpire))
	{
		Tracen("SendSelectEmpirePacket - Error");
		return false;
	}

	SetEmpireID(dwEmpireID);
	return SendSequence();
}

bool CPythonNetworkStream::SendSelectCharacterPacket(BYTE Index)
{
	TPacketCGSelectCharacter SelectCharacterPacket;

	SelectCharacterPacket.header = HEADER_CG_PLAYER_SELECT;
	SelectCharacterPacket.player_index = Index;

	if (!Send(sizeof(TPacketCGSelectCharacter), &SelectCharacterPacket))
	{
		Tracen("SendSelectCharacterPacket - Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendDestroyCharacterPacket(BYTE index, const char * szPrivateCode)
{
    TPacketCGDestroyCharacter DestroyCharacterPacket;

	DestroyCharacterPacket.header = HEADER_CG_PLAYER_DESTROY;
	DestroyCharacterPacket.index = index;
	strncpy(DestroyCharacterPacket.szPrivateCode, szPrivateCode, PRIVATE_CODE_LENGTH-1);

	if (!Send(sizeof(TPacketCGDestroyCharacter), &DestroyCharacterPacket))
	{
		Tracen("SendDestroyCharacterPacket");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendCreateCharacterPacket(BYTE index, const char *name, BYTE job, BYTE shape, BYTE byCON, BYTE byINT, BYTE bySTR, BYTE byDEX)
{
	TPacketCGCreateCharacter createCharacterPacket;

	createCharacterPacket.header = HEADER_CG_PLAYER_CREATE;
	createCharacterPacket.index = index;
	strncpy(createCharacterPacket.name, name, CHARACTER_NAME_MAX_LEN);
	createCharacterPacket.job = job;
	createCharacterPacket.shape = shape;
	createCharacterPacket.CON = byCON;
	createCharacterPacket.INT = byINT;
	createCharacterPacket.STR = bySTR;
	createCharacterPacket.DEX = byDEX;

	if (!Send(sizeof(TPacketCGCreateCharacter), &createCharacterPacket))
	{
		Tracen("Failed to SendCreateCharacterPacket");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChangeNamePacket(BYTE index, const char *name)
{
	TPacketCGChangeName ChangeNamePacket;
	ChangeNamePacket.header = HEADER_CG_CHANGE_NAME;
	ChangeNamePacket.index = index;
	strncpy(ChangeNamePacket.name, name, CHARACTER_NAME_MAX_LEN);

	if (!Send(sizeof(TPacketCGChangeName), &ChangeNamePacket))
	{
		Tracen("Failed to SendChangeNamePacket");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::__RecvPlayerCreateSuccessPacket()
{
	TPacketGCPlayerCreateSuccess kCreateSuccessPacket;

	if (!Recv(sizeof(kCreateSuccessPacket), &kCreateSuccessPacket))
		return false;

	if (kCreateSuccessPacket.bAccountCharacterSlot>=PLAYER_PER_ACCOUNT4)
	{
		TraceError("CPythonNetworkStream::RecvPlayerCreateSuccessPacket - OUT OF RANGE SLOT(%d) > PLATER_PER_ACCOUNT(%d)",
			kCreateSuccessPacket.bAccountCharacterSlot, PLAYER_PER_ACCOUNT4);
		return true;
	}

	m_akSimplePlayerInfo[kCreateSuccessPacket.bAccountCharacterSlot]=kCreateSuccessPacket.kSimplePlayerInfomation;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_CREATE], "OnCreateSuccess", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::__RecvPlayerCreateFailurePacket()
{
	TPacketGCCreateFailure packet;

	if (!Recv(sizeof(TPacketGCCreateFailure), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_CREATE], "OnCreateFailure", Py_BuildValue("(i)", packet.bType));
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_SELECT], "OnCreateFailure", Py_BuildValue("(i)", packet.bType));
	return true;
}

bool CPythonNetworkStream::__RecvPlayerDestroySuccessPacket()
{
	TPacketGCDestroyCharacterSuccess packet;
	if (!Recv(sizeof(TPacketGCDestroyCharacterSuccess), &packet))
		return false;

	memset(&m_akSimplePlayerInfo[packet.account_index], 0, sizeof(m_akSimplePlayerInfo[packet.account_index]));
	m_adwGuildID[packet.account_index] = 0;
	m_astrGuildName[packet.account_index] = "";

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_SELECT], "OnDeleteSuccess", Py_BuildValue("(i)", packet.account_index));
	return true;
}

bool CPythonNetworkStream::__RecvPlayerDestroyFailurePacket()
{
	TPacketGCBlank packet_blank;
	if (!Recv(sizeof(TPacketGCBlank), &packet_blank))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_SELECT], "OnDeleteFailure", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::__RecvChangeName()
{
	TPacketGCChangeName ChangeNamePacket;
	if (!Recv(sizeof(TPacketGCChangeName), &ChangeNamePacket))
		return false;

	for (int i = 0; i < PLAYER_PER_ACCOUNT4; ++i)
	{
		if (ChangeNamePacket.pid == m_akSimplePlayerInfo[i].dwID)
		{
			m_akSimplePlayerInfo[i].bChangeName = FALSE;
			strncpy(m_akSimplePlayerInfo[i].szName, ChangeNamePacket.name, CHARACTER_NAME_MAX_LEN);

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_SELECT], "OnChangeName", Py_BuildValue("(is)", i, ChangeNamePacket.name));
			return true;
		}
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_SELECT], "OnCreateFailure", Py_BuildValue("(i)", 100));
	return true;
}
