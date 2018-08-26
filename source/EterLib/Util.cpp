#include "StdAfx.h"
#include "../eterPack/EterPackManager.h"

#include "TextFileLoader.h"

void PrintfTabs(FILE * File, int iTabCount, const char * c_szString, ...)
{
	va_list args;
	va_start(args, c_szString);

	static char szBuf[1024];
	_vsnprintf(szBuf, sizeof(szBuf), c_szString, args);
	va_end(args);

	for (int i = 0; i < iTabCount; ++i)
		fprintf(File, "    ");

	fprintf(File, szBuf);
}

bool LoadTextData(const char * c_szFileName, CTokenMap & rstTokenMap)
{
	LPCVOID pMotionData;
	CMappedFile File;

	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pMotionData))
		return false;

	CMemoryTextFileLoader textFileLoader;
	CTokenVector stTokenVector;

	textFileLoader.Bind(File.Size(), pMotionData);

	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &stTokenVector))
			continue;

		if (2 != stTokenVector.size())
			return false;

		stl_lowers(stTokenVector[0]);
		stl_lowers(stTokenVector[1]);

		rstTokenMap[stTokenVector[0]] = stTokenVector[1];
	}

	return true;
}

bool LoadMultipleTextData(const char * c_szFileName, CTokenVectorMap & rstTokenVectorMap)
{
	LPCVOID pModelData;
	CMappedFile File;

	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pModelData))
		return false;

	DWORD i;

	CMemoryTextFileLoader textFileLoader;
	CTokenVector stTokenVector;

	textFileLoader.Bind(File.Size(), pModelData);

	for (i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &stTokenVector))
			continue;

		stl_lowers(stTokenVector[0]);

		// Start or End
		if (0 == stTokenVector[0].compare("start"))
		{
			CTokenVector stSubTokenVector;

			stl_lowers(stTokenVector[1]);
			std::string key = stTokenVector[1];
			stTokenVector.clear();

			for (i=i+1; i < textFileLoader.GetLineCount(); ++i)
			{
				if (!textFileLoader.SplitLine(i, &stSubTokenVector))
					continue;

				stl_lowers(stSubTokenVector[0]);

				if (0 == stSubTokenVector[0].compare("end"))
				{
					break;
				}

				for (DWORD j = 0; j < stSubTokenVector.size(); ++j)
				{
					stTokenVector.push_back(stSubTokenVector[j]);
				}
			}

			rstTokenVectorMap.insert(CTokenVectorMap::value_type(key, stTokenVector));
		}
		else
		{
			std::string key = stTokenVector[0];
			stTokenVector.erase(stTokenVector.begin());
			rstTokenVectorMap.insert(CTokenVectorMap::value_type(key, stTokenVector));
		}
	}

	return true;
}

D3DXVECTOR3 TokenToVector(CTokenVector & rVector)
{
	if (3 != rVector.size())
	{
		assert(!"Size of token vector which will be converted to vector is not 3");
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return D3DXVECTOR3(atof(rVector[0].c_str()),
						atof(rVector[1].c_str()),
						atof(rVector[2].c_str()));
}

D3DXCOLOR TokenToColor(CTokenVector & rVector)
{
	if (4 != rVector.size())
	{
		assert(!"Size of token vector which will be converted to color is not 4");
		return D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	return D3DXCOLOR(atof(rVector[0].c_str()),
						atof(rVector[1].c_str()),
						atof(rVector[2].c_str()),
						atof(rVector[3].c_str()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

static std::string	gs_fontFace="";
static DWORD		gs_codePage=0;

int CALLBACK EnumFontFamExProc(CONST LOGFONT* plogFont, CONST TEXTMETRIC* /*textMetric*/, DWORD /*dwWord*/, LPARAM lParam)
{
	return stricmp((const char*)lParam, plogFont->lfFaceName);
}

int GetCharsetFromCodePage(WORD codePage)
{
	switch( codePage )
	{
	case CP_932:
		return SHIFTJIS_CHARSET;
	case CP_949:
		return HANGUL_CHARSET;
	case CP_936:
		return GB2312_CHARSET;
	case CP_950:
		return CHINESEBIG5_CHARSET;
	case CP_1253:
		return GREEK_CHARSET;
	case CP_1254:
		return TURKISH_CHARSET;
	case CP_1255:
		return HEBREW_CHARSET;
	case CP_1256:
		return ARABIC_CHARSET;
	case CP_1257:
		return BALTIC_CHARSET;
	case CP_1258:
		return VIETNAMESE_CHARSET;
	case CP_874:
		return THAI_CHARSET;
	case CP_1250:
		return EASTEUROPE_CHARSET;
	case CP_1251:
		return RUSSIAN_CHARSET;
	default:
		return DEFAULT_CHARSET;
	}
}


const char* GetFontFaceFromCodePageNT(WORD codePage)
{
	switch( codePage )
	{
	case CP_932:
		return "MS PGothic";
	case CP_949:
		return "GulimChe";
	case CP_936:
		return "SimSun";
	case CP_950:
		return "MingLiU";
	case CP_874:
		return "Tahoma";
	case CP_1252:
		return "Arial";
	case CP_1256:
		return "Tahoma"; 
	case CP_1258:
		return "Tahoma"; 
	case CP_65001:
		return "Arial";
	default:
		return "Arial";
	}
}
const char* GetFontFaceFromCodePage9x(WORD codePage)
{
	switch( codePage )
	{
	case CP_932:
		return "굃굍 굊긕긘긞긏";
	case CP_949:
		return "굴림체";
	case CP_936:
		return "芥竟";
	case CP_950:
		return "꾄ⁿ톱";
	case CP_874:
		return "Tahoma";
	case CP_1252:
		return "Arial";
	case CP_1256:
		return "Tahoma"; 
	case CP_1258:
		return "Tahoma"; 
	case CP_65001:
		return "Arial"; 
	default:
		return "Arial";
	}
}

DWORD GetDefaultCodePage()
{
	return gs_codePage;
}

const char * GetDefaultFontFace()
{
	return gs_fontFace.c_str();
}

const char*	GetFontFaceFromCodePage(WORD codePage)
{
	LOGFONT logFont;

	memset(&logFont, 0, sizeof(logFont));

	logFont.lfCharSet = GetCharsetFromCodePage(codePage);

	const char* fontFace = GetFontFaceFromCodePage9x(codePage);

	HDC hDC=GetDC(NULL);

	if(EnumFontFamiliesEx(hDC, &logFont, (FONTENUMPROC)EnumFontFamExProc, (LONG)fontFace, 0) == 0)
	{
		ReleaseDC(NULL, hDC);
		return fontFace;
	}

	fontFace = GetFontFaceFromCodePageNT(codePage);

	if(EnumFontFamiliesEx(hDC, &logFont, (FONTENUMPROC)EnumFontFamExProc, (LONG)fontFace, 0) == 0)
	{
		ReleaseDC(NULL, hDC);
		return fontFace;
	}

	ReleaseDC(NULL, hDC);

	return GetDefaultFontFace();
}

void SetDefaultFontFace(const char* fontFace)
{
	gs_fontFace=fontFace;
}

bool SetDefaultCodePage(DWORD codePage)
{
	gs_codePage=codePage;

	std::string fontFace=GetFontFaceFromCodePage(codePage);
	if (fontFace.empty())
		return false;

	SetDefaultFontFace(fontFace.c_str());

	return true;
}


int __base64_get( const int c )
{
	if( 'A' <= c && c <= 'Z' )
		return c-'A';
	if( 'a' <= c && c <= 'z' )
		return c - 'a' + 26;
	if( '0' <= c && c <= '9' )
		return c - '0' + 52;
	if( c == '+' )
		return 62;
	if( c == '/' )
		return 63;
	if( c == '=' )	// end of line
		return -1;
	return -2;	// non value;
}

void __strcat1(char * str,int i)
{
	char result[2];
	result[0] = i;
	result[1] = NULL;
	strcat(str,result);
}

void base64_decode(const char * str,char * resultStr)
{
	int nCount=0, i=0, r, result;
	int length = strlen(str);
	char szDest[5]="";

	strcpy(resultStr,"");
	while(nCount < length)
	{
		i=0;
		strcpy(szDest, "");
		while(nCount<length && i<4)	// 4개의 바이트를 얻는다.
		{			
			r = str[nCount++];
			result = __base64_get(r);
			if(result!=-2)
			{
				if(result!=-1)
					szDest[i++] = result;
				else szDest[i++] = '@';	// It's end  (64번은 디코딩시 사용되지 않기 때문)
			}
		}

		if(i==4)	// 4개의 소스를 모두 얻어냈다. 디코드 시작
		{	
			if( nCount+3 >= length )	// 데이터의 끝에 도달했다.
			{
				if( szDest[1] == '@' )
				{
					__strcat1(resultStr,(szDest[0]<<2));		
					break;
				}// exit while loop
				else 
					__strcat1(resultStr,(szDest[0]<<2 | szDest[1]>>4));	// 1 Byte
				if( szDest[2] == '@' )
				{
					__strcat1(resultStr,(szDest[1]<<4));
					break;
				}
				else 
					__strcat1(resultStr,(szDest[1]<<4 | szDest[2]>>2));	// 2 Byte				
				if( szDest[3] == '@' )
				{
					__strcat1(resultStr,(szDest[2]<<6));
					break;
				}
				else 
					__strcat1(resultStr,(szDest[2]<<6 | szDest[3]));	// 3 Byte				
			}
			else
			{
				__strcat1(resultStr,(szDest[0]<<2 | szDest[1]>>4));	// 1 Byte
				__strcat1(resultStr,(szDest[1]<<4 | szDest[2]>>2));	// 2 Byte				
				__strcat1(resultStr,(szDest[2]<<6 | szDest[3]));	// 3 Byte
			}
		}
		
	}// end of while

	for (i = 0; i < strlen(resultStr); i++) 
	{
		char c = resultStr[i];
		int xor = i + 5;
		resultStr[i] = char(c ^ xor);
	}	
	// E
}
