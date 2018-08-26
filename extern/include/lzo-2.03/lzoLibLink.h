#ifndef _LZOLIBLINK_H_
#define _LZOLIBLINK_H_

#ifndef _RUNTIME_LIBRARY

#if defined( _UNICODE )
#define _RUNTIME_CHARSET				"W"
#else
#define _RUNTIME_CHARSET				"A"
#endif

#if defined( _MT )
	#if defined( _DLL )
		#define _RUNTIME_THREADMODEL	"MD"
	#else
		#define _RUNTIME_THREADMODEL	"MT"
	#endif
#else
	#define _RUNTIME_THREADMODEL		"ST"
#endif

#if defined( _DEBUG ) || defined( DEBUG )
#define _RUNTIME_DEBUG					"d"
#else
#define _RUNTIME_DEBUG					""
#endif

#define _RUNTIME_LIBRARY				_RUNTIME_THREADMODEL _RUNTIME_DEBUG
#define _RUNTIME_CONFIGURATION			_RUNTIME_CHARSET _RUNTIME_THREADMODEL _RUNTIME_DEBUG

#endif

#pragma comment( lib, "lzo-2.03" _RUNTIME_LIBRARY ".lib" )

#endif /* !_LZOLIBLINK_H_ */
