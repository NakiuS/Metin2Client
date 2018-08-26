#include "StdAfx.h"
#include "NProtectGameGuard.h"

#ifdef USE_NPROTECT_GAMEGUARD

static struct GameGuard
{
	bool	 isInitError;
	bool	 isProcError;
	unsigned msg;
	unsigned arg;
} gs_gameGuard = {
	false, 0, 0,
};

#ifdef LOCALE_SERVICE_HONGKONG
#include "NProtect/NPGameLibHK.h"
#pragma comment(lib, "NPGameLibHK_MT.lib")
CNPGameLib npgl("Metin2HK");
#endif

#ifdef LOCALE_SERVICE_TAIWAN
#include "NProtect/NPGameLibTW.h"
#pragma comment(lib, "NPGameLibTW_MT.lib")
CNPGameLib npgl("Metin2TW");
#endif

#ifdef LOCALE_SERVICE_EUROPE
#include "NProtect/NPGameLibEU.h"
#pragma comment(lib, "NPGameLibEU_MT.lib")
CNPGameLib npgl("Metin2EU");
#endif

BOOL CALLBACK NPGameMonCallback(DWORD msg, DWORD arg)
{
	switch (msg)
    {
	case NPGAMEMON_COMM_ERROR:
	case NPGAMEMON_COMM_CLOSE:
	case NPGAMEMON_INIT_ERROR:
	case NPGAMEMON_SPEEDHACK:
	case NPGAMEMON_GAMEHACK_KILLED:
	case NPGAMEMON_GAMEHACK_DETECT:
	case NPGAMEMON_GAMEHACK_DOUBT:		
		gs_gameGuard.msg = msg;
		gs_gameGuard.arg = arg;
		gs_gameGuard.isProcError = true;
		return false;		
		break;
	}	
	return true;
}

bool LocaleService_IsGameGuardError()
{
	return gs_gameGuard.isProcError;
}

bool LocaleService_InitGameGuard()
{
	unsigned result = npgl.Init();
	if (NPGAMEMON_SUCCESS != result)
	{
		gs_gameGuard.isInitError = true;
		gs_gameGuard.msg = result;
		return false;
	}
	return true;
}

bool LocaleService_RunGameGuard(HWND hWnd)
{
	npgl.SetHwnd(hWnd);
	return true;
}

void LocaleService_NoticeGameGuardInitError_HongKong()
{
	char msg[256];
	switch (gs_gameGuard.msg)
	{
		case NPGAMEMON_ERROR_EXIST:
			sprintf(msg, "GameGuard執?中。稍等??或是?新啟動之後再執?。 ");
			break;
		case NPGAMEMON_ERROR_GAME_EXIST:
			sprintf(msg, "遊戲?複執?或GameGuard已在執?中。結?遊戲後請?新執?。 ");
			break;
		case NPGAMEMON_ERROR_INIT:
			sprintf(msg, "GameGuard初期化錯?。?新啟動後再執?或是關閉可??發生衝突的?式之後再執?。 ");
			break;
		case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
		case NPGAMEMON_ERROR_NFOUND_GG:
		case NPGAMEMON_ERROR_AUTH_INI:
		case NPGAMEMON_ERROR_NFOUND_INI:
			sprintf(msg, "無GameGuard檔?或產生異變。請安裝GameGuard 的安裝檔。 ");
			break;
		case NPGAMEMON_ERROR_CRYPTOAPI:
			sprintf(msg, "Window的某部份系統檔?受損。請?新安裝IE。 ");
			break;
		case NPGAMEMON_ERROR_EXECUTE:
			sprintf(msg, "GameGuard執?失敗。請?新執?GameGuard安裝檔。 ");
			break;
		case NPGAMEMON_ERROR_ILLEGAL_PRG:
			sprintf(msg, "發?非法?式。請結?不需要的?式之後?新執?。 ");
			break;
		case NPGMUP_ERROR_ABORT:
			sprintf(msg, "GameGuard升?取?。如果?直無法連線，請?調整網路或?人的防火牆?定。 ");
			break;
		case NPGMUP_ERROR_CONNECT:
		case NPGMUP_ERROR_DOWNCFG:
			sprintf(msg, "GameGuard改版(升?)?服連線失敗。稍等之後再?新執?或是如果有?人防火牆的話請調整?人防火牆?定之後再試。 ");
			break;
		case NPGMUP_ERROR_AUTH:
			sprintf(msg, "GameGuard改版沒?完成。請?停止執?防毒?式後再?新執?或是使用PC管理?式調整?定後再執?。 ");
			break;
		case NPGAMEMON_ERROR_NPSCAN:
			sprintf(msg, "病毒及登入駭客入侵工具檢查模組失敗。可?是記憶體不足或是感染了病毒。 ");
			break;
		default:
			sprintf(msg, "GameGuard執??發?錯?。請將遊戲資?夾裡的GameGuard資?夾中的*.erl檔?隨電子郵件??寄到game2@inca.co.kr信箱。 ");
			break;
	}

	MessageBox(NULL, msg, "GameGuard 錯?", MB_OK);	
}

void LocaleService_NoticeGameGuardInitError_International()
{
	char msg[256];
	switch (gs_gameGuard.msg)
	{
	case NPGAMEMON_ERROR_EXIST:
		sprintf(msg, "GameGuard is already running.\nPlease reboot and restart the game.");
		break;
	case NPGAMEMON_ERROR_GAME_EXIST:
		sprintf(msg, "GameGuard is already running.\nPlease restart the game.");
		break;
	case NPGAMEMON_ERROR_INIT:
		sprintf(msg, "GameGuard has initial error.\nPlease kill other conflict programs and restart game.");
		break;
	case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
	case NPGAMEMON_ERROR_NFOUND_GG:
	case NPGAMEMON_ERROR_AUTH_INI:
	case NPGAMEMON_ERROR_NFOUND_INI:
		sprintf(msg, "GameGuard files are modified or deleted.\nPlease reinstall GameGuard.");
		break;
	case NPGAMEMON_ERROR_CRYPTOAPI:
		sprintf(msg, "GameGuard detects Windows system file error.\nPlease reinstall Internet Explorer(IE)");
		break;
	case NPGAMEMON_ERROR_EXECUTE:
		sprintf(msg, "GameGuard running is failed.\nPlease reinstall GameGuard.");
		break;
	case NPGAMEMON_ERROR_ILLEGAL_PRG:
		sprintf(msg, "GameGuard detects Illegal Program.\nPlease kill other programs not needs and restart game");
		break;
	case NPGMUP_ERROR_ABORT:
		sprintf(msg, "GameGuard update was canceled.\nWhen not connect, change the internal or private firewall settings");
		break;
	case NPGMUP_ERROR_CONNECT:
		sprintf(msg, "GameGuard hooking is failed.\nPlease download newer anti-virus and check all system.");
		break;
	case NPGAMEMON_ERROR_GAMEGUARD:
		sprintf(msg, "GameGuard has initial error or old game guard.\nPlease reinstall GameGuard");
		break;
	case NPGMUP_ERROR_PARAM:
		sprintf(msg, "GameGuard detects .ini file is modified.\nPlease reinstall GameGuard");		
		break;
	case NPGMUP_ERROR_INIT:
		sprintf(msg, "GameGuard detects npgmup.des initial error.\nPlease delete GameGuard Folder and reinstall GameGuard");
		break;
	case NPGMUP_ERROR_DOWNCFG:
		sprintf(msg, "GameGuard update server connection is failed.\nPlease restart or check private firewall settings.");
		break;
	case NPGMUP_ERROR_AUTH:
		sprintf(msg, "GameGuard update is not completed.\nPlease pause anti-virus and restart game.");
		break;
	case NPGAMEMON_ERROR_NPSCAN:
		sprintf(msg, "GameGuard virus-hacking checker loading is failed\nPlease check memory lack or virus.");
		break;
	default:
		sprintf(msg, "UnknownErrorCode: %d\nPlease send a letter that has *.erl in game folder to Game1@inca.co.kr", gs_gameGuard.msg);
		break;
	}
	
	MessageBox(NULL, msg, "GameGuard Initiail Error", MB_OK);	
}

void LocaleService_NoticeGameGuardProcError_HongKong()
{
	char msg[256];
	switch (gs_gameGuard.msg)
	{
		case NPGAMEMON_COMM_ERROR:
		case NPGAMEMON_COMM_CLOSE:
			return;
		case NPGAMEMON_INIT_ERROR:
			sprintf(msg, "GameGuard初期化錯? : %lu", gs_gameGuard.arg);
			break;
		case NPGAMEMON_SPEEDHACK:
			sprintf(msg,  "發?speed hack。 ");
			break;
		case NPGAMEMON_GAMEHACK_KILLED:
			sprintf(msg,  "發?遊戲hack。 ");
			break;
		case NPGAMEMON_GAMEHACK_DETECT:
			sprintf(msg,  "發?遊戲hack。 ");
			break;
		case NPGAMEMON_GAMEHACK_DOUBT:
			sprintf(msg,  "遊戲或GameGuard已變?。 ");
			break;
	}
	MessageBox(NULL, msg, "GameGuard Error", MB_OK);
}

void LocaleService_NoticeGameGuardProcError_International()
{
	char msg[256];
	switch (gs_gameGuard.msg)
	{
	case NPGAMEMON_COMM_ERROR:
	case NPGAMEMON_COMM_CLOSE:
		break;
	case NPGAMEMON_INIT_ERROR:
		wsprintf(msg, "GameGuard has initial error : %lu", gs_gameGuard.arg);			
		break;
	case NPGAMEMON_SPEEDHACK:
		wsprintf(msg, "GameGuard detects SpeedHack");			
		break;
	case NPGAMEMON_GAMEHACK_KILLED:
		wsprintf(msg, "GameGuard detects GameHack\r\n%s", npgl.GetInfo());			
		break;
	case NPGAMEMON_GAMEHACK_DETECT:
		wsprintf(msg, "GameGuard detects GameHack\r\n%s", npgl.GetInfo());			
		break;
	case NPGAMEMON_GAMEHACK_DOUBT:
		wsprintf(msg, "Game or Gamguard was modified.");			
		break;
	}
	MessageBox(NULL, msg, "GameGuard Error", MB_OK);
}

void LocaleService_NoticeGameGuardMessasge()
{
	if (gs_gameGuard.isInitError)
	{		
		if (LocaleService_IsHONGKONG())
			LocaleService_NoticeGameGuardInitError_HongKong();
		else
			LocaleService_NoticeGameGuardInitError_International();		
	
	}
	else if (gs_gameGuard.isProcError)
	{
		if (LocaleService_IsHONGKONG())
			LocaleService_NoticeGameGuardProcError_HongKong();
		else
			LocaleService_NoticeGameGuardProcError_International();		
	}
}

#endif /* USE_NPROTECT_GAMEGUARD */
