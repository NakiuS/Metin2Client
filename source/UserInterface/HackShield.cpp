#include "StdAfx.h"
#include "Hackshield.h"

#ifdef USE_AHNLAB_HACKSHIELD

#if defined(LOCALE_SERVICE_EUROPE)
#define METIN2HS_MONITORING_SERVER_ADDR "79.110.88.84"
#elif
#define METIN2HS_MONITORING_SERVER_ADDR "79.110.88.84" // GF
#endif
//#define METIN2HS_MONITORING_SERVER_ADDR "119.192.130.160"  //김용욱 pc

#include METIN2HS_INCLUDE_HSUPCHK
#include METIN2HS_INCLUDE_HSUSERUTIL
#include METIN2HS_INCLUDE_HSHIELD
#include METIN2HS_INCLUDE_HSHIELDLIBLINK

#define MA_MAX_PATH MAX_PATH
#define MA_T
#define MA_TCHAR char
#define MA_StringFormat snprintf
#define MA_ARRAYCOUNT sizeof

static DWORD gs_dwMainThreadID = 0;

static bool gs_isHackDetected = false;
static char gs_szHackMsg[256];

void MA_PathSplit(MA_TCHAR* src, MA_TCHAR* branch, size_t branchLen, MA_TCHAR* leaf, size_t leafLen)
{
	char* p = src;
	char* n = p;
	while (n = strchr(p, '\\'))
		p = n + 1;

	if (branch)
		strncpy(branch, src, p - src);

	if (leaf)
		strcpy(leaf, p);	
}

void MA_PathMerge(MA_TCHAR* dst, size_t dstLen, MA_TCHAR* branch, MA_TCHAR* leaf)
{
	snprintf(dst, dstLen, "%s\\%s", branch, leaf);
}

int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam)
{
	switch (lCode)
	{
		//Engine Callback
	case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			MA_StringFormat(gs_szHackMsg, MA_ARRAYCOUNT(gs_szHackMsg), MA_T("DETECT_GAME_HACK\n%s"), (char*)pParam);
			gs_isHackDetected = true;
			break;
		}
		//창모드 해킹툴 감지
	case AHNHS_ENGINE_DETECT_WINDOWED_HACK:
		{
			//MA_StringFormat(gs_szHackMsg, MA_ARRAYCOUNT(gs_szHackMsg), MA_T("DETECT_WINDOWED_HACK"));
			//gs_isHackDetected = true;
			break;
		}

		//Speed 관련
	case AHNHS_ACTAPC_DETECT_SPEEDHACK:
		{
			MA_StringFormat(gs_szHackMsg, MA_ARRAYCOUNT(gs_szHackMsg), MA_T("DETECT_SPEED_HACK"));
			gs_isHackDetected = true;
			break;
		}

		//디버깅 방지 
	case AHNHS_ACTAPC_DETECT_KDTRACE:
//	case AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED:
		{
			MA_StringFormat(gs_szHackMsg, MA_ARRAYCOUNT(gs_szHackMsg), MA_T("DETECT_DEBUGING(checkCode=%x)"), lCode);			
			gs_isHackDetected = true;
			break;
		}

	case AHNHS_ACTAPC_DETECT_AUTOMACRO:
		{
			MA_StringFormat(gs_szHackMsg, MA_ARRAYCOUNT(gs_szHackMsg), MA_T("DETECT_MACRO(checkCode=%x)"), lCode);
			gs_isHackDetected = true;
			break;
		}

		// 코드 패치 감지
	case AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS:
		{
			MA_StringFormat(gs_szHackMsg, MA_ARRAYCOUNT(gs_szHackMsg), MA_T("DETECT_MEMORY_ACCESS\n%s"), (char*)pParam);
			gs_isHackDetected = true;
			break;
		}

		//메모리 변조 감지.
	case AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP:  //임의 기준 주소 (Randomized Base Address) 확인해보기. Linker->Advanced->(/DYNAMICBASE:NO)
		{
			MA_StringFormat(gs_szHackMsg, MA_ARRAYCOUNT(gs_szHackMsg), MA_T("DETECT_MEMORY_MODIFY"));
			gs_isHackDetected = true;
			
		}
		break;

		// 핵쉴드 로컬 메모리 보호 기능이 정상적이지 않습니다.
	case AHNHS_ACTAPC_DETECT_LMP_FAILED:
		{
			MA_StringFormat(gs_szHackMsg, MA_ARRAYCOUNT(gs_szHackMsg), MA_T("LOCAL_MEMORY_PROTECT_FAILED"));
			gs_isHackDetected = true;
			
		}
		break;

		//그외 해킹 방지 기능 이상 
//	case AHNHS_ACTAPC_DETECT_AUTOMOUSE:
	case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
	case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
//	case AHNHS_ACTAPC_DETECT_MESSAGEHOOK:
//	case AHNHS_ACTAPC_DETECT_MODULE_CHANGE:
	case AHNHS_ACTAPC_DETECT_ENGINEFAILED:
	case AHNHS_ACTAPC_DETECT_CODEMISMATCH:
//	case AHNHS_ACTAPC_DETECT_PROTECTSCREENFAILED:	
		{
			MA_StringFormat(gs_szHackMsg, MA_ARRAYCOUNT(gs_szHackMsg), MA_T("DETECT_HACKING(checkCode = %x)\n"), lCode);
			gs_isHackDetected = true;
			break;
		}
	}
	return 1;
}

bool HackShield_PollEvent()
{
	return gs_isHackDetected;
	//return false;
}

bool HackShield_Init()
{

	MA_TCHAR szModuleDirPath[MA_MAX_PATH];
	::GetCurrentDirectory(MA_ARRAYCOUNT(szModuleDirPath), szModuleDirPath);

#ifndef _DEBUG
	//핵쉴드 업데이트
	DWORD dwUpRet = 0; 
	MA_TCHAR szFullFilePath[MA_MAX_PATH];
	// 핵쉴드 폴더 위치를 지정합니다. 
	MA_PathMerge(szFullFilePath, MA_ARRAYCOUNT(szFullFilePath), szModuleDirPath, "hshield");
	AHNHS_EXT_ERRORINFO HsExtError = {0,}; 
	
	// _AhnHS_HSUpdate 함수 호출 
	dwUpRet = _AhnHS_HSUpdateEx( szFullFilePath, // 핵쉴드 폴더 경로 
							1000 * 600, // 업데이트 전체 타임 아웃 
							METIN2HS_CODE, // 게임 코드 
							AHNHSUPDATE_CHKOPT_HOSTFILE| AHNHSUPDATE_CHKOPT_GAMECODE, 
							HsExtError, 
							1000* 20 ); // 서버 연결 타임아웃 
							
	// Ex 함수를 사용하실때는 반드시 HSUpSetEnv.exe 설정 툴로 env 파일에 
	// 게임 코드를 입력하셔야 합니다. 
	if ( dwUpRet != ERROR_SUCCESS) 
	{ 
		
		// 에러 처리 
		switch ( dwUpRet ) 
		{ 
			case HSERROR_ENVFILE_NOTREAD: 
				MessageBox(NULL, MA_T("HACK_SHIELD_UPDATE_ERROR : HSERROR_ENVFILE_NOTREAD"), "HACK_SHIELD", MB_OK);
				break;
			case HSERROR_ENVFILE_NOTWRITE: 
				MessageBox(NULL, MA_T("HACK_SHIELD_UPDATE_ERROR : HSERROR_ENVFILE_NOTWRITE"), "HACK_SHIELD", MB_OK);
				break;
			case HSERROR_NETWORK_CONNECT_FAIL: 
				MessageBox(NULL, MA_T("HACK_SHIELD_UPDATE_ERROR : HSERROR_NETWORK_CONNECT_FAIL"), "HACK_SHIELD", MB_OK);
				break;
			case HSERROR_HSUPDATE_TIMEOUT: 
				MessageBox(NULL, MA_T("HACK_SHIELD_UPDATE_ERROR : HSERROR_HSUPDATE_TIMEOUT"), "HACK_SHIELD", MB_OK);
				break;
			case HSERROR_MISMATCH_ENVFILE:
				MessageBox(NULL, MA_T("HACK_SHIELD_UPDATE_ERROR : HSERROR_MISMATCH_ENVFILE"), "HACK_SHIELD", MB_OK);
				break;
			case HSERROR_HOSTFILE_MODIFICATION: 
				MessageBox(NULL, MA_T("HACK_SHIELD_UPDATE_ERROR : HSERROR_HOSTFILE_MODIFICATION"), "HACK_SHIELD", MB_OK);
				break;
			default:
				break;
		}
		
		MessageBox(NULL, MA_T("HACK_SHIELD_UPDATE_ERROR"), "HACK_SHIELD", MB_OK);

		return false;
		
	}
#endif

	MA_TCHAR szInterfaceFilePath[MA_MAX_PATH];
	{
		if (!_AhnHSUserUtil_IsAdmin() && !_AhnHsUserUtil_IsEnableHSAdminRights())
		{
			DWORD dwRet = _AhnHsUserUtil_CreateUser();
			switch (dwRet)
			{
			case HSUSERUTIL_ERR_OK: // pass
				break;
			case HSUSERUTIL_ERR_NOT_ADMIN: // 유저 실행을 허용하므로 에러 아님
				break;
			case HSUSERUTIL_ERR_NOT_NT: // 98 유저 실행을 허용하므로 에러 아님
				break;
			case HSUSERUTIL_ERR_DELHIDEIDINFO_FAIL:
				MessageBox(NULL, MA_T("DEL_SHADOW_HIDDEN_ERROR"), "HACK_SHIELD", MB_OK);
				return false;
			case HSUSERUTIL_ERR_DELSHADOWACNT_FAIL:
				MessageBox(NULL, MA_T("DEL_SHADOW_ACCOUNT_ERROR"), "HACK_SHIELD", MB_OK);
				return false;			
			case HSUSERUTIL_ERR_ADDSHADOWACNT_FAIL:
				MessageBox(NULL, MA_T("ADD_SHADOW_ACCOUNT_ERROR"), "HACK_SHIELD", MB_OK);
				return false;						
			}

		}

#ifdef _DEBUG
		MA_PathMerge(szInterfaceFilePath, MA_ARRAYCOUNT(szInterfaceFilePath), szModuleDirPath, "hshield\\EHsvc.dll");
#else
		MA_PathMerge(szInterfaceFilePath, MA_ARRAYCOUNT(szInterfaceFilePath), szModuleDirPath, "hshield\\EHsvc.dll");
#endif
	}

#ifndef _DEBUG
	{
		AHNHS_EXT_ERRORINFO HsExtError = { 0, };
		strcpy(HsExtError.szServer, METIN2HS_MONITORING_SERVER_ADDR); //모니터링 주소	
		strcpy(HsExtError.szGameVersion, "1.0.0.0"); //Game 버전
		strcpy(HsExtError.szUserId, "Metin2User_test"); //유저 ID

		DWORD dwRet = _AhnHS_StartMonitor (HsExtError, szInterfaceFilePath);
		if( dwRet != ERROR_SUCCESS )
		{
			MessageBox(NULL, MA_T("START_MONITORING_SERVICE_ERROR"), "HACK_SHIELD", MB_OK);
		}
	}
#endif

	{
		int nRet = _AhnHS_Initialize(szInterfaceFilePath, AhnHS_Callback, 
			METIN2HS_CODE,
			METIN2HS_LICENSE,
			AHNHS_CHKOPT_SPEEDHACK 
			| AHNHS_CHKOPT_READWRITEPROCESSMEMORY 
			| AHNHS_CHKOPT_KDTRACER 
//			| AHNHS_CHKOPT_OPENPROCESS 
			| AHNHS_CHKOPT_AUTOMOUSE 			
			| AHNHS_CHKOPT_MESSAGEHOOK
			| AHNHS_CHKOPT_PROTECT_D3DX
			| AHNHS_CHKOPT_LOCAL_MEMORY_PROTECTION // 코드 영역 로딩 보호
			//| AHNHS_CHKOPT_ANTIFREESERVER // 프리 서버 접속 방지
			| AHNHS_USE_LOG_FILE			
//			| AHNHS_ALLOW_SVCHOST_OPENPROCESS
			//| AHNHS_ALLOW_LSASS_OPENPROCESS // 신용 카드 결제 관련
			//| AHNHS_ALLOW_CSRSS_OPENPROCESS // 신용 카드 결제 관련
			//| AHNHS_CHKOPT_SELF_DESTRUCTION // 강제 종료 처리: 콜백 처리 주의
			| AHNHS_CHKOPT_PROCESSSCAN
			| AHNHS_CHKOPT_UPDATED_FILE_CHECK
			| AHNHS_CHKOPT_SEND_MONITOR_ONCE // 모니터링용: 1회만 보냄
			| AHNHS_CHKOPT_SEND_MONITOR_DELAY // 모니터링용: 아이디를 천천히 보냄
			| AHNHS_DONOT_TERMINATE_PROCESS, // 서버 알림	
			AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL
			);

		if (nRet != HS_ERR_OK) 
		{
			//Error 처리 
			switch(nRet)
			{
				case HS_ERR_ANOTHER_SERVICE_RUNNING:
				{
					MessageBox(NULL, MA_T("ANOTHER_SERVICE_RUNNING"), "HACK_SHIELD", MB_OK);
					break;
				}
				case HS_ERR_INVALID_LICENSE:
				{
					MessageBox(NULL, MA_T("INVALID_LICENSE"), "HACK_SHIELD", MB_OK);
					break;
				}
				case HS_ERR_INVALID_FILES:
				{
					MessageBox(NULL, MA_T("INVALID_FILES"), "HACK_SHIELD", MB_OK);
					break;
				}
				case HS_ERR_DEBUGGER_DETECT:
				{
					MessageBox(NULL, MA_T("DEBUG_DETECT"), "HACK_SHIELD", MB_OK);
					break;
				}
				case HS_ERR_NEED_ADMIN_RIGHTS:
				{
					MessageBox(NULL, MA_T("NEED_ADMIN_RIGHTS"), "HACK_SHIELD", MB_OK);
					break;
				}
				case HS_ERR_COMPATIBILITY_MODE_RUNNING:
				{
					MessageBox(NULL, MA_T("COMPATIBILITY_MODE_RUNNING"), "HACK_SHIELD", MB_OK);
					break;
				}
				default:
				{
					MA_TCHAR szMsg[255];
					MA_StringFormat(szMsg, MA_ARRAYCOUNT(szMsg), MA_T("UNKNOWN_ERROR(errorCode=%x)"), nRet);
					MessageBox(NULL, szMsg, "HACK_SHIELD", MB_OK);
					break;
				}
			}
			return false;
		}
	}

	gs_dwMainThreadID = GetCurrentThreadId();

	{
		int nRet = _AhnHS_StartService();
		assert(nRet != HS_ERR_NOT_INITIALIZED);
		assert(nRet != HS_ERR_ALREADY_SERVICE_RUNNING);

		if (nRet != HS_ERR_OK)
			{
			MA_TCHAR szMsg[255];
			MA_StringFormat(szMsg, MA_ARRAYCOUNT(szMsg), MA_T("START_SERVICE_ERROR(errorCode=%x)"), nRet);
			MessageBox(NULL, szMsg, "HACK_SHIELD", MB_OK);
			return false;
		}
	}
	return true;
}

bool HackShield_Shutdown()
{
	if (gs_dwMainThreadID)
	{
		gs_dwMainThreadID = 0;
		_AhnHS_StopService();
	}

	int nRet = _AhnHS_Uninitialize();
	if (nRet != HS_ERR_OK)
	{
		MA_TCHAR szMsg[255];
		MA_StringFormat(szMsg, MA_ARRAYCOUNT(szMsg), MA_T("UNINITIALIZE_ERROR(errorCode=%x)"), nRet);
		MessageBox(NULL, szMsg, "HACK_SHIELD", MB_OK);
	}

	if (_AhnHsUserUtil_CheckHSShadowAccount())
	{
		 _AhnHsUserUtil_DeleteUser();
	}

	if (gs_isHackDetected)
	{
		MessageBox(NULL, gs_szHackMsg, "HACK_SHIELD", MB_OK);
	}
	return true;
}

#define PREFIX_LOCALE		"locale/"

void HackShield_SetUserInfo(const char* szAccountName)
{
	MA_TCHAR szUserInfo[256];

	const char* szLocaleName = LocaleService_IsYMIR() ? MA_T("unknown") : LocaleService_GetLocalePath();
	if( strncmp( szLocaleName, PREFIX_LOCALE, strlen(PREFIX_LOCALE) ) == 0 )
		szLocaleName += strlen(PREFIX_LOCALE);
	MA_StringFormat(szUserInfo, MA_ARRAYCOUNT(szUserInfo), MA_T("%s / %s"), szAccountName, szLocaleName );
	_AhnHS_SetUserId(szUserInfo);
}

#endif /* USE_AHNLAB_HACKSHIELD */
