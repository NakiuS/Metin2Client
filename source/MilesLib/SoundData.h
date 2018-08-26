#ifndef __MILESLIB_CSOUNDDATA_H__
#define __MILESLIB_CSOUNDDATA_H__

#include <mss.h>
#include "../eterBase/MappedFile.h"

class CSoundData
{
	public:
		enum
		{
			FLAG_DATA_SIZE = 1,
			SOUND_FILE_MAX_NUM = 5,
		};

	public:
		static void		SetPackMode();

		CSoundData();
		virtual ~CSoundData();

		void			Assign(const char* filename);
		LPVOID			Get();
		ULONG			GetSize();
		void			Release();
		DWORD			GetAccessTime();
		const char *	GetFileName();

		void			SetPlayTime(DWORD dwPlayTime);
		DWORD			GetPlayTime();

	protected:
		bool			ReadFromDisk();
		void			Destroy();
		
	protected:
		char			m_filename[128];
		int				m_iRefCount;
		DWORD			m_dwAccessTime;
		DWORD			m_dwPlayTime;
		ULONG			m_size;
		LPVOID			m_data;
		long			m_flag;
		bool			m_assigned;

	private:
		static U32 AILCALLBACK		open_callback(char const * filename, U32 *file_handle);
		static void AILCALLBACK		close_callback(U32 file_handle);
		static S32 AILCALLBACK		seek_callback(U32 file_handle, S32 offset, U32 type);
		static U32 AILCALLBACK		read_callback(U32 file_handle, void *buffer, U32 bytes);

		static bool					isSlotIndex(DWORD dwIndex);
		static int					GetEmptySlotIndex();

		static bool					ms_isSoundFile[SOUND_FILE_MAX_NUM];
		static CMappedFile			ms_SoundFile[SOUND_FILE_MAX_NUM];
};

#endif