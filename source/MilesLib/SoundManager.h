#pragma once

#include "../eterBase/Singleton.h"

#include "SoundManagerStream.h"
#include "SoundManager2D.h"
#include "SoundManager3D.h"
#include "Type.h"

class CSoundManager : public CSingleton<CSoundManager>
{
public:
	CSoundManager();
	virtual ~CSoundManager();

	BOOL Create();
	void Destroy();

	void SetPosition(float fx, float fy, float fz);
	void SetDirection(float fxDir, float fyDir, float fzDir, float fxUp, float fyUp, float fzUp);
	void Update();

	float GetSoundScale();
	void SetSoundScale(float fScale);
	void SetAmbienceSoundScale(float fScale);
	void SetSoundVolume(float fVolume);
	void SetSoundVolumeRatio(float fRatio);
	void SetMusicVolume(float fVolume);
	void SetMusicVolumeRatio(float fRatio);
	void SetSoundVolumeGrade(int iGrade);
	void SetMusicVolumeGrade(int iGrade);
	void SaveVolume();
	void RestoreVolume();
	float GetSoundVolume();
	float GetMusicVolume();

	// Sound
	void PlaySound2D(const char * c_szFileName);
	void PlaySound3D(float fx, float fy, float fz, const char * c_szFileName, int iPlayCount = 1);
	void StopSound3D(int iIndex);
	int  PlayAmbienceSound3D(float fx, float fy, float fz, const char * c_szFileName, int iPlayCount = 1);
	void PlayCharacterSound3D(float fx, float fy, float fz, const char * c_szFileName, BOOL bCheckFrequency = FALSE);
	void SetSoundVolume3D(int iIndex, float fVolume);
	void StopAllSound3D();

	// Music
	void PlayMusic(const char * c_szFileName);
	void FadeInMusic(const char * c_szFileName, float fVolumeSpeed = 0.016f);
	void FadeOutMusic(const char * c_szFileName, float fVolumeSpeed = 0.016f);
	void FadeLimitOutMusic(const char * c_szFileName, float fLimitVolume, float fVolumeSpeed = 0.016f);
	void FadeOutAllMusic();
	void FadeAll();

	// Sound Node
	void UpdateSoundData(DWORD dwcurFrame, const NSound::TSoundDataVector * c_pSoundDataVector);
	void UpdateSoundData(float fx, float fy, float fz, DWORD dwcurFrame, const NSound::TSoundDataVector * c_pSoundDataVector);
	void UpdateSoundInstance(float fx, float fy, float fz, DWORD dwcurFrame, const NSound::TSoundInstanceVector * c_pSoundInstanceVector, BOOL bCheckFrequency = FALSE);
	void UpdateSoundInstance(DWORD dwcurFrame, const NSound::TSoundInstanceVector * c_pSoundInstanceVector);

protected:
	enum EMusicState
	{
		MUSIC_STATE_OFF,
		MUSIC_STATE_PLAY,
		MUSIC_STATE_FADE_IN,
		MUSIC_STATE_FADE_OUT,
		MUSIC_STATE_FADE_LIMIT_OUT,
	};
	typedef struct SMusicInstance
	{
		DWORD dwMusicFileNameCRC;
		EMusicState MusicState;
		float fVolume;
		float fLimitVolume;
		float fVolumeSpeed;
	} TMusicInstance;

	void PlayMusic(DWORD dwIndex, const char * c_szFileName, float fVolume, float fVolumeSpeed);
	void StopMusic(DWORD dwIndex);
	BOOL GetMusicIndex(const char * c_szFileName, DWORD * pdwIndex);

protected:
	float __ConvertGradeVolumeToApplyVolume(int nVolumeGrade);
	float __ConvertRatioVolumeToApplyVolume(float fVolumeRatio);
	void __SetMusicVolume(float fVolume);
	BOOL GetSoundInstance2D(const char * c_szSoundFileName, ISoundInstance ** ppInstance);
	BOOL GetSoundInstance3D(const char * c_szFileName, ISoundInstance ** ppInstance);

protected:
	BOOL							m_bInitialized;
	BOOL							m_isSoundDisable;

	float							m_fxPosition;
	float							m_fyPosition;
	float							m_fzPosition;

	float							m_fSoundScale;
	float							m_fAmbienceSoundScale;
	float							m_fSoundVolume;
	float							m_fMusicVolume;

	float							m_fBackupMusicVolume;
	float							m_fBackupSoundVolume;

	TMusicInstance					m_MusicInstances[CSoundManagerStream::MUSIC_INSTANCE_MAX_NUM];
	std::map<std::string, float>	m_PlaySoundHistoryMap;

	static CSoundManager2D			ms_SoundManager2D;
	static CSoundManager3D			ms_SoundManager3D;
	static CSoundManagerStream		ms_SoundManagerStream;
};
