#include "Stdafx.h"
#include "EterPackPolicy_CSHybridCrypt.h"
#include "../EterBase/Stl.h"
#include "../EterBase/FileName.h"
#include "../EterBase/FileBase.h"
#include "../EterBase/Crc32.h"
#include "../EterBase/lzo.h"
#include "../EterBase/Random.h"
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>

using namespace CryptoPP;

#define CIPHER_MODE	CTR_Mode

#ifdef __THEMIDA__
#include <ThemidaSDK.h>
#endif
//Cipher
//Block Size
//Key Length
// 
//Default Minimum Maximum 
//AES(Rijndael) 16 16 16 32 
//Blowfish 8 16 0 56 
//Camellia 16 16 16 32 
//CAST-128 8 16 5 16 
//CAST-256 16 16 16 32 
//DES 8 8 8 8 
//DES-EDE2 8 16 16 16 
//DES-EDE3 8 24 24 24 
//DES-XEX3 8 24 24 24 
//GOST 8 32 32 32 
//IDEA 8 16 16 16 
//MARS 16 16 16 56 
//RC2 8 16 1 128 
//RC5 8 16 0 255 
//RC6 16 16 0 255 
//SAFER-K 8 16 8 16 
//SAFER-SK 8 16 8 16 
//Serpent 16 16 1 32 
//SHACAL-2 32 16 1 64 
//SHARK-E 8 16 1 16 
//SKIPJACK 8 10 1 10 
//3-Way 12 12 1 12 
//Twofish 16 16 0 32 
//XTEA 8 16 1 16 

inline std::string GetFileExt( std::string& rfileName )
{
	stl_lowers(rfileName);
	return CFileNameHelper::GetExtension(rfileName);
}

EterPackPolicy_CSHybridCrypt::~EterPackPolicy_CSHybridCrypt()
{
	m_mapHybridCryptKey.clear();
	m_mapSDBMap.clear();
}

bool EterPackPolicy_CSHybridCrypt::IsContainingCryptKey() const
{
	return (m_mapHybridCryptKey.size() > 0) ? true : false;
}


bool EterPackPolicy_CSHybridCrypt::GenerateCryptKey( std::string& rfileName )
{
#ifdef __THEMIDA__
	VM_START
#endif

	//make lower & extract ext
	std::string extName = GetFileExt(rfileName);
	stl_lowers(extName);

	DWORD  dwExtHash  = stringhash().GetHash(extName);

	TCSHybridCryptKeyMap::const_iterator cit = m_mapHybridCryptKey.find( dwExtHash );

	if( cit != m_mapHybridCryptKey.end() )
	{
		//TODO : log already registered
		return false;
	}

	static AutoSeededRandomPool rnd;

	TCSHybridCryptKey info;
	{
		rnd.GenerateBlock( &(info.uEncryptKey.key[0]), sizeof(info.uEncryptKey) ); 
		rnd.GenerateBlock( &(info.uEncryptIV.iv[0]), sizeof(info.uEncryptIV) );

		//for test
	/*	memset( &info.uEncryptKey.key, 0x10, sizeof(info.uEncryptKey) ); 
		memset( &info.uEncryptIV.iv, 0x10, sizeof(info.uEncryptIV) ); */
	}
	m_mapHybridCryptKey[dwExtHash] = info;

#ifdef __THEMIDA__
	VM_END
#endif

	return true;
}

bool EterPackPolicy_CSHybridCrypt::GetPerFileCryptKey( std::string& rfileName, eHybridCipherAlgorithm& eAlgorithm, TEncryptKey& key, TEncryptIV& iv )
{
#ifdef __THEMIDA__
	VM_START
#endif

	std::string fileNamelower = rfileName;
	stl_lowers(fileNamelower);

	std::string extName = GetFileExt(fileNamelower);

	TCSHybridCryptKeyMap::const_iterator cit = m_mapHybridCryptKey.find( stringhash().GetHash(extName));

	if( cit == m_mapHybridCryptKey.end() )
	{
		//TODO : log no file ext info
		return false;
	}

	DWORD dwfileNameCrc = GetCRC32(fileNamelower.c_str(), fileNamelower.size());

	//make file specific algorithm & key & iv
	eAlgorithm = (eHybridCipherAlgorithm)(dwfileNameCrc % Num_Of_Ciphers);
	
	::memcpy(key.key, cit->second.uEncryptKey.key, sizeof(key) );
	::memcpy(iv.iv,   cit->second.uEncryptIV.iv,   sizeof(iv) );


	//Themida Warning
	for( int i = 0; i < (sizeof(key)/sizeof(dwfileNameCrc)); ++i)
	{
		*((DWORD*)key.key + i) ^= dwfileNameCrc;
	}
	for( int i = 0; i < (sizeof(iv)/sizeof(dwfileNameCrc)); ++i)
	{
		*((DWORD*)iv.iv + i) ^= dwfileNameCrc;
	}
#ifdef __THEMIDA__
	VM_END
#endif

	return true;
}


bool EterPackPolicy_CSHybridCrypt::EncryptMemory( std::string& rfileName, IN const BYTE* pSrcData, IN int iSrcLen, OUT CLZObject& zObj )
{
#ifdef __THEMIDA__
	VM_START
#endif

	eHybridCipherAlgorithm eAlgorithm;
	TEncryptKey			   key;
	TEncryptIV			   iv;

	if( !GetPerFileCryptKey( rfileName, eAlgorithm, key, iv ) )
	{
		return false;
	}

	// start cipher
	std::string strCipher;
	//NOTE : ciphered stream size could be different from original size if you choose diffrent cipher mode & algorithm
	//( i.e ECB or CBC mode )
	strCipher.reserve(iSrcLen);

	if( eAlgorithm == e_Cipher_Camellia )
	{
		// Encryptor
		CIPHER_MODE<Camellia>::Encryption Encryptor;
		Encryptor.SetKeyWithIV(key.keyCamellia, sizeof(key.keyCamellia), iv.ivCamellia, sizeof(iv.ivCamellia));

		ArraySource(pSrcData, iSrcLen, true,
							           new StreamTransformationFilter(Encryptor, new CryptoPP::StringSink(strCipher)));
	}
	else if( eAlgorithm == e_Cipher_Twofish )
	{
		// Encryptor
		CIPHER_MODE<Twofish>::Encryption Encryptor;
		Encryptor.SetKeyWithIV(key.keyTwofish, sizeof(key.keyTwofish), iv.ivTwofish, sizeof(iv.ivTwofish));

		ArraySource(pSrcData, iSrcLen, true,
							           new StreamTransformationFilter(Encryptor, new CryptoPP::StringSink(strCipher)));
	}
	else if( eAlgorithm == e_Cipher_XTEA )
	{
		// Encryptor
		CIPHER_MODE<XTEA>::Encryption Encryptor;
		Encryptor.SetKeyWithIV(key.keyXTEA, sizeof(key.keyXTEA), iv.ivXTEA, sizeof(iv.ivXTEA));

		ArraySource(pSrcData, iSrcLen, true,
							           new StreamTransformationFilter(Encryptor, new CryptoPP::StringSink(strCipher)));
	}
	
	if (strCipher.length() != iSrcLen)
	{
		//TODO: size error log
		return false;
	}

	zObj.AllocBuffer(iSrcLen);
	memcpy(zObj.GetBuffer(), strCipher.c_str(), strCipher.length() );

#ifdef __THEMIDA__
	VM_END
#endif

	return true;
}

bool EterPackPolicy_CSHybridCrypt::DecryptMemory( std::string& rfilename, IN const BYTE* pEncryptedData, IN int iEncryptedLen, OUT CLZObject& zObj )
{
#ifdef __THEMIDA__
	VM_START
#endif

	eHybridCipherAlgorithm eAlgorithm;
	TEncryptKey			   key;
	TEncryptIV			   iv;

	if( !GetPerFileCryptKey( rfilename, eAlgorithm, key, iv ) )
	{
		return false;
	}

	// start decipher
	std::string strDecipher;
	//NOTE : ciphered stream size could be different from original size if you choose diffrent cipher mode & algorithm
	//( i.e ECB or CBC mode )
	strDecipher.reserve(iEncryptedLen);

	if( eAlgorithm == e_Cipher_Camellia )
	{
		// Decryptor
		CIPHER_MODE<Camellia>::Decryption Decryptor;
		Decryptor.SetKeyWithIV(key.keyCamellia, sizeof(key.keyCamellia), iv.ivCamellia, sizeof(iv.ivCamellia));

		ArraySource(pEncryptedData, iEncryptedLen, true,
							           new StreamTransformationFilter(Decryptor, new CryptoPP::StringSink(strDecipher)));
	}
	else if( eAlgorithm == e_Cipher_Twofish )
	{
		// Decryptor
		CIPHER_MODE<Twofish>::Decryption Decryptor;
		Decryptor.SetKeyWithIV(key.keyTwofish, sizeof(key.keyTwofish), iv.ivTwofish, sizeof(iv.ivTwofish));

		ArraySource(pEncryptedData, iEncryptedLen, true,
							           new StreamTransformationFilter(Decryptor, new CryptoPP::StringSink(strDecipher)));
	}
	else if( eAlgorithm == e_Cipher_XTEA )
	{
		// Decryptor
		CIPHER_MODE<XTEA>::Decryption Decryptor;
		Decryptor.SetKeyWithIV(key.keyXTEA, sizeof(key.keyXTEA), iv.ivXTEA, sizeof(iv.ivXTEA));

		ArraySource(pEncryptedData, iEncryptedLen, true,
							           new StreamTransformationFilter(Decryptor, new CryptoPP::StringSink(strDecipher)));
	}


	if (strDecipher.length() != iEncryptedLen)
	{
		//TODO: size error log
		return false;
	}

	zObj.AllocBuffer(iEncryptedLen);
	memcpy(zObj.GetBuffer(), strDecipher.c_str(), strDecipher.length() );

#ifdef __THEMIDA__
	VM_END
#endif

	return true;
}

void EterPackPolicy_CSHybridCrypt::WriteCryptKeyToFile( CFileBase& rFile )
{
	//		ext cnt 4byte
	//		for	ext hash ( crc32 )
	//			key-16byte 
	//			iv-16byte

	DWORD dwCryptKeySize = m_mapHybridCryptKey.size();
	rFile.Write( &dwCryptKeySize, sizeof(DWORD) );

	TCSHybridCryptKeyMap::const_iterator cit;
	for( cit = m_mapHybridCryptKey.begin(); cit != m_mapHybridCryptKey.end(); ++cit )
	{
		DWORD extNamehash = cit->first;
		const TCSHybridCryptKey& CryptKey = cit->second;

		rFile.Write( &extNamehash, sizeof(DWORD) );
		rFile.Write( CryptKey.uEncryptKey.key, sizeof(TEncryptKey) );
		rFile.Write( CryptKey.uEncryptIV.iv, sizeof(TEncryptIV) );
	}
}

int EterPackPolicy_CSHybridCrypt::ReadCryptKeyInfoFromStream( IN const BYTE* pStream )
{
	int	iStreamOffset = 0;
	
	DWORD dwCryptoInfoSize;
	memcpy(&dwCryptoInfoSize, pStream, sizeof(DWORD) );
	iStreamOffset += sizeof(DWORD);

	DWORD dwExtHash;

	m_mapHybridCryptKey.clear();

	for( int i = 0; i < dwCryptoInfoSize; ++i )
	{
		memcpy(&dwExtHash, pStream + iStreamOffset, sizeof(DWORD) );
		iStreamOffset += sizeof(DWORD);

		TCSHybridCryptKey info;
		{
			memcpy(info.uEncryptKey.key, pStream + iStreamOffset, sizeof(TEncryptKey) );
			iStreamOffset += sizeof(TEncryptKey);

			memcpy(info.uEncryptIV.iv, pStream + iStreamOffset, sizeof(TEncryptIV) );
			iStreamOffset += sizeof(TEncryptIV);
		}

		m_mapHybridCryptKey[dwExtHash] = info;
	}

	return iStreamOffset;
}

bool EterPackPolicy_CSHybridCrypt::GenerateSupplementaryDataBlock(std::string& rfilename, const std::string& strMapName, IN const BYTE* pSrcData, IN int iSrcLen, OUT LPBYTE& pDestData, OUT int& iDestLen )
{
#ifdef __THEMIDA__
	VM_START
#endif

	std::string fileNamelower = rfilename;
	stl_lowers( fileNamelower );

	DWORD dwFileNameHash  = stringhash().GetHash(fileNamelower);
	TSupplementaryDataBlockMap::const_iterator cit = m_mapSDBMap.find( dwFileNameHash );

	if( cit != m_mapSDBMap.end() )
	{
		//TODO : log already registered
		return false;
	}

	//TODO : Find Better Method for deciding SDB Postion & Size

	//prevent stream copy duplication
	TSupplementaryDataBlockInfo info;
	m_mapSDBMap[dwFileNameHash] = info;

	std::string& strRelatedMapName = m_mapSDBMap[dwFileNameHash].strRelatedMapName;
	std::vector<BYTE>& sdbVector   = m_mapSDBMap[dwFileNameHash].vecStream;


	//fill the data!!
	{
		strRelatedMapName = strMapName;

		int iSDBSize = random_range( 64, 128 );

		if( iSrcLen < iSDBSize )
		{
			iSDBSize = iSrcLen - 1;
			if( iSDBSize <= 0 )
			{
				//TODO : is there 1byte file exist???
				return false;
			}
		}

		sdbVector.resize( iSDBSize );
		
		iDestLen  = iSrcLen - iSDBSize;
		pDestData = (LPBYTE)pSrcData;

		memcpy( &sdbVector[0], pDestData + iDestLen, iSDBSize );
	}
#ifdef __THEMIDA__
	VM_END
#endif
	return true;
}


bool EterPackPolicy_CSHybridCrypt::GetSupplementaryDataBlock( std::string& rfilename, OUT LPBYTE& pSDB, OUT int& iSDBSize )
{
#ifdef __THEMIDA__
	VM_START
#endif

	std::string fileNamelower = rfilename;
	stl_lowers( fileNamelower );

	DWORD dwFileNameHash  = stringhash().GetHash(fileNamelower);
	TSupplementaryDataBlockMap::const_iterator cit = m_mapSDBMap.find( dwFileNameHash );

	if( cit == m_mapSDBMap.end() )
	{
		//TODO : log already registered
		return false;
	}

	const std::vector<BYTE>& vecSDB = cit->second.vecStream;

	iSDBSize = vecSDB.size();

	if(iSDBSize <= 0)
	{
		pSDB = NULL;
		return false;
	}

	pSDB = (BYTE*)&vecSDB[0]; 
#ifdef __THEMIDA__
	VM_END
#endif

	return true;
}

bool EterPackPolicy_CSHybridCrypt::IsContainingSDBFile() const
{
	return m_mapSDBMap.size() > 0 ? true : false;
}

void EterPackPolicy_CSHybridCrypt::WriteSupplementaryDataBlockToFile( CFileBase& rFile )
{
	//about SDB data
	//		sdb file cnt( 4byte )
	//		for	sdb file cnt
	//			filename hash ( stl.h stringhash )
	//			related map name size(4), relate map name
	//			sdb block size( 1byte )
	//			sdb blocks 

	DWORD dwSDBMapSize = m_mapSDBMap.size();
	rFile.Write( &dwSDBMapSize, sizeof(DWORD) );

	TSupplementaryDataBlockMap::const_iterator cit;
	for( cit = m_mapSDBMap.begin(); cit != m_mapSDBMap.end(); ++cit )
	{
		DWORD dwFileNamehash = cit->first;
		rFile.Write( &dwFileNamehash, sizeof(DWORD) );

		const std::string strRelatedMapName = cit->second.strRelatedMapName;
		DWORD dwMapNameSize = strRelatedMapName.size();
		rFile.Write( &dwMapNameSize, sizeof(DWORD) );
		rFile.Write( strRelatedMapName.c_str(), dwMapNameSize );

		const std::vector<BYTE>& sdbVector = cit->second.vecStream;
		BYTE bSDBSize = (BYTE)(sdbVector.size());

		rFile.Write( &bSDBSize, sizeof(bSDBSize) );
		if( bSDBSize > 0 )
			rFile.Write( &sdbVector[0], bSDBSize );
	}
}

int EterPackPolicy_CSHybridCrypt::ReadSupplementatyDataBlockFromStream( IN const BYTE* pStream )
{
#ifdef __THEMIDA__
	VM_START
#endif

	//DWORD				dwFileIdentifier;
	//std::vector<BYTE>	vecSDBStream;

	DWORD dwFileNameHash;
	BYTE  bSDBSize;
	int   iStreamOffset = 0;
	
	memcpy(&dwFileNameHash, pStream + iStreamOffset, sizeof(DWORD) );
	iStreamOffset += sizeof(DWORD);

	memcpy(&bSDBSize, pStream + iStreamOffset, sizeof(BYTE) );
	iStreamOffset += sizeof(BYTE);

	// NOTE : related map name isn`t required in client. so we don`t recv it from stream to reduce packet size.
	TSupplementaryDataBlockInfo info;
	{
		info.vecStream.resize( bSDBSize );
		memcpy(&info.vecStream[0],  pStream + iStreamOffset, bSDBSize );
		iStreamOffset += bSDBSize;

		m_mapSDBMap[dwFileNameHash] = info;
	}

#ifdef __THEMIDA__
	VM_END
#endif

	return iStreamOffset;
}
