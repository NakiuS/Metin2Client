#include "StdAfx.h"
#include "GrpDevice.h"
#include "../eterBase/Stl.h"
#include "../eterBase/Debug.h"

bool GRAPHICS_CAPS_CAN_NOT_DRAW_LINE = false;
bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW = false;
bool GRAPHICS_CAPS_HALF_SIZE_IMAGE = false;
bool GRAPHICS_CAPS_CAN_NOT_TEXTURE_ADDRESS_BORDER = false;
bool GRAPHICS_CAPS_SOFTWARE_TILING = false;

D3DPRESENT_PARAMETERS g_kD3DPP;
bool g_isBrowserMode=false;
RECT g_rcBrowser;

CGraphicDevice::CGraphicDevice()
: m_uBackBufferCount(0)
{
	__Initialize();
}

CGraphicDevice::~CGraphicDevice()
{
	Destroy();
}

void CGraphicDevice::__Initialize()
{
	ms_iD3DAdapterInfo=D3DADAPTER_DEFAULT;
	ms_iD3DDevInfo=D3DADAPTER_DEFAULT;
	ms_iD3DModeInfo=D3DADAPTER_DEFAULT;

	ms_lpd3d			= NULL;
	ms_lpd3dDevice		= NULL;
	ms_lpd3dMatStack	= NULL;

	ms_dwWavingEndTime = 0;
	ms_dwFlashingEndTime = 0;

	m_pStateManager		= NULL;

	__InitializeDefaultIndexBufferList();
	__InitializePDTVertexBufferList();
}

void CGraphicDevice::RegisterWarningString(UINT uiMsg, const char * c_szString)
{
	m_kMap_strWarningMessage[uiMsg] = c_szString;
}

void CGraphicDevice::__WarningMessage(HWND hWnd, UINT uiMsg)
{
	if (m_kMap_strWarningMessage.end() == m_kMap_strWarningMessage.find(uiMsg))
		return;
	MessageBox(hWnd, m_kMap_strWarningMessage[uiMsg].c_str(), "Warning", MB_OK|MB_TOPMOST);
}

void CGraphicDevice::MoveWebBrowserRect(const RECT& c_rcWebPage)
{
	g_rcBrowser=c_rcWebPage;
}

void CGraphicDevice::EnableWebBrowserMode(const RECT& c_rcWebPage)
{
	if (!ms_lpd3dDevice)
		return;

	D3DPRESENT_PARAMETERS& rkD3DPP=ms_d3dPresentParameter;
	
	g_isBrowserMode=true;

	if (D3DSWAPEFFECT_COPY==rkD3DPP.SwapEffect)
		return;

	g_kD3DPP=rkD3DPP;
	g_rcBrowser=c_rcWebPage;
	
	//rkD3DPP.Windowed=TRUE;
	rkD3DPP.SwapEffect=D3DSWAPEFFECT_COPY;
	rkD3DPP.BackBufferCount = 1;
	rkD3DPP.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	
	IDirect3DDevice8& rkD3DDev=*ms_lpd3dDevice;
	HRESULT hr=rkD3DDev.Reset(&rkD3DPP);
	if (FAILED(hr))
		return;
	
	STATEMANAGER.SetDefaultState();	
}

void CGraphicDevice::DisableWebBrowserMode()
{
	if (!ms_lpd3dDevice)
		return;

	D3DPRESENT_PARAMETERS& rkD3DPP=ms_d3dPresentParameter;
	
	g_isBrowserMode=false;

	rkD3DPP=g_kD3DPP;

	IDirect3DDevice8& rkD3DDev=*ms_lpd3dDevice;
	HRESULT hr=rkD3DDev.Reset(&rkD3DPP);
	if (FAILED(hr))
		return;
	
	STATEMANAGER.SetDefaultState();	
}
		
bool CGraphicDevice::ResizeBackBuffer(UINT uWidth, UINT uHeight)
{
	if (!ms_lpd3dDevice)
		return false;

	D3DPRESENT_PARAMETERS& rkD3DPP=ms_d3dPresentParameter;
	if (rkD3DPP.Windowed)
	{
		if (rkD3DPP.BackBufferWidth!=uWidth || rkD3DPP.BackBufferHeight!=uHeight)
		{
			rkD3DPP.BackBufferWidth=uWidth;
			rkD3DPP.BackBufferHeight=uHeight;

			IDirect3DDevice8& rkD3DDev=*ms_lpd3dDevice;

			HRESULT hr=rkD3DDev.Reset(&rkD3DPP);
			if (FAILED(hr))
			{
				return false;
			}

			STATEMANAGER.SetDefaultState();
		}
	}

	return true;
}

DWORD CGraphicDevice::CreatePNTStreamVertexShader()
{
	assert(ms_lpd3dDevice != NULL);
	
	DWORD declVector[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),
		D3DVSD_REG(3, D3DVSDT_FLOAT3),
		D3DVSD_REG(7, D3DVSDT_FLOAT2),
		D3DVSD_END()
	};
	
	DWORD ret;
	
	if (FAILED(ms_lpd3dDevice->CreateVertexShader(&declVector[0], NULL, &ret, 0)))
		return 0;
	
	return ret;
}

DWORD CGraphicDevice::CreatePNT2StreamVertexShader()
{
	assert(ms_lpd3dDevice != NULL);

	DWORD declVector[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),
		D3DVSD_REG(3, D3DVSDT_FLOAT3),
		D3DVSD_REG(7, D3DVSDT_FLOAT2),
		D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2),
//		D3DVSD_STREAM(1),
		D3DVSD_END()
	};

	DWORD ret;

	if (FAILED(ms_lpd3dDevice->CreateVertexShader(&declVector[0], NULL, &ret, 0)))
		return 0;

	return ret;
}

DWORD CGraphicDevice::CreatePTStreamVertexShader()
{
	assert(ms_lpd3dDevice != NULL);

	DWORD declVector[] = 
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),
		D3DVSD_STREAM(1),
		D3DVSD_REG(7, D3DVSDT_FLOAT2),
		D3DVSD_END()
	};

	DWORD ret;

	if (FAILED(ms_lpd3dDevice->CreateVertexShader(&declVector[0], NULL, &ret, 0)))
		return 0;

	return (ret);
}

DWORD CGraphicDevice::CreateDoublePNTStreamVertexShader()
{
	assert(ms_lpd3dDevice != NULL);

	DWORD declVector[] = 
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),
		D3DVSD_REG(3, D3DVSDT_FLOAT3),
		D3DVSD_REG(7, D3DVSDT_FLOAT2),
		D3DVSD_STREAM(1),
		D3DVSD_REG(D3DVSDE_POSITION2, D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_NORMAL2, D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2),
		D3DVSD_END()
	};

	DWORD ret;

	if (FAILED(ms_lpd3dDevice->CreateVertexShader(&declVector[0], NULL, &ret, 0)))
		return 0;

	return ret;
}

CGraphicDevice::EDeviceState CGraphicDevice::GetDeviceState()
{
	if (!ms_lpd3dDevice)
		return DEVICESTATE_NULL;

	HRESULT hr;

	if (FAILED(hr = ms_lpd3dDevice->TestCooperativeLevel()))
	{
		if (D3DERR_DEVICELOST == hr)
			return DEVICESTATE_BROKEN;

		if (D3DERR_DEVICENOTRESET == hr)
			return DEVICESTATE_NEEDS_RESET;

		return DEVICESTATE_BROKEN;
	}
	
	return DEVICESTATE_OK;
}

bool CGraphicDevice::Reset()
{
	HRESULT hr;

	if (FAILED(hr = ms_lpd3dDevice->Reset(&ms_d3dPresentParameter)))
		return false;

	return true;
}

static LPDIRECT3DSURFACE8 s_lpStencil;
static DWORD   s_MaxTextureWidth, s_MaxTextureHeight;

BOOL EL3D_ConfirmDevice(D3DCAPS8& rkD3DCaps, UINT uBehavior, D3DFORMAT /*eD3DFmt*/)
{
	// PUREDEVICE는 GetTransform / GetViewport 등이 되지 않는다.
	if (uBehavior & D3DCREATE_PUREDEVICE) 
        return FALSE;
	
	if (uBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING) 
	{	
		// DirectionalLight
		if (!(rkD3DCaps.VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
			return FALSE;
		
		// PositionalLight
		if (!(rkD3DCaps.VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS))
			return FALSE;

		// Software T&L Support - ATI NOT SUPPORT CLIP, USE DIRECTX SOFTWARE PROCESSING CLIPPING
		if (GRAPHICS_CAPS_SOFTWARE_TILING)
		{
			if (!(rkD3DCaps.PrimitiveMiscCaps & D3DPMISCCAPS_CLIPTLVERTS))
				return FALSE;
		}
		else
		{
			// Shadow/Terrain
			if (!(rkD3DCaps.VertexProcessingCaps & D3DVTXPCAPS_TEXGEN))
				return FALSE;
		}
	}

	s_MaxTextureWidth = rkD3DCaps.MaxTextureWidth;
	s_MaxTextureHeight = rkD3DCaps.MaxTextureHeight;
	
	return TRUE;
}

DWORD GetMaxTextureWidth()
{
	return s_MaxTextureWidth;
}

DWORD GetMaxTextureHeight()
{
	return s_MaxTextureHeight;
}

bool CGraphicDevice::__IsInDriverBlackList(D3D_CAdapterInfo& rkD3DAdapterInfo)
{
	D3DADAPTER_IDENTIFIER8& rkD3DAdapterIdentifier=rkD3DAdapterInfo.GetIdentifier();

	char szSrcDriver[256];
	strncpy(szSrcDriver, rkD3DAdapterIdentifier.Driver, sizeof(szSrcDriver)-1);
	DWORD dwSrcHighVersion=rkD3DAdapterIdentifier.DriverVersion.QuadPart>>32;
	DWORD dwSrcLowVersion=rkD3DAdapterIdentifier.DriverVersion.QuadPart&0xffffffff;

	bool ret=false;
		
	FILE* fp=fopen("grpblk.txt", "r");
	if (fp)
	{
		DWORD dwChkHighVersion;
		DWORD dwChkLowVersion;

		char szChkDriver[256];

		char szLine[256];
		while (fgets(szLine, sizeof(szLine)-1, fp))
		{			
			sscanf(szLine, "%s %x %x", szChkDriver, &dwChkHighVersion, &dwChkLowVersion);
			
			if (strcmp(szSrcDriver, szChkDriver)==0)
				if (dwSrcHighVersion==dwChkHighVersion)
					if (dwSrcLowVersion==dwChkLowVersion)
					{
						ret=true;				
						break;
					}

			szLine[0]='\0';
		}
		fclose(fp);
	}

	return ret;
}

int CGraphicDevice::Create(HWND hWnd, int iHres, int iVres, bool Windowed, int /*iBit*/, int iReflashRate)
{
	int iRet = CREATE_OK;

	Destroy();

	ms_iWidth	= iHres;
	ms_iHeight	= iVres;

	ms_hWnd		= hWnd;
	ms_hDC		= GetDC(hWnd);
	ms_lpd3d	= Direct3DCreate8(D3D_SDK_VERSION);

	if (!ms_lpd3d)
		return CREATE_NO_DIRECTX;

	if (!ms_kD3DDetector.Build(*ms_lpd3d, EL3D_ConfirmDevice))
		return CREATE_ENUM;

	if (!ms_kD3DDetector.Find(800, 600, 32, TRUE, &ms_iD3DModeInfo, &ms_iD3DDevInfo, &ms_iD3DAdapterInfo))
		return CREATE_DETECT;

	std::string stDevList;
	ms_kD3DDetector.GetString(&stDevList);

	//Tracen(stDevList.c_str());
	//Tracenf("adapter %d, device %d, mode %d", ms_iD3DAdapterInfo, ms_iD3DDevInfo, ms_iD3DModeInfo);

	D3D_CAdapterInfo * pkD3DAdapterInfo = ms_kD3DDetector.GetD3DAdapterInfop(ms_iD3DAdapterInfo);
	if (!pkD3DAdapterInfo)
	{
		Tracenf("adapter %d is EMPTY", ms_iD3DAdapterInfo);
		return CREATE_DETECT;
	}

	if (__IsInDriverBlackList(*pkD3DAdapterInfo))
	{
		iRet |= CREATE_BAD_DRIVER;
		__WarningMessage(hWnd, CREATE_BAD_DRIVER);
	}

	D3D_SModeInfo * pkD3DModeInfo = pkD3DAdapterInfo->GetD3DModeInfop(ms_iD3DDevInfo, ms_iD3DModeInfo);		
	if (!pkD3DModeInfo)
	{
		Tracenf("device %d, mode %d is EMPTY", ms_iD3DDevInfo, ms_iD3DModeInfo);
		return CREATE_DETECT;
	}

	D3DADAPTER_IDENTIFIER8& rkD3DAdapterId=pkD3DAdapterInfo->GetIdentifier();
	if (Windowed &&
		strnicmp(rkD3DAdapterId.Driver, "3dfx", 4)==0 &&
		22 == pkD3DAdapterInfo->GetDesktopD3DDisplayModer().Format)
	{
		return CREATE_FORMAT;
	}

	if (pkD3DModeInfo->m_dwD3DBehavior==D3DCREATE_SOFTWARE_VERTEXPROCESSING)
	{
		iRet |= CREATE_NO_TNL;

		// DISABLE_NOTIFY_NOT_SUPPORT_TNL_MESSAGE
		//__WarningMessage(hWnd, CREATE_NO_TNL);
		// END_OF_DISABLE_NOTIFY_NOT_SUPPORT_TNL_MESSAGE
	}

	std::string stModeInfo;
	pkD3DModeInfo->GetString(&stModeInfo);

	//Tracen(stModeInfo.c_str());

	int ErrorCorrection = 0;

RETRY:
	ZeroMemory(&ms_d3dPresentParameter, sizeof(ms_d3dPresentParameter));
	
	ms_d3dPresentParameter.Windowed							= Windowed;
	ms_d3dPresentParameter.BackBufferWidth					= iHres;
	ms_d3dPresentParameter.BackBufferHeight					= iVres;
	ms_d3dPresentParameter.hDeviceWindow					= hWnd;
	ms_d3dPresentParameter.BackBufferCount					= m_uBackBufferCount;
	ms_d3dPresentParameter.SwapEffect						= D3DSWAPEFFECT_DISCARD;

	if (Windowed)
	{
		ms_d3dPresentParameter.BackBufferFormat				= pkD3DAdapterInfo->GetDesktopD3DDisplayModer().Format;
	}
	else
	{
		ms_d3dPresentParameter.BackBufferFormat				= pkD3DModeInfo->m_eD3DFmtPixel;
		ms_d3dPresentParameter.FullScreen_RefreshRateInHz	= iReflashRate;
	}

	ms_d3dPresentParameter.Flags							= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	ms_d3dPresentParameter.EnableAutoDepthStencil			= TRUE;
	ms_d3dPresentParameter.AutoDepthStencilFormat			= pkD3DModeInfo->m_eD3DFmtDepthStencil;

	ms_dwD3DBehavior = pkD3DModeInfo->m_dwD3DBehavior;

	if (FAILED(ms_hLastResult = ms_lpd3d->CreateDevice(
				ms_iD3DAdapterInfo,
				D3DDEVTYPE_HAL,
				hWnd,
				// 2004. 1. 9 myevan 버텍스 프로세싱 방식 자동 선택 추가
				pkD3DModeInfo->m_dwD3DBehavior,
				&ms_d3dPresentParameter,
				&ms_lpd3dDevice)))
	{
		switch (ms_hLastResult)
		{
			case D3DERR_INVALIDCALL:
				Tracen("IDirect3DDevice.CreateDevice - ERROR D3DERR_INVALIDCALL\nThe method call is invalid. For example, a method's parameter may have an invalid value.");					
				break;
			case D3DERR_NOTAVAILABLE:
				Tracen("IDirect3DDevice.CreateDevice - ERROR D3DERR_NOTAVAILABLE\nThis device does not support the queried technique. ");
				break;
			case D3DERR_OUTOFVIDEOMEMORY:
				Tracen("IDirect3DDevice.CreateDevice - ERROR D3DERR_OUTOFVIDEOMEMORY\nDirect3D does not have enough display memory to perform the operation");
				break;
			default:
				Tracenf("IDirect3DDevice.CreateDevice - ERROR %d", ms_hLastResult);
				break;
		}

		if (ErrorCorrection)
			return CREATE_DEVICE;
	
		// 2004. 1. 9 myevan 큰의미 없는 코드인듯.. 에러나면 표시하고 종료하자
		iReflashRate = 0;
		++ErrorCorrection;
		iRet = CREATE_REFRESHRATE;
		goto RETRY;
	}

	// Check DXT Support Info
	if(ms_lpd3d->CheckDeviceFormat(
				ms_iD3DAdapterInfo, 
				D3DDEVTYPE_HAL,
				ms_d3dPresentParameter.BackBufferFormat,
				0,
				D3DRTYPE_TEXTURE,
				D3DFMT_DXT1) == D3DERR_NOTAVAILABLE)
	{
		ms_bSupportDXT = false;
	}

	if(ms_lpd3d->CheckDeviceFormat(
				ms_iD3DAdapterInfo, 
				D3DDEVTYPE_HAL,
				ms_d3dPresentParameter.BackBufferFormat,
				0,
				D3DRTYPE_TEXTURE,
				D3DFMT_DXT3) == D3DERR_NOTAVAILABLE)
	{
		ms_bSupportDXT = false;
	}

	if(ms_lpd3d->CheckDeviceFormat(
				ms_iD3DAdapterInfo, 
				D3DDEVTYPE_HAL,
				ms_d3dPresentParameter.BackBufferFormat,
				0,
				D3DRTYPE_TEXTURE,
				D3DFMT_DXT5) == D3DERR_NOTAVAILABLE)
	{
		ms_bSupportDXT = false;
	}	

	if (FAILED((ms_hLastResult = ms_lpd3dDevice->GetDeviceCaps(&ms_d3dCaps))))
	{
		Tracenf("IDirect3DDevice.GetDeviceCaps - ERROR %d", ms_hLastResult);
		return CREATE_GET_DEVICE_CAPS2;
	}

	if (!Windowed)
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, iHres, iVres, SWP_SHOWWINDOW);

	//Tracef("vertex shader version : %X\n",(DWORD)ms_d3dCaps.VertexShaderVersion);

	ms_lpd3dDevice->GetViewport(&ms_Viewport);

	m_pStateManager = new CStateManager(ms_lpd3dDevice);

	D3DXCreateMatrixStack(0, &ms_lpd3dMatStack);
	ms_lpd3dMatStack->LoadIdentity();

	ms_ptVS	= CreatePTStreamVertexShader();
	ms_pntVS = CreatePNTStreamVertexShader();
	ms_pnt2VS = CreatePNT2StreamVertexShader();

	D3DXMatrixIdentity(&ms_matIdentity);
	D3DXMatrixIdentity(&ms_matView);
	D3DXMatrixIdentity(&ms_matProj);
	D3DXMatrixIdentity(&ms_matInverseView);
	D3DXMatrixIdentity(&ms_matInverseViewYAxis);
	D3DXMatrixIdentity(&ms_matScreen0);
	D3DXMatrixIdentity(&ms_matScreen1);
	D3DXMatrixIdentity(&ms_matScreen2);

	ms_matScreen0._11 = 1;
	ms_matScreen0._22 = -1;	

	ms_matScreen1._41 = 1;
	ms_matScreen1._42 = 1;

	ms_matScreen2._11 = (float) iHres / 2;
	ms_matScreen2._22 = (float) iVres / 2;
	
	D3DXCreateSphere(ms_lpd3dDevice, 1.0f, 32, 32, &ms_lpSphereMesh, NULL);
	D3DXCreateCylinder(ms_lpd3dDevice, 1.0f, 1.0f, 1.0f, 8, 8, &ms_lpCylinderMesh, NULL);

	ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

	if (!__CreateDefaultIndexBufferList())
		return false;

	if (!__CreatePDTVertexBufferList())
		return false;
	
	DWORD dwTexMemSize = GetAvailableTextureMemory();

	if (dwTexMemSize < 64 * 1024 * 1024)
		ms_isLowTextureMemory = true;
	else
		ms_isLowTextureMemory = false;

	if (dwTexMemSize > 100 * 1024 * 1024)
		ms_isHighTextureMemory = true;
	else
		ms_isHighTextureMemory = false;

	if (ms_d3dCaps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER)
		GRAPHICS_CAPS_CAN_NOT_TEXTURE_ADDRESS_BORDER=false;
	else
		GRAPHICS_CAPS_CAN_NOT_TEXTURE_ADDRESS_BORDER=true;

	//D3DADAPTER_IDENTIFIER8& rkD3DAdapterId=pkD3DAdapterInfo->GetIdentifier();
	if (strnicmp(rkD3DAdapterId.Driver, "SIS", 3) == 0)
	{
		GRAPHICS_CAPS_CAN_NOT_DRAW_LINE = true;
		GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW = true;
		GRAPHICS_CAPS_HALF_SIZE_IMAGE = true;
		ms_isLowTextureMemory = true;
	}
	else if (strnicmp(rkD3DAdapterId.Driver, "3dfx", 4) == 0)
	{
		GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW = true;
		GRAPHICS_CAPS_HALF_SIZE_IMAGE = true;
		ms_isLowTextureMemory = true;
	}

	return (iRet);
}

void CGraphicDevice::__InitializePDTVertexBufferList()
{
	for (UINT i=0; i<PDT_VERTEXBUFFER_NUM; ++i)
		ms_alpd3dPDTVB[i]=NULL;	
}
		
void CGraphicDevice::__DestroyPDTVertexBufferList()
{
	for (UINT i=0; i<PDT_VERTEXBUFFER_NUM; ++i)
	{
		if (ms_alpd3dPDTVB[i])
		{
			ms_alpd3dPDTVB[i]->Release();
			ms_alpd3dPDTVB[i]=NULL;
		}
	}
}

bool CGraphicDevice::__CreatePDTVertexBufferList()
{
	for (UINT i=0; i<PDT_VERTEXBUFFER_NUM; ++i)
	{
		if (FAILED(
			ms_lpd3dDevice->CreateVertexBuffer(
			sizeof(TPDTVertex)*PDT_VERTEX_NUM, 
			D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 
			D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1, 
			D3DPOOL_SYSTEMMEM, 
			&ms_alpd3dPDTVB[i])
		))
		return false;
	}
	return true;
}

void CGraphicDevice::__InitializeDefaultIndexBufferList()
{
	for (UINT i=0; i<DEFAULT_IB_NUM; ++i)
		ms_alpd3dDefIB[i]=NULL;
}

void CGraphicDevice::__DestroyDefaultIndexBufferList()
{
	for (UINT i=0; i<DEFAULT_IB_NUM; ++i)
		if (ms_alpd3dDefIB[i])
		{
			ms_alpd3dDefIB[i]->Release();
			ms_alpd3dDefIB[i]=NULL;
		}	
}

bool CGraphicDevice::__CreateDefaultIndexBuffer(UINT eDefIB, UINT uIdxCount, const WORD* c_awIndices)
{
	assert(ms_alpd3dDefIB[eDefIB]==NULL);

	if (FAILED(
		ms_lpd3dDevice->CreateIndexBuffer(
			sizeof(WORD)*uIdxCount, 
			D3DUSAGE_WRITEONLY, 
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&ms_alpd3dDefIB[eDefIB])
	)) return false;
	
	WORD* dstIndices;
	if (FAILED(
		ms_alpd3dDefIB[eDefIB]->Lock(0, 0, (BYTE**)&dstIndices, 0)
	)) return false;

	memcpy(dstIndices, c_awIndices, sizeof(WORD)*uIdxCount);

	ms_alpd3dDefIB[eDefIB]->Unlock();

	return true;
}

bool CGraphicDevice::__CreateDefaultIndexBufferList()
{
	static const WORD c_awLineIndices[2] = { 0, 1, };
	static const WORD c_awLineTriIndices[6] = { 0, 1, 0, 2, 1, 2, };
	static const WORD c_awLineRectIndices[8] = { 0, 1, 0, 2, 1, 3, 2, 3,};
	static const WORD c_awLineCubeIndices[24] = { 
		0, 1, 0, 2, 1, 3, 2, 3,
		0, 4, 1, 5, 2, 6, 3, 7,
		4, 5, 4, 6, 5, 7, 6, 7,
	};
	static const WORD c_awFillTriIndices[3]= { 0, 1, 2, };
	static const WORD c_awFillRectIndices[6] = { 0, 2, 1, 2, 3, 1, };
	static const WORD c_awFillCubeIndices[36] = { 
		0, 1, 2, 1, 3, 2,
		2, 0, 6, 0, 4, 6,
		0, 1, 4, 1, 5, 4,
		1, 3, 5, 3, 7, 5,
		3, 2, 7, 2, 6, 7,
		4, 5, 6, 5, 7, 6,
	};
	
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_LINE, 2, c_awLineIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_LINE_TRI, 6, c_awLineTriIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_LINE_RECT, 8, c_awLineRectIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_LINE_CUBE, 24, c_awLineCubeIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_FILL_TRI, 3, c_awFillTriIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_FILL_RECT, 6, c_awFillRectIndices))
		return false;
	if (!__CreateDefaultIndexBuffer(DEFAULT_IB_FILL_CUBE, 36, c_awFillCubeIndices))
		return false;
	
	return true;
}

void CGraphicDevice::InitBackBufferCount(UINT uBackBufferCount)
{
	m_uBackBufferCount=uBackBufferCount;
}

void CGraphicDevice::Destroy()
{
	__DestroyPDTVertexBufferList();
	__DestroyDefaultIndexBufferList();

	if (ms_hDC)
	{
		ReleaseDC(ms_hWnd, ms_hDC);
		ms_hDC = NULL;
	}

	if (ms_ptVS)
	{	
		ms_lpd3dDevice->DeleteVertexShader(ms_ptVS);
		ms_ptVS = 0;;
	}

	if (ms_pntVS)
	{	
		ms_lpd3dDevice->DeleteVertexShader(ms_pntVS);
		ms_pntVS = 0;
	}

	if (ms_pnt2VS)
	{	
		ms_lpd3dDevice->DeleteVertexShader(ms_pnt2VS);
		ms_pnt2VS = 0;
	}

	safe_release(ms_lpSphereMesh);
	safe_release(ms_lpCylinderMesh);

	safe_release(ms_lpd3dMatStack);
	safe_release(ms_lpd3dDevice);
	safe_release(ms_lpd3d);	

	if (m_pStateManager)
	{
		delete m_pStateManager;
		m_pStateManager = NULL;
	}

	__Initialize();
}
