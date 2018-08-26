#pragma once

#ifdef USE_AHNLAB_HACKSHIELD

#include "HackshieldLicense.h"

#ifndef METIN2HS_INCLUDE
//#define METIN2HS_INCLUDE				hshield-5.4.8.1
#define METIN2HS_INCLUDE				hackshield  //2012³â 3¿ù
#endif

#define METIN2HS_INCLUDE_HSUPCHK		<METIN2HS_INCLUDE/hsupchk.h>
#define METIN2HS_INCLUDE_HSUSERUTIL		<METIN2HS_INCLUDE/hsuserutil.h>
#define METIN2HS_INCLUDE_HSHIELD		<METIN2HS_INCLUDE/hshield.h>
#define METIN2HS_INCLUDE_HSHIELDLIBLINK	<METIN2HS_INCLUDE/hshieldLibLink.h>
#define METIN2HS_INCLUDE_ANTICPXSVR		<METIN2HS_INCLUDE/AntiCpXSvr.h>

bool HackShield_Init();
bool HackShield_Shutdown();
bool HackShield_PollEvent();
void HackShield_SetUserInfo(const char* szAccountName);

#endif /* USE_AHNLAB_HACKSHIELD */
