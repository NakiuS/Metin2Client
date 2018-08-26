#include "StdAfx.h"
#include "GrpPixelShader.h"
#include "GrpD3DXBuffer.h"
#include "StateManager.h"

CPixelShader::CPixelShader()
{
	Initialize();
}

CPixelShader::~CPixelShader()
{
	Destroy();
}

void CPixelShader::Initialize()
{
	m_handle=0;
}

void CPixelShader::Destroy()
{
	if (m_handle)
	{
		if (ms_lpd3dDevice)
			ms_lpd3dDevice->DeletePixelShader(m_handle);
		m_handle=0;
	}
}

bool CPixelShader::CreateFromDiskFile(const char* c_szFileName)
{
	Destroy();

	LPD3DXBUFFER lpd3dxShaderBuffer;
	LPD3DXBUFFER lpd3dxErrorBuffer;
	
	if (FAILED(
		D3DXAssembleShaderFromFile(c_szFileName, 0, NULL, &lpd3dxShaderBuffer, &lpd3dxErrorBuffer)
	))
		return false;

	CDirect3DXBuffer shaderBuffer(lpd3dxShaderBuffer);
	CDirect3DXBuffer errorBuffer(lpd3dxErrorBuffer);

	if (FAILED(ms_lpd3dDevice->CreatePixelShader((DWORD*)shaderBuffer.GetPointer(), &m_handle)))
		return false;

	return true;
}

void CPixelShader::Set()
{
	STATEMANAGER.SetPixelShader(m_handle);
}
