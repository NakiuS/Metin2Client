#ifndef _CRYPTOPPLIBLINK_H_
#define _CRYPTOPPLIBLINK_H_

#ifdef _DLL
#ifndef CRYPTOPP_IMPORTS
#define CRYPTOPP_IMPORTS
#endif
#endif

#if defined(CRYPTOPP_IMPORTS)
#include "dll.h"
#else
	#ifdef _WIN32
		#ifdef _DEBUG
			#pragma comment( lib, "Cryptopp_lib_debug.lib" )
		#else
			#pragma comment( lib, "Cryptopp_lib_release.lib" )
		#endif
	#endif
#endif

#endif /* !_CRYPTOPPLIBLINK_H_ */
