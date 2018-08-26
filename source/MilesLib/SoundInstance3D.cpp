#include "stdafx.h"
#include "SoundManager3D.h"
#include "../eterBase/Timer.h"

CSoundInstance3D::CSoundInstance3D() : m_sample(NULL), m_pSoundData(NULL)
{
}

CSoundInstance3D::~CSoundInstance3D()
{
	Destroy();
}

void CSoundInstance3D::Destroy()
{
	SAFE_RELEASE(m_pSoundData);

	if (m_sample)
	{
		AIL_release_3D_sample_handle(m_sample);
		m_sample = NULL;
	}
}

bool CSoundInstance3D::Initialize()
{
	if (m_sample)
		return true;

	m_sample = AIL_allocate_3D_sample_handle(ms_pProviderDefault->hProvider);
	return m_sample ? true : false;
}

bool CSoundInstance3D::SetSound(CSoundData* pSoundData)
{
	assert(m_sample != NULL && pSoundData != NULL);

	// 레퍼런스 카운트가 1이 될 때 로드를 해야 제대로 사이즈가 리턴
	// 되므로 반드시 Get을 호출 하고 진행해야 한다.
	// 또, m_pSoundData가 pSoundData와 같고 m_pSoundData의 레퍼런스
	// 카운터가 1일 경우, 불필요하게 로드가 일어나므로 미리 레퍼런스
	// 카운터를 올려놔야 한다.
	LPVOID lpData = pSoundData->Get();
	
	if (m_pSoundData != NULL)
	{
		m_pSoundData->Release();
		m_pSoundData = NULL;
	}
	
	if (AIL_set_3D_sample_file(m_sample, lpData) == NULL)
	{
		TraceError("%s: %s", AIL_last_error(), pSoundData->GetFileName());
		pSoundData->Release();
		return false;
	}

	m_pSoundData = pSoundData;

	AIL_set_3D_position(m_sample, 0.0F, 0.0F, 0.0F);
	AIL_auto_update_3D_position(m_sample, 0);
	return true;
}

bool CSoundInstance3D::IsDone() const
{
	return AIL_3D_sample_status(m_sample) == SMP_DONE;
}

void CSoundInstance3D::Play(int iLoopCount, DWORD dwPlayCycleTimeLimit) const
{
	if (!m_pSoundData)
		return;

	DWORD dwCurTime = ELTimer_GetMSec();

	if (dwCurTime - m_pSoundData->GetPlayTime() < dwPlayCycleTimeLimit)
		return;

	m_pSoundData->SetPlayTime(dwCurTime);

	AIL_set_3D_sample_loop_count(m_sample, iLoopCount);
	AIL_start_3D_sample(m_sample);
}

void CSoundInstance3D::Pause() const
{
	AIL_stop_3D_sample(m_sample);
}

void CSoundInstance3D::Resume() const
{
	AIL_resume_3D_sample(m_sample);
}

void CSoundInstance3D::Stop()
{
	AIL_end_3D_sample(m_sample);
//	m_sample = NULL;
// NOTE : IsDone을 체크하려면 m_sample이 살아있어야 합니다 - [levites]
}

void CSoundInstance3D::GetVolume(float& rfVolume) const
{
	rfVolume = AIL_3D_sample_volume(m_sample);
}

void CSoundInstance3D::SetVolume(float volume) const
{
	volume = max(0.0f, min(1.0f, volume));
	AIL_set_3D_sample_volume(m_sample, volume);
}

void CSoundInstance3D::SetPosition(float x, float y, float z) const
{
	AIL_set_3D_position(m_sample, x, y, -z);
}

void CSoundInstance3D::SetOrientation(float x_face, float y_face, float z_face, 
									  float x_normal, float y_normal, float z_normal) const
{
	assert(!" CSoundInstance3D::SetOrientation - 사용 하지 않는 함수");
//	AIL_set_3D_orientation(m_sample, 
//						   x_face, y_face, z_face,
//						   x_normal, y_normal, z_normal);
}

void CSoundInstance3D::SetVelocity(float fDistanceX, float fDistanceY, float fDistanceZ, float fNagnitude) const
{
	AIL_set_3D_velocity(m_sample, fDistanceX, fDistanceY, fDistanceZ, fNagnitude);
	AIL_auto_update_3D_position(m_sample, 1);
}

void CSoundInstance3D::UpdatePosition(float fElapsedTime)
{
	AIL_update_3D_position(m_sample, fElapsedTime);
}