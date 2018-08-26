#ifndef __MILESLIB_CSOUNDBASE_H__
#define __MILESLIB_CSOUNDBASE_H__

#include <map>
#include <vector>
#include "SoundData.h"

typedef struct SProvider
{
	char*		name;
	HPROVIDER	hProvider;
} TProvider;

typedef std::map<DWORD, CSoundData*> TSoundDataMap;

class CSoundBase
{
	public:
		CSoundBase();
		virtual ~CSoundBase();

		void					Initialize();
		void					Destroy();

		CSoundData *			AddFile(DWORD dwFileCRC, const char* filename);
		DWORD					GetFileCRC(const char* filename);

	protected:
		static int								ms_iRefCount;
		static HDIGDRIVER						ms_DIGDriver;
		static TProvider *						ms_pProviderDefault;
		static std::vector<TProvider>			ms_ProviderVector;
		static TSoundDataMap					ms_dataMap;
		static bool								ms_bInitialized;
};

#endif
