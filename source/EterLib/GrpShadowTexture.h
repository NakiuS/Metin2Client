#pragma once

#include "GrpTexture.h"

class CGraphicShadowTexture : public CGraphicTexture
{
	public:
		CGraphicShadowTexture();
		virtual ~CGraphicShadowTexture();
		
		void Destroy();
		
		bool Create(int width, int height);

		void Begin();
		void End();
		void Set(int stage = 0) const;

		const D3DXMATRIX& GetLightVPMatrixReference() const;
		LPDIRECT3DTEXTURE8 GetD3DTexture() const;

	protected:
		void Initialize();
		
	protected:
		D3DXMATRIX			m_d3dLightVPMatrix;
		D3DVIEWPORT8		m_d3dOldViewport;
		
		LPDIRECT3DTEXTURE8	m_lpd3dShadowTexture;
		LPDIRECT3DSURFACE8	m_lpd3dShadowSurface;
		LPDIRECT3DSURFACE8	m_lpd3dDepthSurface;

		LPDIRECT3DSURFACE8	m_lpd3dOldBackBufferSurface;
		LPDIRECT3DSURFACE8	m_lpd3dOldDepthBufferSurface;
};
