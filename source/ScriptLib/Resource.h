#pragma once

#include "../EffectLib/StdAfx.h"
#include "../eterlib/Resource.h"
#include "../eterlib/ResourceManager.h"

enum EResourceTypes
{
	RES_TYPE_UNKNOWN,
};

class CPythonResource : public CSingleton<CPythonResource>
{
	public:
		CPythonResource();
		virtual ~CPythonResource();
		
		void Destroy();

		void DumpFileList(const char * c_szFileName);

	protected:
		CResourceManager m_resManager;
};