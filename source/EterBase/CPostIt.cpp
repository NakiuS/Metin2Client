#include "StdAfx.h"
#include "CPostIt.h"
#include "../eterBase/utils.h"

class _CPostItMemoryBlock
{
public:
	_CPostItMemoryBlock( void );
	~_CPostItMemoryBlock( void );

	BOOL Assign( HANDLE hBlock );
	HANDLE CreateHandle( void );
	BOOL DestroyHandle( void );

	LPSTR Find( LPCSTR lpszKeyName );
	BOOL Put( LPCSTR lpBuffer );
	BOOL Put( LPCSTR lpszKeyName, LPCSTR lpBuffer );
	BOOL Get( LPCSTR lpszKeyName, LPSTR lpBuffer, DWORD nSize );

protected:
	typedef std::list<CHAR *>	StrList;
	typedef StrList::iterator	StrListItr;

	HANDLE	m_hHandle;
	StrList m_StrList;
};

CPostIt::CPostIt( LPCSTR szAppName ) : m_pMemoryBlock(NULL), m_bModified(FALSE)
{
	Init( szAppName );
}

CPostIt::~CPostIt( void )
{
	Destroy();
}

BOOL CPostIt::Init( LPCSTR szAppName )
{
	if( szAppName == NULL || !*szAppName ) {
		strcpy( m_szClipFormatName, "YMCF" );
	} else {
		strcpy( m_szClipFormatName, "YMCF_" );
		strcat( m_szClipFormatName, szAppName );
	}
	return TRUE;
}

BOOL CPostIt::CopyTo( CPostIt *pPostIt, LPCSTR lpszKeyName )
{
	if( m_pMemoryBlock == NULL )
		return FALSE;
	LPSTR szText = m_pMemoryBlock->Find( lpszKeyName );
	if( szText == NULL )
		return FALSE;
	return pPostIt->Set( szText );
}

BOOL CPostIt::Flush( void )
{
	if( m_bModified == FALSE )
		return FALSE;
	if( m_pMemoryBlock == NULL )
		return FALSE;
	UINT	uDGPFormat;

	uDGPFormat = ::RegisterClipboardFormatA( m_szClipFormatName );
	if( ::OpenClipboard( NULL ) == FALSE )
		return FALSE;
	if( ::SetClipboardData( uDGPFormat, m_pMemoryBlock->CreateHandle() ) == NULL ) {
//		DWORD dwLastError = ::GetLastError();
		m_pMemoryBlock->DestroyHandle();
		::CloseClipboard();
		m_bModified = FALSE;
		return FALSE;
	}
	::CloseClipboard();
	m_bModified = FALSE;
	return TRUE;
}

void CPostIt::Empty( void )
{
	SAFE_DELETE( m_pMemoryBlock );

	UINT	uDGPFormat;

	uDGPFormat = ::RegisterClipboardFormatA( m_szClipFormatName );
	if( ::OpenClipboard( NULL ) == FALSE )
		return;
	HANDLE hClipboardMemory = ::GetClipboardData( uDGPFormat );
	if( hClipboardMemory ) {
//		::GlobalFree( hClipboardMemory );
		::SetClipboardData( uDGPFormat, NULL );
	}
	::CloseClipboard();

	m_bModified = FALSE;
}

void CPostIt::Destroy( void )
{
	Flush();
	SAFE_DELETE( m_pMemoryBlock );
}

BOOL CPostIt::Set( LPCSTR lpszKeyName, LPCSTR lpBuffer )
{
	if( m_pMemoryBlock == NULL )
		m_pMemoryBlock = new _CPostItMemoryBlock;
	m_pMemoryBlock->Put( lpszKeyName, lpBuffer );
	m_bModified = TRUE;
	return TRUE;
}

BOOL CPostIt::Set( LPCSTR lpszKeyName, DWORD dwValue )
{
	CHAR szValue[12];

	_snprintf( szValue, 12, "%d", dwValue );
	return Set( lpszKeyName, szValue );
}

BOOL CPostIt::Set( LPCSTR lpBuffer )
{
	if( lpBuffer == NULL )
		return FALSE;
	if( m_pMemoryBlock == NULL )
		m_pMemoryBlock = new _CPostItMemoryBlock;
	m_pMemoryBlock->Put( lpBuffer );
	m_bModified = TRUE;
	return TRUE;
}

BOOL CPostIt::Get( LPCSTR lpszKeyName, LPSTR lpBuffer, DWORD nSize )
{
	if( m_pMemoryBlock == NULL ) {
		UINT	uDGPFormat;

		uDGPFormat = ::RegisterClipboardFormatA( m_szClipFormatName );
		if( ::OpenClipboard( NULL ) == FALSE )
			return FALSE;

		HANDLE hClipboardMemory = ::GetClipboardData( uDGPFormat );

		if( hClipboardMemory == NULL ) {
			::CloseClipboard();
			return FALSE;
		}
		m_pMemoryBlock = new _CPostItMemoryBlock;
		m_pMemoryBlock->Assign( hClipboardMemory );

		::CloseClipboard();
	}

	return m_pMemoryBlock->Get( lpszKeyName, lpBuffer, nSize );
}

_CPostItMemoryBlock::_CPostItMemoryBlock( void ) : m_hHandle( NULL )
{
}

_CPostItMemoryBlock::~_CPostItMemoryBlock( void )
{
	for( StrListItr itr = m_StrList.begin(); itr != m_StrList.end(); ) {
		LPSTR	lpszText = *itr;
		SAFE_DELETE_ARRAY( lpszText );
		itr = m_StrList.erase( itr );
	}
}

BOOL _CPostItMemoryBlock::Assign( HANDLE hBlock )
{
	if( hBlock == NULL || hBlock == INVALID_HANDLE_VALUE )
		return FALSE;
	LPBYTE lpBuffer = (LPBYTE) ::GlobalLock( hBlock );

	if( lpBuffer == NULL )
		return FALSE;
	DWORD dwCount = *((LPDWORD) lpBuffer);				lpBuffer += sizeof( DWORD );
	for( DWORD dwI=0; dwI < dwCount; dwI++ ) {
		WORD	wLen = *((LPWORD) lpBuffer);			lpBuffer += sizeof( WORD );

		LPSTR	lpszText = new CHAR[ wLen + 1 ];
		::CopyMemory( lpszText, lpBuffer, wLen );
		lpszText[ wLen ] = '\0';

		lpBuffer += wLen;

		Put( lpszText );
	}
	::GlobalUnlock( hBlock ); 
	return TRUE;
}

HANDLE _CPostItMemoryBlock::CreateHandle( void )
{
	if( m_StrList.size() == 0 )
		return INVALID_HANDLE_VALUE;
	DWORD dwBlockSize = sizeof( DWORD );
	StrListItr itr;

	// Calculation for Memory Block Size
	for( itr = m_StrList.begin(); itr != m_StrList.end(); ++itr ) {
		dwBlockSize += sizeof( WORD );
		dwBlockSize += (DWORD) strlen( *itr );
	}

	HANDLE hBlock = ::GlobalAlloc( GMEM_ZEROINIT | GMEM_MOVEABLE, dwBlockSize );
	if( hBlock == NULL )
		return INVALID_HANDLE_VALUE;
	LPBYTE lpBuffer = (LPBYTE) ::GlobalLock( hBlock );
	if( lpBuffer == NULL ) {
		::GlobalFree( hBlock );
		return INVALID_HANDLE_VALUE;
	}
	*((LPDWORD) lpBuffer) = (DWORD) m_StrList.size();	lpBuffer += sizeof( DWORD );
	for( itr = m_StrList.begin(); itr != m_StrList.end(); ++itr ) {
		*((LPWORD) lpBuffer) = (WORD) strlen( *itr );	lpBuffer += sizeof( WORD );
		::CopyMemory( lpBuffer, *itr, strlen( *itr ) );	lpBuffer += strlen( *itr );
	}
	::GlobalUnlock( hBlock );

	m_hHandle = hBlock;
	return hBlock;
}

BOOL _CPostItMemoryBlock::DestroyHandle( void )
{
	::GlobalFree( m_hHandle );
	m_hHandle = NULL;
	return TRUE;
}

LPSTR _CPostItMemoryBlock::Find( LPCSTR lpszKeyName )
{
	for( StrListItr itr = m_StrList.begin(); itr != m_StrList.end(); ++itr ) {
		LPSTR	lpszText = *itr;

		if( _strnicmp( lpszText, lpszKeyName, strlen( lpszKeyName ) ) != 0 )
			continue;
		if( *(lpszText + strlen( lpszKeyName )) != '=' )
			continue;
		return lpszText;
	}
	return NULL;
}

BOOL _CPostItMemoryBlock::Put( LPCSTR lpszKeyName, LPCSTR lpBuffer )
{
	LPSTR	lpszText;

	if( (lpszText = Find( lpszKeyName )) != NULL ) {
		for( StrListItr itr = m_StrList.begin(); itr != m_StrList.end(); ++itr ) {
			if( lpszText == *itr ) {
				SAFE_DELETE_ARRAY( lpszText );
				m_StrList.erase( itr );
				break;
			}
		}
	}
	if( lpBuffer == NULL || !*lpBuffer )
		return TRUE;
	size_t nStrLen	= strlen( lpszKeyName ) + 1 /* '=' */ + strlen( lpBuffer );
	lpszText = new CHAR[ nStrLen + 1 ];
	::CopyMemory( lpszText, lpszKeyName, strlen( lpszKeyName ) );
	*(lpszText + strlen( lpszKeyName )) = '=';
	::CopyMemory( lpszText + strlen( lpszKeyName ) + 1, lpBuffer, strlen( lpBuffer ) );
	*(lpszText + nStrLen) = '\0';

	m_StrList.push_back( lpszText );
	return TRUE;
}

BOOL _CPostItMemoryBlock::Put( LPCSTR lpBuffer )
{
	LPSTR	lpszText;

	if( lpBuffer == NULL || !*lpBuffer )
		return TRUE;
	size_t nStrLen	= strlen( lpBuffer );
	lpszText = new CHAR[ nStrLen + 1 ];
	::CopyMemory( lpszText, lpBuffer, nStrLen );
	*(lpszText + nStrLen) = '\0';

	m_StrList.push_back( lpszText );
	return TRUE;
}

BOOL _CPostItMemoryBlock::Get( LPCSTR lpszKeyName, LPSTR lpBuffer, DWORD nSize )
{
	LPSTR lpszText = Find( lpszKeyName );
	if( lpszText == NULL )
		return FALSE;
	lpszText += (strlen( lpszKeyName ) + 1);
	::ZeroMemory( lpBuffer, nSize );
	strncpy( lpBuffer, lpszText, (nSize < strlen( lpszText )) ? nSize : strlen( lpszText ) );
	return TRUE;
}
