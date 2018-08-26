/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation

  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      A simple class to manage rendering state.  Created as a singleton.
	  Create it as a static global, or with new.  It doesn't matter as long as it is created 
	  before you use the CStateManager::GetSingleton() API to get a reference to it.

	  Call it with STATEMANAGER.SetRenderState(...)
	  Call it with STATEMANAGER.SetTextureStageState(...), etc.

	  Call the 'Save' versions of the function if you want to deviate from the current state.
	  Call the 'Restore' version to retrieve the last Save.

	  There are two levels of caching:
	  - All Sets/Saves/Restores are tracked for redundancy.  This reduces the size of the batch to
	  be flushed
	  - The flush function is called before rendering, and only copies state that is 
	  different from the current chip state.

  If you get an assert it is probably because an API call failed.

  See NVLink for a good example of how this class is used.

  Don't be afraid of the vector being used to track the flush batch.  It will grow as big as
  it needs to be and then stop, so it shouldn't be reallocated.

  The state manager holds a reference to the d3d device.

  - cmaughan@nvidia.com
        
******************************************************************************/

#ifndef __CSTATEMANAGER_H
#define __CSTATEMANAGER_H

#include <d3d8.h>
#include <d3dx8.h>

#include <vector>

#include "../eterBase/Singleton.h"

#define CHECK_D3DAPI(a)		\
{							\
	HRESULT hr = (a);		\
							\
	if (hr != S_OK)			\
		assert(!#a);		\
}

static const DWORD STATEMANAGER_MAX_RENDERSTATES = 256;
static const DWORD STATEMANAGER_MAX_TEXTURESTATES = 128;
static const DWORD STATEMANAGER_MAX_STAGES = 8;
static const DWORD STATEMANAGER_MAX_VCONSTANTS = 96;
static const DWORD STATEMANAGER_MAX_PCONSTANTS = 8;
static const DWORD STATEMANAGER_MAX_TRANSFORMSTATES = 300;	// World1 lives way up there...
static const DWORD STATEMANAGER_MAX_STREAMS = 16;

class CStreamData
{
	public:
		CStreamData(LPDIRECT3DVERTEXBUFFER8 pStreamData = NULL, UINT Stride = 0) : m_lpStreamData(pStreamData), m_Stride(Stride)
		{
		}

		bool operator == (const CStreamData& rhs) const
		{
			return ((m_lpStreamData == rhs.m_lpStreamData) && (m_Stride == rhs.m_Stride));
		}

		LPDIRECT3DVERTEXBUFFER8	m_lpStreamData;
		UINT					m_Stride;
};

class CIndexData
{
	public:
		CIndexData(LPDIRECT3DINDEXBUFFER8 pIndexData = NULL, UINT BaseVertexIndex = 0)
			: m_lpIndexData(pIndexData),
		m_BaseVertexIndex(BaseVertexIndex)
		{
		}

		bool operator == (const CIndexData& rhs) const
		{
			return ((m_lpIndexData == rhs.m_lpIndexData) && (m_BaseVertexIndex == rhs.m_BaseVertexIndex));
		}

		LPDIRECT3DINDEXBUFFER8	m_lpIndexData;
		UINT					m_BaseVertexIndex;
};

// State types managed by the class
typedef enum eStateType
{
	STATE_MATERIAL = 0,
	STATE_RENDER,
	STATE_TEXTURE,
	STATE_TEXTURESTAGE,
	STATE_VSHADER,
	STATE_PSHADER,
	STATE_TRANSFORM,
	STATE_VCONSTANT,
	STATE_PCONSTANT,
	STATE_STREAM,
	STATE_INDEX
} eStateType;

class CStateID
{
	public:
		CStateID(eStateType Type, DWORD dwValue0 = 0, DWORD dwValue1 = 0)
			: m_Type(Type),
		m_dwValue0(dwValue0),
		m_dwValue1(dwValue1)
		{
		}

		CStateID(eStateType Type, DWORD dwStage, D3DTEXTURESTAGESTATETYPE StageType)
			: m_Type(Type),
		m_dwStage(dwStage),
		m_TextureStageStateType(StageType)
		{
		}

		CStateID(eStateType Type, D3DRENDERSTATETYPE RenderType)
			: m_Type(Type),
		m_RenderStateType(RenderType)
		{
		}

		eStateType m_Type;

		union
		{
			DWORD					m_dwValue0;
			DWORD					m_dwStage;
			D3DRENDERSTATETYPE		m_RenderStateType;
			D3DTRANSFORMSTATETYPE	m_TransformStateType;
		};

		union
		{
			DWORD						m_dwValue1;
			D3DTEXTURESTAGESTATETYPE	m_TextureStageStateType;
		};
};

typedef std::vector<CStateID> TStateID;

class CStateManagerState
{
	public:
		CStateManagerState()
		{
		}

		void ResetState()
		{
			DWORD i, y;

			for (i = 0; i < STATEMANAGER_MAX_RENDERSTATES; i++)
				m_RenderStates[i] = 0x7FFFFFFF;

			for (i = 0; i < STATEMANAGER_MAX_STAGES; i++)
				for (y = 0; y < STATEMANAGER_MAX_TEXTURESTATES; y++)
					m_TextureStates[i][y] = 0x7FFFFFFF;

			for (i = 0; i < STATEMANAGER_MAX_STREAMS; i++)
				m_StreamData[i] = CStreamData();

			m_IndexData = CIndexData();

			for (i = 0; i < STATEMANAGER_MAX_STAGES; i++)
				m_Textures[i] = NULL;

			// Matrices and constants are not cached, just restored.  It's silly to check all the 
			// data elements (by which time the driver could have been sent them).
			for (i = 0; i < STATEMANAGER_MAX_TRANSFORMSTATES; i++)
				D3DXMatrixIdentity(&m_Matrices[i]);

			for (i = 0; i < STATEMANAGER_MAX_VCONSTANTS; i++)
				m_VertexShaderConstants[i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

			for (i = 0; i < STATEMANAGER_MAX_PCONSTANTS; i++)
				m_PixelShaderConstants[i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

			m_dwPixelShader = 0;
			m_dwVertexShader = D3DFVF_XYZ;

			ZeroMemory(&m_Matrices, sizeof(D3DXMATRIX) * STATEMANAGER_MAX_TRANSFORMSTATES);
		}

		// Renderstates
		DWORD					m_RenderStates[STATEMANAGER_MAX_RENDERSTATES];

		// Texture stage states
		DWORD					m_TextureStates[STATEMANAGER_MAX_STAGES][STATEMANAGER_MAX_TEXTURESTATES];

		// Vertex shader constants
		D3DXVECTOR4				m_VertexShaderConstants[STATEMANAGER_MAX_VCONSTANTS];

		// Pixel shader constants
		D3DXVECTOR4				m_PixelShaderConstants[STATEMANAGER_MAX_PCONSTANTS];

		// Textures
		LPDIRECT3DBASETEXTURE8	m_Textures[STATEMANAGER_MAX_STAGES];

		// Shaders
		DWORD					m_dwPixelShader;
		DWORD					m_dwVertexShader;

		D3DXMATRIX				m_Matrices[STATEMANAGER_MAX_TRANSFORMSTATES];

		D3DMATERIAL8			m_D3DMaterial;

		CStreamData				m_StreamData[STATEMANAGER_MAX_STREAMS];
		CIndexData				m_IndexData;
};

class CStateManager : public CSingleton<CStateManager>
{
	public:
		CStateManager(LPDIRECT3DDEVICE8 lpDevice);
		virtual ~CStateManager();

		void	SetDefaultState();
		void	Restore();

		bool	BeginScene();
		void	EndScene();
		
		// Material
		void	SaveMaterial();
		void	SaveMaterial(const D3DMATERIAL8 * pMaterial);
		void	RestoreMaterial();
		void	SetMaterial(const D3DMATERIAL8 * pMaterial);
		void	GetMaterial(D3DMATERIAL8 * pMaterial);

		void	SetLight(DWORD index, CONST D3DLIGHT8* pLight);
		void	GetLight(DWORD index, D3DLIGHT8* pLight);

		// Renderstates
		void	SaveRenderState(D3DRENDERSTATETYPE Type, DWORD dwValue);
		void	RestoreRenderState(D3DRENDERSTATETYPE Type);
		void	SetRenderState(D3DRENDERSTATETYPE Type, DWORD Value);
		void	GetRenderState(D3DRENDERSTATETYPE Type, DWORD * pdwValue);

		// Textures
		void	SaveTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE8 pTexture);
		void	RestoreTexture(DWORD dwStage);
		void	SetTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE8 pTexture);
		void	GetTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE8 * ppTexture);

		// Texture stage states
		void	SaveTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue);
		void	RestoreTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type);
		void	SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue);
		void	GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD * pdwValue);
		void	SetBestFiltering(DWORD dwStage); // if possible set anisotropy filtering, or use trilinear

		// Vertex Shader
		void	SaveVertexShader(DWORD dwShader);
		void	RestoreVertexShader();
		void	SetVertexShader(DWORD dwShader);
		void	GetVertexShader(DWORD * pdwShader);

		// Pixel Shader
		void	SavePixelShader(DWORD dwShader);
		void	RestorePixelShader();
		void	SetPixelShader(DWORD dwShader);
		void	GetPixelShader(DWORD * pdwShader);

		// *** These states are cached, but not protected from multiple sends of the same value.
		// Transform
		void SaveTransform(D3DTRANSFORMSTATETYPE Transform, const D3DMATRIX* pMatrix);
		void RestoreTransform(D3DTRANSFORMSTATETYPE Transform);

		// Don't cache-check the transform.  To much to do
		void SetTransform(D3DTRANSFORMSTATETYPE Type, const D3DMATRIX* pMatrix);
		void GetTransform(D3DTRANSFORMSTATETYPE Type, D3DMATRIX * pMatrix);

		// SetVertexShaderConstant
		void SaveVertexShaderConstant(DWORD dwRegister, CONST void* pConstantData, DWORD dwConstantCount);
		void RestoreVertexShaderConstant(DWORD dwRegister, DWORD dwConstantCount);
		void SetVertexShaderConstant(DWORD dwRegister, CONST void* pConstantData, DWORD dwConstantCount);

		// SetPixelShaderConstant
		void SavePixelShaderConstant(DWORD dwRegister, CONST void* pConstantData, DWORD dwConstantCount);
		void RestorePixelShaderConstant(DWORD dwRegister, DWORD dwConstantCount);
		void SetPixelShaderConstant(DWORD dwRegister, CONST void* pConstantData, DWORD dwConstantCount);

		void SaveStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData, UINT Stride);
		void RestoreStreamSource(UINT StreamNumber);
		void SetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData, UINT Stride);

		void SaveIndices(LPDIRECT3DINDEXBUFFER8 pIndexData, UINT BaseVertexIndex);
		void RestoreIndices();
		void SetIndices(LPDIRECT3DINDEXBUFFER8 pIndexData,UINT BaseVertexIndex);
		
		HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
		HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
		HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount);
		HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, CONST void * pIndexData, D3DFORMAT IndexDataFormat, CONST void * pVertexStreamZeroData, UINT VertexStreamZeroStride);

		// Codes For Debug
		DWORD GetRenderState(D3DRENDERSTATETYPE Type);

	private:
		void SetDevice(LPDIRECT3DDEVICE8 lpDevice);

	private:
		CStateManagerState	m_ChipState;
		CStateManagerState	m_CurrentState;
		CStateManagerState	m_CopyState;
		TStateID			m_DirtyStates;
		bool				m_bForce;
		bool				m_bScene;
		DWORD				m_dwBestMinFilter;
		DWORD				m_dwBestMagFilter;
		LPDIRECT3DDEVICE8	m_lpD3DDev;

#ifdef _DEBUG
		// Saving Flag
		BOOL				m_bRenderStateSavingFlag[STATEMANAGER_MAX_RENDERSTATES];
		BOOL				m_bTextureStageStateSavingFlag[STATEMANAGER_MAX_STAGES][STATEMANAGER_MAX_TEXTURESTATES];
		BOOL				m_bTransformSavingFlag[STATEMANAGER_MAX_TRANSFORMSTATES];
#endif _DEBUG
};

#define STATEMANAGER (CStateManager::Instance())

#endif __CSTATEMANAGER_H
