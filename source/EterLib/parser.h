#ifndef __INC_SCRIPT_PARSER_H__
#define __INC_SCRIPT_PARSER_H__

#include <list>
#include <string>

namespace script
{
    typedef struct SArgumet
    {
        SArgumet(const std::string& c_stName, const std::string& c_stValue)
        {
            strName = c_stName;
            strValue = c_stValue;
        }
		SArgumet(const SArgumet& c_arg)
		{
			strName = c_arg.strName;
			strValue = c_arg.strValue;
		}		
		void operator=(const SArgumet& c_arg)
		{
			strName = c_arg.strName;
			strValue = c_arg.strValue;
		}		
        std::string strName;
        std::string strValue;
    } TArg;
	
    typedef std::list<TArg> TArgList;
	
    typedef struct SCmd
    {
        std::string     name;
        TArgList        argList;

		SCmd()
		{}
		SCmd(const SCmd& c_cmd)
		{
			name = c_cmd.name;
			argList = c_cmd.argList;
		}
		void operator=(const SCmd& c_cmd)
		{
			name = c_cmd.name;
			argList = c_cmd.argList;
		}
    } TCmd;
	
    class Group
    {
		public:
			Group();
			~Group();
			
		public:
			/** 스트링으로 부터 스크립트 그룹을 만든다.
			*
			* 실패하면 GetError 메소드로 확인할 수 있다.
			*
			* @param stSource 이 스트링으로 부터 그룹이 만들어 진다.
			* @return 성공시 true, 실패하면 false
			*/
			bool                Create(const std::string & stSource);
			
			/** 명령어를 받는 메소드
			*
			* @param cmd 성공시에 이 구조체로 명령어가 복사 된다.
			* @return 명령어가 남아 있다면 true, 없다면 false
			*/
			bool                GetCmd(TCmd & cmd);

			/*
				명령어를 가져오되 꺼내지는 않는다.
			*/
			bool                ReadCmd(TCmd & cmd);
			
			/** 에러를 출력 받는 메소드
			*
			* @return stError 이 곳으로 에러가 출력 된다.
			*/
			std::string &       GetError();
			
		private:
			void                SetError(const char *str);
			bool                GetArg(const char * c_atr_base, int arg_len, TArgList & argList);
			
			std::string         m_stError;
			std::list<TCmd>     m_cmdList;
    };
}

#endif
