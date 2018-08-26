#ifndef __MILESLIB_CSOUNDINSTANCE_H__
#define __MILESLIB_CSOUNDINSTANCE_H__

#include "SoundBase.h"

class ISoundInstance : public CSoundBase
{
public:
	ISoundInstance() {}
	virtual ~ISoundInstance() {}

	virtual bool	Initialize() = 0;
	virtual void	Destroy() = 0;
	virtual bool	SetSound(CSoundData* pSound) = 0;
	virtual void	Play(int iLoopCount = 1, DWORD dwPlayCycleTimeLimit = 0) const = 0;
	virtual void	Pause() const = 0;
	virtual void	Resume() const = 0;
	virtual void	Stop() = 0;
	virtual void	GetVolume(float& rfVolume) const = 0;
	virtual void	SetVolume(float volume) const = 0;
	virtual bool	IsDone() const = 0;
	virtual void	SetPosition(float x, float y, float z) const = 0;
	virtual void	SetOrientation(float x_face, float y_face, float z_face, 
						   float x_normal, float y_normal, float z_normal) const = 0;
	virtual void	SetVelocity(float x, float y, float z, float fMagnitude) const = 0;
};

class CSoundInstance2D : public ISoundInstance
{
public:
	CSoundInstance2D();
	virtual ~CSoundInstance2D();

public: // from interface
	bool	Initialize();
	void	Destroy();

	bool	SetSound(CSoundData* pSound);
	void	Play(int iLoopCount = 1, DWORD dwPlayCycleTimeLimit = 0) const;
	void	Pause() const;
	void	Resume() const;
	void	Stop();
	void	GetVolume(float& rfVolume) const;
	void	SetVolume(float volume) const;
	bool	IsDone() const;
	void	SetPosition(float x, float y, float z) const;
	void	SetOrientation(float x_face, float y_face, float z_face, 
						   float x_normal, float y_normal, float z_normal) const;
	void	SetVelocity(float fx, float fy, float fz, float fMagnitude) const;

private:
	HSAMPLE			m_sample;
	CSoundData*		m_pSoundData;
};

class CSoundInstance3D : public ISoundInstance
{
	public:
		CSoundInstance3D();
		virtual ~CSoundInstance3D();

	public: // from interface
		bool	Initialize();
		void	Destroy();

		bool	SetSound(CSoundData * pSound);
		void	Play(int iLoopCount = 1, DWORD dwPlayCycleTimeLimit = 0) const;
		void	Pause() const;
		void	Resume() const;
		void	Stop();
		void	GetVolume(float& rfVolume) const;
		void	SetVolume(float volume) const;
		bool	IsDone() const;

		void	SetPosition(float x, float y, float z) const;
		void	SetOrientation(float x_face, float y_face, float z_face, 
							   float x_normal, float y_normal, float z_normal) const;
		void	SetVelocity(float fx, float fy, float fz, float fMagnitude) const;

		void	UpdatePosition(float fElapsedTime);

	private:
		H3DSAMPLE		m_sample;
		CSoundData *		m_pSoundData;
};

class CSoundInstanceStream : public ISoundInstance
{
public:
	CSoundInstanceStream();
	virtual ~CSoundInstanceStream();

public: // from interface
	bool	Initialize();
	void	Destroy();
	
	void	SetStream(HSTREAM stream);
	bool	SetSound(CSoundData* pSound);

	void	Play(int iLoopCount = 1, DWORD dwPlayCycleTimeLimit = 0) const;
	void	Pause() const;
	void	Resume() const;
	void	Stop();
	void	GetVolume(float& rfVolume) const;
	void	SetVolume(float volume) const;
	bool	IsDone() const;
	bool	IsData() const;

	void	SetPosition(float x, float y, float z) const;
	void	SetOrientation(float x_face, float y_face, float z_face, 
						   float x_normal, float y_normal, float z_normal) const;
	void	SetVelocity(float fx, float fy, float fz, float fMagnitude) const;

private:
	HSTREAM		m_stream;
};

#endif
