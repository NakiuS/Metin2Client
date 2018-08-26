#pragma once

#include <windows.h>
#include <boost/unordered_map.hpp>
#include "../eterBase/Singleton.h"
#include "../eterBase/Stl.h"

#include "EterPack.h"

class CEterPackManager : public CSingleton<CEterPackManager>
{
	public:
		struct SCache
		{
			BYTE* m_abBufData;
			DWORD m_dwBufSize;
		};
	public:
		enum ESearchModes
		{
			SEARCH_FILE_FIRST,
			SEARCH_PACK_FIRST
		};

		typedef std::list<CEterPack*> TEterPackList;
		typedef boost::unordered_map<std::string, CEterPack*, stringhash> TEterPackMap;

	public:
		CEterPackManager();
		virtual ~CEterPackManager();

		void SetCacheMode();
		void SetRelativePathMode();
		
		void LoadStaticCache(const char* c_szFileName);
		
		void SetSearchMode(bool bPackFirst);
		int GetSearchMode();

		//THEMIDA
		bool Get(CMappedFile & rMappedFile, const char * c_szFileName, LPCVOID * pData);
		
		//THEMIDA
		bool GetFromPack(CMappedFile & rMappedFile, const char * c_szFileName, LPCVOID * pData);
		
		//THEMIDA
		bool GetFromFile(CMappedFile & rMappedFile, const char * c_szFileName, LPCVOID * pData);
		bool isExist(const char * c_szFileName);
		bool isExistInPack(const char * c_szFileName);

		bool RegisterPack(const char * c_szName, const char * c_szDirectory, const BYTE* c_pbIV = NULL);		
		void RegisterRootPack(const char * c_szName);
		bool RegisterPackWhenPackMaking(const char * c_szName, const char * c_szDirectory, CEterPack* pPack);		


		bool DecryptPackIV(DWORD key);

		const char * GetRootPackFileName();

		//for hybridcrypt
		//THEMIDA
		void WriteHybridCryptPackInfo(const char* pFileName);

		//THEMIDA
		void RetrieveHybridCryptPackKeys( const BYTE* pStream );
		//THEMIDA
		void RetrieveHybridCryptPackSDB( const BYTE* pStream );

		// 메모리에 매핑된 팩들 가운데, 정리해야할 것들 정리.
	public:
		void ArrangeMemoryMappedPack();

	protected:
		int ConvertFileName(const char * c_szFileName, std::string & rstrFileName); // StringPath std::string 버전
		bool CompareName(const char * c_szDirectoryName, DWORD iLength, const char * c_szFileName);

		CEterPack* FindPack(const char* c_szPathName);

		SCache* __FindCache(DWORD dwFileNameHash);
		void	__ClearCacheMap();

	protected:
		bool					m_bTryRelativePath;
		bool					m_isCacheMode;
		int						m_iSearchMode;

		CEterFileDict			m_FileDict;
		CEterPack				m_RootPack;
		TEterPackList			m_PackList;
		TEterPackMap			m_PackMap;
		TEterPackMap			m_DirPackMap;		

		boost::unordered_map<DWORD, SCache> m_kMap_dwNameKey_kCache;

		CRITICAL_SECTION		m_csFinder;
};
