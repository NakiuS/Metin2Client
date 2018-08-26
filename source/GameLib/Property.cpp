#include "StdAfx.h"
#include <string.h>
#include "../eterBase/TempFile.h"

#include "PropertyManager.h"
#include "Property.h"
/*
 *	CProperty 파일 포맷
 *
 *  0 ~ 4 bytes: fourcc
 *  5 ~ 6 bytes: \r\n
 *
 *  그 이후의 바이트들은 텍스트 파일 로더와 같은 구조
 */
CProperty::CProperty(const char * c_pszFileName) : mc_pFileName(NULL), m_dwCRC(0)
{
	m_stFileName = c_pszFileName;
	StringPath(m_stFileName);

	mc_pFileName = strrchr(m_stFileName.c_str(), '/');

	if (!mc_pFileName)
		mc_pFileName = m_stFileName.c_str();
	else
		++mc_pFileName;
}

CProperty::~CProperty()
{
}

DWORD CProperty::GetCRC()
{
	return m_dwCRC;
}

const char * CProperty::GetFileName()
{
	return (m_stFileName.c_str());
}

bool CProperty::GetString(const char * c_pszKey, const char ** c_ppString)
{
	std::string stTempKey = c_pszKey;
	stl_lowers(stTempKey);
	CTokenVectorMap::iterator it = m_stTokenMap.find(stTempKey.c_str());

//	printf("GetString %s %d\n", stTempKey.c_str(), m_stTokenMap.size());

	if (m_stTokenMap.end() == it)
		return false;

	*c_ppString = it->second[0].c_str();
	return true;
}

DWORD CProperty::GetSize()
{
	return m_stTokenMap.size();
}

bool CProperty::GetVector(const char * c_pszKey, CTokenVector & rTokenVector)
{
	std::string stTempKey = c_pszKey;
	stl_lowers(stTempKey);
	CTokenVectorMap::iterator it = m_stTokenMap.find(stTempKey.c_str());

	if (m_stTokenMap.end() == it)
		return false;

// NOTE : 튕김 현상 발견
//	std::copy(rTokenVector.begin(), it->second.begin(), it->second.end());
// NOTE : 레퍼런스에는 이런 식으로 하게끔 되어 있음
///////////////////////////////////////////////////////////////////////////////
//	template <class InputIterator, class OutputIterator>
//	OutputIterator copy(InputIterator first, InputIterator last,
//                    OutputIterator result);
//
//	vector<int> V(5);
//	iota(V.begin(), V.end(), 1);
//	list<int> L(V.size());
//	copy(V.begin(), V.end(), L.begin());
//	assert(equal(V.begin(), V.end(), L.begin()));
///////////////////////////////////////////////////////////////////////////////
// 헌데 그래도 튕김. - [levites]
//	std::copy(it->second.begin(), it->second.end(), rTokenVector.begin());

// 결국 이렇게.. - [levites]
// 현재 사용하는 곳 : WorldEditor/Dialog/MapObjectPropertyPageBuilding.cpp
	CTokenVector & rSourceTokenVector = it->second;
	CTokenVector::iterator itor = rSourceTokenVector.begin();
	for (; itor != rSourceTokenVector.end(); ++itor)
	{
		rTokenVector.push_back(*itor);
	}

	return true;
}

void CProperty::PutString(const char * c_pszKey, const char * c_pszString)
{
	std::string stTempKey = c_pszKey;
	stl_lowers(stTempKey);

	// 이미 있는걸 지움
	CTokenVectorMap::iterator itor = m_stTokenMap.find(stTempKey);

	if (itor != m_stTokenMap.end())
		m_stTokenMap.erase(itor);

	CTokenVector tokenVector;
	tokenVector.push_back(c_pszString);

	m_stTokenMap.insert(CTokenVectorMap::value_type(stTempKey, tokenVector));
}

void CProperty::PutVector(const char * c_pszKey, const CTokenVector & c_rTokenVector)
{
	std::string stTempKey = c_pszKey;
	stl_lowers(stTempKey);

	m_stTokenMap.insert(CTokenVectorMap::value_type(stTempKey, c_rTokenVector));
}

void GetTimeString(char * str, time_t ct)
{
    struct tm tm;
    tm = *localtime(&ct);

    _snprintf(str, 15, "%04d%02d%02d%02d%02d%02d",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec);
}

bool CProperty::Save(const char * c_pszFileName)
{
	CTempFile file;

	DWORD fourcc = MAKEFOURCC('Y', 'P', 'R', 'T');
	file.Write(&fourcc, sizeof(DWORD));
	file.Write("\r\n", 2);

	if (0 == m_stCRC.length())
	{
		char szCRC[MAX_PATH + 16 + 1];

		GetTimeString(szCRC, time(0));
		strcpy(szCRC + strlen(szCRC), c_pszFileName);

		m_dwCRC = CPropertyManager::Instance().GetUniqueCRC(szCRC);
		_snprintf(szCRC, sizeof(szCRC), "%u", m_dwCRC);

		m_stCRC.assign(szCRC);
	}

	file.Write(m_stCRC.c_str(), m_stCRC.length());
	file.Write("\r\n", 2);

	CTokenVectorMap::iterator itor = m_stTokenMap.begin();
	char buf[4096 + 1];

	while (itor != m_stTokenMap.end())
	{
		CTokenVector & tokenVector = itor->second;

		int len = _snprintf(buf, sizeof(buf), "%s\t", itor->first.c_str());
		file.Write(buf, len);

		for (DWORD i = 0; i < tokenVector.size(); ++i)
		{
			len = _snprintf(buf, sizeof(buf), "\t\"%s\"", tokenVector[i].c_str());
			file.Write(buf, len);
		}

		file.Write("\r\n", 2);
		++itor;
	}

	file.Close();
	return CPropertyManager::Instance().Put(c_pszFileName, file.GetFileName());
}

bool CProperty::ReadFromMemory(const void * c_pvData, int iLen, const char * c_pszFileName)
{
	const char * pcData = (const char *) c_pvData;

	if (*(DWORD *) pcData != MAKEFOURCC('Y', 'P', 'R', 'T'))
		return false;

	pcData += sizeof(DWORD);

	if (*pcData != '\r' || *(pcData + 1) != '\n')
	{
		TraceError("CProperty::ReadFromMemory: File format error after FourCC: %s\n", c_pszFileName);
		return false;
	}

	pcData += 2;

	CTokenVector stTokenVector;
/*
	char szTimeStamp[64];
	memcpy(szTimeStamp, pcData, 14);
	szTimeStamp[14] = '\0';
	pcData += 14;

 	if (*pcData != '\r' || *(pcData + 1) != '\n')
	{
		TraceError("CProperty::ReadFromMemory: File format error after TimeStamp: %s\n", c_pszFileName);
		return false;
	}

	std::string m_stTimeStamp;
	
	m_stTimeStamp = szTimeStamp;

	int iTimeStampLen = 14 + _snprintf(szTimeStamp + 14, 64 - 14, "%s", mc_pFileName);
	m_dwCRC = GetCRC32(szTimeStamp, iTimeStampLen);

	char tmp[64];
	sprintf(tmp, "%u", m_dwCRC);
	m_stCRC.assign(tmp);

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(iLen - (sizeof(DWORD) + 2 + 14 + 2), pcData);

	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &stTokenVector))
			continue;

		stl_lowers(stTokenVector[0]);
		std::string stKey = stTokenVector[0];

		stTokenVector.erase(stTokenVector.begin());
		PutVector(stKey.c_str(), stTokenVector);
	}
	return true;
	*/
	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(iLen - (sizeof(DWORD) + 2), pcData);

	m_stCRC = textFileLoader.GetLineString(0);
	m_dwCRC = atoi(m_stCRC.c_str());

	for (DWORD i = 1; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &stTokenVector))
			continue;

		stl_lowers(stTokenVector[0]);
		std::string stKey = stTokenVector[0];

		stTokenVector.erase(stTokenVector.begin());
		PutVector(stKey.c_str(), stTokenVector);
	}

	//Tracef("Property: %s\n", c_pszFileName);
	return true;
}

void CProperty::Clear()
{
	m_stTokenMap.clear();
}
