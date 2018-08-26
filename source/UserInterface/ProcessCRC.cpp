#include "StdAfx.h"

#include <tlhelp32.h>

static BYTE abCRCMagicCube[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static BYTE abCRCXorTable[8] = { 102, 30, 188, 44, 39, 201, 43, 5 };
static BYTE bMagicCubeIdx = 0;

const char * stristr(const char * big, const char * little)
{
	const char * t = big;
	size_t len = strlen(little) - 1;

	for (t = big; *t; ++t)
		if (!_strnicmp(t, little, len))
			return t;

	return NULL;
}

bool GetProcessInformation(std::string & exeFileName, LPCVOID * ppvAddress)
{
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if (hModuleSnap != INVALID_HANDLE_VALUE) 
	{		
		std::string filename;

		GetExcutedFileName(filename);

		MODULEENTRY32 me32;
		memset(&me32, 0, sizeof(me32));
		me32.dwSize = sizeof(MODULEENTRY32);

		BOOL bRet = Module32First(hModuleSnap, &me32);

		while (bRet) 
		{
			if (stristr(me32.szExePath, filename.c_str()))
			{
				exeFileName = me32.szExePath;
				*ppvAddress = me32.modBaseAddr;
				CloseHandle(hModuleSnap);
				return true;
			}

			ZeroMemory(&me32, sizeof(MODULEENTRY32));
			me32.dwSize = sizeof(MODULEENTRY32);

			bRet = Module32Next(hModuleSnap, &me32);
		}

		CloseHandle(hModuleSnap);
	}

	return false;
}

DWORD GetProcessMemoryCRC(LPCVOID c_pvBaseAddress)
{
	HANDLE hProcess = GetCurrentProcess();
	char * pBuf = new char[1024*1024];
	DWORD dwBytesRead;

	BOOL bRet = ReadProcessMemory(hProcess, c_pvBaseAddress, pBuf, 1024*1024, &dwBytesRead);

	if (!bRet && GetLastError() == ERROR_PARTIAL_COPY)
		bRet = true;

	if (bRet)
	{
		DWORD dwCRC = GetCRC32(pBuf, dwBytesRead);
		delete [] pBuf;
		return dwCRC;
	}

	delete [] pBuf;
	return 0;
}

bool __GetExeCRC(DWORD & r_dwProcCRC, DWORD & r_dwFileCRC)
{
	std::string exeFileName;
	LPCVOID c_pvBaseAddress;

	GetExcutedFileName(exeFileName);

	if (GetProcessInformation(exeFileName, &c_pvBaseAddress))
		r_dwProcCRC = GetProcessMemoryCRC(c_pvBaseAddress);
	else
		r_dwProcCRC = 0;

	r_dwFileCRC = GetFileCRC32(exeFileName.c_str());
	return true;
}

void BuildProcessCRC()
{
	if (LocaleService_IsHONGKONG() || LocaleService_IsTAIWAN())
	{
		memset(abCRCMagicCube, 0, sizeof(abCRCMagicCube));
		bMagicCubeIdx = 0;
		return;
	}
	
	DWORD dwProcCRC, dwFileCRC;

	if (__GetExeCRC(dwProcCRC, dwFileCRC))
	{
		abCRCMagicCube[0] = BYTE(dwProcCRC & 0x000000ff);
		abCRCMagicCube[1] = BYTE(dwFileCRC & 0x000000ff);
		abCRCMagicCube[2] = BYTE( (dwProcCRC & 0x0000ff00) >> 8 );
		abCRCMagicCube[3] = BYTE( (dwFileCRC & 0x0000ff00) >> 8 );
		abCRCMagicCube[4] = BYTE( (dwProcCRC & 0x00ff0000) >> 16 );
		abCRCMagicCube[5] = BYTE( (dwFileCRC & 0x00ff0000) >> 16 );
		abCRCMagicCube[6] = BYTE( (dwProcCRC & 0xff000000) >> 24 );
		abCRCMagicCube[7] = BYTE( (dwFileCRC & 0xff000000) >> 24 );

		bMagicCubeIdx = 0;
	}
}

BYTE GetProcessCRCMagicCubePiece()
{
	BYTE bPiece = BYTE(abCRCMagicCube[bMagicCubeIdx] ^ abCRCXorTable[bMagicCubeIdx]);

	if (!(++bMagicCubeIdx & 7))
		bMagicCubeIdx = 0;

	return bPiece;
}
