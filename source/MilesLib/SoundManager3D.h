#ifndef __MILESLIB_CSOUNDMANAGER3D_H__
#define __MILESLIB_CSOUNDMANAGER3D_H__

#include "SoundBase.h"
#include "SoundInstance.h"

class CSoundManager3D : public CSoundBase
{
	public:
		enum
		{
			INSTANCE_MAX_COUNT = 32,
			MAX_PROVIDERS = 32,
		};

	public:
		CSoundManager3D();
		virtual ~CSoundManager3D();

		bool				Initialize();
		void				Destroy();

		int					GetEmptyInstanceIndex();
		int					SetInstance(const char * c_szFileName);
		ISoundInstance *	GetInstance(DWORD dwIndex);

		void				SetListenerDirection(float fxDir, float fyDir, float fzDir, float fxUp, float fyUp, float fzUp);
		void				SetListenerPosition(float x, float y, float z);
		void				SetListenerVelocity(float fDistanceX, float fDistanceY, float fDistanceZ, float fNagnitude);

		void				Lock(int iIndex);
		void				Unlock(int iIndex);

	protected:
		bool				IsValidInstanceIndex(int iIndex);

	protected:
		bool				m_bLockingFlag[INSTANCE_MAX_COUNT];
		CSoundInstance3D	m_Instances[INSTANCE_MAX_COUNT];
		H3DPOBJECT			m_pListener;

		bool m_bInit;
};

#endif