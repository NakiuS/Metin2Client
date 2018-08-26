#include "StdAfx.h"
#include "MapOutdoor.h"

#include "../eterlib/StateManager.h"

void CMapOutdoor::__RenderTerrain_RenderHardwareTransformPatch()
{
	DWORD dwFogColor;
	float fFogFarDistance;
	float fFogNearDistance;
	if (mc_pEnvironmentData)
	{
		dwFogColor=mc_pEnvironmentData->FogColor;
		fFogNearDistance=mc_pEnvironmentData->GetFogNearDistance();
		fFogFarDistance=mc_pEnvironmentData->GetFogFarDistance();
	}
	else
	{
		dwFogColor=0xffffffff;
		fFogNearDistance=5000.0f;
		fFogFarDistance=10000.0f;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// Render State & TextureStageState	

	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0x00000000);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, dwFogColor);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);

#ifdef WORLD_EDITOR
	if (GetAsyncKeyState(VK_CAPITAL))
	{
		CSpeedTreeWrapper::ms_bSelfShadowOn = false;
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER,	D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER,	D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MIPFILTER,	D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MINFILTER,	D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MAGFILTER,	D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MIPFILTER,	D3DTEXF_GAUSSIANCUBIC);
	}
	else
	{
		CSpeedTreeWrapper::ms_bSelfShadowOn = true;
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER,	D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER,	D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MIPFILTER,	D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MINFILTER,	D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MAGFILTER,	D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MIPFILTER,	D3DTEXF_LINEAR);
	}
#else
	CSpeedTreeWrapper::ms_bSelfShadowOn = true;
	STATEMANAGER.SetBestFiltering(0);
	STATEMANAGER.SetBestFiltering(1);
#endif

	m_matWorldForCommonUse._41 = 0.0f;
	m_matWorldForCommonUse._42 = 0.0f;
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldForCommonUse);

	STATEMANAGER.SaveTransform(D3DTS_TEXTURE0, &m_matWorldForCommonUse);
	STATEMANAGER.SaveTransform(D3DTS_TEXTURE1, &m_matWorldForCommonUse);

	// Render State & TextureStageState
	//////////////////////////////////////////////////////////////////////////

	STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL);

	m_iRenderedSplatNumSqSum = 0;
	m_iRenderedPatchNum = 0;
	m_iRenderedSplatNum = 0;
	m_RenderedTextureNumVector.clear();

	std::pair<float, long> fog_far(fFogFarDistance+1600.0f, 0);
	std::pair<float, long> fog_near(fFogNearDistance-3200.0f, 0);

	std::vector<std::pair<float ,long> >::iterator far_it = std::upper_bound(m_PatchVector.begin(),m_PatchVector.end(),fog_far);
	std::vector<std::pair<float ,long> >::iterator near_it = std::upper_bound(m_PatchVector.begin(),m_PatchVector.end(),fog_near);

	// NOTE: Word Editor 툴에서는 fog far보다 멀리있는 물체를 텍스쳐 없이 그리는 작업을 하지 않음
#ifdef WORLD_EDITOR
	near_it = m_PatchVector.begin();
	far_it = m_PatchVector.end();
#endif

	WORD wPrimitiveCount;
	D3DPRIMITIVETYPE ePrimitiveType;

	BYTE byCUrrentLODLevel = 0;

	float fLODLevel1Distance = __GetNoFogDistance();
	float fLODLevel2Distance = __GetFogDistance();

	SelectIndexBuffer(0, &wPrimitiveCount, &ePrimitiveType);

	DWORD dwFogEnable = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
	std::vector<std::pair<float, long> >::iterator it = m_PatchVector.begin();

	// NOTE: 맵툴에서는 view ~ fog near 사이의 지형을 fog disabled 상태로 그리는 작업을 하지 않음.
#ifndef WORLD_EDITOR
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);

	for( ; it != near_it; ++it)
	{
		if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
		{
			byCUrrentLODLevel = 1;
			SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
		}
		else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
		{
			byCUrrentLODLevel = 2;
			SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
		}
		
		__HardwareTransformPatch_RenderPatchSplat(it->second, wPrimitiveCount, ePrimitiveType);
		if (m_iRenderedSplatNum >= m_iSplatLimit)
			break;
		
 		if (m_bDrawWireFrame)
			DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
	}
#endif

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);

	if (m_iRenderedSplatNum < m_iSplatLimit)
	{
		for(it = near_it; it != far_it; ++it)
		{
			if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
			{
				byCUrrentLODLevel = 1;
				SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
			}
			else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
			{
				byCUrrentLODLevel = 2;
				SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
			}

			__HardwareTransformPatch_RenderPatchSplat(it->second, wPrimitiveCount, ePrimitiveType);

			if (m_iRenderedSplatNum >= m_iSplatLimit)
				break;

			if (m_bDrawWireFrame)
				DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
		}
	}

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SetTexture(0, NULL);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, FALSE);

	STATEMANAGER.SetTexture(1, NULL);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, FALSE);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);	

	if (m_iRenderedSplatNum < m_iSplatLimit)
	{
		for(it = far_it; it != m_PatchVector.end(); ++it)
		{
			if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
			{
				byCUrrentLODLevel = 1;
				SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
			}
			else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
			{
				byCUrrentLODLevel = 2;
				SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
			}

			__HardwareTransformPatch_RenderPatchNone(it->second, wPrimitiveCount, ePrimitiveType);

			if (m_iRenderedSplatNum >= m_iSplatLimit)
				break;

			if (m_bDrawWireFrame)
 				DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
		}
	}

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);

	std::sort(m_RenderedTextureNumVector.begin(),m_RenderedTextureNumVector.end());

	//////////////////////////////////////////////////////////////////////////
	// Render State & TextureStageState

	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE0);
	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE1);

	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS);

	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);

	// Render State & TextureStageState
	//////////////////////////////////////////////////////////////////////////
}

void CMapOutdoor::__HardwareTransformPatch_RenderPatchSplat(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
	assert(NULL!=m_pTerrainPatchProxyList && "__HardwareTransformPatch_RenderPatchSplat");
	CTerrainPatchProxy * pTerrainPatchProxy = &m_pTerrainPatchProxyList[patchnum];
	
	if (!pTerrainPatchProxy->isUsed())
		return;

	long sPatchNum = pTerrainPatchProxy->GetPatchNum();
	if (sPatchNum < 0)
		return;

	BYTE ucTerrainNum = pTerrainPatchProxy->GetTerrainNum();
	if (0xFF == ucTerrainNum)
		return;

	CTerrain * pTerrain;
	if (!GetTerrainPointer(ucTerrainNum, &pTerrain))
		return;

	DWORD dwFogColor;
	if (mc_pEnvironmentData)
		dwFogColor=mc_pEnvironmentData->FogColor;
	else
		dwFogColor=0xffffffff;

	WORD wCoordX, wCoordY;
	pTerrain->GetCoordinate(&wCoordX, &wCoordY);

	TTerrainSplatPatch & rTerrainSplatPatch = pTerrain->GetTerrainSplatPatch();
	
	D3DXMATRIX matTexTransform, matSplatAlphaTexTransform, matSplatColorTexTransform;
	m_matWorldForCommonUse._41 = -(float) (wCoordX * CTerrainImpl::TERRAIN_XSIZE);
	m_matWorldForCommonUse._42 = (float) (wCoordY * CTerrainImpl::TERRAIN_YSIZE);
	D3DXMatrixMultiply(&matTexTransform, &m_matViewInverse, &m_matWorldForCommonUse);
	D3DXMatrixMultiply(&matSplatAlphaTexTransform, &matTexTransform, &m_matSplatAlpha);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &matSplatAlphaTexTransform);

	D3DXMATRIX matTiling;
	D3DXMatrixScaling(&matTiling, 1.0f/640.0f, -1.0f/640.0f, 0.0f);
	matTiling._41=0.0f;
	matTiling._42=0.0f;
	
	D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &matTiling);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);
					
	CGraphicVertexBuffer* pkVB=pTerrainPatchProxy->HardwareTransformPatch_GetVertexBufferPtr();
	if (!pkVB)
		return;

	STATEMANAGER.SetStreamSource(0, pkVB->GetD3DVertexBuffer(), m_iPatchTerrainVertexSize);
	
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	int iPrevRenderedSplatNum=m_iRenderedSplatNum;

#ifdef WORLD_EDITOR

	int nRenderTextureCount = 0;

//	if (!m_bShowEntirePatchTextureCount && !(GetAsyncKeyState(VK_LCONTROL) & 0x8000) )
	if (1)
	{
		for (DWORD j = 1; j < pTerrain->GetNumTextures(); ++j)
		{
			TTerainSplat & rSplat = rTerrainSplatPatch.Splats[j];
			
			if (!rSplat.Active)
				continue;
			
			if (rTerrainSplatPatch.PatchTileCount[sPatchNum][j] == 0)
				continue;

			++nRenderTextureCount;
		}
		
		DWORD dwTextureFactor = STATEMANAGER.GetRenderState(D3DRS_TEXTUREFACTOR);
		
		int TextureCountThreshold = 8;
		DWORD dwTFactor = 0xFFFFFFFF;
		
		if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
		{
			if (GetAsyncKeyState(VK_1) & 0x8000)
			{
				TextureCountThreshold = 2;
				dwTFactor = 0xFF0000FF;
			}
			else if (GetAsyncKeyState(VK_2) & 0x8000)
			{
				TextureCountThreshold = 3;
				dwTFactor = 0xFF00FF00;
			}
			else if (GetAsyncKeyState(VK_3) & 0x8000)
			{
				TextureCountThreshold = 4;
				dwTFactor = 0xFF00FFFF;
			}
			else if (GetAsyncKeyState(VK_4) & 0x8000)
			{
				TextureCountThreshold = 5;
				dwTFactor = 0xFFFF0000;
			}
			else if (GetAsyncKeyState(VK_5) & 0x8000)
			{
				TextureCountThreshold = 6;
				dwTFactor = 0xFFFFFF00;
			}
			else if (GetAsyncKeyState(VK_6) & 0x8000)
			{
				TextureCountThreshold = 7;
				dwTFactor = 0xFFFF00ff;
			}
			
		}

		if (nRenderTextureCount>=TextureCountThreshold)
		{
			STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, dwTFactor);
			STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
			STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
			STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, dwTextureFactor);
			STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
			STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
		}
		else
		{
			// 0번 텍스처
			if ( 0 < rTerrainSplatPatch.PatchTileCount[sPatchNum][0] )
			{
				DWORD dwTextureFactorFor0Texture = STATEMANAGER.GetRenderState(D3DRS_TEXTUREFACTOR);
				STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF88FF88);
				STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
				STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
				STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, dwTextureFactorFor0Texture);
				STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
				STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
			}

			for (DWORD j = 1; j < pTerrain->GetNumTextures(); ++j)
			{
				TTerainSplat & rSplat = rTerrainSplatPatch.Splats[j];
				
				if (!rSplat.Active)
					continue;
				
				DWORD dwTextureCount = rTerrainSplatPatch.PatchTileCount[sPatchNum][j];
				if (dwTextureCount == 0)
					continue;
				
				DWORD dwTextureFactorForTextureBalance = 0xFFFFFFFF;

				if (!(GetAsyncKeyState(VK_LSHIFT) & 0x8000))
				{
					const TTerrainTexture & rTexture = m_TextureSet.GetTexture(j);
					
					D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &rTexture.m_matTransform);
					STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);
					
					STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
					STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
					STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
				}
				else
				{
					if (dwTextureCount < 71)
					{
						dwTextureFactorForTextureBalance = STATEMANAGER.GetRenderState(D3DRS_TEXTUREFACTOR);
						if (dwTextureCount < 51)
							STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFF0000);
						else
							STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF0000FF);
						STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
						STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
						STATEMANAGER.SetTexture(0, NULL);
					}
					else
					{
						const TTerrainTexture & rTexture = m_TextureSet.GetTexture(j);
						
						D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &rTexture.m_matTransform);
						STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);
						
						STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
					}
					STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
					STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
					if (dwTextureCount < 71)
					{
						STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, dwTextureFactorForTextureBalance);
						STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
						STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
					}
				}
				
				std::vector<int>::iterator aIterator = std::find(m_RenderedTextureNumVector.begin(), m_RenderedTextureNumVector.end(), (int)j);
				if (aIterator == m_RenderedTextureNumVector.end())
					m_RenderedTextureNumVector.push_back(j);
				++m_iRenderedSplatNum;
				if (m_iRenderedSplatNum >= m_iSplatLimit)
					break;
			}	
		}
	}
	else
	{
		int TextureCountThreshold = 6;
		DWORD dwTFactor = 0xFFFF00FF;
		
		if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
		{
			if (GetAsyncKeyState(VK_1) & 0x8000)
			{
				TextureCountThreshold = 1;
				dwTFactor = 0xFF0000FF;
			}
			else if (GetAsyncKeyState(VK_2) & 0x8000)
			{
				TextureCountThreshold = 2;
				dwTFactor = 0xFF00FF00;
			}
			else if (GetAsyncKeyState(VK_3) & 0x8000)
			{
				TextureCountThreshold = 3;
				dwTFactor = 0xFF00FFFF;
			}
			else if (GetAsyncKeyState(VK_4) & 0x8000)
			{
				TextureCountThreshold = 4;
				dwTFactor = 0xFFFF0000;
			}
			else if (GetAsyncKeyState(VK_5) & 0x8000)
			{
				TextureCountThreshold = 5;
				dwTFactor = 0xFFFFFF00;
			}
		}
		
		for (DWORD j = 1; j < pTerrain->GetNumTextures(); ++j)
		{
			TTerainSplat & rSplat = rTerrainSplatPatch.Splats[j];
			
			if (!rSplat.Active)
				continue;
			
			if (rTerrainSplatPatch.PatchTileCount[sPatchNum][j] == 0)
				continue;
			
			DWORD dwTextureFactor;
			
			if (nRenderTextureCount>=TextureCountThreshold)
			{
				dwTextureFactor = STATEMANAGER.GetRenderState(D3DRS_TEXTUREFACTOR);
				STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, dwTFactor);
				STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
				STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				STATEMANAGER.SetTexture(0, NULL);
			}
			else
			{
				const TTerrainTexture & rTexture = m_TextureSet.GetTexture(j);
				
				D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &rTexture.m_matTransform);
				STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);
				
				STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
			}
			STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
			STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
			if (nRenderTextureCount>=TextureCountThreshold)
			{
				STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, dwTextureFactor);
				STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
				STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
			}
			
			++nRenderTextureCount;
			
			std::vector<int>::iterator aIterator = std::find(m_RenderedTextureNumVector.begin(), m_RenderedTextureNumVector.end(), (int)j);
			if (aIterator == m_RenderedTextureNumVector.end())
				m_RenderedTextureNumVector.push_back(j);
			++m_iRenderedSplatNum;
			if (m_iRenderedSplatNum >= m_iSplatLimit)
				break;
			
		}	
	}

#else
	bool isFirst=true;
	for (DWORD j = 1; j < pTerrain->GetNumTextures(); ++j)
	{
		TTerainSplat & rSplat = rTerrainSplatPatch.Splats[j];
		
		if (!rSplat.Active)
			continue;
		
		if (rTerrainSplatPatch.PatchTileCount[sPatchNum][j] == 0)
			continue;
		
		const TTerrainTexture & rTexture = m_TextureSet.GetTexture(j);
		
		D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &rTexture.m_matTransform);
		STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);
		if (isFirst)
		{
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
			STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
			STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
			isFirst=false;
		}
		else
		{
			STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
			STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
			STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);			
		}

		std::vector<int>::iterator aIterator = std::find(m_RenderedTextureNumVector.begin(), m_RenderedTextureNumVector.end(), (int)j);
		if (aIterator == m_RenderedTextureNumVector.end())
			m_RenderedTextureNumVector.push_back(j);
		++m_iRenderedSplatNum;
		if (m_iRenderedSplatNum >= m_iSplatLimit)
			break;
		
	}

/*
	if (GetAsyncKeyState(VK_CAPITAL) & 0x8000)
	{
		TTerainSplat & rSplat = rTerrainSplatPatch.Splats[200];
		
		if (rSplat.Active)
		{
			const TTerrainTexture & rTexture = m_TextureSet.GetTexture(1);
			
			D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &rTexture.m_matTransform);
			STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);
			
			STATEMANAGER.SetTexture(0, NULL);
			STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
			STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
		}
	}
*/
#endif

	// 그림자
	if (m_bDrawShadow)
	{
		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);
		
		STATEMANAGER.SetRenderState(D3DRS_FOGCOLOR, 0xFFFFFFFF);
		STATEMANAGER.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		STATEMANAGER.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
				
		D3DXMATRIX matShadowTexTransform;
		D3DXMatrixMultiply(&matShadowTexTransform, &matTexTransform, &m_matStaticShadow);

		STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matShadowTexTransform);
 		STATEMANAGER.SetTexture(0, pTerrain->GetShadowTexture());		
		
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSU,	D3DTADDRESS_CLAMP);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSV,	D3DTADDRESS_CLAMP);	

		if (m_bDrawChrShadow)
		{
			STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &m_matDynamicShadow);

 			STATEMANAGER.SetTexture(1, m_lpCharacterShadowMapTexture);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
		}		
		else
		{
			STATEMANAGER.SetTexture(1, NULL);			
		}
		
		ms_faceCount += wPrimitiveCount;
		STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
  		++m_iRenderedSplatNum;

		if (m_bDrawChrShadow)
		{
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		}			

 		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP);
		
		
		STATEMANAGER.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		STATEMANAGER.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		STATEMANAGER.SetRenderState(D3DRS_FOGCOLOR, dwFogColor);

		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);
	}
	++m_iRenderedPatchNum;

	int iCurRenderedSplatNum=m_iRenderedSplatNum-iPrevRenderedSplatNum;

	m_iRenderedSplatNumSqSum+=iCurRenderedSplatNum*iCurRenderedSplatNum;

}

void CMapOutdoor::__HardwareTransformPatch_RenderPatchNone(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
	assert(NULL!=m_pTerrainPatchProxyList && "__HardwareTransformPatch_RenderPatchNone");
	CTerrainPatchProxy * pTerrainPatchProxy = &m_pTerrainPatchProxyList[patchnum];
	
	if (!pTerrainPatchProxy->isUsed())
		return;

	CGraphicVertexBuffer* pkVB=pTerrainPatchProxy->HardwareTransformPatch_GetVertexBufferPtr();
	if (!pkVB)
		return;

	STATEMANAGER.SetStreamSource(0, pkVB->GetD3DVertexBuffer(), m_iPatchTerrainVertexSize);
	STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
}
