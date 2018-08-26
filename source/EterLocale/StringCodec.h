#pragma once

int Ymir_WideCharToMultiByte(
  UINT CodePage, 
  DWORD dwFlags, 
  LPCWSTR lpWideCharStr, 
  int cchWideChar, 
  LPSTR lpMultiByteStr, 
  int cbMultiByte, 
  LPCSTR lpDefaultChar, 
  LPBOOL lpUsedDefaultChar 
);

int Ymir_MultiByteToWideChar(
  UINT CodePage, 
  DWORD dwFlags, 
  LPCSTR lpMultiByteStr, 
  int cbMultiByte, 
  LPWSTR lpWideCharStr, 
  int cchWideChar 
);