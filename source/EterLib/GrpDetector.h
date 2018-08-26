#pragma once

#include <d3d8.h>

typedef BOOL (*PFNCONFIRMDEVICE) (D3DCAPS8& rkD3DCaps, UINT uBehavior, D3DFORMAT eD3DFmt);

enum
{
	D3DDEVICETYPE_HAL,
	D3DDEVICETYPE_REF,
	D3DDEVICETYPE_NUM,
};

struct D3D_SModeInfo
{
	UINT m_uScrWidth;     
	UINT m_uScrHeight;   
	UINT m_uScrDepthBit;
 	UINT m_dwD3DBehavior;
	D3DFORMAT m_eD3DFmtPixel;
	D3DFORMAT m_eD3DFmtDepthStencil;

	VOID GetString(std::string* pstEnumList);
};

class D3D_CAdapterDisplayModeList
{
	public:
		D3D_CAdapterDisplayModeList() {}
		~D3D_CAdapterDisplayModeList() {}
		VOID Build(IDirect3D8& rkD3D, D3DFORMAT eD3DFmtDefault, UINT iAdapter);

		UINT GetDisplayModeNum();
		UINT GetPixelFormatNum();
		
		const D3DDISPLAYMODE&	GetDisplayModer(UINT iD3DDM);
		const D3DFORMAT&		GetPixelFormatr(UINT iD3DFmt);

	protected:
		enum
		{			
			D3DDISPLAYMODE_MAX = 100,
			D3DFORMAT_MAX = 20,

			FILTEROUT_LOWRESOLUTION_WIDTH = 640,
			FILTEROUT_LOWRESOLUTION_HEIGHT = 480,
		};

	protected:
		D3DDISPLAYMODE m_akD3DDM[D3DDISPLAYMODE_MAX];
		D3DFORMAT m_aeD3DFmt[D3DFORMAT_MAX];

		UINT m_uD3DDMNum;
		UINT m_uD3DFmtNum;
		
};

class D3D_CDeviceInfo
{
	public:
		D3D_CDeviceInfo() {}
		~D3D_CDeviceInfo() {}
		BOOL Build(IDirect3D8& rkD3D, UINT iAdapter, UINT iDevType, D3D_CAdapterDisplayModeList& rkD3DADMList, PFNCONFIRMDEVICE pfnConfirmDevice);
		BOOL Find(UINT uScrWidth, UINT uScrHeight, UINT uScrDepthBits, BOOL isWindowed, UINT* piD3DModeInfo);	

		UINT GetD3DModeInfoNum();
		
		VOID GetString(std::string* pstEnumList);
		
		BOOL FindDepthStencilFormat(IDirect3D8& rkD3D, UINT iAdapter, D3DDEVTYPE DeviceType, D3DFORMAT TargetFormat, D3DFORMAT* pDepthStencilFormat);

		D3D_SModeInfo& GetD3DModeInfor(UINT iD3DModeInfo);
		D3D_SModeInfo* GetD3DModeInfop(UINT iD3DModeInfo);
				
	protected:
		enum
		{
			D3DMODEINFO_NUM = 150,
		};

	
	protected:
		const TCHAR* m_szDevDesc;

		D3DDEVTYPE	m_eD3DDevType;
		D3DCAPS8	m_kD3DCaps; 			
		BOOL		m_canDoWindowed;

		UINT		m_iCurD3DModeInfo;
		UINT		m_uD3DModeInfoNum;
		D3D_SModeInfo	m_akD3DModeInfo[D3DMODEINFO_NUM];

	
		BOOL		m_isWindowed;

		D3DMULTISAMPLE_TYPE m_eD3DMSTWindowed;
		D3DMULTISAMPLE_TYPE m_eD3DMSTFullscreen;

	
	protected:
		static const CHAR*		msc_aszD3DDevDesc[D3DDEVICETYPE_NUM];
		static const D3DDEVTYPE msc_aeD3DDevType[D3DDEVICETYPE_NUM];		
};

class D3D_CAdapterInfo
{
	public:
		D3D_CAdapterInfo() {}
		~D3D_CAdapterInfo() {}
		BOOL Find(UINT uScrWidth, UINT uScrHeight, UINT uScrDepthBits, BOOL isWindowed, UINT* piD3DModeInfo, UINT* piD3DDevInfo);

		BOOL Build(IDirect3D8& rkD3D, UINT iAdapter, PFNCONFIRMDEVICE pfnConfirmDevice);
		VOID GetString(std::string* pstEnumList);

		D3DADAPTER_IDENTIFIER8& GetIdentifier()
		{
			return m_kD3DAdapterIdentifier;
		}
		
		D3DDISPLAYMODE&	GetDesktopD3DDisplayModer();
		D3DDISPLAYMODE*	GetDesktopD3DDisplayModep();
		
		D3D_CDeviceInfo*	GetD3DDeviceInfop(UINT iD3DDevInfo);
		D3D_SModeInfo*		GetD3DModeInfop(UINT iD3DDevInfo, UINT iD3DModeInfo);		

	protected:
		enum
		{
			D3DDEVICEINFO_NUM = 5,
		};
		
		
	protected:
		D3DADAPTER_IDENTIFIER8	m_kD3DAdapterIdentifier;
		D3DDISPLAYMODE			m_kD3DDMDesktop;

		UINT			m_iCurD3DDevInfo;
		UINT			m_uD3DDevInfoNum;
		D3D_CDeviceInfo	m_akD3DDevInfo[D3DDEVICEINFO_NUM];	
	
};

class D3D_CDisplayModeAutoDetector
{
	public:
		D3D_CDisplayModeAutoDetector();
		~D3D_CDisplayModeAutoDetector();

		BOOL Find(UINT uScrWidth, UINT uScrHeight, UINT uScrDepthBits, BOOL isWindowed, UINT* piD3DModeInfo, UINT* piD3DDevInfo, UINT* piD3DAdapterInfo);
		BOOL Build(IDirect3D8& rkD3D, PFNCONFIRMDEVICE pfnConfirmDevice);

		D3D_CAdapterInfo*	GetD3DAdapterInfop(UINT iD3DAdapterInfo);
		D3D_SModeInfo*		GetD3DModeInfop(UINT iD3DAdapterInfo, UINT iD3DDevInfo, UINT iD3DModeInfo);

		VOID GetString(std::string* pstEnumList);

	protected:
		enum
		{
			D3DADAPTERINFO_NUM = 10,
		};

	protected:
		D3D_CAdapterInfo	m_akD3DAdapterInfo[D3DADAPTERINFO_NUM];
		UINT				m_uD3DAdapterInfoCount;
};