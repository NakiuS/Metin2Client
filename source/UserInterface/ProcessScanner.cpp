#include "StdAfx.h"
#include "ProcessScanner.h"

#include <tlhelp32.h>

static std::vector<CRCPair> gs_kVct_crcPair;
static CRITICAL_SECTION gs_csData;
static HANDLE gs_evReqExit=NULL;
static HANDLE gs_evResExit=NULL;
static HANDLE gs_hThread=NULL;

void ScanProcessList(std::map<DWORD, DWORD>& rkMap_crcProc, std::vector<CRCPair>* pkVct_crcPair)
{
	SYSTEM_INFO si;
	memset(&si, 0, sizeof(si));
	GetSystemInfo(&si);

	PROCESSENTRY32 pro;
    pro.dwSize = sizeof(PROCESSENTRY32);

    LPPROCESSENTRY32 Entry;
    Entry = &pro;

    HANDLE process = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

    BOOL bOK = Process32First(process, Entry);

    while(bOK)
    {
		HANDLE hProc = OpenProcess(PROCESS_VM_READ, FALSE, Entry->th32ProcessID);
		if (hProc)
		{
			HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, Entry->th32ProcessID);
			if (hModuleSnap != INVALID_HANDLE_VALUE) 
			{
				MODULEENTRY32	me32;
				memset(&me32, 0, sizeof(me32));
				me32.dwSize = sizeof(MODULEENTRY32);

				BOOL bRet = Module32First(hModuleSnap, &me32);
				while (bRet) 
				{
					DWORD crcExtPath=GetCRC32((const char*)me32.szExePath, strlen(me32.szExePath));

					std::map<DWORD, DWORD>::iterator f=rkMap_crcProc.find(crcExtPath);
					if (rkMap_crcProc.end()==f)
					{
						DWORD crcProc=GetFileCRC32(me32.szExePath);
						rkMap_crcProc.insert(std::make_pair(crcExtPath, crcProc));
						pkVct_crcPair->push_back(std::make_pair(crcProc, (const char*)me32.szExePath));						
					}
					Sleep(1);
						
					ZeroMemory(&me32, sizeof(MODULEENTRY32));
					me32.dwSize = sizeof(MODULEENTRY32);		
					bRet = Module32Next(hModuleSnap, &me32);
				}

				CloseHandle(hModuleSnap);
			}

			CloseHandle(hProc);		
		}

	
        bOK = Process32Next(process, Entry);
    }
    CloseHandle(process);
}

void ProcessScanner_ReleaseQuitEvent()
{
	SetEvent(gs_evReqExit);
}

void ProcessScanner_Destroy()
{
	ProcessScanner_ReleaseQuitEvent();

	WaitForSingleObject(gs_evResExit, INFINITE);	
	CloseHandle(gs_evReqExit);
	CloseHandle(gs_evResExit);
	DeleteCriticalSection(&gs_csData);
}

bool ProcessScanner_PopProcessQueue(std::vector<CRCPair>* pkVct_crcPair)
{
	EnterCriticalSection(&gs_csData);
	*pkVct_crcPair=gs_kVct_crcPair;
	gs_kVct_crcPair.clear();
	LeaveCriticalSection(&gs_csData);

	if (pkVct_crcPair->empty())
		return false;

	return true;	
}

void ProcessScanner_Thread(void* pv)
{	
	DWORD dwDelay=(rand()%10)*1000+1000*10;

	std::map<DWORD, DWORD>	kMap_crcProc;
	std::vector<CRCPair>	kVct_crcPair;
	
	while (WAIT_OBJECT_0 != WaitForSingleObject(gs_evReqExit, dwDelay))
	{
		kVct_crcPair.clear();
		ScanProcessList(kMap_crcProc, &kVct_crcPair);

		EnterCriticalSection(&gs_csData);
		gs_kVct_crcPair.insert(gs_kVct_crcPair.end(), kVct_crcPair.begin(), kVct_crcPair.end());
		LeaveCriticalSection(&gs_csData);				

		dwDelay=(rand()%10)*1000+1000;
	}

	SetEvent(gs_evResExit);
}


bool ProcessScanner_Create()
{
	InitializeCriticalSection(&gs_csData);
	gs_evReqExit=CreateEvent(NULL, FALSE, FALSE, "ProcessScanner_ReqExit");
	gs_evResExit=CreateEvent(NULL, FALSE, FALSE, "ProcessScanner_ResExit");

	gs_hThread=(HANDLE)_beginthread(ProcessScanner_Thread, 64*1024, NULL);
	if (INVALID_HANDLE_VALUE==gs_hThread)
	{
		LogBox("CreateThread Error");
		return false;
	}

	if (!SetThreadPriority(gs_hThread, THREAD_PRIORITY_LOWEST))
	{
		LogBox("SetThreadPriority Error");
		return false;
	}

	return true;
}
