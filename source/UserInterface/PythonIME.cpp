#include "StdAfx.h"
#include "PythonIME.h"
#include "AbstractApplication.h"

CPythonIME::CPythonIME()
: CIME()
{
	ms_pEvent = this;
}

CPythonIME::~CPythonIME()
{
	Tracen("PythonIME Clear");
}

void CPythonIME::Create(HWND hWnd)
{
	Initialize(hWnd);
}

void CPythonIME::MoveLeft()
{
	DecCurPos();
}

void CPythonIME::MoveRight()
{
	IncCurPos();
}

void CPythonIME::MoveHome()
{
	ms_curpos = 0;
}

void CPythonIME::MoveEnd()
{
	ms_curpos = ms_lastpos;
}

void CPythonIME::SetCursorPosition(int iPosition)
{
	SetCurPos(iPosition);
}

void CPythonIME::Delete()
{
	DelCurPos();
}

void CPythonIME::OnUpdate()
{
	IAbstractApplication::GetSingleton().RunIMEUpdate();
}

void CPythonIME::OnTab()
{
	IAbstractApplication::GetSingleton().RunIMETabEvent();
}

void CPythonIME::OnReturn()
{
	IAbstractApplication::GetSingleton().RunIMEReturnEvent();
}

void CPythonIME::OnEscape()
{
//	IAbstractApplication::GetSingleton().RunIMEEscapeEvent();
}

bool CPythonIME::OnWM_CHAR( WPARAM wParam, LPARAM lParam )
{
	unsigned char c = unsigned char(wParam & 0xff);

	switch (c) 
	{
	case VK_RETURN:
		OnReturn();
		return true;

	case VK_TAB:
		if(ms_bCaptureInput == false)
			return 0;
		OnTab();
		return true;

	case VK_ESCAPE:
		if(ms_bCaptureInput == false)
			return 0;
		OnEscape();
		return true;
	}
	return false;
}

void CPythonIME::OnChangeCodePage()
{
	IAbstractApplication::GetSingleton().RunIMEChangeCodePage();
}

void CPythonIME::OnOpenCandidateList()
{
	IAbstractApplication::GetSingleton().RunIMEOpenCandidateListEvent();
}

void CPythonIME::OnCloseCandidateList()
{
	IAbstractApplication::GetSingleton().RunIMECloseCandidateListEvent();
}

void CPythonIME::OnOpenReadingWnd()
{
	IAbstractApplication::GetSingleton().RunIMEOpenReadingWndEvent();
}

void CPythonIME::OnCloseReadingWnd()
{
	IAbstractApplication::GetSingleton().RunIMECloseReadingWndEvent();
}
