#include "StdAfx.h"
#include "GrpText.h"
#include "../eterBase/Stl.h"

#include "Util.h"

CGraphicFontTexture::CGraphicFontTexture()
{
	Initialize();
}

CGraphicFontTexture::~CGraphicFontTexture()
{
	Destroy();
}

void CGraphicFontTexture::Initialize()
{
	CGraphicTexture::Initialize();
	m_hFontOld = NULL;
	m_hFont = NULL;
	m_isDirty = false;
	m_bItalic = false;
}

bool CGraphicFontTexture::IsEmpty() const
{
	return m_fontMap.size() == 0;
}

void CGraphicFontTexture::Destroy()
{
	HDC hDC = m_dib.GetDCHandle();
	if (hDC)
		SelectObject(hDC, m_hFontOld);

	m_dib.Destroy();

	m_lpd3dTexture = NULL;
	CGraphicTexture::Destroy();
	stl_wipe(m_pFontTextureVector);
	m_charInfoMap.clear();

	if (m_fontMap.size())
	{
		TFontMap::iterator i = m_fontMap.begin();

		while(i != m_fontMap.end())
		{
			DeleteObject((HGDIOBJ)i->second);
			++i;
		}

		m_fontMap.clear();
	}
	
	Initialize();
}

bool CGraphicFontTexture::CreateDeviceObjects()
{
	return true;
}

void CGraphicFontTexture::DestroyDeviceObjects()
{
}

bool CGraphicFontTexture::Create(const char* c_szFontName, int fontSize, bool bItalic)
{
	Destroy();
	
	strncpy(m_fontName, c_szFontName, sizeof(m_fontName)-1);
	m_fontSize	= fontSize;
	m_bItalic	= bItalic;

	m_x = 0;
	m_y = 0;
	m_step = 0;

	DWORD width = 256,height = 256;
	if (GetMaxTextureWidth() > 512)
		width = 512;
	if (GetMaxTextureHeight() > 512)
		height = 512;
	
	if (!m_dib.Create(ms_hDC, width, height))
		return false;

	HDC hDC = m_dib.GetDCHandle();

	m_hFont = GetFont(GetDefaultCodePage());

	m_hFontOld=(HFONT)SelectObject(hDC, m_hFont);
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC,	0);

	if (!AppendTexture())
		return false;
	
	return true;
}



HFONT CGraphicFontTexture::GetFont(WORD codePage)
{
	HFONT hFont = NULL;
	TFontMap::iterator i = m_fontMap.find(codePage);

	if(i != m_fontMap.end())
	{
		hFont = i->second;
	}
	else
	{
		LOGFONT logFont;

		memset(&logFont, 0, sizeof(LOGFONT));

		logFont.lfHeight			= m_fontSize;
		logFont.lfEscapement		= 0;
		logFont.lfOrientation		= 0;
		logFont.lfWeight			= FW_NORMAL;
		logFont.lfItalic			= (BYTE) m_bItalic;
		logFont.lfUnderline			= FALSE;
		logFont.lfStrikeOut			= FALSE;
		logFont.lfCharSet			= GetCharsetFromCodePage(codePage);
		logFont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
		logFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
		logFont.lfQuality			= ANTIALIASED_QUALITY;
		logFont.lfPitchAndFamily	= DEFAULT_PITCH;
		//Tracenf("font: %s", GetFontFaceFromCodePage(codePage));
		strcpy(logFont.lfFaceName, m_fontName); //GetFontFaceFromCodePage(codePage));
		//strcpy(logFont.lfFaceName, GetFontFaceFromCodePage(codePage));

		hFont = CreateFontIndirect(&logFont);

		m_fontMap.insert(TFontMap::value_type(codePage, hFont));
	}

	return hFont;
}

bool CGraphicFontTexture::AppendTexture()
{
	CGraphicImageTexture * pNewTexture = new CGraphicImageTexture;

	if (!pNewTexture->Create(m_dib.GetWidth(), m_dib.GetHeight(), D3DFMT_A4R4G4B4))
	{
		delete pNewTexture;
		return false;
	}

	m_pFontTextureVector.push_back(pNewTexture);
	return true;
}

bool CGraphicFontTexture::UpdateTexture()
{
	if(!m_isDirty)
		return true;

	m_isDirty = false;

	CGraphicImageTexture * pFontTexture = m_pFontTextureVector.back();

	if (!pFontTexture)
		return false;

	WORD* pwDst;
	int pitch;

	if (!pFontTexture->Lock(&pitch, (void**)&pwDst))
		return false;

	pitch /= 2;

	int width = m_dib.GetWidth();
	int height = m_dib.GetHeight();

	DWORD * pdwSrc = (DWORD*)m_dib.GetPointer();

	for (int y = 0; y < height; ++y, pwDst += pitch, pdwSrc += width)
		for (int x = 0; x < width; ++x)
			pwDst[x]=pdwSrc[x];
	
	pFontTexture->Unlock();
	return true;
}

CGraphicFontTexture::TCharacterInfomation* CGraphicFontTexture::GetCharacterInfomation(WORD codePage, wchar_t keyValue)
{
	TCharacterKey code(codePage, keyValue);

	TCharacterInfomationMap::iterator f = m_charInfoMap.find(code);

	if (m_charInfoMap.end() == f)
	{
		return UpdateCharacterInfomation(code);
	}
	else
	{
		return &f->second;	
	}
}

CGraphicFontTexture::TCharacterInfomation* CGraphicFontTexture::UpdateCharacterInfomation(TCharacterKey code)
{
	HDC hDC = m_dib.GetDCHandle();
	SelectObject(hDC, GetFont(code.first));

	wchar_t keyValue = code.second;

	if (keyValue == 0x08)
		keyValue = L' '; // ÅÇÀº °ø¹éÀ¸·Î ¹Ù²Û´Ù (¾Æ¶ø Ãâ·Â½Ã ÅÇ »ç¿ë: NAME:\tTEXT -> TEXT\t:NAME ·Î ÀüÈ¯µÊ )

	ABCFLOAT	stABC;
	SIZE		size;

	if (!GetTextExtentPoint32W(hDC, &keyValue, 1, &size) || !GetCharABCWidthsFloatW(hDC, keyValue, keyValue, &stABC))
		return NULL;

	size.cx = stABC.abcfB;
	if( stABC.abcfA > 0.0f )
		size.cx += ceilf(stABC.abcfA);
	if( stABC.abcfC > 0.0f )
		size.cx += ceilf(stABC.abcfC);
	size.cx++;

	LONG lAdvance = ceilf( stABC.abcfA + stABC.abcfB + stABC.abcfC );

	int width = m_dib.GetWidth();
	int height = m_dib.GetHeight();

	if (m_x + size.cx >= (width - 1))
	{
		m_y += (m_step + 1);
		m_step = 0;
		m_x = 0;

		if (m_y + size.cy >= (height - 1))
		{
			if (!UpdateTexture())
			{
				return NULL;
			}

			if (!AppendTexture())
				return NULL;

			m_y = 0;
		}
	}

	TextOutW(hDC, m_x, m_y, &keyValue, 1);
		
	int nChrX;
	int nChrY;
	int nChrWidth = size.cx;
	int nChrHeight = size.cy;
	int nDIBWidth = m_dib.GetWidth();

	
	DWORD*pdwDIBData=(DWORD*)m_dib.GetPointer();		
	DWORD*pdwDIBBase=pdwDIBData+nDIBWidth*m_y+m_x;
	DWORD*pdwDIBRow;
	
	pdwDIBRow=pdwDIBBase;
	for (nChrY=0; nChrY<nChrHeight; ++nChrY, pdwDIBRow+=nDIBWidth)
	{			
		for (nChrX=0; nChrX<nChrWidth; ++nChrX)
		{
			pdwDIBRow[nChrX]=(pdwDIBRow[nChrX]&0xff) ? 0xffff : 0;
		}
	}

	float rhwidth = 1.0f / float(width);
	float rhheight = 1.0f / float(height);

	TCharacterInfomation& rNewCharInfo = m_charInfoMap[code];

	rNewCharInfo.index = m_pFontTextureVector.size() - 1;
	rNewCharInfo.width = size.cx;
	rNewCharInfo.height = size.cy;
	rNewCharInfo.left = float(m_x) * rhwidth;
	rNewCharInfo.top = float(m_y) * rhheight;
	rNewCharInfo.right = float(m_x+size.cx) * rhwidth;
	rNewCharInfo.bottom = float(m_y+size.cy) * rhheight;
	rNewCharInfo.advance = (float) lAdvance;

	m_x += size.cx;

	if (m_step < size.cy)
		m_step = size.cy;	

	m_isDirty = true;

	return &rNewCharInfo;
}

bool CGraphicFontTexture::CheckTextureIndex(DWORD dwTexture)
{
	if (dwTexture >= m_pFontTextureVector.size())
		return false;

	return true;
}

void CGraphicFontTexture::SelectTexture(DWORD dwTexture)
{
	assert(CheckTextureIndex(dwTexture));
	m_lpd3dTexture = m_pFontTextureVector[dwTexture]->GetD3DTexture();
}
