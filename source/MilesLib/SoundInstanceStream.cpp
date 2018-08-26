#include "stdafx.h"
#include "SoundInstance.h"

CSoundInstanceStream::CSoundInstanceStream() : m_stream(NULL)
{
}

CSoundInstanceStream::~CSoundInstanceStream()
{
	Destroy();
}

void CSoundInstanceStream::Destroy()
{
	if (m_stream != NULL)
	{
		AIL_close_stream(m_stream);
		m_stream = NULL;
	}
}

bool CSoundInstanceStream::Initialize()
{
	return true;
}

void CSoundInstanceStream::SetStream(HSTREAM stream)
{
	m_stream = stream;
}

bool CSoundInstanceStream::IsDone() const
{
	return AIL_stream_status(m_stream) == -1;
}

bool CSoundInstanceStream::IsData() const
{
	if (m_stream)
		return true;

	return false;
}

void CSoundInstanceStream::Play(int count, DWORD dwPlayCycleTimeLimit) const
{
	if (!IsData())
		return;

	AIL_set_stream_loop_count(m_stream, count);
	AIL_start_stream(m_stream);
}

void CSoundInstanceStream::Pause() const
{
	if (!IsData())
		return;

	AIL_pause_stream(m_stream, 1);
}

void CSoundInstanceStream::Resume() const
{
	if (!IsData())
		return;

	AIL_pause_stream(m_stream, 0);
}

void CSoundInstanceStream::Stop()
{
	if (!IsData())
		return;

	AIL_close_stream(m_stream);
	m_stream = NULL;
}

void CSoundInstanceStream::GetVolume(float& rfVolume) const
{
	float tmp;

	if (!IsData())
		return;

	AIL_stream_volume_levels(m_stream, &rfVolume, &tmp);
}

void CSoundInstanceStream::SetVolume(float volume) const
{
	if (!IsData())
		return;

	volume = max(0.0f, min(1.0f, volume));
	AIL_set_stream_volume_levels(m_stream, volume, volume);
}

bool CSoundInstanceStream::SetSound(CSoundData* pSound)
{
	return true;
}

void CSoundInstanceStream::SetPosition(float x, float y, float z) const
{
}

void CSoundInstanceStream::SetOrientation(float x_face, float y_face, float z_face, 
										  float x_normal, float y_normal, float z_normal) const
{
}

void CSoundInstanceStream::SetVelocity(float fx, float fy, float fz, float fMagnitude) const
{
}
