#pragma once

#ifdef USE_NPROTECT_GAMEGUARD

bool GameGuard_Init();
bool GameGuard_IsError();
bool GameGuard_Run(HWND hWnd);
void GameGuard_NoticeMessage();

#endif /* USE_NPROTECT_GAMEGUARD */
