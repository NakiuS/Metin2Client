#include "stdafx.h"
#include "StringCodec.h"
#include "StringCodec_Vietnamese.h"

int Ymir_WideCharToMultiByte(
  UINT CodePage, 
  DWORD dwFlags, 
  LPCWSTR lpWideCharStr, 
  int cchWideChar, 
  LPSTR lpMultiByteStr, 
  int cbMultiByte, 
  LPCSTR lpDefaultChar, 
  LPBOOL lpUsedDefaultChar 
)
{
	if (CodePage == CP_1258)
	{
		return EL_String_Encode_Vietnamese(lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte);
	}
	else
	{
		return WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
	}
}

int Ymir_MultiByteToWideChar(
  UINT CodePage, 
  DWORD dwFlags, 
  LPCSTR lpMultiByteStr, 
  int cbMultiByte, 
  LPWSTR lpWideCharStr, 
  int cchWideChar 
)
{
	if (CodePage == CP_1258)
	{
		return EL_String_Decode_Vietnamese(lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
	}
	else
	{
		return MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
	}
}