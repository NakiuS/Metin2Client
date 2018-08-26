#pragma once

#include "../eterlib/GrpBase.h"
#include "../eterLib/Pool.h"
#include "EffectUpdateDecorator.h"
class CParticleProperty;
class CEmitterProperty;

class CParticleInstance
{
	friend class CParticleSystemData;
	friend class CParticleSystemInstance;

	friend class NEffectUpdateDecorator::CBaseDecorator;
	friend class NEffectUpdateDecorator::CAirResistanceDecorator;
	friend class NEffectUpdateDecorator::CGravityDecorator;
	friend class NEffectUpdateDecorator::CRotationDecorator;

	public:
		CParticleInstance();
		~CParticleInstance();

		float GetRadiusApproximation();
		
		BOOL Update(float fElapsedTime, float fAngle);
		//virtual void Transform(const D3DXMATRIX * c_matLocal, const float c_fZRotation)=0;
		//virtual void Transform(const D3DXMATRIX * c_matLocal = NULL)=0;

		//virtual TPTVertex * GetParticleMeshPointer() = 0;

		//__forceinline float GetLifePercentage()
		//{
		//	return m_fLifePercentage;
			//return (m_fLifeTime - m_fLastLifeTime) / m_fLifeTime;
		//}

		//virtual void DeleteThis() = 0;

	protected:
		//float				m_fLifePercentage;
		D3DXVECTOR3			m_v3StartPosition;

		D3DXVECTOR3			m_v3Position;
		D3DXVECTOR3			m_v3LastPosition;
		D3DXVECTOR3			m_v3Velocity;

		D3DXVECTOR2			m_v2HalfSize;
		D3DXVECTOR2			m_v2Scale;

		float				m_fRotation;
#ifdef WORLD_EDITOR
		D3DXCOLOR			m_Color;
#else
		DWORDCOLOR			m_dcColor;
#endif

		BYTE				m_byTextureAnimationType;
		float				m_fLastFrameTime;
		BYTE				m_byFrameIndex;

		float				m_fLifeTime;
		float				m_fLastLifeTime;

		CParticleProperty *	m_pParticleProperty;
		CEmitterProperty *	m_pEmitterProperty;

		float m_fAirResistance;
		float m_fRotationSpeed;
		float m_fGravity;

		NEffectUpdateDecorator::CBaseDecorator * m_pDecorator;	
	public:
		static CParticleInstance* New();
		static void DestroySystem();

		void Transform(const D3DXMATRIX * c_matLocal=NULL);
		void Transform(const D3DXMATRIX * c_matLocal, const float c_fZRotation);

		TPTVertex * GetParticleMeshPointer();
		
		void DeleteThis();

		void Destroy();

	protected:
		void __Initialize();
		TPTVertex			m_ParticleMesh[4];
	public:
		static CDynamicPool<CParticleInstance> ms_kPool;
		
};
