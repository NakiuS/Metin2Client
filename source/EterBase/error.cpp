#include "StdAfx.h"

#include <stdio.h>
#include <time.h>
#include <winsock.h>
#include <imagehlp.h>

FILE * fException;

/*
static char __msg[4000], __cmsg[4000];
static int __idx;
CLZObject __l;
*/
/*
typedef BOOL
(CALLBACK *PENUMLOADED_MODULES_CALLBACK)(
    __in PCSTR ModuleName,
    __in ULONG ModuleBase,
    __in ULONG ModuleSize,
    __in_opt PVOID UserContext
    );
*/
#if _MSC_VER >= 1400
BOOL CALLBACK EnumerateLoadedModulesProc(PCSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize, PVOID UserContext)
#else
BOOL CALLBACK EnumerateLoadedModulesProc(PSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize, PVOID UserContext)
#endif
{
	DWORD offset = *((DWORD*)UserContext);
	
	if (offset >= ModuleBase && offset <= ModuleBase + ModuleSize)
	{
		fprintf(fException, "%s", ModuleName);
		//__idx += sprintf(__msg+__idx, "%s", ModuleName);
		return FALSE;
	}
	else
		return TRUE;
}

LONG __stdcall EterExceptionFilter(_EXCEPTION_POINTERS* pExceptionInfo)
{
	HANDLE		hProcess	= GetCurrentProcess();
	HANDLE		hThread		= GetCurrentThread();
	
	fException = fopen("ErrorLog.txt", "wt");
	if (fException)
	{
		char module_name[256];
		time_t module_time;
		
		HMODULE hModule = GetModuleHandle(NULL);
		
		GetModuleFileName(hModule, module_name, sizeof(module_name));
		module_time = (time_t)GetTimestampForLoadedLibrary(hModule);
		
		fprintf(fException, "Module Name: %s\n", module_name);
		fprintf(fException, "Time Stamp: 0x%08x - %s\n", module_time, ctime(&module_time));
		fprintf(fException, "\n");
		fprintf(fException, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
		fprintf(fException, "\n");

		/*
		{
			__idx+=sprintf(__msg+__idx,"Module Name: %s\n", module_name);
			__idx+=sprintf(__msg+__idx, "Time Stamp: 0x%08x - %s\n", module_time, ctime(&module_time));
			__idx+=sprintf(__msg+__idx, "\n");
			__idx+=sprintf(__msg+__idx, "Exception Type: 0x%08x\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
			__idx+=sprintf(__msg+__idx, "\n");
		}
		*/
		
		CONTEXT&	context		= *pExceptionInfo->ContextRecord;
		
		fprintf(fException, "eax: 0x%08x\tebx: 0x%08x\n", context.Eax, context.Ebx);
		fprintf(fException, "ecx: 0x%08x\tedx: 0x%08x\n", context.Ecx, context.Edx);
		fprintf(fException, "esi: 0x%08x\tedi: 0x%08x\n", context.Esi, context.Edi);
		fprintf(fException, "ebp: 0x%08x\tesp: 0x%08x\n", context.Ebp, context.Esp);
		fprintf(fException, "\n");
		/*
		{
			__idx+=sprintf(__msg+__idx, "eax: 0x%08x\tebx: 0x%08x\n", context.Eax, context.Ebx);
			__idx+=sprintf(__msg+__idx, "ecx: 0x%08x\tedx: 0x%08x\n", context.Ecx, context.Edx);
			__idx+=sprintf(__msg+__idx, "esi: 0x%08x\tedi: 0x%08x\n", context.Esi, context.Edi);
			__idx+=sprintf(__msg+__idx, "ebp: 0x%08x\tesp: 0x%08x\n", context.Ebp, context.Esp);
			__idx+=sprintf(__msg+__idx, "\n");
		}
		*/
		
		STACKFRAME stackFrame = {0,};
		stackFrame.AddrPC.Offset	= context.Eip;
		stackFrame.AddrPC.Mode		= AddrModeFlat;
		stackFrame.AddrStack.Offset	= context.Esp;
		stackFrame.AddrStack.Mode	= AddrModeFlat;
		stackFrame.AddrFrame.Offset	= context.Ebp;
		stackFrame.AddrFrame.Mode	= AddrModeFlat;
		
		for (int i=0; i < 512 && stackFrame.AddrPC.Offset; ++i)
		{
			if (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stackFrame, &context, NULL, NULL, NULL, NULL) != FALSE)
			{
				fprintf(fException, "0x%08x\t", stackFrame.AddrPC.Offset);
				//__idx+=sprintf(__msg+__idx, "0x%08x\t", stackFrame.AddrPC.Offset);
				EnumerateLoadedModules(hProcess, (PENUMLOADED_MODULES_CALLBACK) EnumerateLoadedModulesProc, &stackFrame.AddrPC.Offset);
				fprintf(fException, "\n");

				//__idx+=sprintf(__msg+__idx,  "\n");
			}
			else
			{
				break;
			}
		} 
		
		fprintf(fException, "\n");
		//__idx+=sprintf(__msg+__idx, "\n");
		
		
/*
		BYTE* stack = (BYTE*)(context.Esp);
		fprintf(fException, "stack %08x - %08x\n", context.Esp, context.Esp+1024);
		//__idx+=sprintf(__msg+__idx, "stack %08x - %08x\n", context.Esp, context.Esp+1024);
		
		for(i=0; i<16; ++i)
		{
			fprintf(fException, "%08X : ", context.Esp+i*16);
			//__idx+=sprintf(__msg+__idx, "%08X : ", context.Esp+i*16);
			for(int j=0; j<16; ++j)
			{
				fprintf(fException, "%02X ", stack[i*16+j]);
				//__idx+=sprintf(__msg+__idx, "%02X ", stack[i*16+j]);
			}
			fprintf(fException, "\n");
			//__idx+=sprintf(__msg+__idx, "\n");
		}
		fprintf(fException, "\n");
		//__idx+=sprintf(__msg+__idx, "\n");
*/

		fflush(fException);

		fclose(fException);
		fException = NULL;
		
		//WinExec()
		/*CreateProcess("cmd.exe",NULL,NULL,NULL,FALSE,
			CREATE_NEW_PROCESS_GROUP|DETACHED_PROCESS,NULL,NULL,NULL,NULL);
		MessageBox(NULL,"게임 실행에 치명적인 문제가 발생하였습니다.\n게임을 종료하고 에러 로그를 남깁니다.\n에러 로그를 서버에 보내시겠습니까?","에러 발생!",MB_YESNO);*/

		/*
		__l.BeginCompressInBuffer(__msg,__idx,__cmsg);
		if (__l.Compress())
		{
			//fprintf(fException,"Compress printing\n");
			// send this to server
			SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			
			ioctlsocket(s,FIONBIO,0);

			if (s==INVALID_SOCKET)
			{
				//fprintf(fException,"INVALID %X\n",WSAGetLastError());
			}
			
			sockaddr_in sa;
			sa.sin_family = AF_INET;
			sa.sin_port = htons(19294);
			sa.sin_addr.s_addr = inet_addr("147.46.127.42");
			if (connect(s,(sockaddr*)&sa,sizeof(sa)))
			{
				//fprintf(fException,"%X\n",WSAGetLastError());
			}

			int total = 0;
			int ret=0;
			while(1)
			{
				//ret = send(s,(char*)__msg+total,__idx-total,0);
				ret = send(s,(char*)__l.GetBuffer()+total,__l.GetSize()-total,0);
				//fprintf(fException,"send %d\n",ret);
				if (ret<0)
				{
					//fprintf(fException,"%X\n",WSAGetLastError());
					break;
				}
				total+=ret;
				if (total>=__idx)
				//if (total>=__l.GetSize())
					break;
			}
			//__l.GetBuffer();
			Sleep(500);
			closesocket(s);
		}*/

		WinExec("errorlog.exe",SW_SHOW);
		
		
	}
	
	return EXCEPTION_EXECUTE_HANDLER;
}

void SetEterExceptionHandler()
{
	SetUnhandledExceptionFilter(EterExceptionFilter);
}