#include "StdAfx.h"
#include "ParticleSystemData.h"
#include "EffectUpdateDecorator.h"
#include "ParticleInstance.h"

CDynamicPool<CParticleSystemData>		CParticleSystemData::ms_kPool;

void CParticleSystemData::DestroySystem()
{
	ms_kPool.Destroy();
}

CParticleSystemData* CParticleSystemData::New()
{
	return ms_kPool.Alloc();
}

void CParticleSystemData::Delete(CParticleSystemData* pkData)
{
	pkData->Clear();
	ms_kPool.Free(pkData);
}

CEmitterProperty * CParticleSystemData::GetEmitterPropertyPointer()
{
	return &m_EmitterProperty;
}
CParticleProperty * CParticleSystemData::GetParticlePropertyPointer()
{
	return &m_ParticleProperty;
}

BOOL CParticleSystemData::OnLoadScript(CTextFileLoader & rTextFileLoader)
{
	{
	CTextFileLoader::CGotoChild GotoChild(&rTextFileLoader, "emitterproperty");

	if (!rTextFileLoader.GetTokenDoubleWord("maxemissioncount", &m_EmitterProperty.m_dwMaxEmissionCount))
		return FALSE;

	if (!rTextFileLoader.GetTokenFloat("cyclelength", &m_EmitterProperty.m_fCycleLength))
	{
		m_EmitterProperty.m_fCycleLength = 0.05f;
	}
	if (!rTextFileLoader.GetTokenBoolean("cycleloopenable", &m_EmitterProperty.m_bCycleLoopFlag))
	{
		m_EmitterProperty.m_bCycleLoopFlag = FALSE;
	}
	if (!rTextFileLoader.GetTokenInteger("loopcount",&m_EmitterProperty.m_iLoopCount))
	{
		m_EmitterProperty.m_iLoopCount = 0;
	}

	if (!rTextFileLoader.GetTokenByte("emittershape", &m_EmitterProperty.m_byEmitterShape))
		return FALSE;
	
	if (!rTextFileLoader.GetTokenByte("emitteradvancedtype", &m_EmitterProperty.m_byEmitterAdvancedType))
	{
		m_EmitterProperty.m_byEmitterShape = CEmitterProperty::EMITTER_ADVANCED_TYPE_FREE;
	}
	if (!rTextFileLoader.GetTokenPosition("emittingsize", &m_EmitterProperty.m_v3EmittingSize))
	{
		m_EmitterProperty.m_v3EmittingSize = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	if (!rTextFileLoader.GetTokenFloat("emittingradius", &m_EmitterProperty.m_fEmittingRadius))
	{
		m_EmitterProperty.m_fEmittingRadius = 0.0f;
	}

	if (!rTextFileLoader.GetTokenBoolean("emitteremitfromedgeflag", &m_EmitterProperty.m_bEmitFromEdgeFlag))
	{
		m_EmitterProperty.m_bEmitFromEdgeFlag = FALSE;
	}
	
	if (!rTextFileLoader.GetTokenPosition("emittingdirection", &m_EmitterProperty.m_v3EmittingDirection))
	{
		m_EmitterProperty.m_v3EmittingDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventemittingsize", &m_EmitterProperty.m_TimeEventEmittingSize))
	{
		m_EmitterProperty.m_TimeEventEmittingSize.clear();
		TTimeEventTypeFloat TimeEventFloat;
		TimeEventFloat.m_fTime = 0.0f;
		TimeEventFloat.m_Value = 0.0f;
		m_EmitterProperty.m_TimeEventEmittingSize.push_back(TimeEventFloat);
	}
	
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventemittingangularvelocity", &m_EmitterProperty.m_TimeEventEmittingAngularVelocity))
	{
		m_EmitterProperty.m_TimeEventEmittingAngularVelocity.clear();
		TTimeEventTypeFloat TimeEventFloat;
		TimeEventFloat.m_fTime = 0.0f;
		TimeEventFloat.m_Value = 0.0f;
		m_EmitterProperty.m_TimeEventEmittingAngularVelocity.push_back(TimeEventFloat);
	}
	
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventemittingdirectionx", &m_EmitterProperty.m_TimeEventEmittingDirectionX))
	{
		m_EmitterProperty.m_TimeEventEmittingDirectionX.clear();
		TTimeEventTypeFloat TimeEventFloat;
		TimeEventFloat.m_fTime = 0.0f;
		TimeEventFloat.m_Value = 0.0f;
		m_EmitterProperty.m_TimeEventEmittingDirectionX.push_back(TimeEventFloat);
	}
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventemittingdirectiony", &m_EmitterProperty.m_TimeEventEmittingDirectionY))
	{
		m_EmitterProperty.m_TimeEventEmittingDirectionY.clear();
		TTimeEventTypeFloat TimeEventFloat;
		TimeEventFloat.m_fTime = 0.0f;
		TimeEventFloat.m_Value = 0.0f;
		m_EmitterProperty.m_TimeEventEmittingDirectionY.push_back(TimeEventFloat);
	}
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventemittingdirectionz", &m_EmitterProperty.m_TimeEventEmittingDirectionZ))
	{
		m_EmitterProperty.m_TimeEventEmittingDirectionZ.clear();
		TTimeEventTypeFloat TimeEventFloat;
		TimeEventFloat.m_fTime = 0.0f;
		TimeEventFloat.m_Value = 0.0f;
		m_EmitterProperty.m_TimeEventEmittingDirectionZ.push_back(TimeEventFloat);
	}
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventemittingvelocity", &m_EmitterProperty.m_TimeEventEmittingVelocity))
	{
		m_EmitterProperty.m_TimeEventEmittingVelocity.clear();
		TTimeEventTypeFloat TimeEventFloat;
		TimeEventFloat.m_fTime = 0.0f;
		TimeEventFloat.m_Value = 0.0f;
		m_EmitterProperty.m_TimeEventEmittingVelocity.push_back(TimeEventFloat);
	}
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventemissioncountpersecond", &m_EmitterProperty.m_TimeEventEmissionCountPerSecond))
		return FALSE;
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventlifetime", &m_EmitterProperty.m_TimeEventLifeTime))
		return FALSE;
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventsizex", &m_EmitterProperty.m_TimeEventSizeX))
		return FALSE;
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventsizey", &m_EmitterProperty.m_TimeEventSizeY))
		return FALSE;
	}

	{
	CTextFileLoader::CGotoChild GotoChild(&rTextFileLoader, "particleproperty");

	if (!rTextFileLoader.GetTokenByte("srcblendtype", &m_ParticleProperty.m_bySrcBlendType))
	{
		m_ParticleProperty.m_bySrcBlendType = D3DBLEND_SRCALPHA;
	}
	if (!rTextFileLoader.GetTokenByte("destblendtype", &m_ParticleProperty.m_byDestBlendType))
	{
		m_ParticleProperty.m_byDestBlendType = D3DBLEND_ONE;
	}
	if (!rTextFileLoader.GetTokenByte("coloroperationtype", &m_ParticleProperty.m_byColorOperationType))
	{
		m_ParticleProperty.m_byColorOperationType = D3DTOP_MODULATE;
	}

	if (!rTextFileLoader.GetTokenByte("billboardtype", &m_ParticleProperty.m_byBillboardType))
		return FALSE;

	if (!rTextFileLoader.GetTokenByte("rotationtype", &m_ParticleProperty.m_byRotationType))
		return FALSE;
	if (!rTextFileLoader.GetTokenFloat("rotationspeed", &m_ParticleProperty.m_fRotationSpeed))
		return FALSE;
	if (!rTextFileLoader.GetTokenWord("rotationrandomstartingbegin", &m_ParticleProperty.m_wRotationRandomStartingBegin))
		return FALSE;
	if (!rTextFileLoader.GetTokenWord("rotationrandomstartingend", &m_ParticleProperty.m_wRotationRandomStartingEnd))
		return FALSE;

	if (!rTextFileLoader.GetTokenBoolean("attachenable", &m_ParticleProperty.m_bAttachFlag))
	{
		m_ParticleProperty.m_bAttachFlag = FALSE;
	}
	if (!rTextFileLoader.GetTokenBoolean("stretchenable", &m_ParticleProperty.m_bStretchFlag))
		return FALSE;

	if (!rTextFileLoader.GetTokenByte("texanitype", &m_ParticleProperty.m_byTexAniType))
		return FALSE;
	if (!rTextFileLoader.GetTokenFloat("texanidelay", &m_ParticleProperty.m_fTexAniDelay))
		return FALSE;
	if (!rTextFileLoader.GetTokenBoolean("texanirandomstartframeenable", &m_ParticleProperty.m_bTexAniRandomStartFrameFlag))
		return FALSE;

	float fGravity;

	if (rTextFileLoader.GetTokenFloat("gravity", &fGravity))
	{
		TTimeEventTypeFloat f;
		f.m_fTime = 0.0f;
		f.m_Value = fGravity;
		m_ParticleProperty.m_TimeEventGravity.push_back(f);
	}
	else if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventgravity", &m_ParticleProperty.m_TimeEventGravity))
	{
		m_ParticleProperty.m_TimeEventGravity.clear();
	}

	float fAirResistance;
	if (rTextFileLoader.GetTokenFloat("airresistance", &fAirResistance))
	{
		TTimeEventTypeFloat f;
		f.m_fTime = 0.0f;
		f.m_Value = fAirResistance;
		m_ParticleProperty.m_TimeEventAirResistance.push_back(f);
	}
	else if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventairresistance", &m_ParticleProperty.m_TimeEventAirResistance))
	{
		m_ParticleProperty.m_TimeEventAirResistance.clear();
	}

	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventscalex", &m_ParticleProperty.m_TimeEventScaleX))
		return FALSE;
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventscaley", &m_ParticleProperty.m_TimeEventScaleY))
		return FALSE;

#ifdef WORLD_EDITOR
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventcolorred", &m_ParticleProperty.m_TimeEventColorRed))
		return FALSE;
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventcolorgreen", &m_ParticleProperty.m_TimeEventColorGreen))
		return FALSE;
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventcolorblue", &m_ParticleProperty.m_TimeEventColorBlue))
		return FALSE;

	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventalpha", &m_ParticleProperty.m_TimeEventAlpha))
		return FALSE;
#else
	TTimeEventTableFloat TimeEventR;
	TTimeEventTableFloat TimeEventB;
	TTimeEventTableFloat TimeEventG;
	TTimeEventTableFloat TimeEventA;
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventcolorred", &TimeEventR))
		return FALSE;
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventcolorgreen", &TimeEventG))
		return FALSE;
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventcolorblue", &TimeEventB))
		return FALSE;
	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventalpha", &TimeEventA))
		return FALSE;

	m_ParticleProperty.m_TimeEventColor.clear();
	{
		std::set<float> times;
		int i;
		for(i=0;i<TimeEventR.size();i++)
			times.insert(TimeEventR[i].m_fTime);
		for(i=0;i<TimeEventG.size();i++)
			times.insert(TimeEventG[i].m_fTime);
		for(i=0;i<TimeEventB.size();i++)
			times.insert(TimeEventB[i].m_fTime);
		for(i=0;i<TimeEventA.size();i++)
			times.insert(TimeEventA[i].m_fTime);
		std::set<float>::iterator it;
		for(it = times.begin(); it != times.end(); ++it)
		{
			float fTime = *it;
			float fR, fG, fB, fA;
			GetTimeEventBlendValue<float>(fTime, TimeEventR, &fR);
			GetTimeEventBlendValue<float>(fTime, TimeEventG, &fG);
			GetTimeEventBlendValue<float>(fTime, TimeEventB, &fB);
			GetTimeEventBlendValue<float>(fTime, TimeEventA, &fA);
			TTimeEventTypeColor t;
			t.m_fTime = fTime;
			D3DXCOLOR c;
			c.r = fR;
			c.g = fG;
			c.b = fB;
			c.a = fA;
			t.m_Value.m_dwColor = /*(DWORD)*/ (DWORD)c;
			m_ParticleProperty.m_TimeEventColor.push_back(t);
		}
	}
#endif

	if (!GetTokenTimeEventFloat(rTextFileLoader, "timeeventrotation", &m_ParticleProperty.m_TimeEventRotation))
		return FALSE;

	CTokenVector * pTextureVector;

	if (!rTextFileLoader.GetTokenVector("texturefiles", &pTextureVector))
		return FALSE;

	for (DWORD i = 0; i < pTextureVector->size(); ++i)
	{
		std::string strTextureFileName = pTextureVector->at(i).c_str();

		if (!IsGlobalFileName(strTextureFileName.c_str()))
			strTextureFileName = GetOnlyPathName(rTextFileLoader.GetFileName()) + strTextureFileName;

		m_ParticleProperty.InsertTexture(strTextureFileName.c_str());
	}
	}

	return TRUE;
}

void CParticleSystemData::ChangeTexture(const char * c_szFileName)
{
	m_ParticleProperty.SetTexture(c_szFileName);
}

void CParticleSystemData::OnClear()
{
	m_EmitterProperty.Clear();
	m_ParticleProperty.Clear();
}

bool CParticleSystemData::OnIsData()
{
	return true;
}

void CParticleSystemData::BuildDecorator(CParticleInstance * pInstance)
{
	using namespace NEffectUpdateDecorator;
	
	pInstance->m_pDecorator = new CNullDecorator;
	
	//////
	
	if (m_ParticleProperty.m_TimeEventAirResistance.size()>1)
	{
		pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(new CAirResistanceDecorator);
		pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(
			new CAirResistanceValueDecorator(m_ParticleProperty.m_TimeEventAirResistance, &pInstance->m_fAirResistance)
			);
	}
	else if (m_ParticleProperty.m_TimeEventAirResistance.size()==1)
	{
		pInstance->m_fAirResistance = m_ParticleProperty.m_TimeEventAirResistance[0].m_Value;
		pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(new CAirResistanceDecorator);
	}
	
	if (m_ParticleProperty.m_TimeEventGravity.size() > 1)
	{
		pInstance->m_pDecorator = pInstance->m_pDecorator->AddChainFront(new CGravityDecorator);
		pInstance->m_pDecorator = pInstance->m_pDecorator->AddChainFront(
			new CGravityValueDecorator(m_ParticleProperty.m_TimeEventGravity, &pInstance->m_fGravity)
			);
	}
	else if (m_ParticleProperty.m_TimeEventGravity.size() == 1)
	{
		pInstance->m_fGravity = m_ParticleProperty.m_TimeEventGravity[0].m_Value;
		pInstance->m_pDecorator = pInstance->m_pDecorator->AddChainFront(new CGravityDecorator);
	}
#ifdef WORLD_EDITOR
	pInstance->m_pDecorator = pInstance->m_pDecorator->AddChainFront(
	new CColorValueDecorator(m_ParticleProperty.m_TimeEventColorRed, &pInstance->m_Color.r));
	pInstance->m_pDecorator = pInstance->m_pDecorator->AddChainFront(
	new CColorValueDecorator(m_ParticleProperty.m_TimeEventColorGreen, &pInstance->m_Color.g));
	pInstance->m_pDecorator = pInstance->m_pDecorator->AddChainFront(
	new CColorValueDecorator(m_ParticleProperty.m_TimeEventColorBlue, &pInstance->m_Color.b));
	pInstance->m_pDecorator = pInstance->m_pDecorator->AddChainFront(
	new CColorValueDecorator(m_ParticleProperty.m_TimeEventAlpha, &pInstance->m_Color.a));
#else
	pInstance->m_pDecorator = pInstance->m_pDecorator->AddChainFront(
		new CColorAllDecorator(m_ParticleProperty.m_TimeEventColor, &pInstance->m_dcColor));
#endif
	
	pInstance->m_pDecorator = pInstance->m_pDecorator->AddChainFront(
		new CScaleValueDecorator(m_ParticleProperty.m_TimeEventScaleX, &pInstance->m_v2Scale.x));
	pInstance->m_pDecorator = pInstance->m_pDecorator->AddChainFront(
		new CScaleValueDecorator(m_ParticleProperty.m_TimeEventScaleY, &pInstance->m_v2Scale.y));
	
	if (m_ParticleProperty.GetTextureAnimationFrameCount()>1 &&m_ParticleProperty.GetTextureAnimationFrameDelay()>1e-6)
	{
		switch (pInstance->m_byTextureAnimationType)
		{
			case CParticleProperty::TEXTURE_ANIMATION_TYPE_CW:
				pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(
					new CTextureAnimationCWDecorator(m_ParticleProperty.GetTextureAnimationFrameDelay(), m_ParticleProperty.GetTextureAnimationFrameCount(), &pInstance->m_byFrameIndex));
				break;
			case CParticleProperty::TEXTURE_ANIMATION_TYPE_CCW:
				pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(
					new CTextureAnimationCCWDecorator(m_ParticleProperty.GetTextureAnimationFrameDelay(), m_ParticleProperty.GetTextureAnimationFrameCount(), &pInstance->m_byFrameIndex));
				break;
			case CParticleProperty::TEXTURE_ANIMATION_TYPE_RANDOM_FRAME:
				pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(
					new CTextureAnimationRandomDecorator(m_ParticleProperty.GetTextureAnimationFrameDelay(), m_ParticleProperty.GetTextureAnimationFrameCount(), &pInstance->m_byFrameIndex));
				break;
		}
	}

	BYTE byRotationType = m_ParticleProperty.m_byRotationType;

	if (m_ParticleProperty.m_fRotationSpeed==0.0f && byRotationType!=CParticleProperty::ROTATION_TYPE_TIME_EVENT)
	{
		byRotationType = CParticleProperty::ROTATION_TYPE_NONE;
	}
	else if (byRotationType==CParticleProperty::ROTATION_TYPE_RANDOM_DIRECTION)
	{
		byRotationType = (random()&1)?CParticleProperty::ROTATION_TYPE_CW:CParticleProperty::ROTATION_TYPE_CCW;
	}

	switch(byRotationType)
	{
		case CParticleProperty::ROTATION_TYPE_TIME_EVENT:
			pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(
				new CRotationDecorator());
			pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(
				new CRotationSpeedValueDecorator(m_ParticleProperty.m_TimeEventRotation,&pInstance->m_fRotationSpeed));
			break;

		case CParticleProperty::ROTATION_TYPE_CW:
			pInstance->m_fRotationSpeed = m_ParticleProperty.m_fRotationSpeed;
			pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(
				new CRotationDecorator());
			break;

		case CParticleProperty::ROTATION_TYPE_CCW:
			pInstance->m_fRotationSpeed = - m_ParticleProperty.m_fRotationSpeed;
			pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(
				new CRotationDecorator());
			break;		
	}

	/////

	pInstance->m_pDecorator=pInstance->m_pDecorator->AddChainFront(new CHeaderDecorator);
}

CParticleSystemData::CParticleSystemData()
{
}
CParticleSystemData::~CParticleSystemData()
{
}
