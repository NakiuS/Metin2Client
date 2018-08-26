#include "stdafx.h"
#include "SoundManagerStream.h"

CSoundManagerStream::CSoundManagerStream()
{
}

CSoundManagerStream::~CSoundManagerStream()
{	
}

bool CSoundManagerStream::Initialize()
{
	CSoundBase::Initialize();
	
	if (ms_DIGDriver)
		return true;

	ms_DIGDriver = AIL_open_digital_driver(44100, 16, 2, 0);

	for (int i = 0; i < MUSIC_INSTANCE_MAX_NUM; ++i)
		m_Instances[i].Initialize();

	return true;
}

void CSoundManagerStream::Destroy()
{	
	for (int i=0; i<MUSIC_INSTANCE_MAX_NUM; ++i)
		m_Instances[i].Stop();

	CSoundBase::Destroy();
}

bool CSoundManagerStream::SetInstance(DWORD dwIndex, const char* filename)
{
	if (!CheckInstanceIndex(dwIndex))
		return false;

	HSTREAM hStream = AIL_open_stream(ms_DIGDriver, filename, 0);

	if (NULL == hStream)
		return false;

	m_Instances[dwIndex].SetStream(hStream);

	return true;
}

CSoundInstanceStream * CSoundManagerStream::GetInstance(DWORD dwIndex)
{
	if (!CheckInstanceIndex(dwIndex))
		return NULL;

	return &m_Instances[dwIndex];
}

bool CSoundManagerStream::CheckInstanceIndex(DWORD dwIndex)
{
	if (dwIndex >= DWORD(MUSIC_INSTANCE_MAX_NUM))
		return false;

	return true;
}