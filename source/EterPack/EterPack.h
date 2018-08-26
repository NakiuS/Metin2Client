#ifndef __INC_ETERPACKLIB_ETERPACK_H__
#define __INC_ETERPACKLIB_ETERPACK_H__

#include <list>
#include <boost/unordered_map.hpp>
#include <boost/shared_array.hpp>

#include "../EterBase/MappedFile.h"

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                                                  \
                        ((DWORD)(BYTE) (ch0       ) | ((DWORD)(BYTE) (ch1) <<  8) |     \
                        ((DWORD)(BYTE) (ch2) << 16) | ((DWORD)(BYTE) (ch3) << 24))
#endif


//#define  CHECKSUM_CHECK_MD5

#include "md5.h"

namespace eterpack
{
	const DWORD	c_PackCC = MAKEFOURCC('E', 'P', 'K', 'D');
	const DWORD	c_IndexCC = MAKEFOURCC('E', 'P', 'K', 'D');
	const DWORD c_Version = 2;
								// FourCC + Version + m_indexCount
	const DWORD c_HeaderSize = sizeof(DWORD) + sizeof(DWORD) + sizeof(long);
};

enum EEterPackTypes
{
	DBNAME_MAX_LEN = 255,
	FILENAME_MAX_LEN = 160,
	FREE_INDEX_BLOCK_SIZE = 32768,
	FREE_INDEX_MAX_SIZE = 512,
	DATA_BLOCK_SIZE = 256,

	COMPRESSED_TYPE_NONE = 0,
	COMPRESSED_TYPE_COMPRESS = 1,
	COMPRESSED_TYPE_SECURITY = 2,
	COMPRESSED_TYPE_PANAMA = 3,
	COMPRESSED_TYPE_HYBRIDCRYPT = 4,
	COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB = 5,
	COMPRESSED_TYPE_COUNT = 6,
};

#pragma pack(push, 4)
typedef struct SEterPackIndex
{
	long			id;
	char			filename[FILENAME_MAX_LEN + 1];
	DWORD			filename_crc;
	long			real_data_size;
	long			data_size;
#ifdef CHECKSUM_CHECK_MD5
	BYTE			MD5Digest[16];	
#else
	DWORD			data_crc;
#endif
	long			data_position;
	char			compressed_type;
} TEterPackIndex;
#pragma pack(pop)

typedef boost::unordered_map<DWORD, TEterPackIndex *> TDataPositionMap;
typedef std::list<TEterPackIndex *> TFreeIndexList;


class CEterPack;

class CEterFileDict
{
public:
	struct Item
	{
		Item() : pkPack(NULL), pkInfo(NULL) {}

		CEterPack*			pkPack;
		TEterPackIndex*		pkInfo;
	};

	enum
	{
		BUCKET_SIZE = 16,
	};

	typedef boost::unordered_multimap<DWORD, Item> TDict;

public:
	void InsertItem(CEterPack* pkPack, TEterPackIndex* pkInfo);
	void UpdateItem(CEterPack* pkPack, TEterPackIndex* pkInfo);

	Item* GetItem(DWORD dwFileNameHash, const char* c_pszFileName);

	const TDict& GetDict() const
	{
		return m_dict;
	}

private:
	TDict m_dict;
};

class EterPackPolicy_CSHybridCrypt;

class CEterPack
{
	public:
		CEterPack();
		virtual ~CEterPack();
		
		void				Destroy();
		bool				Create(CEterFileDict& rkFileDict, const char * dbname, const char * pathName, bool bReadOnly = true, const BYTE* iv = NULL);
		bool				DecryptIV(DWORD dwPanamaKey);

		const std::string&	GetPathName();
		const char *		GetDBName();

		//THEMIDA
		bool				Get(CMappedFile & mappedFile, const char * filename, LPCVOID * data);
		//THEMIDA
		bool				Get2(CMappedFile & mappedFile, const char * filename, TEterPackIndex* index, LPCVOID * data);


		//THEMIDA
		bool				Put(const char * filename, const char * sourceFilename, BYTE packType, const std::string& strRelateMapName);
		//THEMIDA
		bool				Put(const char * filename, LPCVOID data, long len, BYTE packType);

		bool				Delete(const char * filename);

		bool				Extract();

		long				GetFragmentSize();

		bool				IsExist(const char * filename);

		TDataPositionMap &	GetIndexMap();

		bool				EncryptIndexFile();
		bool				DecryptIndexFile();

		DWORD				DeleteUnreferencedData();	// 몇개가 삭제 되었는지 리턴 한다.

		bool				GetNames(std::vector<std::string>* retNames);
		
		EterPackPolicy_CSHybridCrypt* GetPackPolicy_HybridCrypt() const;

	private:
		bool				__BuildIndex(CEterFileDict& rkFileDict, bool bOverwirte=false);

		bool				CreateIndexFile();
		TEterPackIndex *	FindIndex(const char * filename);
		long				GetNewIndexPosition(CFileBase& file);
		TEterPackIndex *	NewIndex(CFileBase& file, const char * filename, long size);
		void				WriteIndex(CFileBase& file, TEterPackIndex * index);
		int					GetFreeBlockIndex(long size);
		void				PushFreeIndex(TEterPackIndex * index);

		bool				CreateDataFile();
		long				GetNewDataPosition(CFileBase& file);
		bool				ReadData(CFileBase& file, TEterPackIndex * index, LPVOID data, long maxsize);
		bool				WriteData(CFileBase& file, TEterPackIndex * index, LPCVOID data);
		bool				WriteNewData(CFileBase& file, TEterPackIndex * index, LPCVOID data);
		
		bool				Delete(TEterPackIndex * pIndex);

	protected:
		CMappedFile				m_file;

		char*					m_file_data;
		unsigned				m_file_size;

		long					m_indexCount;
		bool					m_bEncrypted;

		char					m_dbName[DBNAME_MAX_LEN+1];
		char					m_indexFileName[MAX_PATH+1];
		TEterPackIndex *		m_indexData;
		long					m_FragmentSize;
		bool					m_bReadOnly;
		bool					m_bDecrypedIV;

		boost::unordered_map<DWORD, DWORD> m_map_indexRefCount;
		TDataPositionMap		m_DataPositionMap;
		TFreeIndexList			m_FreeIndexList[FREE_INDEX_MAX_SIZE + 1];	// MAX 도 억세스 하므로 + 1 크기만큼 만든다.

		std::string				m_stDataFileName;
		std::string				m_stPathName;

		
		EterPackPolicy_CSHybridCrypt* m_pCSHybridCryptPolicy;

	private:
		void				__CreateFileNameKey_Panama(const char * filename, BYTE * key, unsigned int keySize);
		bool				__Decrypt_Panama(const char* filename, const BYTE* data, SIZE_T dataSize, CLZObject& zObj);
		bool				__Encrypt_Panama(const char* filename, const BYTE* data, SIZE_T dataSize, CLZObject& zObj);
		std::string			m_stIV_Panama;

	//private:
	//	bool				m_bIsDataLoaded;
	//	// 그냥 time_t를 쓰면, 32bit time_t를 사용하는 소스에서는, 
	//	// CEterPack의 size를 실제 size - 4로 인식하기 때문에 문제가 발생할 수 있다.
	//	__time64_t			m_tLastAccessTime;
	//public:
	//	__time64_t			GetLastAccessTime() { return m_tLastAccessTime; }
	//	void				UpdateLastAccessTime();
	//	void				ClearDataMemoryMap();

#ifdef CHECKSUM_CHECK_MD5
		void	GenerateMD5Hash( BYTE* pData, int nLength, IN OUT MD5_CTX& context );
#endif
};

class CMakePackLog
{
	public:
		static CMakePackLog& GetSingleton();

	public:
		CMakePackLog();
		~CMakePackLog();

		void SetFileName(const char* c_szFileName);

		void Writef(const char* c_szFormat, ...);
		void Writenf(const char* c_szFormat, ...);
		void Write(const char* c_szBuf);

		void WriteErrorf(const char* c_szFormat, ...);
		void WriteErrornf(const char* c_szFormat, ...);
		void WriteError(const char* c_szBuf);

		void FlushError();

	private:
		void __Write(const char* c_szBuf, int nBufLen);
		void __WriteError(const char* c_szBuf, int nBufLen);
		bool __IsLogMode();

	private:
		FILE* m_fp;
		FILE* m_fp_err;
		
		std::string m_stFileName;
		std::string m_stErrorFileName;
};

#endif
