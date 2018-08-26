#include "StdAfx.h"
#include "Locale.h"
#include "WiseLogicXTrap.h"

#ifdef XTRAP_CLIENT_ENABLE

	#include "urlmon.h"
	#pragma comment(lib, "urlmon")
	#pragma comment(lib, "wininet")

	#include <XTrap/Xtrap_C_Interface.h>

	#pragma comment(lib, "XTrap4Client_mt.lib")
	#pragma comment(lib, "XTrap4Client_ClientPart_mt.lib")
	#pragma comment(lib, "XTrap4Client_ServerPart_mt.lib")
	#pragma comment(lib, "XTrap_Unipher_mt.lib")
	//#pragma comment(lib, "XTrap4Launcher_mt.lib" )

bool XTrap_Init()
{
	//XTrap_L_Patch( XTRAP_PATCH_ARG, NULL, 60 );
	XTrap_C_Start( XTRAP_PATCH_ARG, NULL );
	return true;
}

bool XTrap_CheckInit()
{
	XTrap_C_KeepAlive();
	return true;
}

void XTrap_PollEvent()
{
	static DWORD dwLastTick = 0;
	DWORD dwCurrentTick = ::GetTickCount();

	if( (dwCurrentTick - dwLastTick) > 2000 ) 
	{
		XTrap_C_CallbackAlive(2000);
		dwLastTick = dwCurrentTick;
	}
}

void XTrap_ValidateCheckStream( LPVOID PacketBufferIn, LPVOID PacketBufferOut )
{
	XTrap_CS_Step2( PacketBufferIn, PacketBufferOut, XTRAP_PROTECT_PE | XTRAP_PROTECT_TEXT | XTRAP_PROTECT_EXCEPT_VIRUS );
}


void XTrap_SetUserInfo(LPCSTR szUserID, LPCSTR szServerName, LPCSTR szCharacterName, LPCSTR szCharacterClass, SOCKET hSocket)
{
	XTrap_C_SetUserInfoEx( szUserID, szServerName, szCharacterName, szCharacterClass, NULL, hSocket );
}

#else

bool XTrap_Init()
{
	return true;
}

bool XTrap_CheckInit()
{
	return true;
}

void XTrap_PollEvent()
{
}

void XTrap_SetUserInfo(LPCSTR szUserID, LPCSTR szServerName, LPCSTR szCharacterName, LPCSTR szCharacterClass, SOCKET hSocket)
{

}

void XTrap_ValidateCheckStream( LPVOID PacketBufferIn, LPVOID PacketBufferOut )
{

}

#endif /* XTRAP_CLIENT_ENABLE */
