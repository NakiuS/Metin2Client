#include "StdAfx.h"
#include "TargaResource.h"

CTargaResource::CTargaResource(const char * c_pszFileName) : CResource(c_pszFileName)
{	
}

CTargaResource::~CTargaResource()
{
}

CTargaResource::TType CTargaResource::Type()
{
	static TType s_type = StringToType("CTargaResource");
	return s_type;
}

bool CTargaResource::OnIsType(TType type)
{
	if (CTargaResource::Type() == type)
		return true;

	return CResource::OnIsType(type);
}

bool CTargaResource::OnLoad(int iSize, const void * c_pvBuf)
{
	return image.LoadFromMemory(iSize, static_cast<const BYTE *>(c_pvBuf));
}

void CTargaResource::OnClear()
{
	image.Clear();
}

bool CTargaResource::OnIsEmpty() const
{
	return image.IsEmpty();
}

void CTargaResource::GetRect(DWORD & w, DWORD & h)
{
	w = image.GetWidth();
	h = image.GetHeight();
}

DWORD * CTargaResource::GetMemPtr()
{
	return image.GetBasePointer();
}

TGA_HEADER &	CTargaResource::GetTgaHeader()
{
	return image.GetHeader();
}
