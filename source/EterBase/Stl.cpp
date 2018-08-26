#include "StdAfx.h"
#include "stl.h"

static std::list<std::string> s_stList;

char korean_tolower(const char c)
{
	char ret = c;
	if (c >= 'A' && c <= 'Z')
		ret = c - 'A' + 'a';

	assert(ret == tolower(c));
	return ret;
}

std::string& stl_static_string(const char * c_sz)
{
	std::string str;
	str.assign(c_sz);

	s_stList.push_back(str);
	return s_stList.back();
}

void stl_lowers(std::string& rstRet)
{
	for (size_t i = 0; i < rstRet.length(); ++i)
		rstRet[i] = korean_tolower(rstRet[i]);
}

int split_string(const string& input, const string& delimiter, vector<string>& results, bool includeEmpties)
{
	int iPos = 0;
	int newPos = -1;
	UINT sizeS2 = delimiter.size();
	UINT isize = input.size();

	if ((isize == 0) || (sizeS2 == 0))
	{
		return 0;
	}

	vector<int> positions;

	newPos = input.find(delimiter, 0);

	if (newPos < 0)
		return 0; 

	int numFound = 0;

	while (newPos >= iPos)
	{
		numFound++;
		positions.push_back(newPos);
		iPos = newPos;
		newPos = input.find(delimiter, iPos+sizeS2);
	}

	if (numFound == 0)
		return 0;

	for (UINT i = 0; i <= positions.size(); ++i)
	{
		string s("");
		if (i == 0) 
		{ 
			s = input.substr(i, positions[i]); 
		}
		else
		{
			UINT offset = positions[i-1] + sizeS2;

			if (offset < isize)
			{
				if (i == positions.size())
				{
					s = input.substr(offset);
				}
				else if (i > 0)
				{
					s = input.substr(positions[i-1] + sizeS2, positions[i] - positions[i-1] - sizeS2);
				}
			}
		}
		if (includeEmpties || (s.size() > 0))
		{
			results.push_back(s);
		}
	}
	return numFound;
}
