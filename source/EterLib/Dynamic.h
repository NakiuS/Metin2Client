#pragma once

template<typename T>
class CDynamic
{
	public:
		struct FClear
		{
			void operator() (CDynamic<T>& rDynamic)
			{
				rDynamic.Clear();
			}
		};

	public:
		CDynamic()
		{
			Initialize();
		}

		~CDynamic()
		{
			Clear();
		}

		void Clear()
		{
			if (m_pObject)
				ms_objectPool.Free(m_pObject);
							
			Initialize();
		}

		T* GetUsablePointer()
		{
			if (!m_pObject)
				m_pObject = ms_objectPool.Alloc();
					
			return m_pObject;
		}

		bool IsNull() const
		{
			if (m_pObject)
				return false;
			return true;
		}

		T* GetPointer() const
		{
			assert(m_pObject != NULL);
			return m_pObject;
		}
		
		T* operator->() const
		{
			assert(m_pObject != NULL);
			return m_pObject;
		}

	private:
		T*	m_pObject;

	private:
		void Initialize()
		{
			m_pObject = NULL;
		}

	private:
		static CDynamicPool<T> ms_objectPool;
};

template<typename T>
CDynamicPool<T> CDynamic<T>::ms_objectPool;
