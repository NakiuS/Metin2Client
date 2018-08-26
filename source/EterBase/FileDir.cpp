#include "StdAfx.h"
#include "FileDir.h"
#include <string>

CDir::CDir()
{
	Initialize();
}

CDir::~CDir()
{
	Destroy();
}

void CDir::Destroy()
{
	if (m_hFind)
		FindClose(m_hFind);

	Initialize();
}

bool CDir::Create(const char * c_szFilter, const char* c_szPath, BOOL bCheckedExtension)
{
	Destroy();
	
	std::string stPath = c_szPath;

	if (stPath.length())
	{
		char end = stPath[stPath.length() - 1];

		if (end != '\\')
			stPath+='\\';
	}

	std::string stQuery;
	stQuery += stPath;
	stQuery += "*.*";
	
	m_wfd.dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
	m_hFind = FindFirstFile(stQuery.c_str(), &m_wfd);

	if (m_hFind == INVALID_HANDLE_VALUE)
		return true;

	do
	{
		if (*m_wfd.cFileName == '.')
			continue;

		if (IsFolder())	
		{
			if (!OnFolder(c_szFilter, stPath.c_str(), m_wfd.cFileName))
				return false;
		}
		else
		{
			const char * c_szExtension = strchr(m_wfd.cFileName, '.');
			if (!c_szExtension)
				continue;

			// NOTE : 임시 변수 - [levites]
			//        최종적으로는 무조건 TRUE 형태로 만든다.
			//        그전에 전 프로젝트의 CDir을 사용하는 곳에서 Extension을 "wav", "gr2" 이런식으로 넣게끔 한다. - [levites]
			if (bCheckedExtension)
			{
				std::string strFilter = c_szFilter;
				int iPos = strFilter.find_first_of(';', 0);
				if (iPos > 0)
				{
					std::string strFirstFilter = std::string(c_szFilter).substr(0, iPos);
					std::string strSecondFilter = std::string(c_szFilter).substr(iPos+1, strlen(c_szFilter));
					if (0 != strFirstFilter.compare(c_szExtension+1) && 0 != strSecondFilter.compare(c_szExtension+1))
						continue;
				}
				else
				{
					if (0 != stricmp(c_szExtension+1, c_szFilter))
						continue;
				}
			}

			if (!OnFile(stPath.c_str(), m_wfd.cFileName))
				return false;
		}
	} 			
	while (FindNextFile(m_hFind, &m_wfd));

	return true;
}

bool CDir::IsFolder()
{
	if (m_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	
	return false;
}		

void CDir::Initialize()
{
	memset(&m_wfd, 0, sizeof(m_wfd));
	m_hFind = NULL;				
}
