#ifndef _EL_CPOSTIT_H_
#define _EL_CPOSTIT_H_

// _CPostItMemoryBlock is defined in CPostIt.cpp
class _CPostItMemoryBlock;

/**
 * @class	CPostIt
 * @brief	게임런처에서 게임 클라이언트로 정보를 전달 및 클라이언트에서 수신하기 위하여 사용되는 클래스
 */
class CPostIt
{
public:
	/**
	 * @brief	CPostIt constructor
	 * @param [in]	szAppName	: 게임의 이름이 들어간다.
	 */
	explicit CPostIt( LPCSTR szAppName );

	/**
	 * @brief	CPostIt destructor
	 */
	~CPostIt( void );

	/**
	 * @brief	CPostIt class에서 보유하고 있는 데이타를 클립보드에 저장한다.
	 */
	BOOL	Flush( void );

	/**
	 * @brief	CPostIt class에서 보유하고 있는 데이타 및 클립보드에 있는 내용을 지운다.
	 */
	void	Empty( void );

	/**
	 * @brief	데이타를 읽어온다.
	 * @param [in]	lpszKeyName	: 불러올 데이타의 키. "KEY" 식의 내용을 넣는다.
	 * @param [in]	lpszData	: 불러올 데이타의 버퍼
	 * @param [in]	nSize		: lpszData 버퍼의 최대사이즈
	 */
	BOOL	Get( LPCSTR lpszKeyName, LPSTR lpszData, DWORD nSize );

	/**
	 * @brief	저장할 데이타를 넣는다.
	 * @param [in]	lpBuffer	: 저장할 데이타. "KEY=DATA" 식의 내용을 넣는다.
	 */
	BOOL	Set( LPCSTR lpszData );

	/**
	 * @brief	저장할 데이타를 넣는다.
	 * @param [in]	lpszKeyName	: 저장할 데이타의 키. "KEY" 식의 내용을 넣는다.
	 * @param [in]	lpszData	: 저장할 데이타. "DATA" 식의 내용을 넣는다.
	 */
	BOOL	Set( LPCSTR lpszKeyName, LPCSTR lpszData );

	/**
	 * @brief	저장할 데이타(DWORD)를 넣는다.
	 * @param [in]	lpBuffer	: 저장할 데이타. "KEY=DATA" 식의 데이타를 넣는다.
	 * @param [in]	dwValue		: 저장할 데이타. (DWORD)
	 */
	BOOL	Set( LPCSTR lpszKeyName, DWORD dwValue );

	/**
	 * @brief	CPostIt class를 복사한다. (클래스 constructor에 이름 인자가 있기 때문에, 새 이름을 지정해야함)
	 * @param [in]	pPostIt		: Destination class
	 * @param [in]	lpszKeyName	: Destination class's new app-name
	 */
	BOOL	CopyTo( CPostIt *pPostIt, LPCSTR lpszKeyName );

protected:
	BOOL					Init( LPCSTR szAppName );
	void					Destroy( void );

protected:
	BOOL					m_bModified;
	CHAR					m_szClipFormatName[_MAX_PATH];
	_CPostItMemoryBlock*	m_pMemoryBlock;
};

#endif /* _EL_CPOSTIT_H_ */
