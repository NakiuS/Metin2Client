#pragma once

int EL_String_Decode_Vietnamese(const char* multi, int multiLen, wchar_t* wide, int wideLen);
int EL_String_Encode_Vietnamese(const wchar_t* wide, int wideLen, char* multi, int multiLen);