#ifndef __INC_ETERPACKLIB_ETERPACKPOLICY_CSHYBRIDCRYPT_H__
#define __INC_ETERPACKLIB_ETERPACKPOLICY_CSHYBRIDCRYPT_H__

#include <boost/unordered_map.hpp>
#include <cryptopp/cryptlib.h>
#include <cryptopp/camellia.h>
#include <cryptopp/twofish.h>
#include <cryptopp/tea.h>

enum eHybridCipherAlgorithm
{
	e_Cipher_Camellia,
	e_Cipher_Twofish,
	e_Cipher_XTEA,
	Num_Of_Ciphers
};


class CFileBase;
class CLZObject;

//THEMIDA
class EterPackPolicy_CSHybridCrypt
{
public:
	~EterPackPolicy_CSHybridCrypt();

	bool GenerateCryptKey( std::string& rfileName );
	bool EncryptMemory( std::string& rfilename, IN const BYTE* pSrcData, IN int iSrcLen, OUT CLZObject& zObj );
	bool DecryptMemory( std::string& rfilename, IN const BYTE* pSrcData, IN int iSrcLen, OUT CLZObject& zObj );
	bool IsContainingCryptKey() const;

	//Supplementary Data Block (SDB)
	bool GenerateSupplementaryDataBlock(std::string& rfilename, const std::string& strMapName, IN const BYTE* pSrcData, IN int iSrcLen, OUT LPBYTE& pDestData, OUT int& iDestLen );
	bool GetSupplementaryDataBlock( std::string& rfilename, OUT LPBYTE& pSDB, OUT int& iSDBSize  );
	bool IsContainingSDBFile() const;

	// Read/Write IO
	void WriteCryptKeyToFile( CFileBase& rFile );
	int  ReadCryptKeyInfoFromStream( IN const BYTE* pStream );

	void WriteSupplementaryDataBlockToFile( CFileBase& rFile );
	int  ReadSupplementatyDataBlockFromStream( IN const BYTE* pStream );

protected:

	typedef union UEncryptKey
	{
		BYTE key[16];
		BYTE keyCamellia[ CryptoPP::Camellia::DEFAULT_KEYLENGTH];
		BYTE keyTwofish [ CryptoPP::Twofish::DEFAULT_KEYLENGTH];
		BYTE keyXTEA	[ CryptoPP::XTEA::DEFAULT_KEYLENGTH];
	
	} TEncryptKey;

	typedef union UEncryptIV
	{
		BYTE iv[16];
		BYTE ivCamellia [ CryptoPP::Camellia::BLOCKSIZE];
		BYTE ivTwofish	[ CryptoPP::Twofish::BLOCKSIZE];
		BYTE ivXTEA		[ CryptoPP::XTEA::BLOCKSIZE];
	
	} TEncryptIV;


	typedef struct SCSHybridCryptKey
	{
		TEncryptKey		uEncryptKey;
		TEncryptIV		uEncryptIV;

	} TCSHybridCryptKey;

	typedef boost::unordered_map<DWORD, TCSHybridCryptKey>	TCSHybridCryptKeyMap;
	TCSHybridCryptKeyMap	m_mapHybridCryptKey;

	typedef struct SSupplementaryDataBlockInfo
	{
		std::string		  strRelatedMapName; 
		std::vector<BYTE> vecStream;

	} TSupplementaryDataBlockInfo;

	typedef boost::unordered_map<DWORD, TSupplementaryDataBlockInfo> TSupplementaryDataBlockMap; //key filename hash
	TSupplementaryDataBlockMap m_mapSDBMap;

private:
	bool GetPerFileCryptKey( std::string& rfileName, eHybridCipherAlgorithm& eAlgorithm, TEncryptKey& key, TEncryptIV& iv );


};



#endif // __INC_ETERPACKLIB_ETERPACKPOLICY_CSHYBRIDCRYPT_H__
