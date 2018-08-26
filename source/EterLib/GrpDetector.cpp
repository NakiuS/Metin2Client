#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpDetector.h"

struct FIsEqualD3DDisplayMode
{
	FIsEqualD3DDisplayMode(D3DDISPLAYMODE* pkD3DDMChk)
	{
		m_pkD3DDMChk=pkD3DDMChk;
	}
	BOOL operator() (D3DDISPLAYMODE& rkD3DDMTest)
	{
		if (rkD3DDMTest.Width!=m_pkD3DDMChk->Width)
			return FALSE;
		
		if (rkD3DDMTest.Height!=m_pkD3DDMChk->Height)
			return FALSE;
		
		if (rkD3DDMTest.Format!=m_pkD3DDMChk->Format)
			return FALSE;
		
		return TRUE;
	}

	D3DDISPLAYMODE* m_pkD3DDMChk;
};

static int CompareD3DDisplayModeOrder( const VOID* arg1, const VOID* arg2 )
{
    D3DDISPLAYMODE* p1 = (D3DDISPLAYMODE*)arg1;
    D3DDISPLAYMODE* p2 = (D3DDISPLAYMODE*)arg2;

    if( p1->Format > p2->Format )   return -1;
    if( p1->Format < p2->Format )   return +1;
    if( p1->Width  < p2->Width )    return -1;
    if( p1->Width  > p2->Width )    return +1;
    if( p1->Height < p2->Height )   return -1;
    if( p1->Height > p2->Height )   return +1;

    return 0;
}



/////////////////////////////////////////////////////////////////////////////////

UINT D3D_CAdapterDisplayModeList::GetDisplayModeNum()
{
	return m_uD3DDMNum;
}

UINT D3D_CAdapterDisplayModeList::GetPixelFormatNum()
{
	return m_uD3DFmtNum;
}


const D3DDISPLAYMODE&	D3D_CAdapterDisplayModeList::GetDisplayModer(UINT iD3DDM)
{
	assert(iD3DDM<m_uD3DDMNum);
	return m_akD3DDM[iD3DDM];
}

const D3DFORMAT&		D3D_CAdapterDisplayModeList::GetPixelFormatr(UINT iD3DFmt)
{
	assert(iD3DFmt<m_uD3DFmtNum);
	return m_aeD3DFmt[iD3DFmt];
}

VOID D3D_CAdapterDisplayModeList::Build(IDirect3D8& rkD3D, D3DFORMAT eD3DFmtDefault, UINT iD3DAdapterInfo)
{
	D3DDISPLAYMODE* akD3DDM=m_akD3DDM;
	D3DFORMAT* aeD3DFmt=m_aeD3DFmt;	

	UINT uD3DDMNum=0;
	UINT uD3DFmtNum=0;

	aeD3DFmt[uD3DFmtNum++]=eD3DFmtDefault;

	UINT uAdapterModeNum=rkD3D.GetAdapterModeCount(iD3DAdapterInfo);
	for (UINT iD3DAdapterInfoMode=0; iD3DAdapterInfoMode<uAdapterModeNum; iD3DAdapterInfoMode++)
	{			
		D3DDISPLAYMODE kD3DDMCur;
		rkD3D.EnumAdapterModes(iD3DAdapterInfo, iD3DAdapterInfoMode, &kD3DDMCur);

		// IsFilterOutLowResolutionMode
		if( kD3DDMCur.Width  < FILTEROUT_LOWRESOLUTION_WIDTH || kD3DDMCur.Height < FILTEROUT_LOWRESOLUTION_HEIGHT )
			continue;
		
		// FindDisplayMode
		D3DDISPLAYMODE* pkD3DDMEnd=akD3DDM+uD3DDMNum;
		D3DDISPLAYMODE* pkD3DDMFind=std::find_if(akD3DDM, pkD3DDMEnd, FIsEqualD3DDisplayMode(&kD3DDMCur));

		// IsNewDisplayMode
		if (pkD3DDMFind==pkD3DDMEnd && uD3DDMNum<D3DDISPLAYMODE_MAX)
		{
			D3DDISPLAYMODE& rkD3DDMNew=akD3DDM[uD3DDMNum++];					
			rkD3DDMNew.Width=kD3DDMCur.Width;
			rkD3DDMNew.Height=kD3DDMCur.Height;
			rkD3DDMNew.Format=kD3DDMCur.Format;									

			// FindFormat
			D3DFORMAT* peD3DFmtEnd=aeD3DFmt+uD3DFmtNum;
			D3DFORMAT* peD3DFmtFind=std::find(aeD3DFmt, peD3DFmtEnd, kD3DDMCur.Format);

			// IsNewFormat
			if (peD3DFmtFind==peD3DFmtEnd && uD3DFmtNum<D3DFORMAT_MAX)
			{
				aeD3DFmt[uD3DFmtNum++]=kD3DDMCur.Format;
			}											
		}				
	}

	qsort(akD3DDM, uD3DDMNum, sizeof(D3DDISPLAYMODE), CompareD3DDisplayModeOrder);				

	m_uD3DFmtNum=uD3DFmtNum;
	m_uD3DDMNum=uD3DDMNum;
}

/////////////////////////////////////////////////////////////////////////////////

VOID D3D_SModeInfo::GetString(std::string* pstEnumList)
{		
	UINT uScrDepthBits=16;
	switch (m_eD3DFmtPixel)
	{
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8R8G8B8:
		case D3DFMT_R8G8B8:
			uScrDepthBits=32;
			break;
	}

	int iVP=0;

	switch (m_dwD3DBehavior)
	{
		case D3DCREATE_HARDWARE_VERTEXPROCESSING:
			iVP=1;
			break;
		case D3DCREATE_MIXED_VERTEXPROCESSING:
			iVP=2;
			break;
		case D3DCREATE_SOFTWARE_VERTEXPROCESSING:
			iVP=3;
			break;
	}

	static const char* szVP[4]=
	{
		"UNKNOWN",
		"HWVP",
		"MXVP",
		"SWVP",
	};

	char szText[1024+1];
	_snprintf(szText, sizeof(szText), "%dx%dx%d %s\r\n", m_uScrWidth, m_uScrHeight, uScrDepthBits, szVP[iVP]);
	pstEnumList->append(szText);
}

/////////////////////////////////////////////////////////////////////////////////

const CHAR*			D3D_CDeviceInfo::msc_aszD3DDevDesc[D3DDEVICETYPE_NUM] = {"HAL", "REF"};
const D3DDEVTYPE	D3D_CDeviceInfo::msc_aeD3DDevType[D3DDEVICETYPE_NUM]={D3DDEVTYPE_HAL, D3DDEVTYPE_REF};
	

UINT D3D_CDeviceInfo::GetD3DModeInfoNum()
{
	return m_uD3DModeInfoNum;
}

D3D_SModeInfo* D3D_CDeviceInfo::GetD3DModeInfop(UINT iD3D_SModeInfo)
{
	if (iD3D_SModeInfo >= m_uD3DModeInfoNum)
		return NULL;

	return &m_akD3DModeInfo[iD3D_SModeInfo];
}

BOOL D3D_CDeviceInfo::FindDepthStencilFormat(IDirect3D8& rkD3D, UINT iD3DAdapterInfo, D3DDEVTYPE DeviceType, D3DFORMAT TargetFormat, D3DFORMAT* pDepthStencilFormat )
{
    UINT m_dwMinDepthBits    = 16;
    UINT m_dwMinStencilBits  = 0;

    if( m_dwMinDepthBits <= 16 && m_dwMinStencilBits == 0 )
    {
        if( SUCCEEDED( rkD3D.CheckDeviceFormat( iD3DAdapterInfo, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 ) ) )
        {
            if( SUCCEEDED( rkD3D.CheckDepthStencilMatch( iD3DAdapterInfo, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D16 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D16;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 15 && m_dwMinStencilBits <= 1 )
    {
        if( SUCCEEDED( rkD3D.CheckDeviceFormat( iD3DAdapterInfo, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1 ) ) )
        {
            if( SUCCEEDED( rkD3D.CheckDepthStencilMatch( iD3DAdapterInfo, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D15S1 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D15S1;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits == 0 )
    {
        if( SUCCEEDED( rkD3D.CheckDeviceFormat( iD3DAdapterInfo, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 ) ) )
        {
            if( SUCCEEDED( rkD3D.CheckDepthStencilMatch( iD3DAdapterInfo, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D24X8 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D24X8;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits <= 8 )
    {
        if( SUCCEEDED( rkD3D.CheckDeviceFormat( iD3DAdapterInfo, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 ) ) )
        {
            if( SUCCEEDED( rkD3D.CheckDepthStencilMatch( iD3DAdapterInfo, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D24S8 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D24S8;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits <= 4 )
    {
        if( SUCCEEDED( rkD3D.CheckDeviceFormat( iD3DAdapterInfo, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4 ) ) )
        {
            if( SUCCEEDED( rkD3D.CheckDepthStencilMatch( iD3DAdapterInfo, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D24X4S4 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D24X4S4;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 32 && m_dwMinStencilBits == 0 )
    {
        if( SUCCEEDED( rkD3D.CheckDeviceFormat( iD3DAdapterInfo, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32 ) ) )
        {
            if( SUCCEEDED( rkD3D.CheckDepthStencilMatch( iD3DAdapterInfo, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D32 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D32;
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL D3D_CDeviceInfo::Build(IDirect3D8& rkD3D, UINT iD3DAdapterInfo, UINT iDevType, D3D_CAdapterDisplayModeList& rkD3DADMList, BOOL (*pfnConfirmDevice)(D3DCAPS8& rkD3DCaps, UINT uBehavior, D3DFORMAT eD3DFmt))
{	
	assert(pfnConfirmDevice!=NULL && "D3D_CDeviceInfo::Build");

	const D3DDEVTYPE	c_eD3DDevType=msc_aeD3DDevType[iDevType];
	const TCHAR*		c_szD3DDevDesc=msc_aszD3DDevDesc[iDevType];

    m_eD3DDevType = c_eD3DDevType;
    rkD3D.GetDeviceCaps(iD3DAdapterInfo, c_eD3DDevType, &m_kD3DCaps);

    m_szDevDesc = c_szD3DDevDesc;
    m_uD3DModeInfoNum=0;
    m_canDoWindowed = FALSE;
    m_isWindowed = FALSE;
    m_eD3DMSTFullscreen = D3DMULTISAMPLE_NONE;
    m_eD3DMSTWindowed = D3DMULTISAMPLE_NONE;

	BOOL  aisFormatConfirmed[20];
	DWORD adwD3DBehavior[20];
	D3DFORMAT aeD3DFmtDepthStencil[20];
	
    BOOL isHALExists = FALSE;
    BOOL isHALWindowedCompatible = FALSE;
    BOOL isHALDesktopCompatible = FALSE;
    BOOL isHALSampleCompatible = FALSE;

	// GetFlagInfo
	{
		UINT uD3DFmtNum=rkD3DADMList.GetPixelFormatNum();

		for (DWORD iFmt=0; iFmt<uD3DFmtNum; ++iFmt)
		{
			D3DFORMAT eD3DFmtPixel=rkD3DADMList.GetPixelFormatr(iFmt);		
			DWORD dwD3DBehavior=0;
			BOOL isFormatConfirmed=FALSE;			

			aeD3DFmtDepthStencil[iFmt] = D3DFMT_UNKNOWN;

			// SkipNoRenderTargetFormat;
			if (FAILED(rkD3D.CheckDeviceType(iD3DAdapterInfo, m_eD3DDevType, eD3DFmtPixel, eD3DFmtPixel, FALSE)))
				continue;

			if (D3DDEVTYPE_HAL==m_eD3DDevType)
			{
				isHALExists=TRUE;
				
				if (m_kD3DCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED)
				{
					isHALWindowedCompatible=TRUE;

					if (iFmt==0)
						isHALDesktopCompatible=TRUE;
                
				}
			}

			// Confirm the device/format for HW vertex processing
			if (m_kD3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			{
				if (m_kD3DCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
				{
					dwD3DBehavior=D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE;

					if (pfnConfirmDevice(m_kD3DCaps, dwD3DBehavior, eD3DFmtPixel))
						isFormatConfirmed = TRUE;
				}

				if (FALSE == isFormatConfirmed)
				{
					dwD3DBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;

					if (pfnConfirmDevice(m_kD3DCaps, dwD3DBehavior, eD3DFmtPixel))
						isFormatConfirmed = TRUE;
				}

				if (FALSE == isFormatConfirmed)
				{
					dwD3DBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;

					if (pfnConfirmDevice(m_kD3DCaps, dwD3DBehavior, eD3DFmtPixel))
						isFormatConfirmed = TRUE;
				}	
			}

			// Confirm the device/format for SW vertex processing        
			if (FALSE == isFormatConfirmed)
			{
				dwD3DBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				
				if (pfnConfirmDevice(m_kD3DCaps, dwD3DBehavior, eD3DFmtPixel))
					isFormatConfirmed = TRUE;            
			}
			
			if (isFormatConfirmed)
			{
				if (!FindDepthStencilFormat(rkD3D, iD3DAdapterInfo, c_eD3DDevType, eD3DFmtPixel, &aeD3DFmtDepthStencil[iFmt]))
					isFormatConfirmed = TRUE;
            
			}
			
			adwD3DBehavior[iFmt]=dwD3DBehavior;
			aisFormatConfirmed[iFmt]=isFormatConfirmed;
		}
	}

	// BuildModeInfoList
	{
		UINT uD3DDMNum=rkD3DADMList.GetDisplayModeNum();
		UINT uD3DFmtNum=rkD3DADMList.GetPixelFormatNum();


		for (UINT iD3DDM=0; iD3DDM<uD3DDMNum; ++iD3DDM)
		{
			const D3DDISPLAYMODE& c_rkD3DDM=rkD3DADMList.GetDisplayModer(iD3DDM);
			for (DWORD iFmt=0; iFmt<uD3DFmtNum; ++iFmt)
			{			
				if (rkD3DADMList.GetPixelFormatr(iFmt)==c_rkD3DDM.Format)
				{
					if (aisFormatConfirmed[iFmt] == TRUE )
					{
						D3D_SModeInfo& rkModeInfo=m_akD3DModeInfo[m_uD3DModeInfoNum++];
						rkModeInfo.m_uScrWidth=c_rkD3DDM.Width;
						rkModeInfo.m_uScrHeight=c_rkD3DDM.Height;
						rkModeInfo.m_eD3DFmtPixel=c_rkD3DDM.Format;
						rkModeInfo.m_dwD3DBehavior=adwD3DBehavior[iFmt];
						rkModeInfo.m_eD3DFmtDepthStencil=aeD3DFmtDepthStencil[iFmt];
						
						if( m_eD3DDevType == D3DDEVTYPE_HAL )
							isHALSampleCompatible = TRUE;
					}
				}
			}
		}
	}

	// Check if the device is compatible with the desktop display mode
	// (which was added initially as formats[0])
	if (aisFormatConfirmed[0] && (m_kD3DCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) )
	{
		m_canDoWindowed=TRUE;
		m_isWindowed=TRUE;
	}
	
	if (m_uD3DModeInfoNum>0)
		return TRUE;

	return FALSE;
}

BOOL D3D_CDeviceInfo::Find(UINT uScrWidth, UINT uScrHeight, UINT uScrDepthBits, BOOL isWindowed, UINT* piD3DModeInfo)
{
	if (isWindowed)
		if (!m_isWindowed)
			return FALSE;

	for (UINT iD3D_SModeInfo=0; iD3D_SModeInfo<m_uD3DModeInfoNum; ++iD3D_SModeInfo)
	{
		D3D_SModeInfo& rkModeInfo=m_akD3DModeInfo[iD3D_SModeInfo];
		if (rkModeInfo.m_uScrWidth==uScrWidth && rkModeInfo.m_uScrHeight==uScrHeight)
		{
			if (uScrDepthBits==16)
			{
				switch (rkModeInfo.m_eD3DFmtPixel)
				{
					case D3DFMT_R5G6B5:
					case D3DFMT_X1R5G5B5:
					case D3DFMT_A1R5G5B5:
						*piD3DModeInfo=iD3D_SModeInfo;
						return TRUE;
						break;
				}
			}
			else
			{
				switch (rkModeInfo.m_eD3DFmtPixel)
				{
					case D3DFMT_X8R8G8B8:
					case D3DFMT_A8R8G8B8:
					case D3DFMT_R8G8B8:
						*piD3DModeInfo=iD3D_SModeInfo;
						return TRUE;
						break;
				}
			}
		}		
	}
	return FALSE;
}

VOID D3D_CDeviceInfo::GetString(std::string* pstEnumList)
{
	char szText[1024+1];
	_snprintf(szText, sizeof(szText), "%s\r\n========================================\r\n", m_szDevDesc);
	pstEnumList->append(szText);
	
	for (UINT iD3D_SModeInfo=0; iD3D_SModeInfo<m_uD3DModeInfoNum; ++iD3D_SModeInfo)
	{
		_snprintf(szText, sizeof(szText), "%d. ", iD3D_SModeInfo);
		pstEnumList->append(szText);

		D3D_SModeInfo& rkModeInfo=m_akD3DModeInfo[iD3D_SModeInfo];
		rkModeInfo.GetString(pstEnumList);
	}
	
	pstEnumList->append("\r\n");
}

/////////////////////////////////////////////////////////////////////////////

D3DDISPLAYMODE&	D3D_CAdapterInfo::GetDesktopD3DDisplayModer()
{
	return m_kD3DDMDesktop;
}

D3DDISPLAYMODE*	D3D_CAdapterInfo::GetDesktopD3DDisplayModep()
{
	return &m_kD3DDMDesktop;
}

D3D_CDeviceInfo* D3D_CAdapterInfo::GetD3DDeviceInfop(UINT iD3DDevInfo)
{
	if (iD3DDevInfo >= m_uD3DDevInfoNum)
		return NULL;

	return &m_akD3DDevInfo[iD3DDevInfo];	
}

D3D_SModeInfo* D3D_CAdapterInfo::GetD3DModeInfop(UINT iD3DDevInfo, UINT iD3D_SModeInfo)
{
	D3D_CDeviceInfo* pkD3DDevInfo=GetD3DDeviceInfop(iD3DDevInfo);
	if (pkD3DDevInfo)
	{
		D3D_SModeInfo* pkD3DModeInfo=pkD3DDevInfo->GetD3DModeInfop(iD3D_SModeInfo);		
		if (pkD3DModeInfo)
			return pkD3DModeInfo;
	}
	return NULL;
}

BOOL D3D_CAdapterInfo::Find(UINT uScrWidth, UINT uScrHeight, UINT uScrDepthBits, BOOL isWindowed, UINT* piD3DModeInfo, UINT* piD3DDevInfo)
{
	for (UINT iDevInfo=0; iDevInfo<m_uD3DDevInfoNum; ++iDevInfo)
	{		
        D3D_CDeviceInfo& rkD3DDevInfo=m_akD3DDevInfo[iDevInfo];
		if (rkD3DDevInfo.Find(uScrWidth, uScrHeight, uScrDepthBits, isWindowed, piD3DModeInfo))
		{
			*piD3DDevInfo=iDevInfo;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL D3D_CAdapterInfo::Build(IDirect3D8& rkD3D, UINT iD3DAdapterInfo, PFNCONFIRMDEVICE pfnConfirmDevice)
{
	D3DDISPLAYMODE& rkD3DDMDesktop=m_kD3DDMDesktop;
	if (FAILED(rkD3D.GetAdapterDisplayMode(iD3DAdapterInfo, &rkD3DDMDesktop)))
		return FALSE;

	rkD3D.GetAdapterIdentifier(iD3DAdapterInfo, D3DENUM_NO_WHQL_LEVEL, &m_kD3DAdapterIdentifier);
	
	m_iCurD3DDevInfo=0;
	m_uD3DDevInfoNum=0;

	D3D_CAdapterDisplayModeList kD3DADMList;
	kD3DADMList.Build(rkD3D, m_kD3DDMDesktop.Format, iD3DAdapterInfo);

	D3D_CDeviceInfo* akD3DDevInfo=m_akD3DDevInfo;
	for (UINT iDevType=0; iDevType<D3DDEVICETYPE_NUM; ++iDevType)
	{		
        D3D_CDeviceInfo& rkD3DDevInfo=akD3DDevInfo[m_uD3DDevInfoNum];
		if (rkD3DDevInfo.Build(rkD3D, iD3DAdapterInfo, iDevType, kD3DADMList, pfnConfirmDevice))
			++m_uD3DDevInfoNum;
	}
	
	if (m_uD3DDevInfoNum>0)
		return TRUE;

	return FALSE;
}

VOID D3D_CAdapterInfo::GetString(std::string* pstEnumList)
{
	for (UINT iDevInfo=0; iDevInfo<m_uD3DDevInfoNum; ++iDevInfo)
	{		
		char szText[1024+1];
		_snprintf(szText, sizeof(szText), "Device %d\r\n", iDevInfo);
		pstEnumList->append(szText);

        D3D_CDeviceInfo& rkD3DDevInfo=m_akD3DDevInfo[iDevInfo];
		rkD3DDevInfo.GetString(pstEnumList);			
	}	
}

/////////////////////////////////////////////////////////////////////////////

D3D_CDisplayModeAutoDetector::D3D_CDisplayModeAutoDetector()
{
	m_uD3DAdapterInfoCount=0;
}

D3D_CDisplayModeAutoDetector::~D3D_CDisplayModeAutoDetector()
{
}

D3D_CAdapterInfo* D3D_CDisplayModeAutoDetector::GetD3DAdapterInfop(UINT iD3DAdapterInfo)
{
	if (iD3DAdapterInfo >= m_uD3DAdapterInfoCount)
		return NULL;

	return &m_akD3DAdapterInfo[iD3DAdapterInfo];
}

D3D_SModeInfo* D3D_CDisplayModeAutoDetector::GetD3DModeInfop(UINT iD3DAdapterInfo, UINT iD3DDevInfo, UINT iD3D_SModeInfo)
{	       
	D3D_CAdapterInfo* pkD3DAdapterInfo=GetD3DAdapterInfop(iD3DAdapterInfo);
	if (pkD3DAdapterInfo)
	{
		D3D_CDeviceInfo* pkD3DDevInfo=pkD3DAdapterInfo->GetD3DDeviceInfop(iD3DDevInfo);
		if (pkD3DDevInfo)
		{
			D3D_SModeInfo* pkD3D_SModeInfo=pkD3DDevInfo->GetD3DModeInfop(iD3D_SModeInfo);		
			if (pkD3D_SModeInfo)
				return pkD3D_SModeInfo;
		}
	}
	return NULL;	
}

BOOL D3D_CDisplayModeAutoDetector::Find(UINT uScrWidth, UINT uScrHeight, UINT uScrDepthBits, BOOL isWindowed, UINT* piD3DModeInfo, UINT* piD3DDevInfo, UINT* piD3DAdapterInfo)
{
	for (UINT iD3DAdapterInfo=0; iD3DAdapterInfo<m_uD3DAdapterInfoCount; ++iD3DAdapterInfo)
	{
		D3D_CAdapterInfo& rkAdapterInfo=m_akD3DAdapterInfo[iD3DAdapterInfo];
		if (rkAdapterInfo.Find(uScrWidth, uScrHeight, uScrDepthBits, isWindowed, piD3DModeInfo, piD3DDevInfo))
		{
			*piD3DAdapterInfo=iD3DAdapterInfo;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL D3D_CDisplayModeAutoDetector::Build(IDirect3D8& rkD3D, PFNCONFIRMDEVICE pfnConfirmDevice)
{
	m_uD3DAdapterInfoCount=0;

	UINT uTotalAdapterCount=rkD3D.GetAdapterCount();	
	uTotalAdapterCount=min(uTotalAdapterCount, D3DADAPTERINFO_NUM);

	for (UINT iD3DAdapterInfo=0; iD3DAdapterInfo<uTotalAdapterCount; ++iD3DAdapterInfo)
	{
		D3D_CAdapterInfo& rkAdapterInfo=m_akD3DAdapterInfo[m_uD3DAdapterInfoCount];	
		if (rkAdapterInfo.Build(rkD3D, iD3DAdapterInfo, pfnConfirmDevice))
			++m_uD3DAdapterInfoCount;
	}

	if (m_uD3DAdapterInfoCount>0)
		return TRUE;

	return FALSE;
}

VOID D3D_CDisplayModeAutoDetector::GetString(std::string* pstEnumList)
{
	for (UINT iD3DAdapterInfo=0; iD3DAdapterInfo<m_uD3DAdapterInfoCount; ++iD3DAdapterInfo)
	{
		char szText[1024+1];
		_snprintf(szText, sizeof(szText), "Adapter %d\r\n", iD3DAdapterInfo);
		pstEnumList->append(szText);

		D3D_CAdapterInfo& rkAdapterInfo=m_akD3DAdapterInfo[iD3DAdapterInfo];	
		rkAdapterInfo.GetString(pstEnumList);
	}
}