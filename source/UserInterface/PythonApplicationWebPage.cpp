#include "StdAfx.h"
#include "PythonApplication.h"
#include "../CWebBrowser/CWebBrowser.h"

bool CPythonApplication::IsWebPageMode()
{
	return WebBrowser_IsVisible() ? true : false;
}

void CPythonApplication::ShowWebPage(const char* c_szURL, const RECT& c_rcWebPage)
{
	if (WebBrowser_IsVisible())
		return;

	m_grpDevice.EnableWebBrowserMode(c_rcWebPage);
	if (!WebBrowser_Show(GetWindowHandle(), c_szURL, &c_rcWebPage))
	{
		TraceError("CREATE_WEBBROWSER_ERROR:%d", GetLastError());
	}

	SetCursorMode(CURSOR_MODE_HARDWARE);
}

void CPythonApplication::MoveWebPage(const RECT& c_rcWebPage)
{
	if (WebBrowser_IsVisible())
	{
		m_grpDevice.MoveWebBrowserRect(c_rcWebPage);
		WebBrowser_Move(&c_rcWebPage);
	}
}

void CPythonApplication::HideWebPage()
{
	if (WebBrowser_IsVisible())
	{
		WebBrowser_Hide();

		m_grpDevice.DisableWebBrowserMode();	

		if (m_pySystem.IsSoftwareCursor())
			SetCursorMode(CURSOR_MODE_SOFTWARE);
		else
			SetCursorMode(CURSOR_MODE_HARDWARE);
	}
}
