#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

extern "C" {

int		WebBrowser_Startup(HINSTANCE hInstance);
void	WebBrowser_Cleanup();
void	WebBrowser_Destroy();
int		WebBrowser_Show(HWND parent, const char* addr, const RECT* rcWebBrowser);
void	WebBrowser_Hide();
void	WebBrowser_Move(const RECT* rcWebBrowser);

int WebBrowser_IsVisible();
const RECT& WebBrowser_GetRect();

}