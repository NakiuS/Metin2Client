#include "StdAfx.h"
#include "EmitterProperty.h"

BYTE CEmitterProperty::GetEmitterShape()
{
	return m_byEmitterShape;
}
BYTE CEmitterProperty::GetEmitterAdvancedType()
{
	return m_byEmitterAdvancedType;
}
BOOL CEmitterProperty::isEmitFromEdge()
{
	return m_bEmitFromEdgeFlag;
}

void CEmitterProperty::GetEmittingSize(float fTime, float * pfValue)
{
	//GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventEmittingSize, pfValue);
	GetTimeEventBlendValue(fTime, m_TimeEventEmittingSize, pfValue);
}
void CEmitterProperty::GetEmittingAngularVelocity(float fTime, float * pfValue)
{
	//GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventEmittingAngularVelocity, pfValue);
	GetTimeEventBlendValue(fTime, m_TimeEventEmittingAngularVelocity, pfValue);
}
void CEmitterProperty::GetEmittingDirectionX(float fTime, float * pfValue)
{
	//GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventEmittingDirectionX, pfValue);
	GetTimeEventBlendValue(fTime, m_TimeEventEmittingDirectionX, pfValue);
}
void CEmitterProperty::GetEmittingDirectionY(float fTime, float * pfValue)
{
	//GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventEmittingDirectionY, pfValue);
	GetTimeEventBlendValue(fTime, m_TimeEventEmittingDirectionY, pfValue);
}
void CEmitterProperty::GetEmittingDirectionZ(float fTime, float * pfValue)
{
	//GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventEmittingDirectionZ, pfValue);
	GetTimeEventBlendValue(fTime, m_TimeEventEmittingDirectionZ, pfValue);
}
void CEmitterProperty::GetEmittingVelocity(float fTime, float * pfValue)
{
	//GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventEmittingVelocity, pfValue);
	GetTimeEventBlendValue(fTime, m_TimeEventEmittingVelocity, pfValue);
}
void CEmitterProperty::GetEmissionCountPerSecond(float fTime, float * pfValue)
{
	//GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventEmissionCountPerSecond, pfValue);
	GetTimeEventBlendValue(fTime, m_TimeEventEmissionCountPerSecond, pfValue);
}
void CEmitterProperty::GetParticleLifeTime(float fTime, float * pfValue)
{
	//GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventLifeTime, pfValue);
	GetTimeEventBlendValue(fTime, m_TimeEventLifeTime, pfValue);
}
void CEmitterProperty::GetParticleSizeX(float fTime, float * pfValue)
{
	//GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventSizeX, pfValue);
	GetTimeEventBlendValue(fTime, m_TimeEventSizeX, pfValue);
}
void CEmitterProperty::GetParticleSizeY(float fTime, float * pfValue)
{
	//GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventSizeY, pfValue);
	GetTimeEventBlendValue(fTime, m_TimeEventSizeY, pfValue);
}

void CEmitterProperty::Clear()
{
	m_dwMaxEmissionCount = 0;

	m_fCycleLength = 0.0f;
	m_bCycleLoopFlag = FALSE;
	m_iLoopCount = 0;

	m_byEmitterShape = EMITTER_SHAPE_POINT;
	m_byEmitterAdvancedType = EMITTER_ADVANCED_TYPE_FREE;
	m_bEmitFromEdgeFlag = FALSE;
	m_v3EmittingSize = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fEmittingRadius = 0.0f;

	m_v3EmittingDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_TimeEventEmittingSize.clear();
	m_TimeEventEmittingDirectionX.clear();
	m_TimeEventEmittingDirectionY.clear();
	m_TimeEventEmittingDirectionZ.clear();
	m_TimeEventEmittingVelocity.clear();
	m_TimeEventEmissionCountPerSecond.clear();
	m_TimeEventLifeTime.clear();
	m_TimeEventSizeX.clear();
	m_TimeEventSizeY.clear();
	m_TimeEventEmittingAngularVelocity.clear();
}

CEmitterProperty::CEmitterProperty()
{
	Clear();
}
CEmitterProperty::~CEmitterProperty()
{
}
