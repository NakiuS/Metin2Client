#include "StdAfx.h"
#include "../eterlib/StateManager.h"
#include "ModelInstance.h"
#include "Model.h"

#ifdef _TEST

#include "../eterlib/GrpScreen.h"

void Granny_RenderBoxBones(const granny_skeleton* pkGrnSkeleton, const granny_world_pose* pkGrnWorldPose, const D3DXMATRIX& matBase)
{
	D3DXMATRIX matWorld;
	CScreen screen;	
	for (int iBone = 0; iBone != pkGrnSkeleton->BoneCount; ++iBone)
	{
		const granny_bone& rkGrnBone = pkGrnSkeleton->Bones[iBone];				
		const D3DXMATRIX* c_matBone=(const D3DXMATRIX*)GrannyGetWorldPose4x4(pkGrnWorldPose, iBone);
		
		D3DXMatrixMultiply(&matWorld, c_matBone, &matBase);
		
		STATEMANAGER.SetTransform(D3DTS_WORLD, &matWorld);
		screen.RenderBox3d(-5.0f, -5.0f, -5.0f, 5.0f, 5.0f, 5.0f);
	}
}

#endif


void CGrannyModelInstance::DeformNoSkin(const D3DXMATRIX * c_pWorldMatrix)
{
	if (IsEmpty())
		return;

	// DELETED
	//m_pgrnWorldPose = m_pgrnWorldPoseReal;
	///////////////////////////////
	
	UpdateWorldPose();
	UpdateWorldMatrices(c_pWorldMatrix);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//// Render
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// With One Texture
void CGrannyModelInstance::RenderWithOneTexture()
{
	// FIXME : Deform, Render, BlendRender를 묶어 상위에서 걸러주는 것이 더 나을 듯 - [levites]
	if (IsEmpty())
		return;

#ifdef _TEST
	Granny_RenderBoxBones(GrannyGetSourceSkeleton(m_pgrnModelInstance), m_pgrnWorldPose, TEST_matWorld);
	if (GetAsyncKeyState('P'))
		Tracef("render %x", m_pgrnModelInstance);	
	return;
#endif

	STATEMANAGER.SetVertexShader(ms_pntVS);

	// WORK
	LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
	// END_OF_WORK

	LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithOneTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_DIFFUSE_PNT);
	}
	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithOneTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_DIFFUSE_PNT);
	}
}

void CGrannyModelInstance::BlendRenderWithOneTexture()
{
	if (IsEmpty())
		return;

	// WORK
	LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
	// END_OF_WORK
	LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();

 	STATEMANAGER.SetVertexShader(ms_pntVS);

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithOneTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_BLEND_PNT);
	}

	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithOneTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_BLEND_PNT);
	}
}

// With Two Texture
void CGrannyModelInstance::RenderWithTwoTexture()
{
	// FIXME : Deform, Render, BlendRender를 묶어 상위에서 걸러주는 것이 더 나을 듯 - [levites]
	if (IsEmpty())
		return;

	STATEMANAGER.SetVertexShader(ms_pntVS);

	// WORK
	LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
	// END_OF_WORK
	LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithTwoTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_DIFFUSE_PNT);
	}
	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithTwoTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_DIFFUSE_PNT);
	}
}

void CGrannyModelInstance::BlendRenderWithTwoTexture()
{
	if (IsEmpty())
		return;

	// WORK
	LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
	// END_OF_WORK
	LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();

 	STATEMANAGER.SetVertexShader(ms_pntVS);

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithTwoTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_BLEND_PNT);
	}

	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithTwoTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_BLEND_PNT);
	}
}

void CGrannyModelInstance::RenderWithoutTexture()
{
	if (IsEmpty())
		return;

	STATEMANAGER.SetVertexShader(ms_pntVS);
	STATEMANAGER.SetTexture(0, NULL);
	STATEMANAGER.SetTexture(1, NULL);

	// WORK
	LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
	// END_OF_WORK
	LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_DIFFUSE_PNT);
		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_BLEND_PNT);
	}

	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_DIFFUSE_PNT);
		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_BLEND_PNT);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//// Render Mesh List
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// With One Texture
void CGrannyModelInstance::RenderMeshNodeListWithOneTexture(CGrannyMesh::EType eMeshType, CGrannyMaterial::EType eMtrlType)
{
	assert(m_pModel != NULL);

	LPDIRECT3DINDEXBUFFER8 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();
	assert(lpd3dIdxBuf != NULL);

	const CGrannyModel::TMeshNode * pMeshNode = m_pModel->GetMeshNodeList(eMeshType, eMtrlType);

	while (pMeshNode)
	{
		const CGrannyMesh * pMesh = pMeshNode->pMesh;
		int vtxMeshBasePos = pMesh->GetVertexBasePosition();

		STATEMANAGER.SetIndices(lpd3dIdxBuf, vtxMeshBasePos);
		STATEMANAGER.SetTransform(D3DTS_WORLD, &m_meshMatrices[pMeshNode->iMesh]);

		/////
		const CGrannyMesh::TTriGroupNode* pTriGroupNode = pMesh->GetTriGroupNodeList(eMtrlType);
		int vtxCount = pMesh->GetVertexCount();
		while (pTriGroupNode)
		{
			ms_faceCount += pTriGroupNode->triCount;

			CGrannyMaterial& rkMtrl=m_kMtrlPal.GetMaterialRef(pTriGroupNode->mtrlIndex);
			rkMtrl.ApplyRenderState();
			STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, vtxCount, pTriGroupNode->idxPos, pTriGroupNode->triCount);
			rkMtrl.RestoreRenderState();
			
			pTriGroupNode = pTriGroupNode->pNextTriGroupNode;
		}
		/////

		pMeshNode = pMeshNode->pNextMeshNode;
	}
}

// With Two Texture
void CGrannyModelInstance::RenderMeshNodeListWithTwoTexture(CGrannyMesh::EType eMeshType, CGrannyMaterial::EType eMtrlType)
{
	assert(m_pModel != NULL);

	LPDIRECT3DINDEXBUFFER8 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();
	assert(lpd3dIdxBuf != NULL);

	const CGrannyModel::TMeshNode * pMeshNode = m_pModel->GetMeshNodeList(eMeshType, eMtrlType);

	while (pMeshNode)
	{
		const CGrannyMesh * pMesh = pMeshNode->pMesh;
		int vtxMeshBasePos = pMesh->GetVertexBasePosition();

		STATEMANAGER.SetIndices(lpd3dIdxBuf, vtxMeshBasePos);
		STATEMANAGER.SetTransform(D3DTS_WORLD, &m_meshMatrices[pMeshNode->iMesh]);

		/////
		const CGrannyMesh::TTriGroupNode* pTriGroupNode = pMesh->GetTriGroupNodeList(eMtrlType);
		int vtxCount = pMesh->GetVertexCount();
		while (pTriGroupNode)
		{
			ms_faceCount += pTriGroupNode->triCount;

			const CGrannyMaterial& rkMtrl=m_kMtrlPal.GetMaterialRef(pTriGroupNode->mtrlIndex);
			STATEMANAGER.SetTexture(0, rkMtrl.GetD3DTexture(0));
			STATEMANAGER.SetTexture(1, rkMtrl.GetD3DTexture(1));
			STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, vtxCount, pTriGroupNode->idxPos, pTriGroupNode->triCount);
			pTriGroupNode = pTriGroupNode->pNextTriGroupNode;
		}
		/////

		pMeshNode = pMeshNode->pNextMeshNode;
	}
}

// Without Texture
void CGrannyModelInstance::RenderMeshNodeListWithoutTexture(CGrannyMesh::EType eMeshType, CGrannyMaterial::EType eMtrlType)
{
	assert(m_pModel != NULL);

	LPDIRECT3DINDEXBUFFER8 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();
	assert(lpd3dIdxBuf != NULL);

	const CGrannyModel::TMeshNode * pMeshNode = m_pModel->GetMeshNodeList(eMeshType, eMtrlType);

	while (pMeshNode)
	{
		const CGrannyMesh * pMesh = pMeshNode->pMesh;
		int vtxMeshBasePos = pMesh->GetVertexBasePosition();

		STATEMANAGER.SetIndices(lpd3dIdxBuf, vtxMeshBasePos);
		STATEMANAGER.SetTransform(D3DTS_WORLD, &m_meshMatrices[pMeshNode->iMesh]);

		/////
		const CGrannyMesh::TTriGroupNode* pTriGroupNode = pMesh->GetTriGroupNodeList(eMtrlType);
		int vtxCount = pMesh->GetVertexCount();

		while (pTriGroupNode)
		{
			ms_faceCount += pTriGroupNode->triCount;
			STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, vtxCount, pTriGroupNode->idxPos, pTriGroupNode->triCount);
			pTriGroupNode = pTriGroupNode->pNextTriGroupNode;
		}
		/////

		pMeshNode = pMeshNode->pNextMeshNode;
	}
}

/*
void CGrannyModelInstance::RenderMeshNodeList(CGrannyMesh::EType eMeshType, CGrannyMaterial::EType eMtrlType)
{
	assert(m_pModel != NULL);

	LPDIRECT3DINDEXBUFFER8 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();
	assert(lpd3dIdxBuf != NULL);

	const CGrannyModel::TMeshNode * pMeshNode = m_pModel->GetMeshNodeList(eMeshType, eMtrlType);

	while (pMeshNode)
	{
		const CGrannyMesh * pMesh = pMeshNode->pMesh;
		int vtxMeshBasePos = pMesh->GetVertexBasePosition();

		STATEMANAGER.SetIndices(lpd3dIdxBuf, vtxMeshBasePos);
		STATEMANAGER.SetTransform(D3DTS_WORLD, &m_meshMatrices[pMeshNode->iMesh]);

		/////
		const CGrannyMesh::TTriGroupNode* pTriGroupNode = pMesh->GetTriGroupNodeList(eMtrlType);
		int vtxCount = pMesh->GetVertexCount();
		while (pTriGroupNode)
		{
			ms_faceCount += pTriGroupNode->triCount;
			STATEMANAGER.SetTexture(0, pTriGroupNode->lpd3dTextures[0]);

			if (pTriGroupNode->lpd3dTextures[1])
				STATEMANAGER.SetTexture(1, pTriGroupNode->lpd3dTextures[1]);
			else
			{
				STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

				STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1,	D3DTA_CURRENT);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1,	D3DTA_CURRENT);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG2,	D3DTA_TEXTURE);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

				STATEMANAGER.SetTexture(1, m_FogInstance.GetTexturePointer()->GetD3DTexture());

				D3DXMATRIX mat;
				memset(&mat, 0, sizeof(D3DXMATRIX));
				mat._31 = -0.001f;
				mat._41 = -7.0f;
				mat._42 = 0.5f;
				STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &mat);
			}

			STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, vtxCount, pTriGroupNode->idxPos, pTriGroupNode->triCount);

			if (!pTriGroupNode->lpd3dTextures[1])
			{
				STATEMANAGER.SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
				STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			}

			pTriGroupNode = pTriGroupNode->pNextTriGroupNode;
		}
		/////

		pMeshNode = pMeshNode->pNextMeshNode;
	}
}
void CGrannyModelInstance::RenderToShadowMap()
{
	if (IsEmpty())
		return;

	STATEMANAGER.SetVertexShader(ms_pntVS);

// 	LPDIRECT3DVERTEXBUFFER8 lpd3dDynamicPNTVtxBuf = m_dynamicPNTVtxBuf.GetD3DVertexBuffer();
	LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = m_dynamicPNTVtxBuf.GetD3DVertexBuffer();
	LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();

//	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
//	STATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0);
//	STATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL);
//
//	if (lpd3dDynamicPNTVtxBuf)
//	{
//		STATEMANAGER.SetStreamSource(0, lpd3dDynamicPNTVtxBuf, sizeof(TPNTVertex));
//		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_BLEND_PNT);
//	}
//
//	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
//	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
//	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_DIFFUSE_PNT);
	}

	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_DIFFUSE_PNT);
	}
}
*/
