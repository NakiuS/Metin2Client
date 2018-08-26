#pragma once

#include "../eterBase/Debug.h"

//#define DYNAMIC_POOL_STRICT

template<typename T>
class CDynamicPool
{	
	public:
		CDynamicPool()
		{
			//Tracen(typeid(T).name());
			m_uInitCapacity=0;
			m_uUsedCapacity=0;
		}
		virtual ~CDynamicPool()
		{
			assert(m_kVct_pkData.empty());
//#ifdef _DEBUG
//			char szText[256];
//			sprintf(szText, "--------------------------------------------------------------------- %s Pool Capacity %d\n", typeid(T).name(), m_uUsedCapacity);
//			OutputDebugString(szText);
//			printf(szText);			
//#endif
		}

		void SetName(const char* c_szName)
		{			
		}

		void Clear()
		{			
			Destroy();
		}

		void Destroy()
		{
/*
#ifdef _DEBUG
			if (!m_kVct_pkData.empty())
			{
				char szText[256];
				sprintf(szText, "--------------------------------------------------------------------- %s Pool Destroy\n", typeid(T).name());
				OutputDebugString(szText);
				printf(szText);
			}
#endif			
*/
			std::for_each(m_kVct_pkData.begin(), m_kVct_pkData.end(), Delete);
			m_kVct_pkData.clear();
			m_kVct_pkFree.clear();
		}

		void Create(UINT uCapacity)
		{
			m_uInitCapacity=uCapacity;
			m_kVct_pkData.reserve(uCapacity);
			m_kVct_pkFree.reserve(uCapacity);
		}
		T* Alloc()
		{
			if (m_kVct_pkFree.empty())
			{
				T* pkNewData=new T;
				m_kVct_pkData.push_back(pkNewData);
				++m_uUsedCapacity;
				return pkNewData;
			}

			T* pkFreeData=m_kVct_pkFree.back();
			m_kVct_pkFree.pop_back();
			return pkFreeData;
		}
		void Free(T* pkData)
		{
#ifdef DYNAMIC_POOL_STRICT
			assert(__IsValidData(pkData));
			assert(!__IsFreeData(pkData));
#endif
			m_kVct_pkFree.push_back(pkData);
		}
		void FreeAll()
		{
			m_kVct_pkFree=m_kVct_pkData;
		}
		
		DWORD GetCapacity()
		{
			return m_kVct_pkData.size();
		}

	protected:
		bool __IsValidData(T* pkData)
		{
			if (m_kVct_pkData.end()==std::find(m_kVct_pkData.begin(), m_kVct_pkData.end(), pkData))
				return false;
			return true;
		}
		bool __IsFreeData(T* pkData)
		{
			if (m_kVct_pkFree.end()==std::find(m_kVct_pkFree.begin(), m_kVct_pkFree.end(), pkData))
				return false;

			return true;
		}

		static void Delete(T* pkData)
		{
			delete pkData;
		}

	protected:
		std::vector<T*> m_kVct_pkData;
		std::vector<T*> m_kVct_pkFree;

		UINT m_uInitCapacity;
		UINT m_uUsedCapacity;
};


template<typename T>
class CDynamicPoolEx
{	
	public:
		CDynamicPoolEx()
		{
			m_uInitCapacity=0;
			m_uUsedCapacity=0;
		}
		virtual ~CDynamicPoolEx()
		{
			assert(m_kVct_pkFree.size()==m_kVct_pkData.size());
			Destroy();

#ifdef _DEBUG
			char szText[256];
			sprintf(szText, "--------------------------------------------------------------------- %s Pool Capacity %d\n", typeid(T).name(), m_uUsedCapacity);
			OutputDebugString(szText);
			printf(szText);			
#endif
		}

		void Clear()
		{			
			Destroy();
		}

		void Destroy()
		{
#ifdef _DEBUG
			if (!m_kVct_pkData.empty())
			{
				char szText[256];
				sprintf(szText, "--------------------------------------------------------------------- %s Pool Destroy\n", typeid(T).name());
				OutputDebugString(szText);
				printf(szText);
			}
#endif			
			std::for_each(m_kVct_pkData.begin(), m_kVct_pkData.end(), Delete);
			m_kVct_pkData.clear();
			m_kVct_pkFree.clear();
		}

		void Create(UINT uCapacity)
		{
			m_uInitCapacity=uCapacity;
			m_kVct_pkData.reserve(uCapacity);
			m_kVct_pkFree.reserve(uCapacity);
		}
		T* Alloc()
		{
			if (m_kVct_pkFree.empty())
			{
				T* pkNewData=New();
				m_kVct_pkData.push_back(pkNewData);
				++m_uUsedCapacity;
				return pkNewData;
			}

			T* pkFreeData=m_kVct_pkFree.back();
			m_kVct_pkFree.pop_back();
			return pkFreeData;
		}
		void Free(T* pkData)
		{
#ifdef DYNAMIC_POOL_STRICT
			assert(__IsValidData(pkData));
			assert(!__IsFreeData(pkData));
#endif
			m_kVct_pkFree.push_back(pkData);
		}
		void FreeAll()
		{
			m_kVct_pkFree=m_kVct_pkData;
		}
		
		DWORD GetCapacity()
		{
			return m_kVct_pkData.size();
		}

	protected:
		bool __IsValidData(T* pkData)
		{
			if (m_kVct_pkData.end()==std::find(m_kVct_pkData.begin(), m_kVct_pkData.end(), pkData))
				return false;
			return true;
		}
		bool __IsFreeData(T* pkData)
		{
			if (m_kVct_pkFree.end()==std::find(m_kVct_pkFree.begin(), m_kVct_pkFree.end(), pkData))
				return false;

			return true;
		}

		static T* New()
		{
			return (T*)::operator new(sizeof(T));
		}
		static void Delete(T* pkData)
		{
			::operator delete(pkData);
		}

	protected:
		std::vector<T*> m_kVct_pkData;
		std::vector<T*> m_kVct_pkFree;
		
		UINT m_uInitCapacity;
		UINT m_uUsedCapacity;
				
};

template <class T>
class CPooledObject
{
    public:
		CPooledObject()
		{
		}
		virtual ~CPooledObject()
		{
		}

        void * operator new(unsigned int /*mem_size*/)
        {
            return ms_kPool.Alloc();
        }
		
        void operator delete(void* pT)
        {
            ms_kPool.Free((T*)pT);
        }
	
		
        static void DestroySystem()
        {
            ms_kPool.Destroy();
        }
		
        static void DeleteAll()
        {
            ms_kPool.FreeAll();
        }
		
    protected:
        static CDynamicPoolEx<T> ms_kPool;
};

template <class T> CDynamicPoolEx<T> CPooledObject<T>::ms_kPool;

/*

template <class T>
class CDynamicSizePool
{
#define GETPREVP(p) *(T**)((char*)p+sizeof(T))
#define GETNEXTP(p) *(T**)((char*)p+sizeof(T)+sizeof(T*))
    public:
        CDynamicSizePool()
        {
            Initialize();
        }
		
        virtual ~CDynamicSizePool()
        {
            Clear();
        }
		
        void Initialize()
        {
            m_nodes = NULL;
            m_nodeCount = 0;
			
            m_pFreeList = NULL;
            m_pUsedList = NULL;
        }
		
        void SetName(const char* c_szName)
        {
            m_stName = c_szName;
        }
		
        T* Alloc()
        {
            void* pnewNode;
			
            if (m_pFreeList)
            {
                pnewNode = m_pFreeList;
                m_pFreeList = GETNEXTP(m_pFreeList);
            }
            else
            {
                pnewNode = AllocNode();
            }
			
            if (!pnewNode)
                return NULL;
			
            if (!m_pUsedList)
            {
                m_pUsedList = pnewNode;
                GETPREVP(m_pUsedList) = NULL;
                GETNEXTP(m_pUsedList) = NULL;
            }
            else
            {
                GETPREVP(m_pUsedList) = (T*) pnewNode;
                GETNEXTP(pnewNode) = (T*) m_pUsedList;
                GETPREVP(pnewNode) = NULL;
                m_pUsedList = pnewNode;
            }
            //Tracef("%s Pool Alloc %p\n", m_stName.c_str(), pnewNode);
            return (T*) pnewNode;
        }
		
        void Free(T * pdata)
        {
            void* pfreeNode = (void*) pdata;
			
            if (pfreeNode == m_pUsedList)
            {
                if (NULL != (m_pUsedList = GETNEXTP(m_pUsedList)))
                    GETPREVP(m_pUsedList) = NULL;
            }
            else
            {
                if (GETNEXTP(pfreeNode))
                    GETPREVP(GETNEXTP(pfreeNode)) = GETPREVP(pfreeNode);
				
                if (GETPREVP(pfreeNode))
                    GETNEXTP(GETPREVP(pfreeNode)) = GETNEXTP(pfreeNode);
            }
			
            GETPREVP(pfreeNode) = NULL;
            GETNEXTP(pfreeNode) = (T*)m_pFreeList;
            m_pFreeList = pfreeNode;
            //Tracef("%s Pool Free\n", m_stName.c_str());
        }
		
        void FreeAll()
        {
            void * pcurNode;
            void * pnextNode;
			
            pcurNode = m_pUsedList;
			
            while (pcurNode)
            {
                pnextNode = GETNEXTP(pcurNode);
                Free(pcurNode);
                pcurNode = pnextNode;
            }
        }
		
        void Clear()
        {
            void* pcurNode;
            void* pnextNode;
            int count = 0;
			
            pcurNode = m_pFreeList;
            while (pcurNode)
            {
                pnextNode = GETNEXTP(pcurNode);
				((T*)pcurNode)->~T();
                ::operator delete(pcurNode);
                pcurNode = pnextNode;
                ++count;
            }
            m_pFreeList = NULL;
			
            pcurNode = m_pUsedList;
            while (pcurNode)
            {
                pnextNode = GETNEXTP(pcurNode);
				((T*)pcurNode)->~T();
                ::operator delete(pcurNode);
                pcurNode = pnextNode;
                ++count;
            }
            m_pUsedList = NULL;
            //Tracef("%s Pool Clear %d\n", m_stName.c_str(), count);
        }
		
    protected:
        void* AllocNode()
        {
            return ::operator new(sizeof(T)+sizeof(T*)*2);
        }
		
    protected:
        void *      m_nodes;
        void *      m_pFreeList;
        void *      m_pUsedList;
		
        int         m_nodeCount;
        std::string m_stName;
#undef GETNEXTP
#undef GETPREVP
		
};

template <class T>
class CPooledObject
{
    public:
		CPooledObject()
		{
		}
		virtual ~CPooledObject()
		{
		}

        void * operator new(unsigned int mem_size)
        {
            return ms_DynamicSizePool.Alloc();
        }
		
        void operator delete(void* pT)
        {
            ms_DynamicSizePool.Free((T*)pT);
        }
		
        static void SetPoolName(const char* szPoolName)
        {
            ms_DynamicSizePool.SetName(szPoolName);
        }
		
        static void ClearPool()
        {
            ms_DynamicSizePool.Clear();
        }
		
        static void FreePool()
        {
            ms_DynamicSizePool.FreeAll();
        }
		
    protected:
        static CDynamicSizePool<T> ms_DynamicSizePool;
};

template <class T> CDynamicSizePool<T> CPooledObject<T>::ms_DynamicSizePool;
*/

/*
template<typename T>
class CPoolNode : public T
{
	public:
		CPoolNode()
		{
			m_pNext = NULL;
			m_pPrev = NULL;
		}

		virtual ~CPoolNode()
		{
		}

	public:
		CPoolNode<T> * m_pNext;	
		CPoolNode<T> * m_pPrev;
};

template<typename T>
class CDynamicPool
{	
	public:
		typedef CPoolNode<T> TNode;

	public:
		CDynamicPool()
		{
			Initialize();
		}

		virtual ~CDynamicPool()
		{
			assert(m_pFreeList==NULL && "CDynamicPool::~CDynamicPool() - NOT Clear");
			assert(m_pUsedList==NULL && "CDynamicPool::~CDynamicPool() - NOT Clear");
			Clear();
		}

		void Initialize()
		{
			m_nodes = NULL;
			m_nodeCount = 0;

			m_pFreeList = NULL;
			m_pUsedList = NULL;
		}

		void SetName(const char* c_szName)
		{
			m_stName = c_szName;
		}

		DWORD GetCapacity()
		{
			return m_nodeCount;
		}

		T* Alloc()
		{
			TNode* pnewNode;

			if (m_pFreeList) 
			{
				pnewNode = m_pFreeList;
				m_pFreeList = m_pFreeList->m_pNext;
			}
			else
			{
				pnewNode = AllocNode();
			}

			if (!pnewNode)
				return NULL;		

			if (!m_pUsedList)
			{
				m_pUsedList = pnewNode;
				m_pUsedList->m_pPrev = m_pUsedList->m_pNext = NULL;
			}
			else
			{
				m_pUsedList->m_pPrev = pnewNode;
				pnewNode->m_pNext = m_pUsedList;
				pnewNode->m_pPrev = NULL;
				m_pUsedList = pnewNode;
			}
			//Tracef("%s Pool Alloc %p\n", m_stName.c_str(), pnewNode);
			return (T*) pnewNode;
		}
		
		bool IsUsedData(T* pdata)
		{
			TNode* pchkNode=(TNode*)pdata;
			TNode* pcurNode = m_pUsedList;
			while (pcurNode)
			{
				if (pcurNode==pdata)
					return true;

				pcurNode = pcurNode->m_pNext;				
			}
			return false;
		}

		bool IsFreeData(T* pdata)
		{
			TNode* pchkNode=(TNode*)pdata;
			TNode* pcurNode = m_pFreeList;
			while (pcurNode)
			{
				if (pcurNode==pdata)
					return true;

				pcurNode = pcurNode->m_pNext;				
			}
			return false;
		}

		void Free(T * pdata)
		{
			assert(IsUsedData(pdata));
			assert(!IsFreeData(pdata));			

			TNode* pfreeNode = (TNode*) pdata;

			if (pfreeNode == m_pUsedList)
			{
				m_pUsedList = m_pUsedList->m_pNext;
				if (NULL != m_pUsedList)
					m_pUsedList->m_pPrev = NULL;
			}
			else
			{
				if (pfreeNode->m_pNext)
					pfreeNode->m_pNext->m_pPrev = pfreeNode->m_pPrev;

				if (pfreeNode->m_pPrev)
					pfreeNode->m_pPrev->m_pNext = pfreeNode->m_pNext;
			}
			
			pfreeNode->m_pPrev = NULL;
			pfreeNode->m_pNext = m_pFreeList;
			m_pFreeList = pfreeNode;
			//Tracef("%s Pool Free\n", m_stName.c_str());
		}

		void FreeAll()
		{
			TNode * pcurNode;
			TNode * pnextNode;

			pcurNode = m_pUsedList;
			
			while (pcurNode)
			{
				pnextNode = pcurNode->m_pNext;
				Free(pcurNode);
				pcurNode = pnextNode;
			}
			
			assert(NULL==m_pUsedList);
		}

		void Clear()
		{
			TNode* pcurNode;
			TNode* pnextNode;

			DWORD count = 0;

			pcurNode = m_pFreeList;
			while (pcurNode)
			{
				pnextNode = pcurNode->m_pNext;
				delete pcurNode;
				pcurNode = pnextNode;
				++count;
			}
			m_pFreeList = NULL;
			
			pcurNode = m_pUsedList;
			while (pcurNode)
			{
				pnextNode = pcurNode->m_pNext;
				delete pcurNode;
				pcurNode = pnextNode;
				++count;
			}

			m_pUsedList = NULL;
						
			assert(count==m_nodeCount && "CDynamicPool::Clear()");

			m_nodeCount=0;
		}

	protected:
		TNode* AllocNode()
		{
			++m_nodeCount;
			return new TNode;
		}

	protected:
		TNode *		m_nodes;
		TNode *		m_pFreeList;
		TNode *		m_pUsedList;

		DWORD		m_nodeCount;
		std::string	m_stName;
};
*/