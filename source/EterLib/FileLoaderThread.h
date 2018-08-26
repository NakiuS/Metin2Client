#ifndef __INC_YMIR_ETERLIB_FILELOADERTHREAD_H__
#define __INC_YMIR_ETERLIB_FILELOADERTHREAD_H__

#include <deque>
#include "Thread.h"
#include "Mutex.h"
#include "../eterBase/MappedFile.h"

class CFileLoaderThread 
{
	public:
		typedef struct SData
		{
			std::string	stFileName;

			CMappedFile	File;
			LPVOID		pvBuf;
			DWORD		dwSize;
		} TData;

	public:
		CFileLoaderThread();
		~CFileLoaderThread();

		int Create(void * arg);
	
	public:
		void	Request(std::string & c_rstFileName);
		bool	Fetch(TData ** ppData);
		void	Shutdown();

	protected:
		static UINT CALLBACK	EntryPoint(void * pThis);
		UINT					Run(void * arg);

		void *					Arg() const		{ return m_pArg; }
		void					Arg(void * arg) { m_pArg = arg; }
		
		HANDLE					m_hThread;

	private:
		void *					m_pArg;
		unsigned				m_uThreadID;

	protected:
		UINT					Setup();
		UINT					Execute(void * pvArg);
		void					Destroy();
		void					Process();

	private:
		std::deque<TData *>		m_pRequestDeque;
		Mutex					m_RequestMutex;

		std::deque<TData *>		m_pCompleteDeque;
		Mutex					m_CompleteMutex;

		HANDLE					m_hSemaphore;
		int						m_iRestSemCount;
		bool					m_bShutdowned;
};

#endif