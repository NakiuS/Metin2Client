#include "StdAfx.h"

#include <time.h>
#include <stdio.h>
#include "Debug.h"
#include "Singleton.h"
#include "Timer.h"

const DWORD DEBUG_STRING_MAX_LEN = 1024;

static int isLogFile = false;
HWND g_PopupHwnd = NULL;

class CLogFile : public CSingleton<CLogFile>
{
	public:
		CLogFile() : m_fp(NULL)
		{
		}

		virtual ~CLogFile()
		{
			if (m_fp)
				fclose(m_fp);

			m_fp = NULL;
		}

		void Initialize()
		{
			m_fp = fopen("log.txt", "w");
		}

		void Write(const char * c_pszMsg)
		{
			if (!m_fp)
				return;

			time_t ct = time(0);
			struct tm ctm = *localtime(&ct);

			fprintf(m_fp, "%02d%02d %02d:%02d:%05d :: %s", 
				ctm.tm_mon + 1, 
				ctm.tm_mday,
				ctm.tm_hour,
				ctm.tm_min,
				ELTimer_GetMSec() % 60000,
				c_pszMsg);

			fflush(m_fp);
		}

	protected:
		FILE *	m_fp;
};

static CLogFile gs_logfile;

static UINT gs_uLevel=0;

void SetLogLevel(UINT uLevel)
{
	gs_uLevel=uLevel;
}

void Log(UINT uLevel, const char* c_szMsg)
{
	if (uLevel>=gs_uLevel)
		Trace(c_szMsg);
}

void Logn(UINT uLevel, const char* c_szMsg)
{
	if (uLevel>=gs_uLevel)
		Tracen(c_szMsg);
}

void Logf(UINT uLevel, const char* c_szFormat, ...)
{
	if (uLevel<gs_uLevel)
		return;
	
	char szBuf[DEBUG_STRING_MAX_LEN+1];

	va_list args;
	va_start(args, c_szFormat);
	_vsnprintf(szBuf, sizeof(szBuf), c_szFormat, args);
	va_end(args);
#ifdef _DEBUG
	OutputDebugString(szBuf);	
	fputs(szBuf, stdout);
#endif

	if (isLogFile)
		LogFile(szBuf);
}

void Lognf(UINT uLevel, const char* c_szFormat, ...)
{
	if (uLevel<gs_uLevel)
		return;

	va_list args;
	va_start(args, c_szFormat);

	char szBuf[DEBUG_STRING_MAX_LEN+2];
	int len = _vsnprintf(szBuf, sizeof(szBuf)-1, c_szFormat, args);

	if (len > 0)
	{
		szBuf[len] = '\n';
		szBuf[len + 1] = '\0';
	}
	va_end(args);
#ifdef _DEBUG
	OutputDebugString(szBuf);	
	puts(szBuf);
#endif

	if (isLogFile)
		LogFile(szBuf);
}


void Trace(const char * c_szMsg)
{
#ifdef _DEBUG
	OutputDebugString(c_szMsg);
	printf("%s", c_szMsg);
#endif

	if (isLogFile)
		LogFile(c_szMsg);
}

void Tracen(const char* c_szMsg)
{
#ifdef _DEBUG
	char szBuf[DEBUG_STRING_MAX_LEN+1];
	_snprintf(szBuf, sizeof(szBuf), "%s\n", c_szMsg);
	OutputDebugString(szBuf);
	puts(c_szMsg);

	if (isLogFile)
		LogFile(szBuf);

	puts(c_szMsg);
	putc('\n', stdout);
#else
	if (isLogFile)
	{
		LogFile(c_szMsg);
		LogFile("\n");
	}
#endif
}

void Tracenf(const char* c_szFormat, ...)
{
	va_list args;
	va_start(args, c_szFormat);

	char szBuf[DEBUG_STRING_MAX_LEN+2];
	int len = _vsnprintf(szBuf, sizeof(szBuf)-1, c_szFormat, args);

	if (len > 0)
	{
		szBuf[len] = '\n';
		szBuf[len + 1] = '\0';
	}
	va_end(args);
#ifdef _DEBUG
	OutputDebugString(szBuf);	
	printf("%s", szBuf);
#endif

	if (isLogFile)
		LogFile(szBuf);
}

void Tracef(const char* c_szFormat, ...)
{
	char szBuf[DEBUG_STRING_MAX_LEN+1];

	va_list args;
	va_start(args, c_szFormat);
	_vsnprintf(szBuf, sizeof(szBuf), c_szFormat, args);
	va_end(args);

#ifdef _DEBUG
	OutputDebugString(szBuf);	
	fputs(szBuf, stdout);
#endif

	if (isLogFile)
		LogFile(szBuf);
}

void TraceError(const char* c_szFormat, ...)
{
#ifndef _DISTRIBUTE 

	char szBuf[DEBUG_STRING_MAX_LEN+2];

	strncpy(szBuf, "SYSERR: ", DEBUG_STRING_MAX_LEN);
	int len = strlen(szBuf);

	va_list args;
	va_start(args, c_szFormat);
	len = _vsnprintf(szBuf + len, sizeof(szBuf) - (len + 1), c_szFormat, args) + len;
	va_end(args);

	szBuf[len] = '\n';
	szBuf[len + 1] = '\0';

	time_t ct = time(0);
	struct tm ctm = *localtime(&ct);

	fprintf(stderr, "%02d%02d %02d:%02d:%05d :: %s", 
					ctm.tm_mon + 1, 
					ctm.tm_mday,
					ctm.tm_hour,
					ctm.tm_min,
					ELTimer_GetMSec() % 60000,
					szBuf + 8);
	fflush(stderr);
	
#ifdef _DEBUG
	OutputDebugString(szBuf);
	fputs(szBuf, stdout);
#endif

	if (isLogFile)
		LogFile(szBuf);

#endif
}

void TraceErrorWithoutEnter(const char* c_szFormat, ...)
{
#ifndef _DISTRIBUTE 

	char szBuf[DEBUG_STRING_MAX_LEN];

	va_list args;
	va_start(args, c_szFormat);
	_vsnprintf(szBuf, sizeof(szBuf), c_szFormat, args);
	va_end(args);

	time_t ct = time(0);
	struct tm ctm = *localtime(&ct);

	fprintf(stderr, "%02d%02d %02d:%02d:%05d :: %s", 
					ctm.tm_mon + 1, 
					ctm.tm_mday,
					ctm.tm_hour,
					ctm.tm_min,
					ELTimer_GetMSec() % 60000,
					szBuf + 8);
	fflush(stderr);

#ifdef _DEBUG
	OutputDebugString(szBuf);
	fputs(szBuf, stdout);
#endif

	if (isLogFile)
		LogFile(szBuf);
#endif
}

void LogBoxf(const char* c_szFormat, ...)
{
	va_list args;
	va_start(args, c_szFormat);

	char szBuf[2048];
	_vsnprintf(szBuf, sizeof(szBuf), c_szFormat, args);

	LogBox(szBuf);
}

void LogBox(const char* c_szMsg, const char * c_szCaption, HWND hWnd)
{
	if (!hWnd)
		hWnd = g_PopupHwnd;

	MessageBox(hWnd, c_szMsg, c_szCaption ? c_szCaption : "LOG", MB_OK);
	Tracen(c_szMsg);
}

void LogFile(const char * c_szMsg)
{
	CLogFile::Instance().Write(c_szMsg);
}

void LogFilef(const char * c_szMessage, ...)
{
	va_list args;
	va_start(args, c_szMessage);
	char szBuf[DEBUG_STRING_MAX_LEN+1];
	_vsnprintf(szBuf, sizeof(szBuf), c_szMessage, args);

	CLogFile::Instance().Write(szBuf);
}

void OpenLogFile(bool bUseLogFIle)
{
#ifndef _DISTRIBUTE 
	freopen("syserr.txt", "w", stderr);

	if (bUseLogFIle)
	{
		isLogFile = true;
		CLogFile::Instance().Initialize();
	}
#endif
}

void OpenConsoleWindow()
{
	AllocConsole();

	freopen("CONOUT$", "a", stdout);
	freopen("CONIN$", "r", stdin);
}
