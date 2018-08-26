#include "StdAfx.h"
#include "Locale.h"
#include "PythonApplication.h"
#include "resource.h"
#include "../eterBase/CRC32.h"
#include "../eterpack/EterPackManager.h"
#include "../eterLocale/Japanese.h"
#include <windowsx.h>

const char* LSS_YMIR		= "YMIR";
const char* LSS_JAPAN		= "JAPAN";
const char* LSS_ENGLISH		= "ENGLISH";
const char* LSS_HONGKONG	= "HONGKONG";
const char* LSS_TAIWAN		= "TAIWAN";
const char* LSS_NEWCIBN		= "NEWCIBN";
const char* LSS_EUROPE		= "EUROPE";
const char* LSS_GLOBAL		= "GLOBAL";

static bool IS_CHEONMA = false;

#ifndef LSS_SECURITY_KEY
#define LSS_SECURITY_KEY	"testtesttesttest"
#endif

std::string __SECURITY_KEY_STRING__ = LSS_SECURITY_KEY;

char	MULTI_LOCALE_SERVICE[256]	= "YMIR";
char	MULTI_LOCALE_PATH[256]		= "locale/ymir";
char	MULTI_LOCALE_NAME[256]		= "ymir";
int		MULTI_LOCALE_CODE			= 949;
int		MULTI_LOCALE_REPORT_PORT	= 10000;

void LocaleService_LoadConfig(const char* fileName)
{
	NANOBEGIN
	FILE* fp = fopen(fileName, "rt");

	if (fp)
	{		
		char	line[256];			
		char	name[256];
		int		code;
		int		id;
		if (fgets(line, sizeof(line)-1, fp))
		{
			line[sizeof(line)-1] = '\0';
			sscanf(line, "%d %d %s", &id, &code, name);

			MULTI_LOCALE_REPORT_PORT		= id;
			MULTI_LOCALE_CODE				= code;
			strcpy(MULTI_LOCALE_NAME, name);
			sprintf(MULTI_LOCALE_PATH, "locale/%s", MULTI_LOCALE_NAME);
		}			
		fclose(fp);
	}
	NANOEND
}

unsigned LocaleService_GetLastExp(int level)
{
	static const int GUILD_LEVEL_MAX = 20;

	if (LocaleService_IsCHEONMA())
	{
		static DWORD CHEONMA_GUILDEXP_LIST[GUILD_LEVEL_MAX+1] = 
		{
			0,			// 0
			15000ul,	// 1
			45000ul,	// 2
			90000ul,	// 3
			160000ul,	// 4
			235000ul,	// 5
			325000ul,	// 6
			430000ul,	// 7
			550000ul,	// 8
			685000ul,	// 9
			835000ul,	// 10
			1000000ul,	// 11
			1500000ul,	// 12
			2100000ul,	// 13
			2800000ul,	// 14
			3600000ul,	// 15
			4500000ul,	// 16
			6500000ul,	// 17
			8000000ul,	// 18
			10000000ul,	// 19			
			42000000UL	// 20
		};
		if (level < 0 && level >= GUILD_LEVEL_MAX)
			return 0;
		
		return CHEONMA_GUILDEXP_LIST[level];
	}
	
	static DWORD INTERNATIONAL_GUILDEXP_LIST[GUILD_LEVEL_MAX+1] = 
	{
		0,			// 0
		6000UL,		// 1
		18000UL,	// 2
		36000UL,	// 3
		64000UL,	// 4
		94000UL,	// 5
		130000UL,	// 6
		172000UL,	// 7
		220000UL,	// 8
		274000UL,	// 9
		334000UL,	// 10
		400000UL,	// 11
		600000UL,	// 12
		840000UL,	// 13
		1120000UL,	// 14
		1440000UL,	// 15
		1800000UL,	// 16
		2600000UL,	// 17
		3200000UL,	// 18
		4000000UL,	// 19		
		16800000UL	// 20		
	};

	if (level < 0 && level >= GUILD_LEVEL_MAX)
		return 0;
	
	return INTERNATIONAL_GUILDEXP_LIST[level];	
}

int LocaleService_GetSkillPower(unsigned level)
{
	static const unsigned SKILL_POWER_NUM = 50;

	if (level >= SKILL_POWER_NUM)
		return 0;

	if (LocaleService_IsCHEONMA())
	{
		static unsigned CHEONMA_SKILL_POWERS[SKILL_POWER_NUM]=
		{
			0,  
				5,  7,  9, 11, 13, 
				15, 17, 19, 20, 22, 
				24, 26, 28, 30, 32, 
				34, 36, 38, 40, 50, // master
				52, 55, 58, 61, 63,
				66, 69, 72, 75, 80, // grand_master
				82, 84, 87, 90, 95,
				100,110,120,130,150,// perfect_master
				150,
		};
		return CHEONMA_SKILL_POWERS[level];		
	}
	
	// 0 5 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 50 52 54 56 58 60 63 66 69 72 82 85 88 91 94 98 102 106 110 115 125 125 125 125 125
	static unsigned INTERNATIONAL_SKILL_POWERS[SKILL_POWER_NUM]=
	{
		0, 
			5,  6,  8, 10, 12, 
			14, 16, 18, 20, 22, 
			24, 26, 28, 30, 32, 
			34, 36, 38, 40, 50, // master
			52, 54, 56, 58, 60, 
			63, 66, 69, 72, 82, // grand_master
			85, 88, 91, 94, 98, 
			102,106,110,115,125,// perfect_master
			125,	
	};
	return INTERNATIONAL_SKILL_POWERS[level];
}

const char*	LocaleService_GetSecurityKey()
{
	return __SECURITY_KEY_STRING__.c_str();
}

// CHEONMA
void LocaleService_SetCHEONMA(bool isEnable)
{
	IS_CHEONMA = isEnable;
}

bool LocaleService_IsCHEONMA()
{
	return LocaleService_IsYMIR();
}

// END_OF_CHEONMA

#if defined(LOCALE_SERVICE_EUROPE) || defined(LOCALE_SERVICE_BRAZIL) || defined(LOCALE_SERVICE_CANADA) || defined(LOCALE_SERVICE_SINGAPORE) || defined(LOCALE_SERVICE_VIETNAM) || defined(LOCALE_SERVICE_TAIWAN) || defined(LOCALE_SERVICE_NEWCIBN)
#define _LSS_USE_LOCALE_CFG			1
#define _LSS_SERVICE_NAME			LSS_EUROPE
#elif defined(LOCALE_SERVICE_ITALY)
#define _LSS_SERVICE_NAME			LSS_ITALY
#define _LSS_SERVICE_CODEPAGE		CP_LATIN
#define _LSS_SERVICE_LOCALE_NAME	"it"
#define _LSS_SERVICE_LOCALE_PATH	"locale/it"
#elif defined(LOCALE_SERVICE_ENGLISH)
#define _LSS_SERVICE_NAME			LSS_ENGLISH
#define _LSS_SERVICE_CODEPAGE		CP_LATIN
#define _LSS_SERVICE_LOCALE_NAME	"english"
#define _LSS_SERVICE_LOCALE_PATH	"locale/english"
#elif defined(LOCALE_SERVICE_JAPAN)
#define _LSS_SERVICE_NAME			LSS_JAPAN
#define _LSS_SERVICE_CODEPAGE		CP_JAPANESE
#define _LSS_SERVICE_LOCALE_NAME	"japan"
#define _LSS_SERVICE_LOCALE_PATH	"locale/japan"
#elif defined(LOCALE_SERVICE_YMIR)
#define _LSS_SERVICE_NAME			LSS_YMIR
#define _LSS_SERVICE_CODEPAGE		CP_HANGUL
#define _LSS_SERVICE_LOCALE_NAME	"ymir"
#define _LSS_SERVICE_LOCALE_PATH	"locale/ymir"
#elif defined(LOCALE_SERVICE_HONGKONG)
#define _LSS_SERVICE_NAME			LSS_HONGKONG
#define _LSS_SERVICE_CODEPAGE		CP_CHINESE_TRAD
#define _LSS_SERVICE_LOCALE_NAME	"hongkong"
#define _LSS_SERVICE_LOCALE_PATH	"locale/hongkong"
#elif defined(LOCALE_SERVICE_TAIWAN)
#define _LSS_SERVICE_NAME			LSS_TAIWAN
#define _LSS_SERVICE_CODEPAGE		CP_CHINESE_TRAD
#define _LSS_SERVICE_LOCALE_NAME	"taiwan"
#define _LSS_SERVICE_LOCALE_PATH	"locale/taiwan"
#elif defined(LOCALE_SERVICE_NEWCIBN)
#define _LSS_SERVICE_NAME			LSS_NEWCIBN
#define _LSS_SERVICE_CODEPAGE		CP_CHINESE_SIMPLE
#define _LSS_SERVICE_LOCALE_NAME	"newcibn"
#define _LSS_SERVICE_LOCALE_PATH	"locale/newcibn"
#endif

#if defined(_LSS_USE_LOCALE_CFG)
#if defined(_LSS_SERVICE_NAME)
const char* LocaleService_GetName()				{ return _LSS_SERVICE_NAME;}
#else
const char* LocaleService_GetName()				{ return MULTI_LOCALE_SERVICE; }
#endif
unsigned int LocaleService_GetCodePage()		{ return MULTI_LOCALE_CODE; }
const char*	LocaleService_GetLocaleName()		{ return MULTI_LOCALE_NAME; }
const char*	LocaleService_GetLocalePath()		{ return MULTI_LOCALE_PATH; }
#elif defined(_LSS_SERVICE_NAME)
const char* LocaleService_GetName()				{ return _LSS_SERVICE_NAME;}
unsigned int LocaleService_GetCodePage()		{ return _LSS_SERVICE_CODEPAGE; }
const char*	LocaleService_GetLocaleName()		{ return _LSS_SERVICE_LOCALE_NAME; }
const char*	LocaleService_GetLocalePath()		{ return _LSS_SERVICE_LOCALE_PATH; }
#endif

void LocaleService_ForceSetLocale(const char* name, const char* localePath)
{
	strcpy(MULTI_LOCALE_NAME, name);
	strcpy(MULTI_LOCALE_PATH, localePath);

	// 기존 천마 서버로 접속시에는 security key 변경 (WE 버전 클라로 천마서버 접속하기 위함)
	if (0 == stricmp(name, "ymir"))
		__SECURITY_KEY_STRING__ = "testtesttesttest";
	if (0 == stricmp(name, "we_korea"))
		__SECURITY_KEY_STRING__ = "1234abcd5678efgh";
}

#if defined(LOCALE_SERVICE_GLOBAL)
struct SLOCALEDATA
{
	const char* szServiceName;
	const char* szLocaleName;
	WORD		wCodePage;
	const char*	szSecurityKey;
} gs_stLocaleData[] = {
	{ LSS_YMIR,		"ymir",			949,	"testtesttesttest"	},		// Korea
	{ LSS_EUROPE,	"de",			1252,	"1234abcd5678efgh"	},		// GameForge (Germany)
	{ LSS_EUROPE,	"en",			1252,	"1234abcd5678efgh"	},		// GameForge (United Kingdom)
	{ LSS_EUROPE,	"us",			1252,	"1234abcd5678efgh"	},		// GameForge (USA)
	{ LSS_EUROPE,	"es",			1252,	"1234abcd5678efgh"	},		// GameForge (Spain)
	{ LSS_EUROPE,	"it",			1252,	"1234abcd5678efgh"	},		// GameForge (Italy)
	{ LSS_EUROPE,	"fr",			1252,	"1234abcd5678efgh"	},		// GameForge (France)
	{ LSS_EUROPE,	"pt",			1252,	"1234abcd5678efgh"	},		// GameForge (Portugal)
	{ LSS_EUROPE,	"tr",			1253,	"1234abcd5678efgh"	},		// GameForge (Greece)
	{ LSS_EUROPE,	"pl",			1250,	"1234abcd5678efgh"	},		// GameForge (Poland)
	{ LSS_EUROPE,	"tr",			1254,	"1234abcd5678efgh"	},		// GameForge (Turkey)
	{ LSS_EUROPE,	"dk",			1252,	"1234abcd5678efgh"	},		// GameForge (Demmark)
	{ LSS_EUROPE,	"ae",			1256,	"1234abcd5678efgh"	},		// GameForge (United Arab Emirate)
	{ LSS_EUROPE,	"mx",			1252,	"1234abcd5678efgh"	},		// GameForge (Mexico)
	{ LSS_EUROPE,	"nl",			1252,	"1234abcd5678efgh"	},		// GameForge (Netherlands)
	{ LSS_EUROPE,	"cz",			1252,	"1234abcd5678efgh"	},		// GameForge (Czech Republic)
	{ LSS_EUROPE,	"ru",			1251,	"1234abcd5678efgh"	},		// GameForge (Russian Federation)
	{ LSS_EUROPE,	"hu",			1250,	"1234abcd5678efgh"	},		// GameForge (Hungary)
	{ LSS_EUROPE,	"ro",			1250,	"1234abcd5678efgh"	},		// GameForge (Romania)
	{ LSS_EUROPE,	"ca",			1252,	"testtesttesttest"	},		// Z8Games (Canada)
	{ LSS_EUROPE,	"sg",			1252,	"testtesttesttest"	},		// TEC (Singapore)
	{ LSS_JAPAN,	"japan",		932,	"testtesttesttest"	},		// Japan
	{ LSS_EUROPE,	"br",			1252,	"testtesttesttest"	},		// OnGame (Brazil)
	{ LSS_HONGKONG,	"hongkong",		950,	"testtesttesttest"	},		// HongKong & Taiwan
	{ LSS_NEWCIBN,	"newcibn",		936,	"testtesttesttest"	},		// CIBN (Free world)
	{ LSS_ENGLISH,	"english",		949,	"testtesttesttest"	},		// English (Obsoleted)
	{ LSS_YMIR,		"kr",			949,	"testtesttesttest"	},		// Korea (Obsoleted)
	{ NULL,			NULL,			0,		"testtesttesttest"	}
};

const char* LocaleService_GetName()
{
	return MULTI_LOCALE_SERVICE;
}

unsigned int LocaleService_GetCodePage()
{
	return MULTI_LOCALE_CODE;
}

const char*	LocaleService_GetLocaleName()
{
	return MULTI_LOCALE_NAME;
}

const char*	LocaleService_GetLocalePath()
{
	return MULTI_LOCALE_PATH;
}

static int gs_iLocale = -1;

LRESULT CALLBACK SelectDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg ) {
	case WM_INITDIALOG : {
		char szLocalePath[256], szDisplayName[256];
		for(int i=0; gs_stLocaleData[i].szServiceName; i++ ) {
			sprintf(szLocalePath, "locale/%s/item_proto", gs_stLocaleData[i].szLocaleName);
			if( CEterPackManager::Instance().isExist(szLocalePath)) {
				sprintf(szDisplayName, "%s (%s, %d)", gs_stLocaleData[i].szLocaleName, gs_stLocaleData[i].szServiceName, gs_stLocaleData[i].wCodePage);
				int iIndex = ListBox_AddString(GetDlgItem(hDlg, IDC_LOCALE_LIST), szDisplayName);
				ListBox_SetItemData(GetDlgItem(hDlg, IDC_LOCALE_LIST), iIndex, i);
			}
		}
		return TRUE;
		}
	case WM_COMMAND :
		switch( LOWORD( wParam ) ) {
		case IDC_LOCALE_LIST: {
			int iSelected = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_LOCALE_LIST));
			switch(HIWORD(wParam)) {
			case LBN_SELCHANGE :
				gs_iLocale = ListBox_GetItemData(GetDlgItem(hDlg, IDC_LOCALE_LIST), iSelected);
				break;
			case LBN_DBLCLK :
				gs_iLocale = ListBox_GetItemData(GetDlgItem(hDlg, IDC_LOCALE_LIST), iSelected);
				::EndDialog(hDlg, 0);
				break;
			} 
			break;
			}
		case IDC_START: {
			::EndDialog(hDlg, 0);
			break;
			}
		case IDC_EXIT: {
			gs_iLocale = -1;
			::EndDialog(hDlg, 0);
			break;
			}
		}
		return FALSE;
	}
	return FALSE;
}

bool LocaleService_LoadGlobal(HINSTANCE hInstance)
{
	int nFoundLocales = 0;
	char szLocalePath[256];

	for(int i=0; gs_stLocaleData[i].szServiceName; i++ ) {
		sprintf(szLocalePath, "locale/%s/item_proto", gs_stLocaleData[i].szLocaleName);
		if( CEterPackManager::Instance().isExist(szLocalePath)) {
			nFoundLocales++;
			if(gs_iLocale == -1)
				gs_iLocale = i;
		}
	}
	if (gs_iLocale < 0)
		return false;
	if(nFoundLocales > 1)
		::DialogBox(hInstance, MAKEINTRESOURCE(IDD_SELECT_LOCALE), NULL, (DLGPROC) SelectDlgProc);
	if (gs_iLocale < 0)
		return false;
	strcpy(MULTI_LOCALE_SERVICE, gs_stLocaleData[gs_iLocale].szServiceName);
	strcpy(MULTI_LOCALE_NAME, gs_stLocaleData[gs_iLocale].szLocaleName);
	sprintf(MULTI_LOCALE_PATH, "locale/%s", gs_stLocaleData[gs_iLocale].szLocaleName);
	MULTI_LOCALE_CODE = gs_stLocaleData[gs_iLocale].wCodePage;
	if(gs_stLocaleData[gs_iLocale].szSecurityKey)
		__SECURITY_KEY_STRING__ = gs_stLocaleData[gs_iLocale].szSecurityKey;
	return true;
}
#else
bool LocaleService_LoadGlobal(HINSTANCE hInstance)
{
	return false;
}
#endif

bool LocaleService_IsYMIR()		{ return (stricmp( LocaleService_GetName(), LSS_YMIR ) == 0) || (stricmp( LocaleService_GetLocaleName(), "ymir" ) == 0);	}
bool LocaleService_IsJAPAN()	{ return (stricmp( LocaleService_GetName(), LSS_JAPAN ) == 0) || (stricmp( LocaleService_GetLocaleName(), "japan" ) == 0);	}
bool LocaleService_IsENGLISH()	{ return (stricmp( LocaleService_GetName(), LSS_ENGLISH ) == 0);	}
bool LocaleService_IsEUROPE()	{ return (stricmp( LocaleService_GetName(), LSS_EUROPE ) == 0);		}
bool LocaleService_IsHONGKONG()	{ return (stricmp( LocaleService_GetName(), LSS_HONGKONG ) == 0);	}
bool LocaleService_IsTAIWAN()	{ return (stricmp( LocaleService_GetName(), LSS_TAIWAN ) == 0);		}
bool LocaleService_IsNEWCIBN()	{ return (stricmp( LocaleService_GetName(), LSS_NEWCIBN ) == 0);	}

#if defined(LOCALE_SERVICE_WE_JAPAN)
BOOL LocaleService_IsLeadByte( const char chByte )
{
	return ShiftJIS_IsLeadByte( chByte );
}

int LocaleService_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength )
{
	return ShiftJIS_StringCompareCI( szStringLeft, szStringRight, sizeLength );
}
#else
BOOL LocaleService_IsLeadByte( const char chByte )
{
	return (((unsigned char) chByte) & 0x80) != 0;
}

int LocaleService_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength )
{
	return strnicmp( szStringLeft, szStringRight, sizeLength );
}
#endif


#ifdef USE_OPENID

std::string __OPENID_AUTH_KEY_STRING__;
int openid_test = 0;

void LocaleService_SetOpenIDAuthKey(const char *authKey)
{
	 __OPENID_AUTH_KEY_STRING__ = authKey;

	 return;
}

const char*	LocaleService_GetOpenIDAuthKey()
{
	return __OPENID_AUTH_KEY_STRING__.c_str();
}

#endif /* USE_OPENID */