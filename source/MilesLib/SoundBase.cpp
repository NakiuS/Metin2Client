#include "Stdafx.h"
#include "SoundBase.h"

HDIGDRIVER				CSoundBase::ms_DIGDriver = NULL;
TSoundDataMap			CSoundBase::ms_dataMap;
TProvider *				CSoundBase::ms_pProviderDefault = NULL;
std::vector<TProvider>	CSoundBase::ms_ProviderVector;
bool					CSoundBase::ms_bInitialized = false;
int						CSoundBase::ms_iRefCount = 0;

CSoundBase::CSoundBase()
{
}

CSoundBase::~CSoundBase()
{
}

void CSoundBase::Destroy()
{
	if (ms_iRefCount > 1)
	{
		--ms_iRefCount;
		return;
	}

	ms_iRefCount = 0;

	if (!ms_dataMap.empty())
	{
		TSoundDataMap::iterator i;
		for (i = ms_dataMap.begin(); i != ms_dataMap.end(); ++i)
		{
			CSoundData * pSoundData = i->second;
			delete pSoundData;
		}

		ms_dataMap.clear();
	}

	AIL_shutdown();
}

void CSoundBase::Initialize()
{
	++ms_iRefCount;

	if (ms_iRefCount > 1)
		return;
	
	AIL_set_redist_directory("miles");
	AIL_startup();

	ms_ProviderVector.clear();
	ms_dataMap.clear();
	
}

DWORD CSoundBase::GetFileCRC(const char * filename)
{
	return GetCRC32(filename, strlen(filename));
}

CSoundData * CSoundBase::AddFile(DWORD dwFileCRC, const char* filename)
{
	CSoundData * pSoundData = new CSoundData;
	pSoundData->Assign(filename);
	ms_dataMap.insert(TSoundDataMap::value_type(dwFileCRC, pSoundData));
	return pSoundData;
}
