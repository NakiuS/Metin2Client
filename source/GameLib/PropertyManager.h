#pragma once

#include "../eterPack/EterPack.h"

class CPropertyManager : public CSingleton<CPropertyManager>
{
	public:
		CPropertyManager();
		virtual ~CPropertyManager();

		void			Clear();

		void			SetPack(CEterPack * pPack);
		bool			BuildPack();

		bool			LoadReservedCRC(const char * c_pszFileName);
		void			ReserveCRC(DWORD dwCRC);
		DWORD			GetUniqueCRC(const char * c_szSeed);

		bool			Initialize(const char * c_pszPackFileName = NULL);
		bool			Register(const char * c_pszFileName, CProperty ** ppProperty = NULL);

		bool			Get(DWORD dwCRC, CProperty ** ppProperty);
		bool			Get(const char * c_pszFileName, CProperty ** ppProperty);

//		bool			Add(const char * c_pszFileName);
//		bool			Remove(DWORD dwCRC);

		bool			Put(const char * c_pszFileName, const char * c_pszSourceFileName);

		bool			Erase(DWORD dwCRC);
		bool			Erase(const char * c_pszFileName);

	protected:
		typedef std::map<DWORD, CProperty *>		TPropertyCRCMap;
		typedef std::set<DWORD>						TCRCSet;

		bool										m_isFileMode;
		TPropertyCRCMap								m_PropertyByCRCMap;
		TCRCSet										m_ReservedCRCSet;
		CEterPack									m_pack;
		CEterFileDict								m_fileDict;
};
