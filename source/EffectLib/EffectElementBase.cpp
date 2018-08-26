#include "StdAfx.h"
#include "EffectElementBase.h"


void CEffectElementBase::GetPosition(float fTime, D3DXVECTOR3 & rPosition)
{
	if (m_TimeEventTablePosition.empty())
	{
		rPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		return;
	}
	if (m_TimeEventTablePosition.size()==1)
	{
		rPosition = m_TimeEventTablePosition[0].m_vecPosition;
		return;
	}
	if (m_TimeEventTablePosition.front().m_fTime > fTime)
	{
		rPosition = m_TimeEventTablePosition.front().m_vecPosition;
		return;
	}
	if (m_TimeEventTablePosition.back().m_fTime < fTime)
	{
		rPosition = m_TimeEventTablePosition.back().m_vecPosition;
		return;
	}

	typedef TTimeEventTablePosition::iterator iterator;
	iterator result = std::lower_bound( m_TimeEventTablePosition.begin(), m_TimeEventTablePosition.end(), fTime );

	TEffectPosition & rEffectPosition = *result;
	iterator rPrev = result;
	if (m_TimeEventTablePosition.begin() != result)
	{
		rPrev = result-1;
	}
	else
	{
		rPosition = result->m_vecPosition;
		return;
	}
	TEffectPosition & rPrevEffectPosition = *rPrev;
	int iMovingType = rPrevEffectPosition.m_iMovingType;

	if (MOVING_TYPE_DIRECT == iMovingType)
	{
		float Head = fabs(rEffectPosition.m_fTime - fTime) / fabs(rEffectPosition.m_fTime - rPrevEffectPosition.m_fTime);
		float Tail = 1.0f - fabs(rEffectPosition.m_fTime - fTime) / fabs(rEffectPosition.m_fTime - rPrevEffectPosition.m_fTime);
		rPosition = (rPrevEffectPosition.m_vecPosition*Head) + (rEffectPosition.m_vecPosition*Tail);
	}
	else if (MOVING_TYPE_BEZIER_CURVE == iMovingType)
	{
		float ft = (fTime - rPrevEffectPosition.m_fTime) / (rEffectPosition.m_fTime - rPrevEffectPosition.m_fTime);

		rPosition = rPrevEffectPosition.m_vecPosition * (1.0f - ft) * (1.0f - ft) +
					(rPrevEffectPosition.m_vecPosition + rPrevEffectPosition.m_vecControlPoint) * (1.0f - ft) * ft * 2 +
					rEffectPosition.m_vecPosition * ft * ft;
	}
}

/*
bool CEffectElementBase::isVisible(float fTime)
{
	for (DWORD i = 0; i < m_TimeEventTableVisible.size(); ++i)
	{
		float fPointTime = m_TimeEventTableVisible[i];

		if (fTime < fPointTime)
		{
			if (1 == i % 2)
				return true;
			else
				return false;
		}
	}

	return 1 == (m_TimeEventTableVisible.size() % 2);
}

void CEffectElementBase::GetAlpha(float fTime, float * pAlpha)
{
	GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventAlpha, pAlpha);
}

void CEffectElementBase::GetScale(float fTime, float * pScale)
{
	GetTimeEventBlendValue<TTimeEventTableFloat, float>(fTime, m_TimeEventScale, pScale);
}
*/

bool CEffectElementBase::isData()
{
	return OnIsData();
}

void CEffectElementBase::Clear()
{
	m_fStartTime = 0.0f;

	OnClear();
}

BOOL CEffectElementBase::LoadScript(CTextFileLoader & rTextFileLoader)
{
	CTokenVector * pTokenVector;
	if (!rTextFileLoader.GetTokenFloat("starttime",&m_fStartTime))
	{
		m_fStartTime = 0.0f;
	}
	if (rTextFileLoader.GetTokenVector("timeeventposition", &pTokenVector))
	{	
		m_TimeEventTablePosition.clear();
		
		DWORD dwIndex = 0;
		for (DWORD i = 0; i < pTokenVector->size(); ++dwIndex)
		{
			TEffectPosition EffectPosition;
			EffectPosition.m_fTime = atof(pTokenVector->at(i++).c_str());
			if (pTokenVector->at(i)=="MOVING_TYPE_BEZIER_CURVE")
			{
				i++;

				EffectPosition.m_iMovingType = MOVING_TYPE_BEZIER_CURVE;

				EffectPosition.m_vecPosition.x = atof(pTokenVector->at(i++).c_str());
				EffectPosition.m_vecPosition.y = atof(pTokenVector->at(i++).c_str());
				EffectPosition.m_vecPosition.z = atof(pTokenVector->at(i++).c_str());

				EffectPosition.m_vecControlPoint.x = atof(pTokenVector->at(i++).c_str());
				EffectPosition.m_vecControlPoint.y = atof(pTokenVector->at(i++).c_str());
				EffectPosition.m_vecControlPoint.z = atof(pTokenVector->at(i++).c_str());
			}
			else if (pTokenVector->at(i) == "MOVING_TYPE_DIRECT")
			{
				i++;

				EffectPosition.m_iMovingType = MOVING_TYPE_DIRECT;

				EffectPosition.m_vecPosition.x = atof(pTokenVector->at(i++).c_str());
				EffectPosition.m_vecPosition.y = atof(pTokenVector->at(i++).c_str());
				EffectPosition.m_vecPosition.z = atof(pTokenVector->at(i++).c_str());

				EffectPosition.m_vecControlPoint = D3DXVECTOR3(0.0f,0.0f,0.0f);
			}
			else
			{
				return FALSE;
			}

			m_TimeEventTablePosition.push_back(EffectPosition);
		}
	}	
	
	return OnLoadScript(rTextFileLoader);
}

float CEffectElementBase::GetStartTime()
{
	return m_fStartTime;
}

CEffectElementBase::CEffectElementBase()
{
	m_fStartTime = 0.0f;
}
CEffectElementBase::~CEffectElementBase()
{
}
