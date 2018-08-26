#include "StdAfx.h"
#include "DibBar.h"
#include "BlockTexture.h"

void CDibBar::Invalidate()
{
	RECT rect = {0, 0, m_dwWidth, m_dwHeight};

	std::vector<CBlockTexture *>::iterator itor = m_kVec_pkBlockTexture.begin();
	for (; itor != m_kVec_pkBlockTexture.end(); ++itor)
	{
		CBlockTexture * pTexture = *itor;
		pTexture->InvalidateRect(rect);
	}
}

void CDibBar::SetClipRect(const RECT & c_rRect)
{
	std::vector<CBlockTexture *>::iterator itor = m_kVec_pkBlockTexture.begin();
	for (; itor != m_kVec_pkBlockTexture.end(); ++itor)
	{
		CBlockTexture * pTexture = *itor;
		assert(pTexture);
		if (!pTexture)
			continue;
		pTexture->SetClipRect(c_rRect);
	}
}

void CDibBar::ClearBar()
{
	DWORD  * pdwBuf = (DWORD *)m_dib.GetPointer();
	memset(pdwBuf, 0, m_dib.GetWidth()*m_dib.GetHeight()*4);
	Invalidate();
}

void CDibBar::Render(int ix, int iy)
{
	std::vector<CBlockTexture *>::iterator itor = m_kVec_pkBlockTexture.begin();
	for (; itor != m_kVec_pkBlockTexture.end(); ++itor)
	{
		CBlockTexture * pTexture = *itor;
		pTexture->Render(ix, iy);
	}
}

DWORD CDibBar::__NearTextureSize(DWORD dwSize)
{
	if ((dwSize & (dwSize-1)) == 0)
		return dwSize;

	DWORD dwRet = 2;
	while (dwRet < dwSize)
	{
		dwRet <<= 1;
	}

	return dwRet;
}

void CDibBar::__DivideTextureSize(DWORD dwSize, DWORD dwMax, DWORD * pdwxStep, DWORD * pdwxCount, DWORD * pdwxRest)
{
	if (dwSize<dwMax)
	{
		*pdwxStep = dwMax;
		*pdwxCount = 0;
		*pdwxRest = dwSize%dwMax;
		return;
	}

	*pdwxStep = dwMax;
	*pdwxCount = dwSize/dwMax;
	*pdwxRest = dwSize%dwMax;
}

CBlockTexture * CDibBar::__BuildTextureBlock(DWORD dwxPos, DWORD dwyPos, DWORD dwImageWidth, DWORD dwImageHeight, DWORD dwTextureWidth, DWORD dwTextureHeight)
{
	if (dwTextureWidth == 0 || dwTextureHeight == 0)
		return NULL;

	RECT posRect = {dwxPos, dwyPos, dwxPos+dwImageWidth, dwyPos+dwImageHeight};

	CBlockTexture * pBlockTexture = new CBlockTexture;
	if (!pBlockTexture->Create(&m_dib, posRect, dwTextureWidth, dwTextureHeight))
	{
		delete pBlockTexture;
		return NULL;
	}

	return pBlockTexture;
}

void CDibBar::__BuildTextureBlockList(DWORD dwWidth, DWORD dwHeight, DWORD dwMax)
{
	DWORD dwxStep, dwyStep;
	DWORD dwxCount, dwyCount;
	DWORD dwxRest, dwyRest;
	__DivideTextureSize(dwWidth, dwMax, &dwxStep, &dwxCount, &dwxRest);
	__DivideTextureSize(dwHeight, dwMax, &dwyStep, &dwyCount, &dwyRest);
	DWORD dwxTexRest = __NearTextureSize(dwxRest);
	DWORD dwyTexRest = __NearTextureSize(dwyRest);

	for (DWORD y = 0; y < dwyCount; ++y)
	{
		for (DWORD x = 0; x < dwxCount; ++x)
		{
			CBlockTexture * pTexture = __BuildTextureBlock(x*dwxStep, y*dwyStep,
														   dwxStep, dwyStep,
														   dwMax, dwMax);
			if (pTexture)
				m_kVec_pkBlockTexture.push_back(pTexture);
		}
		
		CBlockTexture * pTexture = __BuildTextureBlock(dwxCount*dwxStep, y*dwyStep,
													   dwxRest, dwyStep,
													   dwxTexRest, dwMax);

		if (pTexture)
			m_kVec_pkBlockTexture.push_back(pTexture);
	}

	for (DWORD x = 0; x < dwxCount; ++x)
	{
		CBlockTexture * pTexture = __BuildTextureBlock(x*dwxStep, dwyCount*dwyStep,
													   dwxStep, dwyRest,
													   dwMax, dwyTexRest);
		if (pTexture)
			m_kVec_pkBlockTexture.push_back(pTexture);
	}

	if (dwxRest > 0 && dwyRest > 0)
	{
		CBlockTexture * pTexture = __BuildTextureBlock(dwxCount*dwxStep, dwyCount*dwyStep,
													   dwxRest, dwyRest,
													   dwxTexRest, dwyTexRest);
		if (pTexture)
			m_kVec_pkBlockTexture.push_back(pTexture);
	}
}

bool CDibBar::Create(HDC hdc, DWORD dwWidth, DWORD dwHeight)
{
	if (!m_dib.Create(hdc, dwWidth, dwHeight))
	{
		Tracef(" Failed to create CDibBar\n");
		return false;
	}

	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;

	__BuildTextureBlockList(dwWidth, dwHeight);
	OnCreate();

	return true;
}

CDibBar::CDibBar()
{
}
CDibBar::~CDibBar()
{
}
