#include "StdAfx.h"
#include "ParticleInstance.h"
#include "ParticleProperty.h"

#include "../eterBase/Random.h"
#include "../eterLib/Camera.h"
#include "../eterLib/StateManager.h"

CDynamicPool<CParticleInstance> CParticleInstance::ms_kPool;

using namespace NEffectUpdateDecorator;

void CParticleInstance::DestroySystem()
{
	ms_kPool.Destroy();
}

CParticleInstance* CParticleInstance::New()
{
	return ms_kPool.Alloc();
}

void CParticleInstance::DeleteThis()
{
	Destroy();
	
	ms_kPool.Free(this);	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//CDynamicPool<CRayParticleInstance> CRayParticleInstance::ms_kPool;

/*void CRayParticleInstance::DestroySystem()
{
	ms_kPool.Destroy();
}

CRayParticleInstance* CRayParticleInstance::New()
{
	return ms_kPool.Alloc();
}

void CRayParticleInstance::DeleteThis()
{
#ifdef RAY_TO_AFTERIMAGE
	m_PositionList.clear();
#else
	m_bStart = false;
#endif
	ms_kPool.Free(this);
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float CParticleInstance::GetRadiusApproximation()
{
	return m_v2HalfSize.y*m_v2Scale.y + m_v2HalfSize.x*m_v2Scale.x;
}
/*
D3DXVECTOR3 CBaseParticleInstance::GetCenterApproximation()
{
	return D3DXVECTOR3(
		m_v3Position.x + mc_pmatLocal._41,
		m_v3Position.y + mc_pmatLocal._42,
		m_v3Position.z + mc_pmatLocal._43
		);
}*/


BOOL CParticleInstance::Update(float fElapsedTime, float fAngle)
{
	m_fLastLifeTime -= fElapsedTime;
	if (m_fLastLifeTime < 0.0f)
		return FALSE;

	float fLifePercentage = (m_fLifeTime - m_fLastLifeTime) / m_fLifeTime;

	m_pDecorator->Excute(CDecoratorData(fLifePercentage,fElapsedTime,this));

	m_v3LastPosition = m_v3Position;
	m_v3Position += m_v3Velocity * fElapsedTime;

	if (fAngle)
	{
		if (m_pParticleProperty->m_bAttachFlag)
		{
			float fCos, fSin;
			fAngle = D3DXToRadian(fAngle);
			fCos = cos(fAngle);
			fSin = sin(fAngle);

			float rx = m_v3Position.x - m_v3StartPosition.x;
			float ry = m_v3Position.y - m_v3StartPosition.y;

			m_v3Position.x =   fCos * rx + fSin * ry + m_v3StartPosition.x;
			m_v3Position.y = - fSin * rx + fCos * ry + m_v3StartPosition.y;
		}
		else
		{
			D3DXQUATERNION q,qc;
			D3DXQuaternionRotationAxis(&q,&m_pParticleProperty->m_v3ZAxis,D3DXToRadian(fAngle));
			D3DXQuaternionConjugate(&qc,&q);

			D3DXQUATERNION qr(
				m_v3Position.x-m_v3StartPosition.x,
				m_v3Position.y-m_v3StartPosition.y,
				m_v3Position.z-m_v3StartPosition.z,
				0.0f);
			D3DXQuaternionMultiply(&qr,&q,&qr);
			D3DXQuaternionMultiply(&qr,&qr,&qc);

			m_v3Position.x = qr.x;
			m_v3Position.y = qr.y;
			m_v3Position.z = qr.z;

			m_v3Position += m_v3StartPosition;
		}
	}

	return TRUE;
}

void CParticleInstance::Transform(const D3DXMATRIX * c_matLocal)
{
#ifdef WORLD_EDITOR
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, m_Color);
#else
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, m_dcColor);
#endif

	/////

	D3DXVECTOR3 v3Up;
	D3DXVECTOR3 v3Cross;

	if (!m_pParticleProperty->m_bStretchFlag)
	{
		CCamera * pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();
		const D3DXVECTOR3 & c_rv3Up = pCurrentCamera->GetUp();
		const D3DXVECTOR3 & c_rv3Cross = pCurrentCamera->GetCross();

		D3DXVECTOR3 v3Rotation;

		switch(m_pParticleProperty->m_byBillboardType) {
		case BILLBOARD_TYPE_LIE:
			{
				float fCos = cosf(D3DXToRadian(m_fRotation)), fSin = sinf(D3DXToRadian(m_fRotation));
				v3Up.x = fCos;
				v3Up.y = -fSin;
				v3Up.z = 0;
				v3Cross.x = fSin;
				v3Cross.y = fCos;
				v3Cross.z = 0;
			}
			break;
		case BILLBOARD_TYPE_2FACE:
		case BILLBOARD_TYPE_3FACE:
			// using setting with y, and local rotation at render
		case BILLBOARD_TYPE_Y:
			{
				v3Up = D3DXVECTOR3(0.0f,0.0f,1.0f);
				//v3Up = D3DXVECTOR3(cosf(D3DXToRadian(m_fRotation)),0.0f,-sinf(D3DXToRadian(m_fRotation)));
				const D3DXVECTOR3 & c_rv3View = pCurrentCamera->GetView();
				if (v3Up.x * c_rv3View.y - v3Up.y * c_rv3View.x<0)
					v3Up*=-1;
				D3DXVec3Cross(&v3Cross, &v3Up, &D3DXVECTOR3(c_rv3View.x,c_rv3View.y,0));
				D3DXVec3Normalize(&v3Cross, &v3Cross);

				if (m_fRotation)
				{
					float fCos = -sinf(D3DXToRadian(m_fRotation)); // + 90
					float fSin = cosf(D3DXToRadian(m_fRotation));
					
					D3DXVECTOR3 v3Temp = v3Up * fCos - v3Cross * fSin;
					v3Cross = v3Cross * fCos + v3Up * fSin;
					v3Up = v3Temp;
				}

				//D3DXVECTOR3 v3Rotation;
				//D3DXVec3Cross(&v3Rotation, &v3Up, &v3Cross);

			}
			break;
		case BILLBOARD_TYPE_ALL:
		default:
			{
				// NOTE : Rotation Routine. Camera의 Up Vector와 Cross Vector 자체를 View Vector 기준으로
				//        Rotation 시킨다.
				// FIXME : 반드시 최적화 할 것!
				if (m_fRotation==0.0f)
				{
					v3Up = -c_rv3Cross;
					v3Cross = c_rv3Up;
				}
				else
				{
					const D3DXVECTOR3 & c_rv3View = pCurrentCamera->GetView();
					D3DXQUATERNION q,qc;
					D3DXQuaternionRotationAxis(&q, &c_rv3View, D3DXToRadian(m_fRotation));
					D3DXQuaternionConjugate(&qc, &q);
					
					{
						D3DXQUATERNION qr(-c_rv3Cross.x, -c_rv3Cross.y, -c_rv3Cross.z, 0);
						D3DXQuaternionMultiply(&qr,&qc,&qr);
						D3DXQuaternionMultiply(&qr,&qr,&q);
						v3Up.x = qr.x;
						v3Up.y = qr.y;
						v3Up.z = qr.z;
					}
					{
						D3DXQUATERNION qr(c_rv3Up.x, c_rv3Up.y, c_rv3Up.z, 0);
						D3DXQuaternionMultiply(&qr,&qc,&qr);
						D3DXQuaternionMultiply(&qr,&qr,&q);
						v3Cross.x = qr.x;
						v3Cross.y = qr.y;
						v3Cross.z = qr.z;
					}

				}
				//D3DXMATRIX matRotation;
				
				//D3DXMatrixRotationAxis(&matRotation, &c_rv3View, D3DXToRadian(m_fRotation));
				
				//D3DXVec3TransformCoord(&v3Up, &(-c_rv3Cross), &matRotation);
				//D3DXVec3TransformCoord(&v3Cross, &c_rv3Up, &matRotation);
			}
			break;
		} 

	}
	else
	{
		v3Up = m_v3Position - m_v3LastPosition;

		if (c_matLocal)
		{
			//if (!m_pParticleProperty->m_bAttachFlag)
				D3DXVec3TransformNormal(&v3Up, &v3Up, c_matLocal);
		}

		// NOTE: 속도가 길이에 주는 영향 : log(velocity)만큼 늘어난다.
		float length = D3DXVec3Length(&v3Up);
		if (length == 0.0f)
		{
			v3Up = D3DXVECTOR3(0.0f,0.0f,1.0f);
		}
		else
			v3Up *=(1+log(1+length))/length;

		CCamera * pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();
		const D3DXVECTOR3 & c_rv3View = pCurrentCamera->GetView();
		D3DXVec3Cross(&v3Cross, &v3Up, &c_rv3View);
		D3DXVec3Normalize(&v3Cross, &v3Cross);

	}

	v3Cross = -(m_v2HalfSize.x*m_v2Scale.x) * v3Cross;
	v3Up = (m_v2HalfSize.y*m_v2Scale.y) * v3Up;

	if (c_matLocal && m_pParticleProperty->m_bAttachFlag)
	{
		D3DXVECTOR3 v3Position;
		D3DXVec3TransformCoord(&v3Position, &m_v3Position, c_matLocal);
		m_ParticleMesh[0].position = v3Position - v3Up + v3Cross;
		m_ParticleMesh[1].position = v3Position - v3Up - v3Cross;
		m_ParticleMesh[2].position = v3Position + v3Up + v3Cross;
		m_ParticleMesh[3].position = v3Position + v3Up - v3Cross;
	}
	else
	{
		m_ParticleMesh[0].position = m_v3Position - v3Up + v3Cross;
		m_ParticleMesh[1].position = m_v3Position - v3Up - v3Cross;
		m_ParticleMesh[2].position = m_v3Position + v3Up + v3Cross;
		m_ParticleMesh[3].position = m_v3Position + v3Up - v3Cross;
	}
}


void CParticleInstance::Transform(const D3DXMATRIX * c_matLocal, const float c_fZRotation)
{
#ifdef WORLD_EDITOR
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, m_Color);
#else
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, (DWORD)m_dcColor);
#endif

	/////

	D3DXVECTOR3 v3Up;
	D3DXVECTOR3 v3Cross;

	if (!m_pParticleProperty->m_bStretchFlag)
	{
		CCamera * pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();
		const D3DXVECTOR3 & c_rv3Up = pCurrentCamera->GetUp();
		const D3DXVECTOR3 & c_rv3Cross = pCurrentCamera->GetCross();

		D3DXVECTOR3 v3Rotation;

		switch(m_pParticleProperty->m_byBillboardType) {
		case BILLBOARD_TYPE_LIE:
			{
				float fCos = cosf(D3DXToRadian(m_fRotation)), fSin = sinf(D3DXToRadian(m_fRotation));
				v3Up.x = fCos;
				v3Up.y = -fSin;
				v3Up.z = 0;

				v3Cross.x = fSin;
				v3Cross.y = fCos;
				v3Cross.z = 0;
			}
			break;
		case BILLBOARD_TYPE_2FACE:
		case BILLBOARD_TYPE_3FACE:
			// using setting with y, and local rotation at render
		case BILLBOARD_TYPE_Y:
			{
				v3Up = D3DXVECTOR3(0.0f,0.0f,1.0f);
				//v3Up = D3DXVECTOR3(cosf(D3DXToRadian(m_fRotation)),0.0f,-sinf(D3DXToRadian(m_fRotation)));
				const D3DXVECTOR3 & c_rv3View = pCurrentCamera->GetView();
				if (v3Up.x * c_rv3View.y - v3Up.y * c_rv3View.x<0)
					v3Up*=-1;
				D3DXVec3Cross(&v3Cross, &v3Up, &D3DXVECTOR3(c_rv3View.x,c_rv3View.y,0));
				D3DXVec3Normalize(&v3Cross, &v3Cross);

				if (m_fRotation)
				{
					float fCos = -sinf(D3DXToRadian(m_fRotation)); // + 90
					float fSin = cosf(D3DXToRadian(m_fRotation));
					
					D3DXVECTOR3 v3Temp = v3Up * fCos - v3Cross * fSin;
					v3Cross = v3Cross * fCos + v3Up * fSin;
					v3Up = v3Temp;
				}

				//D3DXVECTOR3 v3Rotation;
				//D3DXVec3Cross(&v3Rotation, &v3Up, &v3Cross);

			}
			break;
		case BILLBOARD_TYPE_ALL:
		default:
			{
				// NOTE : Rotation Routine. Camera의 Up Vector와 Cross Vector 자체를 View Vector 기준으로
				//        Rotation 시킨다.
				// FIXME : 반드시 최적화 할 것!
				if (m_fRotation==0.0f)
				{
					v3Up = -c_rv3Cross;
					v3Cross = c_rv3Up;
				}
				else
				{
					const D3DXVECTOR3 & c_rv3View = pCurrentCamera->GetView();
					D3DXMATRIX matRotation;
					
					D3DXMatrixRotationAxis(&matRotation, &c_rv3View, D3DXToRadian(m_fRotation));
					D3DXVec3TransformCoord(&v3Up, &(-c_rv3Cross), &matRotation);
					D3DXVec3TransformCoord(&v3Cross, &c_rv3Up, &matRotation);
				}
			}
			break;
		}
	}
	else
	{
		v3Up = m_v3Position - m_v3LastPosition;

		if (c_matLocal)
		{
			//if (!m_pParticleProperty->m_bAttachFlag)
				D3DXVec3TransformNormal(&v3Up, &v3Up, c_matLocal);
		}

		// NOTE: 속도가 길이에 주는 영향 : log(velocity)만큼 늘어난다.
		float length = D3DXVec3Length(&v3Up);
		if (length == 0.0f)
		{
			v3Up = D3DXVECTOR3(0.0f,0.0f,1.0f);
		}
		else
			v3Up *=(1+log(1+length))/length;
		//D3DXVec3Normalize(&v3Up,&v3Up);
		//v3Up *= 1+log(1+length);

		CCamera * pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();
		const D3DXVECTOR3 & c_rv3View = pCurrentCamera->GetView();
		D3DXVec3Cross(&v3Cross, &v3Up, &c_rv3View);
		D3DXVec3Normalize(&v3Cross, &v3Cross);

	}

	if (c_fZRotation)
	{
		float x, y;
		float fCos = cosf(c_fZRotation);
		float fSin = sinf(c_fZRotation);

		x = v3Up.x;
		y = v3Up.y;
		v3Up.x = x * fCos - y * fSin;
		v3Up.y = y * fCos + x * fSin;

		x = v3Cross.x;
		y = v3Cross.y;
		v3Cross.x = x * fCos - y * fSin;
		v3Cross.y = y * fCos + x * fSin;
	}

	v3Cross = -(m_v2HalfSize.x*m_v2Scale.x) * v3Cross;
	v3Up = (m_v2HalfSize.y*m_v2Scale.y) * v3Up;

	if (c_matLocal && m_pParticleProperty->m_bAttachFlag)
	{
		D3DXVECTOR3 v3Position;
		D3DXVec3TransformCoord(&v3Position, &m_v3Position, c_matLocal);
		m_ParticleMesh[0].position = v3Position - v3Up + v3Cross;
		m_ParticleMesh[1].position = v3Position - v3Up - v3Cross;
		m_ParticleMesh[2].position = v3Position + v3Up + v3Cross;
		m_ParticleMesh[3].position = v3Position + v3Up - v3Cross;
	}
	else
	{
		m_ParticleMesh[0].position = m_v3Position - v3Up + v3Cross;
		m_ParticleMesh[1].position = m_v3Position - v3Up - v3Cross;
		m_ParticleMesh[2].position = m_v3Position + v3Up + v3Cross;
		m_ParticleMesh[3].position = m_v3Position + v3Up - v3Cross;
	}
}

void CParticleInstance::Destroy()
{
	if (m_pDecorator)
		m_pDecorator->DeleteThis();

	__Initialize();

}

void CParticleInstance::__Initialize()
{
	//*
	m_pDecorator=NULL;

	m_v3Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_v3LastPosition = m_v3Position;
	m_v3Velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_v2Scale = D3DXVECTOR2(1.0f, 1.0f);
#ifdef WORLD_EDITOR
	m_Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
#else
	m_dcColor.m_dwColor = 0xffffffff;
#endif

	m_byFrameIndex = 0;
	m_ParticleMesh[0].texCoord = D3DXVECTOR2(0.0f, 1.0f);
	m_ParticleMesh[1].texCoord = D3DXVECTOR2(0.0f, 0.0f);
	m_ParticleMesh[2].texCoord = D3DXVECTOR2(1.0f, 1.0f);
	m_ParticleMesh[3].texCoord = D3DXVECTOR2(1.0f, 0.0f);
}

CParticleInstance::CParticleInstance()
{
	__Initialize();
}

CParticleInstance::~CParticleInstance()
{
	Destroy();
}

/*CRayParticleInstance::CRayParticleInstance()
{
#ifdef RAY_TO_AFTERIMAGE
	int i;
	for(i=0;i<RAY_VERTEX_COUNT*2;i+=2)
	{
		m_ParticleMesh[i].texCoord = D3DXVECTOR2(i+0.0f, 1.0f);
		m_ParticleMesh[i+1].texCoord = D3DXVECTOR2(i+1.0f, 0.0f);
	}
#else
	m_ParticleMesh[0].texCoord = D3DXVECTOR2(0.0f, 1.0f);
	m_ParticleMesh[1].texCoord = D3DXVECTOR2(0.0f, 0.0f);
	m_ParticleMesh[2].texCoord = D3DXVECTOR2(0.5f, 1.0f);
	m_ParticleMesh[3].texCoord = D3DXVECTOR2(0.5f, 0.0f);
	m_ParticleMesh[4].texCoord = D3DXVECTOR2(1.0f, 1.0f);
	m_ParticleMesh[5].texCoord = D3DXVECTOR2(1.0f, 0.0f);

	m_bStart = false;
#endif
}

CRayParticleInstance::~CRayParticleInstance()
{
}*/


TPTVertex * CParticleInstance::GetParticleMeshPointer()
{
	return m_ParticleMesh;
}

/*TPTVertex * CRayParticleInstance::GetParticleMeshPointer()
{
	return m_ParticleMesh;
}

void CRayParticleInstance::Transform(const D3DXMATRIX * c_matLocal, const float c_fZRotation)
{
	assert(false && "NOT_REACHED");
}*/
