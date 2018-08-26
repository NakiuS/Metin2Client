#pragma once

#include "../eterBase/Timer.h"
#include "../eterBase/Debug.h"

/*
class CProfiler : public CSingleton<CProfiler>
{
	public:
		enum
		{
			STACK_DATA_MAX_NUM = 64,
		};

	public:
		typedef struct SProfileStackData
		{
			int iCallStep;
			long iStartTime;
			long iEndTime;

			std::string strName;
		} TProfileStackData;
		typedef struct SProfileAccumulationData
		{
			int iStartTime;
			int iCallingCount;
			int iCollapsedTime;

			std::string strName;
		} TProfileAccumulationData;

		typedef std::map<std::string, CGraphicTextInstance*> TGraphicTextInstanceMap;
		typedef std::map<std::string, TProfileAccumulationData> TProfileAccumulationDataMap;

	public:
		CProfiler()
		{
			Clear();
			m_ProfileAccumulationDataMap.clear();
		}
		virtual ~CProfiler()
		{
		}

		void Clear()
		{
			m_ProfileStackDataCount = 0;
			m_iCallStep = 0;

			TProfileAccumulationDataMap::iterator itor = m_ProfileAccumulationDataMap.begin();
			for (; itor != m_ProfileAccumulationDataMap.end(); ++itor)
			{
				TProfileAccumulationData & rData = itor->second;
				rData.iCallingCount = 0;
				rData.iCollapsedTime = 0;
			}
		}

		void Push(const char * c_szName)
		{
			assert(m_ProfileStackDataCount < STACK_DATA_MAX_NUM);
			TProfileStackData & rProfileStackData = m_ProfileStackDatas[m_ProfileStackDataCount++];

			rProfileStackData.iCallStep = m_iCallStep;
			rProfileStackData.iStartTime = ELTimer_GetMSec();
			rProfileStackData.strName = c_szName;

			++m_iCallStep;

			TGraphicTextInstanceMap::iterator itor = m_GraphicTextInstanceMap.find(c_szName);

			if (m_GraphicTextInstanceMap.end() == itor)
			{
				CGraphicTextInstance * pGraphicTextInstance = CGraphicTextInstance::New();

				CResource * pResource = CResourceManager::Instance().GetResourcePointer("±¼¸²Ã¼.fnt");
				pGraphicTextInstance->Clear();
				pGraphicTextInstance->SetTextPointer(static_cast<CGraphicText*>(pResource));

				m_GraphicTextInstanceMap.insert(TGraphicTextInstanceMap::value_type(c_szName, pGraphicTextInstance));
			}
		}
		void Pop(const char * c_szName)
		{
			TProfileStackData * pProfileStackData;

			if (!GetProfileStackDataPointer(c_szName, &pProfileStackData))
			{
				assert(!"The name doesn't exist");
				return;
			}

			pProfileStackData->iEndTime = ELTimer_GetMSec();

			--m_iCallStep;
		}

		void PushAccumulation(const char * c_szName)
		{
			TProfileAccumulationDataMap::iterator itor = m_ProfileAccumulationDataMap.find(c_szName);

			if (itor == m_ProfileAccumulationDataMap.end())
			{
				TProfileAccumulationData ProfileAccumulationData;
				ProfileAccumulationData.iCollapsedTime = 0;
				ProfileAccumulationData.iCallingCount = 0;
				ProfileAccumulationData.strName = c_szName;
				m_ProfileAccumulationDataMap.insert(TProfileAccumulationDataMap::value_type(c_szName, ProfileAccumulationData));

				itor = m_ProfileAccumulationDataMap.find(c_szName);

				/////

				CGraphicTextInstance * pGraphicTextInstance = m_GraphicTextInstancePool.Alloc();

				CResource * pResource = CResourceManager::Instance().GetResourcePointer("±¼¸²Ã¼.fnt");
				pGraphicTextInstance->Clear();
				pGraphicTextInstance->SetTextPointer(static_cast<CGraphicText*>(pResource));

				m_GraphicTextInstanceMap.insert(TGraphicTextInstanceMap::value_type(c_szName, pGraphicTextInstance));
			}

			TProfileAccumulationData & rData = itor->second;
			rData.iStartTime = ELTimer_GetMSec();
		}

		void PopAccumulation(const char * c_szName)
		{
			TProfileAccumulationDataMap::iterator itor = m_ProfileAccumulationDataMap.find(c_szName);

			if (itor == m_ProfileAccumulationDataMap.end())
				return;

			TProfileAccumulationData & rData = itor->second;
			rData.iCollapsedTime += ELTimer_GetMSec() - rData.iStartTime;
			++rData.iCallingCount;
		}

		void ProfileByConsole()
		{
			for (int i = 0; i < m_ProfileStackDataCount; ++i)
			{
				TProfileStackData & rProfileStackData = m_ProfileStackDatas[i];

//				for (int i = 0; i < rProfileStackData.iCallStep; ++i)
//					Tracef("\t");

				Tracef("%-10s: %2d\t", rProfileStackData.strName.c_str(), rProfileStackData.iEndTime - rProfileStackData.iStartTime);
			}
			Tracef("\n");
		}

		void ProfileOneStackDataByConsole(const char * c_szName)
		{
			TProfileStackData * pProfileStackData;

			if (!GetProfileStackDataPointer(c_szName, &pProfileStackData))
			{
				return;
			}

			Tracef("%-10s: %3d\n", pProfileStackData->strName.c_str(), pProfileStackData->iEndTime - pProfileStackData->iStartTime);
		}

		void ProfileOneAccumulationDataByConsole(const char * c_szName)
		{
			TProfileAccumulationDataMap::iterator itor = m_ProfileAccumulationDataMap.find(c_szName);
			if (itor == m_ProfileAccumulationDataMap.end())
				return;

			TProfileAccumulationData & rData = itor->second;
			Tracef("%-10s : [CollapsedTime : %3d] / [CallingCount : %3d]\n", rData.strName.c_str(),
																			rData.iCollapsedTime,
																			rData.iCallingCount);
		}

		void ProfileByScreen()
		{
			float fxPosition = 0;
			float fyPosition = 10;

			char szText[128];

			for (int i = 0; i < m_ProfileStackDataCount; ++i)
			{
				TProfileStackData & rProfileStackData = m_ProfileStackDatas[i];

				TGraphicTextInstanceMap::iterator itor = m_GraphicTextInstanceMap.find(rProfileStackData.strName);

				if (m_GraphicTextInstanceMap.end() != itor)
				{
					CGraphicTextInstance * pGraphicTextInstance = itor->second;

					fxPosition = 10 + (float) rProfileStackData.iCallStep * 10 * 4;

					sprintf(szText, "%-10s : %3d", rProfileStackData.strName.c_str(), rProfileStackData.iEndTime - rProfileStackData.iStartTime);

					pGraphicTextInstance->SetColor(0.7f, 0.7f, 0.7f);
					pGraphicTextInstance->SetValue(szText, strlen(szText));
					pGraphicTextInstance->SetPosition(fxPosition, fyPosition);
					pGraphicTextInstance->Update();
					pGraphicTextInstance->Render();

					fyPosition += 17;
				}
			}

			fxPosition = 10;
			fyPosition += 10;
			TProfileAccumulationDataMap::iterator itor = m_ProfileAccumulationDataMap.begin();
			for (; itor != m_ProfileAccumulationDataMap.end(); ++itor)
			{
				TProfileAccumulationData & rData = itor->second;
				TGraphicTextInstanceMap::iterator itor = m_GraphicTextInstanceMap.find(rData.strName);

				if (m_GraphicTextInstanceMap.end() != itor)
				{
					CGraphicTextInstance * pGraphicTextInstance = itor->second;

					sprintf(szText, "%-10s : [CollapsedTime : %3d] / [CallingCount : %3d]", rData.strName.c_str(),
																							rData.iCollapsedTime,
																							rData.iCallingCount);

					pGraphicTextInstance->SetColor(0.7f, 0.7f, 0.7f);
					pGraphicTextInstance->SetValue(szText, strlen(szText));
					pGraphicTextInstance->SetPosition(fxPosition, fyPosition);
					pGraphicTextInstance->Update();
					pGraphicTextInstance->Render();

					fyPosition += 17;
				}
			}
		}

	protected:
		bool GetProfileStackDataPointer(const char * c_szName, TProfileStackData ** ppProfileStackData)
		{
			for (int i = 0; i < m_ProfileStackDataCount; ++i)
			{
				if (0 == m_ProfileStackDatas[i].strName.compare(c_szName))
				{
					*ppProfileStackData = &m_ProfileStackDatas[i];

					return true;
				}
			}

			return false;
		}

	protected:
		// Profile Stack Data
		int m_ProfileStackDataCount;
		TProfileStackData m_ProfileStackDatas[STACK_DATA_MAX_NUM];

		// Profile Increase Data
		TProfileAccumulationDataMap m_ProfileAccumulationDataMap;

		int m_iCallStep;

		TGraphicTextInstanceMap m_GraphicTextInstanceMap;
};
*/