#include "StdAfx.h"
#include "../eterPack/EterPackManager.h"
#include "../eterBase/tea.h"

// CHINA_CRYPT_KEY
DWORD g_adwEncryptKey[4];
DWORD g_adwDecryptKey[4];

#include "AccountConnector.h"

inline const BYTE* GetKey_20050304Myevan() 
{
	volatile static DWORD s_adwKey[1938];

	volatile DWORD seed=1491971513;
	for (UINT i=0; i<BYTE(seed); i++)
	{
		seed^=2148941891;
		seed+=3592385981;
		s_adwKey[i]=seed;
	}

	return (const BYTE*)s_adwKey;
}

//#include <eterCrypt.h>

void CAccountConnector::__BuildClientKey_20050304Myevan()
{
	const BYTE * c_pszKey = GetKey_20050304Myevan();
	memcpy(g_adwEncryptKey, c_pszKey+157, 16);

	for (DWORD i = 0; i < 4; ++i)
		g_adwEncryptKey[i] = random();

	tea_encrypt((DWORD *) g_adwDecryptKey, (const DWORD *) g_adwEncryptKey, (const DWORD *) (c_pszKey+37), 16);
//	TEA_Encrypt((DWORD *) g_adwDecryptKey, (const DWORD *) g_adwEncryptKey, (const DWORD *) (c_pszKey+37), 16);
}
// END_OF_CHINA_CRYPT_KEY

PyObject * packExist(PyObject * poSelf, PyObject * poArgs)
{
	char * strFileName;

	if (!PyTuple_GetString(poArgs, 0, &strFileName))
		return Py_BuildException();

	return Py_BuildValue("i", CEterPackManager::Instance().isExist(strFileName)?1:0);
}

PyObject * packGet(PyObject * poSelf, PyObject * poArgs)
{
	char * strFileName;
	
	if (!PyTuple_GetString(poArgs, 0, &strFileName))
		return Py_BuildException();

	// 파이썬에서 읽어드리는 패킹 파일은 python 파일과 txt 파일에 한정한다
	const char* pcExt = strrchr(strFileName, '.');
	if (pcExt) // 확장자가 있고
	{
		if ((stricmp(pcExt, ".py") == 0) ||
			(stricmp(pcExt, ".pyc") == 0) ||
			(stricmp(pcExt, ".txt") == 0))
		{
			CMappedFile file;
			const void * pData = NULL;

			if (CEterPackManager::Instance().Get(file,strFileName,&pData))
				return Py_BuildValue("s#",pData, file.Size());
		}
	}

	return Py_BuildException();
}

void initpack()
{
	static PyMethodDef s_methods[] =
	{
		{ "Exist",		packExist,		METH_VARARGS },
		{ "Get",		packGet,		METH_VARARGS },
		{ NULL, NULL },		
	};
	
	Py_InitModule("pack", s_methods);
}