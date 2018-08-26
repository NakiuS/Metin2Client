#include "StdAfx.h"
#include <math.h>

#include "SoundManager.h"
#include "../EterBase/Timer.h"

CSoundManager2D CSoundManager::ms_SoundManager2D;
CSoundManager3D CSoundManager::ms_SoundManager3D;
CSoundManagerStream CSoundManager::ms_SoundManagerStream;

CSoundManager::CSoundManager()
{
	m_bInitialized			= FALSE;
	m_isSoundDisable		= FALSE;

	m_fxPosition			= 0.0f;
	m_fyPosition			= 0.0f;
	m_fzPosition			= 0.0f;

	m_fSoundScale			= 200.0f;
	m_fAmbienceSoundScale	= 1000.0f;
	m_fSoundVolume			= 1.0f;
	m_fMusicVolume			= 1.0f;

	m_fBackupMusicVolume	= 0.0f;
	m_fBackupSoundVolume	= 0.0f;

	for (int i = 0; i < CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM; ++i)
	{
		TMusicInstance & rInstance = m_MusicInstances[i];
		rInstance.dwMusicFileNameCRC = 0;
		rInstance.MusicState = MUSIC_STATE_OFF;
		rInstance.fVolume = 0.0f;
		rInstance.fVolumeSpeed = 0.0f;
	}
}

CSoundManager::~CSoundManager()
{
}

BOOL CSoundManager::Create()
{
	if (!ms_SoundManager2D.Initialize()) {
		Tracen("CSoundManager::Create - Sound2D::Initialize - FAILURE");
		return FALSE;
	}

	if (!ms_SoundManagerStream.Initialize())
	{
		Tracen("CSoundManager::Create - SoundStream::Initialize - FAILURE");
		return FALSE;
	}

	if (!ms_SoundManager3D.Initialize())
	{
		Tracen("CSoundManager::Create - Sound3D::Initialize - FAILURE");
		return FALSE;
	}

	return TRUE;
}

void CSoundManager::Destroy()
{
	ms_SoundManagerStream.Destroy();
	ms_SoundManager3D.Destroy();		
	ms_SoundManager2D.Destroy();	
}

void CSoundManager::SetPosition(float fx, float fy, float fz)
{
	m_fxPosition = fx;
	m_fyPosition = fy;
	m_fzPosition = fz;
}

void CSoundManager::SetDirection(float fxDir, float fyDir, float fzDir, float fxUp, float fyUp, float fzUp)
{
	ms_SoundManager3D.SetListenerDirection(fxDir, fyDir, fzDir, fxUp, fyUp, fzUp);
}

void CSoundManager::Update()
{
	// Update Information about 3D Sound
	ms_SoundManager3D.SetListenerPosition(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM; ++i)
	{
		TMusicInstance & rMusicInstance = m_MusicInstances[i];
		if (MUSIC_STATE_OFF == rMusicInstance.MusicState)
			continue;

		switch (rMusicInstance.MusicState)
		{
			case MUSIC_STATE_FADE_IN:
				rMusicInstance.fVolume += rMusicInstance.fVolumeSpeed;

				if (rMusicInstance.fVolume >= GetMusicVolume())
				{
					rMusicInstance.fVolume = GetMusicVolume();
					rMusicInstance.fVolumeSpeed = 0.0f;
					rMusicInstance.MusicState = MUSIC_STATE_PLAY;
				}

				{
					CSoundInstanceStream * pInstance = ms_SoundManagerStream.GetInstance(i);
					if (pInstance)
						pInstance->SetVolume(rMusicInstance.fVolume);
				}
				break;
				
			case MUSIC_STATE_FADE_LIMIT_OUT:
				rMusicInstance.fVolume -= rMusicInstance.fVolumeSpeed;
				if (rMusicInstance.fVolume <= rMusicInstance.fLimitVolume)
				{
					rMusicInstance.fVolume = rMusicInstance.fLimitVolume;
					rMusicInstance.fVolumeSpeed = 0.0f;
					rMusicInstance.MusicState = MUSIC_STATE_PLAY;				
				}
				{
					CSoundInstanceStream * pInstance = ms_SoundManagerStream.GetInstance(i);
					if (pInstance)
						pInstance->SetVolume(rMusicInstance.fVolume);
				}
				break;
			case MUSIC_STATE_FADE_OUT:
				rMusicInstance.fVolume -= rMusicInstance.fVolumeSpeed;
				if (rMusicInstance.fVolume <= 0.0f)
				{
					rMusicInstance.fVolume = 0.0f;
					rMusicInstance.fVolumeSpeed = 0.0f;
					rMusicInstance.MusicState = MUSIC_STATE_OFF;

					StopMusic(i);
				}
				{
					CSoundInstanceStream * pInstance = ms_SoundManagerStream.GetInstance(i);
					if (pInstance)
						pInstance->SetVolume(rMusicInstance.fVolume);
				}
				break;
		}
	}
}

void CSoundManager::UpdateSoundData(DWORD dwcurTime, const NSound::TSoundDataVector * c_pSoundDataVector)
{
	assert(!"CSoundManager::UpdateSoundData");
}

void CSoundManager::UpdateSoundData(float fx, float fy, float fz, DWORD dwcurTime, const NSound::TSoundDataVector * c_pSoundDataVector)
{
	assert(!"CSoundManager::UpdateSoundData");
}

void CSoundManager::UpdateSoundInstance(float fx, float fy, float fz, DWORD dwcurFrame, const NSound::TSoundInstanceVector * c_pSoundInstanceVector, BOOL bCheckFrequency)
{
	for (DWORD i = 0; i < c_pSoundInstanceVector->size(); ++i)
	{
		const NSound::TSoundInstance & c_rSoundInstance = c_pSoundInstanceVector->at(i);
		if (c_rSoundInstance.dwFrame == dwcurFrame)
		{
			//Tracenf("PLAY SOUND %s", c_rSoundInstance.strSoundFileName.c_str());
			PlayCharacterSound3D(fx, fy, fz, c_rSoundInstance.strSoundFileName.c_str(), bCheckFrequency);
		}
	}
}

void CSoundManager::UpdateSoundInstance(DWORD dwcurFrame, const NSound::TSoundInstanceVector * c_pSoundInstanceVector)
{
	for (DWORD i = 0; i < c_pSoundInstanceVector->size(); ++i)
	{
		const NSound::TSoundInstance & c_rSoundInstance = c_pSoundInstanceVector->at(i);

		if (c_rSoundInstance.dwFrame == dwcurFrame)
		{
			PlaySound2D(c_rSoundInstance.strSoundFileName.c_str());
		}
	}
}

float CSoundManager::GetSoundScale()
{
	return m_fSoundScale;
}

void CSoundManager::SetSoundScale(float fScale)
{
	m_fSoundScale = fScale;
}

void CSoundManager::SetAmbienceSoundScale(float fScale)
{
	m_fAmbienceSoundScale = fScale;
}

void CSoundManager::SetSoundVolume(float fVolume)
{
	if (m_isSoundDisable)
	{
		m_fBackupSoundVolume = fVolume;
		return;
	}

	fVolume = fMAX(fVolume, 0.0f);
	fVolume = fMIN(fVolume, 1.0f);
	m_fSoundVolume = fVolume;

	if (!m_isSoundDisable)
	{
		m_fBackupSoundVolume = fVolume;
	}
}

void CSoundManager::__SetMusicVolume(float fVolume)
{
	if (m_isSoundDisable)
	{
		m_fBackupMusicVolume = fVolume;
		return;
	}

	fVolume = fMAX(fVolume, 0.0f);
	fVolume = fMIN(fVolume, 1.0f);
	m_fMusicVolume = fVolume;

	if (!m_isSoundDisable)
	{
		m_fBackupMusicVolume = fVolume;
	}

	for (int i = 0; i < CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM; ++i)
	{
		TMusicInstance & rMusicInstance = m_MusicInstances[i];
		if (MUSIC_STATE_OFF == rMusicInstance.MusicState)
			continue;
		if (MUSIC_STATE_FADE_OUT == rMusicInstance.MusicState)
			continue;

		rMusicInstance.fVolume = fVolume;

		CSoundInstanceStream * pInstance = ms_SoundManagerStream.GetInstance(i);
		if (pInstance)
			pInstance->SetVolume(fVolume);
	}
}

float CSoundManager::__ConvertRatioVolumeToApplyVolume(float fRatioVolume)
{
	if (0.1f>fRatioVolume)
		return fRatioVolume;

	return (float)pow(10.0f, (-1.0f + fRatioVolume));
}

float CSoundManager::__ConvertGradeVolumeToApplyVolume(int nGradeVolume)
{
	return __ConvertRatioVolumeToApplyVolume(nGradeVolume/5.0f);	
}


void CSoundManager::SetSoundVolumeGrade(int iGrade)
{
	float fVolume=__ConvertGradeVolumeToApplyVolume(iGrade);	
	SetSoundVolume(fVolume);
}

//void CSoundManager::SetMusicVolumeGrade(int iGrade)
//{
//	float fVolume=__ConvertGradeVolumeToApplyVolume(iGrade);
//	__SetMusicVolume(fVolume);
//}

void CSoundManager::SetSoundVolumeRatio(float fRatio)
{
	float fVolume = __ConvertRatioVolumeToApplyVolume(fRatio);
	SetSoundVolume(fVolume);
}

void CSoundManager::SetMusicVolume(float fVolume)
{	
	//float fVolume = __ConvertRatioVolumeToApplyVolume(fRatio);
	__SetMusicVolume(fVolume);
}

float CSoundManager::GetSoundVolume()
{
	return m_fSoundVolume;
}

float CSoundManager::GetMusicVolume()
{
	return m_fMusicVolume;
}

BOOL CSoundManager::GetSoundInstance2D(const char * c_szSoundFileName, ISoundInstance ** ppInstance)
{
	*ppInstance = ms_SoundManager2D.GetInstance(c_szSoundFileName);

	if (!*ppInstance)
		return FALSE;

	return TRUE;
}

BOOL CSoundManager::GetSoundInstance3D(const char * c_szFileName, ISoundInstance ** ppInstance)
{
	int iIndex = ms_SoundManager3D.SetInstance(c_szFileName);

	if (-1 == iIndex)
		return FALSE;

	*ppInstance = ms_SoundManager3D.GetInstance(iIndex);

	if (!*ppInstance)
		return FALSE;

	return TRUE;
}

void CSoundManager::PlaySound2D(const char * c_szFileName)
{
	if (0.0f == GetSoundVolume())
		return;

	ISoundInstance * pInstance;
	if (!GetSoundInstance2D(c_szFileName, &pInstance))
		return;

	pInstance->SetVolume(GetSoundVolume());
	pInstance->Play(1);
}

void CSoundManager::PlaySound3D(float fx, float fy, float fz, const char * c_szFileName, int iPlayCount)
{
	if (0.0f == GetSoundVolume())
		return;

	int iIndex = ms_SoundManager3D.SetInstance(c_szFileName);
	if (-1 == iIndex)
		return;

	ISoundInstance * pInstance = ms_SoundManager3D.GetInstance(iIndex);
	if (!pInstance)
		return;

	pInstance->SetPosition((fx - m_fxPosition) / m_fSoundScale,
						   (fy - m_fyPosition) / m_fSoundScale,
						   (fz - m_fzPosition) / m_fSoundScale);

	pInstance->SetVolume(GetSoundVolume());
	pInstance->Play(iPlayCount);
}

int CSoundManager::PlayAmbienceSound3D(float fx, float fy, float fz, const char * c_szFileName, int iPlayCount)
{
	if (0.0f == GetSoundVolume())
		return -1;

	int iIndex = ms_SoundManager3D.SetInstance(c_szFileName);
	if (-1 == iIndex)
		return -1;

	ISoundInstance * pInstance = ms_SoundManager3D.GetInstance(iIndex);
	if (!pInstance)
		return -1;

	pInstance->SetPosition((fx - m_fxPosition) / m_fAmbienceSoundScale,
						   (fy - m_fyPosition) / m_fAmbienceSoundScale,
						   (fz - m_fzPosition) / m_fAmbienceSoundScale);

	pInstance->SetVolume(GetSoundVolume());
	pInstance->Play(iPlayCount);

	return iIndex;
}

void CSoundManager::PlayCharacterSound3D(float fx, float fy, float fz, const char * c_szFileName, BOOL bCheckFrequency)
{
	if (0.0f == GetSoundVolume())
		return;

	// 어느 정도의 최적화가 필요할 수도 있다 - [levites]
	if (bCheckFrequency)
	{
		static float s_fLimitDistance = 5000*5000;
		float fdx = (fx - m_fxPosition) * (fx - m_fxPosition);
		float fdy = (fy - m_fyPosition) * (fy - m_fyPosition);

		if (fdx+fdy > s_fLimitDistance)
			return;

		std::map<std::string, float>::iterator itor = m_PlaySoundHistoryMap.find(c_szFileName);
		if (m_PlaySoundHistoryMap.end() != itor)
		{
			float fTime = itor->second;
			if (CTimer::Instance().GetCurrentSecond() - fTime < 0.3f)
			{
				//Tracef("똑같은 소리가 0.3초 내에 다시 플레이 %s\n", c_szFileName);
				return;
			}
		}

		m_PlaySoundHistoryMap.erase(c_szFileName);
		m_PlaySoundHistoryMap.insert(std::map<std::string, float>::value_type(c_szFileName, CTimer::Instance().GetCurrentSecond()));
	}

	ISoundInstance * pInstance;

	if (!GetSoundInstance3D(c_szFileName, &pInstance))
		return;

	pInstance->SetPosition((fx - m_fxPosition) / m_fSoundScale,
						   (fy - m_fyPosition) / m_fSoundScale,
						   (fz - m_fzPosition) / m_fSoundScale);

	pInstance->SetVolume(GetSoundVolume());
	pInstance->Play(1);
}

void CSoundManager::StopSound3D(int iIndex)
{
	ISoundInstance * pInstance = ms_SoundManager3D.GetInstance(iIndex);

	if (!pInstance)
		return;

	pInstance->Stop();
	//bool bisDone = pInstance->IsDone();
}

void CSoundManager::SetSoundVolume3D(int iIndex, float fVolume)
{
	ISoundInstance * pInstance = ms_SoundManager3D.GetInstance(iIndex);

	if (!pInstance)
		return;

	pInstance->SetVolume(fVolume);
}

void CSoundManager::StopAllSound3D()
{
	for (int i = 0; i < CSoundManager3D::INSTANCE_MAX_COUNT; ++i)
	{
		StopSound3D(i);
	}
}

void CSoundManager::PlayMusic(const char * c_szFileName)
{
	PlayMusic(0, c_szFileName, GetMusicVolume(), 0.0f);
}

void CSoundManager::FadeInMusic(const char * c_szFileName, float fVolumeSpeed)
{
	DWORD dwIndex;
	if (GetMusicIndex(c_szFileName, &dwIndex))
	{
		m_MusicInstances[dwIndex].MusicState = MUSIC_STATE_FADE_IN;
		m_MusicInstances[dwIndex].fVolumeSpeed = fVolumeSpeed;
		return;
	}

	FadeOutAllMusic();

	//Tracenf("FadeInMusic: %s", c_szFileName);
	
	for (int i = 0; i < CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM; ++i)
	{
		TMusicInstance & rMusicInstance = m_MusicInstances[i];
		if (MUSIC_STATE_OFF != rMusicInstance.MusicState)
			continue;

		PlayMusic(i, c_szFileName, 0.0f, fVolumeSpeed);
		return;
	}

	return;

	// If there is no empty music slot, then play music on slot 0.
	/*
	StopMusic(0);
	PlayMusic(0, c_szFileName, 0.0f, fVolumeSpeed);
	*/
}

void CSoundManager::FadeLimitOutMusic(const char * c_szFileName, float fLimitVolume, float fVolumeSpeed)
{
	//Tracenf("FadeLimitOutMusic: %s", c_szFileName);

	DWORD dwIndex;
	if (!GetMusicIndex(c_szFileName, &dwIndex))
	{
		Tracenf("FadeOutMusic: %s - ERROR NOT EXIST", c_szFileName);
		return;
	}

	if (dwIndex >= CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM)
	{
		Tracenf("FadeOutMusic: %s - ERROR OUT OF RANGE", c_szFileName);
		return;
	}

	SMusicInstance& rkMusicInst=m_MusicInstances[dwIndex];
	rkMusicInst.MusicState = MUSIC_STATE_FADE_LIMIT_OUT;	
	rkMusicInst.fVolumeSpeed = fVolumeSpeed;	
	rkMusicInst.fLimitVolume = __ConvertRatioVolumeToApplyVolume(fLimitVolume);

	//Tracenf("LimitVolume %f(%f)", fLimitVolume, rkMusicInst.fLimitVolume);
}

void CSoundManager::FadeOutMusic(const char * c_szFileName, float fVolumeSpeed)
{
	//Tracenf("FadeOutMusic: %s", c_szFileName);

	DWORD dwIndex;
	if (!GetMusicIndex(c_szFileName, &dwIndex))
	{
		Tracenf("FadeOutMusic: %s - ERROR NOT EXIST", c_szFileName);
		return;
	}

	if (dwIndex >= CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM)
	{
		Tracenf("FadeOutMusic: %s - ERROR OUT OF RANGE", c_szFileName);
		return;
	}

	m_MusicInstances[dwIndex].MusicState = MUSIC_STATE_FADE_OUT;
	m_MusicInstances[dwIndex].fVolumeSpeed = fVolumeSpeed;
}

void CSoundManager::FadeOutAllMusic()
{
	//Tracenf("FadeOutAllMusic");

	for (int i = 0; i < CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM; ++i)
	{
		if (MUSIC_STATE_OFF == m_MusicInstances[i].MusicState)
			continue;

		m_MusicInstances[i].MusicState = MUSIC_STATE_FADE_OUT;
		m_MusicInstances[i].fVolumeSpeed = 0.01f;
	}
}

void CSoundManager::SaveVolume()
{
	// NOTE : 두번 이상 Save를 시도할때는 그냥 Return
	if (m_isSoundDisable)
		return;

	float fBackupMusicVolume = m_fMusicVolume;
	float fBackupSoundVolume = m_fSoundVolume;
	__SetMusicVolume(0.0f);
	SetSoundVolume(0.0f);
	m_fBackupMusicVolume = fBackupMusicVolume;
	m_fBackupSoundVolume = fBackupSoundVolume;
	m_isSoundDisable = TRUE;
}

void CSoundManager::RestoreVolume()
{
	m_isSoundDisable = FALSE;
	__SetMusicVolume(m_fBackupMusicVolume);
	SetSoundVolume(m_fBackupSoundVolume);
}

void CSoundManager::PlayMusic(DWORD dwIndex, const char * c_szFileName, float fVolume, float fVolumeSpeed)
{
	if (dwIndex >= CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM)
		return;

	if (!ms_SoundManagerStream.SetInstance(dwIndex, c_szFileName))
	{
		TraceError("CSoundManager::PlayMusic - Failed to load stream sound : %s", c_szFileName);
		return;
	}

	CSoundInstanceStream * pInstance = ms_SoundManagerStream.GetInstance(dwIndex);
	if (!pInstance)
	{
		TraceError("CSoundManager::PlayMusic - There is no stream sound instance : %s", c_szFileName);
		return;
	}

	pInstance->SetVolume(fVolume);
	pInstance->Play(0);

	TMusicInstance & rMusicInstance = m_MusicInstances[dwIndex];
	rMusicInstance.fVolume = fVolume;
	rMusicInstance.fVolumeSpeed = fVolumeSpeed;
	rMusicInstance.MusicState = MUSIC_STATE_FADE_IN;

	std::string strFileName;
	StringPath(c_szFileName, strFileName);
	rMusicInstance.dwMusicFileNameCRC = GetCaseCRC32(strFileName.c_str(), strFileName.length());
}

void CSoundManager::StopMusic(DWORD dwIndex)
{
	if (dwIndex >= CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM)
		return;

	CSoundInstanceStream * pInstance = ms_SoundManagerStream.GetInstance(dwIndex);
	if (!pInstance)
		return;

	pInstance->Stop();

	TMusicInstance & rMusicInstance = m_MusicInstances[dwIndex];
	rMusicInstance.fVolume = 0.0f;
	rMusicInstance.fVolumeSpeed = 0.0f;
	rMusicInstance.MusicState = MUSIC_STATE_OFF;
	rMusicInstance.dwMusicFileNameCRC = 0;
}

BOOL CSoundManager::GetMusicIndex(const char * c_szFileName, DWORD * pdwIndex)
{
	std::string strFileName;
	StringPath(c_szFileName, strFileName);
	DWORD dwCRC = GetCaseCRC32(strFileName.c_str(), strFileName.length());

	for (int i = 0; i < CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM; ++i)
	{
		const TMusicInstance & c_rMusicInstance = m_MusicInstances[i];
		if (MUSIC_STATE_OFF != c_rMusicInstance.MusicState)
		if (c_rMusicInstance.dwMusicFileNameCRC == dwCRC)
		{
			*pdwIndex = i;
			return TRUE;
		}
	}

	return FALSE;
}

void CSoundManager::FadeAll()
{
	FadeOutAllMusic();
}
