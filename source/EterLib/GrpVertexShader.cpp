#include "StdAfx.h"
#include "GrpVertexShader.h"
#include "GrpD3DXBuffer.h"
#include "StateManager.h"

CVertexShader::CVertexShader()
{
	Initialize();
}

CVertexShader::~CVertexShader()
{
	Destroy();
}

void CVertexShader::Initialize()
{
	m_handle=0;
}

void CVertexShader::Destroy()
{
	if (m_handle)
	{
		if (ms_lpd3dDevice)
			ms_lpd3dDevice->DeleteVertexShader(m_handle);

		m_handle = 0;
	}
}

bool CVertexShader::CreateFromDiskFile(const char* c_szFileName, const DWORD* c_pdwVertexDecl)
{
	Destroy();

	LPD3DXBUFFER lpd3dxShaderBuffer;
	LPD3DXBUFFER lpd3dxErrorBuffer;
	
	if (FAILED(
		D3DXAssembleShaderFromFile(c_szFileName, 0, NULL, &lpd3dxShaderBuffer, &lpd3dxErrorBuffer)
	)) return false;

	CDirect3DXBuffer shaderBuffer(lpd3dxShaderBuffer);
	CDirect3DXBuffer errorBuffer(lpd3dxErrorBuffer);

	if (FAILED(
		ms_lpd3dDevice->CreateVertexShader(c_pdwVertexDecl, (DWORD*)shaderBuffer.GetPointer(), &m_handle, 0 )
		))
		return false;

	return true;
}

void CVertexShader::Set()
{
	STATEMANAGER.SetVertexShader(m_handle);
}
