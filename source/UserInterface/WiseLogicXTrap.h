#pragma once

bool XTrap_Init();
bool XTrap_CheckInit();
void XTrap_PollEvent();
void XTrap_ValidateCheckStream( LPVOID PacketBufferIn, LPVOID PacketBufferOut );
void XTrap_SetUserInfo(LPCSTR szUserID, LPCSTR szServerName, LPCSTR szCharacterName, LPCSTR szCharacterClass, SOCKET hSocket);

