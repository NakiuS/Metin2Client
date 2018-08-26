#include "StdAfx.h"
#include "../eterLib/GrpExpandedImageInstance.h"
#include "../eterLib/GrpTextInstance.h"
#include "../eterLib/GrpMarkInstance.h"
#include "../eterLib/GrpSubImage.h"
#include "../eterLib/GrpText.h"
#include "../eterLib/AttributeData.h"
#include "../eterGrnLib/Thing.h"
#include "../eterGrnLib/ThingInstance.h"
#include "../effectLib/EffectMesh.h"
#include "../effectLib/EffectInstance.h"
#include "../gamelib/WeaponTrace.h"
#include "../gamelib/MapType.h"
#include "../gamelib/GameType.h"
#include "../gamelib/RaceData.h"
#include "../gamelib/RaceMotionData.h"
#include "../gamelib/ActorInstance.h"
#include "../gamelib/Area.h"
#include "../gamelib/ItemData.h"
#include "../gamelib/FlyingData.h"
#include "../gamelib/FlyTrace.h"
#include "../gamelib/FlyingInstance.h"
#include "../gamelib/FlyingData.h"

#include "Resource.h"

CResource * NewImage(const char* c_szFileName)
{
	return new CGraphicImage(c_szFileName);
}

CResource * NewSubImage(const char* c_szFileName)
{
	return new CGraphicSubImage(c_szFileName);
}

CResource * NewText(const char* c_szFileName)
{
	return new CGraphicText(c_szFileName);
}

CResource * NewThing(const char* c_szFileName)
{
	return new CGraphicThing(c_szFileName);
}

CResource * NewEffectMesh(const char* c_szFileName)
{
	return new CEffectMesh(c_szFileName);
}

CResource * NewAttributeData(const char* c_szFileName)
{
	return new CAttributeData(c_szFileName);
}

void CPythonResource::DumpFileList(const char * c_szFileName)
{
	m_resManager.DumpFileListToTextFile(c_szFileName);
}

void CPythonResource::Destroy()
{		
	CFlyingInstance::DestroySystem();
	CActorInstance::DestroySystem();
	CArea::DestroySystem();
	CGraphicExpandedImageInstance::DestroySystem();
	CGraphicImageInstance::DestroySystem();	
	CGraphicMarkInstance::DestroySystem();
	CGraphicThingInstance::DestroySystem();
	CGrannyModelInstance::DestroySystem();
	CGraphicTextInstance::DestroySystem();
	CEffectInstance::DestroySystem();
	CWeaponTrace::DestroySystem();	
	CFlyTrace::DestroySystem();
	
	m_resManager.DestroyDeletingList();

	CFlyingData::DestroySystem();
	CItemData::DestroySystem();
	CEffectData::DestroySystem();
	CEffectMesh::SEffectMeshData::DestroySystem();
	CRaceData::DestroySystem();
	NRaceData::DestroySystem();
	CRaceMotionData::DestroySystem();	

	m_resManager.Destroy();	
}

CPythonResource::CPythonResource()
{
	m_resManager.RegisterResourceNewFunctionPointer("sub", NewSubImage);
	m_resManager.RegisterResourceNewFunctionPointer("dds", NewImage);
	m_resManager.RegisterResourceNewFunctionPointer("jpg", NewImage);
	m_resManager.RegisterResourceNewFunctionPointer("tga", NewImage);
	m_resManager.RegisterResourceNewFunctionPointer("bmp", NewImage);
	m_resManager.RegisterResourceNewFunctionPointer("fnt", NewText);
	m_resManager.RegisterResourceNewFunctionPointer("gr2", NewThing);
	m_resManager.RegisterResourceNewFunctionPointer("mde", NewEffectMesh);
	m_resManager.RegisterResourceNewFunctionPointer("mdatr", NewAttributeData);
}

CPythonResource::~CPythonResource()
{	
}
