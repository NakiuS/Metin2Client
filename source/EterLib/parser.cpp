#include "StdAfx.h"
#include "parser.h"

using namespace script;

#define ishan(ch)		(((ch) & 0xE0) > 0x90)
#define isnhspace(ch)	(!ishan(ch) && isspace(ch))


extern DWORD GetDefaultCodePage();

const char* LocaleString_FindChar(const char* base, int len, char test)
{
	if (!base)
		return NULL;

	DWORD codePage = GetDefaultCodePage();
	
	int pos = 0;
	while (pos < len)
	{
		const char* cur = base + pos;
		const char* next = CharNextExA(codePage, cur, 0);
		int cur_len = next - cur;
		if (cur_len > 1)
		{
			pos += cur_len;
		}
		else if (1 == cur_len)
		{
			if (*cur == test)
				return cur;

			++pos;
		}
		else
		{
			break;
		}
	}
	return NULL;
}

int LocaleString_RightTrim(char* base, int len)
{
	DWORD codePage = GetDefaultCodePage();

	int pos = len;
	
	while (pos > 0)
	{
		char* cur = base + pos;
		char* prev = CharPrevExA(codePage, base, cur , 0);
		
		int prev_len = cur - prev;
		if (prev_len != 1)
			break;
		
		if (!isspace((unsigned char) *prev) && *prev != '\n' && *prev != '\r')
			break;				
		
		*prev = '\0';
		
		pos -= prev_len;
	}

	if (pos > 0)
		return pos;

	return 0;
}

void LocaleString_RightTrim(char* base)
{
	LocaleString_RightTrim(base, strlen(base));
}

void OLD_rtrim(char* base)
{
	if (!base)
		return;

	DWORD codePage = GetDefaultCodePage();

	if (949 == codePage || 936 == codePage)
	{
		char* end = base + strlen(base) - 1;

		while (end != base)
		{
			if (!isnhspace((unsigned char) *end) && *end != '\n' && *end != '\r' || (end!=base && *((unsigned char*)end-1)>0xa0))
				break;
			
			*end = '\0';
			
			end = CharPrevExA(codePage, base, end, 0);
		}
	}
	else
	{
		char* end = base + strlen(base);

		while (end != base)
		{
			char* prev = CharPrevExA(codePage, base, end, 0);

			int prev_len = end - prev;
			if (prev_len != 1)
				break;

			if (!isspace((unsigned char) *prev) && *prev != '\n' && *prev != '\r')
				break;				
			
			*prev = '\0';
			
			end = prev;
		}
	}
}

const char* LocaleString_Skip(DWORD codePage, const char* cur)
{
	int loopCount = 0;

	while (*cur)
	{
		if (++loopCount > 9216)
		{
			TraceError("Infinite loop in LocaleString_Skip [%s]", cur);
			break;
		}

		const char* next = CharNextExA(codePage, cur, 0);
		int cur_len = next - cur;
		if (cur_len > 1)
		{
			cur = next;
		}
		else if (1 == cur_len)
		{
			if (!isspace((unsigned char) *cur) && *cur != '\n' && *cur != '\r')
				return cur;
		}
		else
		{
			break;
		}
	}
	return cur;
}

bool Group::GetArg(const char *c_arg_base, int arg_len, TArgList & argList)
{
    char szName[32 + 1];
    char szValue[64 + 1];

    int iNameLen = 0;
    int iValueLen = 0;
	int iCharLen = 0;

	int pos = 0;

    bool isValue = false;

	DWORD codePage = GetDefaultCodePage();

    while (pos < arg_len)
    {
		const char* cur = c_arg_base + pos;
		const char* next = CharNextExA(codePage, cur, 0); 
		iCharLen = next - cur;

		if (iCharLen > 1)
		{
			if (isValue)
			{
				if (iValueLen >= 64)
				{
					TraceError("argument value overflow: must be shorter than 64 letters");
					return false;
				}

				memcpy(szValue+iValueLen, cur, iCharLen);                
				iValueLen += iCharLen;
				szValue[iValueLen] = '\0';
			}
			else
			{
				if (iNameLen >= 32)
				{
					TraceError("argument name overflow: must be shorter than 32 letters");
					return false;
				}
				memcpy(szName+iNameLen, cur, iCharLen);				                
				iNameLen += iCharLen;
				szName[iNameLen] = '\0';
			}
		}
		else if (iCharLen == 1)
		{
			const char c = *cur;
			if (c == '|')
			{
				if (iNameLen == 0)
				{
					TraceError("no argument name");
					return false;
				}

				isValue = false;

				iNameLen = LocaleString_RightTrim(szName, iNameLen);
				iValueLen = LocaleString_RightTrim(szValue, iValueLen);
				argList.push_back(TArg(szName, szValue));

				iNameLen = 0;
				iValueLen = 0;
			}
			else if (c == ';')
			{
				isValue = true;
			}
			// 값이 아니고, 이름이 시작되지 않았을 경우 빈칸은 건너 뛴다.
			else if (!isValue && iNameLen == 0 && isspace((unsigned char) c))
			{
			}
			// 엔터는 건너 뛴다
			else if (c == '\r' || c == '\n')
			{
			}
			else
			{
				if (isValue)
				{
					if (iValueLen >= 64)
					{
						TraceError("argument value overflow: must be shorter than 64 letters");
						return false;
					}

					memcpy(szValue+iValueLen, cur, iCharLen);                
					iValueLen += iCharLen;
					szValue[iValueLen]        = '\0';
				}
				else
				{
					if (iNameLen >= 32)
					{
						TraceError("argument name overflow: must be shorter than 32 letters");
						return false;
					}
					memcpy(szName+iNameLen, cur, iCharLen);				                
					iNameLen += iCharLen;
					szName[iNameLen]        = '\0';
				}				
			}
		}
		else
		{
			break;
		}

		pos += iCharLen;
    }

    if (iNameLen != 0 && iValueLen != 0)
    {
		iNameLen = LocaleString_RightTrim(szName, iNameLen);
		iValueLen = LocaleString_RightTrim(szValue, iValueLen);
        argList.push_back(TArg(szName, szValue));
    }

    return true;
}


bool Group::Create(const std::string & stSource)
{
	m_cmdList.clear();

	if (stSource.empty())
		return false;

    const char *str_base = stSource.c_str();
    if (!str_base || !*str_base)
    {
        TraceError("Source file has no content");
        return false;
    }
	int str_len = stSource.length();
	int str_pos = 0;
	
	DWORD codePage = GetDefaultCodePage();

    char box_data[1024 + 1];

	static std::string stLetter;
	
    while (str_pos < str_len)
    {
        TCmd cmd;

		const char* word = str_base + str_pos;
		const char* word_next = CharNextExA(codePage, word, 0);
		
		int word_len = word_next - word;
		
		if (word_len > 1)
		{
			str_pos += word_len;

			{
				stLetter.assign(word, word_next);

				cmd.name.assign("LETTER");
				cmd.argList.push_back(TArg("value", stLetter));
			
				m_cmdList.push_back(cmd);
			}

		}
		else if (word_len == 1)
		{
			const char cur = *word;

			if ('[' == cur)
			{
				++str_pos;

				const char* box_begin = str_base + str_pos;
				const char* box_end = LocaleString_FindChar(box_begin, str_len - str_pos, ']');				
				if (!box_end)
				{
					TraceError(" !! PARSING ERROR - Syntax Error : %s\n", box_begin);
					return false;
				}
				str_pos += box_end - box_begin + 1;
				

				int data_len = 0;
				{
					const char* data_begin = LocaleString_Skip(codePage, box_begin);
					const char* data_end = box_end;
					data_len = data_end - data_begin;
					if (data_len >= 1024)
					{
						TraceError(" !! PARSING ERROR - Buffer Overflow : %d, %s\n", data_len, str_base);
						return false;
					}
					memcpy(box_data, data_begin, data_len);
					box_data[data_len] = '\0';

					data_len = LocaleString_RightTrim(box_data, data_len); // 오른쪽 빈칸 자르기
				}

				{
					const char* space = LocaleString_FindChar(box_data, data_len, ' ');
					if (space)  // 인자가 있음
					{
						int name_len = space - box_data;
						cmd.name.assign(box_data, name_len);
						
						const char* space_next = CharNextExA(codePage, space, 0);
						const char* arg = LocaleString_Skip(codePage, space_next);

						int arg_len = data_len - (arg - box_data);
						
						if (!GetArg(arg, arg_len, cmd.argList))
						{
							TraceError(" !! PARSING ERROR - Unknown Arguments : %d, %s\n", arg_len, arg);
							return false;
						}
					}
					else        // 인자가 없으므로 모든 스트링이 명령어다.
					{
						cmd.name.assign(box_data);
						cmd.argList.clear();
					}
					
					m_cmdList.push_back(cmd);
				}
			}
			else if (cur == '\r' || cur == '\n')
			{
				++str_pos;
			}
			else
			{
				++str_pos;
				
				{
					stLetter.assign(1, cur);
					cmd.name.assign("LETTER");
					cmd.argList.push_back(TArg("value", stLetter));
					m_cmdList.push_back(cmd);
				}					
			}
		}
		else
		{
			break;
		}
    }

    return true;
}

bool Group::GetCmd(TCmd & cmd)
{
    if (m_cmdList.empty())
        return false;

    cmd = m_cmdList.front();
    m_cmdList.pop_front();
    return true;
}

bool Group::ReadCmd(TCmd & cmd)
{
    if (m_cmdList.empty())
        return false;

    cmd = m_cmdList.front();
    return true;
}

std::string & Group::GetError()
{
    return m_stError;
}

void Group::SetError(const char * c_pszError)
{
    m_stError.assign(c_pszError);
}

Group::Group()
{
}

Group::~Group()
{
}
