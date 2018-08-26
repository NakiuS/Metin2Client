#include "StdAfx.h"
#include "Type.h"
#include "../EterLib/TextFileLoader.h"

std::string NSound::strResult;

const char * NSound::GetResultString()
{
	return strResult.c_str();
}

void NSound::SetResultString(const char * c_pszStr)
{
	strResult.assign(c_pszStr);
}

bool NSound::LoadSoundInformationPiece(const char * c_szFileName, NSound::TSoundDataVector & rSoundDataVector, const char * c_szPathHeader)
{
	std::string strResult;
	strResult = c_szFileName;

	CTextFileLoader* pkTextFileLoader=CTextFileLoader::Cache(c_szFileName);
	if (!pkTextFileLoader)
		return false;

	CTextFileLoader& rkTextFileLoader=*pkTextFileLoader;
	if (rkTextFileLoader.IsEmpty())
	{
		SetResultString((strResult + " 읽기용 파일을 열 수 없음").c_str());
		return false;
	}

	rkTextFileLoader.SetTop();

	int iCount;
	if (!rkTextFileLoader.GetTokenInteger("sounddatacount", &iCount))
	{
		SetResultString((strResult + " 파일 포멧 에러, SoundDataCount를 찾을 수 없음").c_str());
		return false;
	}

	rSoundDataVector.clear();
	rSoundDataVector.resize(iCount);

	char szSoundDataHeader[32+1];
	for (DWORD i = 0; i < rSoundDataVector.size(); ++i)
	{
		_snprintf(szSoundDataHeader, sizeof(szSoundDataHeader), "sounddata%02d", i);
		CTokenVector * pTokenVector;
		if (!rkTextFileLoader.GetTokenVector(szSoundDataHeader, &pTokenVector))
		{
			SetResultString((strResult + " 파일 포멧 에러: " + szSoundDataHeader + " 를 찾을 수 없음").c_str());
			return false;
		}

		if (2 != pTokenVector->size())
		{
			SetResultString((strResult + " 파일 포멧 에러: 벡터 크기가 2가 아님").c_str());
			return false;
		}

		rSoundDataVector[i].fTime = (float) atof(pTokenVector->at(0).c_str());
		if (c_szPathHeader)
		{
			rSoundDataVector[i].strSoundFileName = c_szPathHeader;
			rSoundDataVector[i].strSoundFileName += pTokenVector->at(1).c_str();
		}
		else
		{
			rSoundDataVector[i].strSoundFileName = pTokenVector->at(1).c_str();
		}
	}

	SetResultString((strResult + " 불러옴").c_str());
	return true;
}

bool NSound::SaveSoundInformationPiece(const char * c_szFileName, NSound::TSoundDataVector & rSoundDataVector)
{
	if (rSoundDataVector.empty())	// 데이터가 없으면 성공으로 간주
	{
		if (IsFile(c_szFileName))	// 데이터는 비어있는데 파일이 있다면
		{
			_unlink(c_szFileName);		// 지운다.
		}
		return true;
	}

	std::string strResult;
	strResult = c_szFileName;

	FILE * File = fopen(c_szFileName, "wt");

	if (!File)
	{
		char szErrorText[256+1];
		_snprintf(szErrorText, sizeof(szErrorText), "Failed to save file (%s).\nPlease check if it is read-only or you have no space on the disk.\n", c_szFileName);
		LogBox(szErrorText, "에러");
		SetResultString((strResult + " 쓰기용 파일을 열 수 없음").c_str());
		return false;
	}

	fprintf(File, "ScriptType        CharacterSoundInformation\n");
	fprintf(File, "\n");

	fprintf(File, "SoundDataCount    %d\n", rSoundDataVector.size());

	for (DWORD i = 0; i < rSoundDataVector.size(); ++i)
	{
		NSound::TSoundData & rSoundData = rSoundDataVector[i];
		fprintf(File, "SoundData%02d       %f \"%s\"\n", i, rSoundData.fTime, rSoundData.strSoundFileName.c_str());
	}

	fclose(File);
	return true;
}

void NSound::DataToInstance(const TSoundDataVector & c_rSoundDataVector, TSoundInstanceVector * pSoundInstanceVector)
{
	if (c_rSoundDataVector.empty())
		return;

	DWORD dwFPS = 60;
	const float c_fFrameTime = 1.0f / float(dwFPS);

	pSoundInstanceVector->clear();
	pSoundInstanceVector->resize(c_rSoundDataVector.size());
	for (DWORD i = 0; i < c_rSoundDataVector.size(); ++i)
	{
		const TSoundData & c_rSoundData = c_rSoundDataVector[i];
		TSoundInstance & rSoundInstance = pSoundInstanceVector->at(i);

		rSoundInstance.dwFrame = (DWORD) (c_rSoundData.fTime / c_fFrameTime);
		rSoundInstance.strSoundFileName = c_rSoundData.strSoundFileName;
	}
}