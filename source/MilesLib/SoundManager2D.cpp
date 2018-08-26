#include "Stdafx.h"
#include "SoundManager2D.h"

CSoundManager2D::CSoundManager2D()
{
}

CSoundManager2D::~CSoundManager2D()
{	
}

bool CSoundManager2D::Initialize()
{
	CSoundBase::Initialize();

	if (ms_DIGDriver)
		return true;

	ms_DIGDriver = AIL_open_digital_driver(44100, 16, 2, 0);
	
	for (int i = 0; i < INSTANCE_MAX_COUNT; ++i)
		ms_Instances[i].Initialize();
/*	ms_DIGDriver = AIL_open_digital_driver(44100,
										   (DIG_F_STEREO_16 & DIG_F_16BITS_MASK) ? 16 : 8,
										   (DIG_F_STEREO_16 & DIG_F_16BITS_MASK) ? 2 : 1,
										   0);
*/
	return true;
}

void CSoundManager2D::Destroy()
{	
	for (int i = 0; i < INSTANCE_MAX_COUNT; ++i)
		ms_Instances[i].Destroy();
	
	if (ms_DIGDriver != NULL)
	{
		AIL_close_digital_driver(ms_DIGDriver);
		ms_DIGDriver = NULL;
	}	

	CSoundBase::Destroy();
}

ISoundInstance * CSoundManager2D::GetInstance(const char * c_pszFileName)
{
	DWORD dwFileCRC = GetFileCRC(c_pszFileName);
	TSoundDataMap::iterator itor = ms_dataMap.find(dwFileCRC);

	CSoundData * pkSoundData;

	if (itor == ms_dataMap.end())
		pkSoundData = AddFile(dwFileCRC, c_pszFileName); // CSoundBase::AddFile
	else
		pkSoundData = itor->second;

	assert(pkSoundData != NULL);

	static DWORD k = 0;

	DWORD start = k++;
	DWORD end = start + INSTANCE_MAX_COUNT;

	while (start < end)
	{
		CSoundInstance2D * pkInst = &ms_Instances[start % INSTANCE_MAX_COUNT];

		if (pkInst->IsDone())
		{
			if (!pkInst->SetSound(pkSoundData))
				TraceError("CSoundManager2D::GetInstance (filename: %s)", c_pszFileName);
			return (pkInst);
		}

		++start;
	}

	return NULL;
}
