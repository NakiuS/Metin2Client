#ifndef __INC_MILESLIB_STDAFX_H__
#define __INC_MILESLIB_STDAFX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#pragma warning(disable:4100)

#pragma warning(disable:4201)
#include <mss.h>
#pragma warning(default:4201)
#include <windows.h>

//#include <crtdbg.h>

#include "../eterBase/CRC32.h"
#include "../eterBase/Utils.h"
#include "../eterBase/Debug.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// Armadillo nanomite protection
#ifndef NANOBEGIN
	#ifdef __BORLANDC__
		#define NANOBEGIN     __emit__ (0xEB,0x03,0xD6,0xD7,0x01)
		#define NANOEND       __emit__ (0xEB,0x03,0xD6,0xD7,0x00)
	#else
		#define NANOBEGIN     __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD7 __asm _emit 0x01
		#define NANOEND       __asm _emit 0xEB __asm _emit 0x03 __asm _emit 0xD6 __asm _emit 0xD7 __asm _emit 0x00
	#endif
#endif

#endif