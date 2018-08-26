#ifndef __INC_ETERIMAGELIB_TGAIMAGE_H__
#define __INC_ETERIMAGELIB_TGAIMAGE_H__

#include "Image.h"

class CTGAImage : public CImage
{
	public:
		enum ETGAImageFlags
		{
			FLAG_RLE_COMPRESS = (1 << 0)
		};
			
		CTGAImage();
		CTGAImage(CImage &image);
		virtual ~CTGAImage();

		virtual void	Create(int width, int height);
		virtual bool	LoadFromMemory(int iSize, const BYTE * c_pbMem);
		virtual bool	LoadFromDiskFile(const char * c_szFileName);
		virtual bool	SaveToDiskFile(const char* c_szFileName);

		void			SetCompressed(bool isCompress = true);
		void			SetAlphaChannel(bool isExist = true);

		TGA_HEADER &	GetHeader();
	protected:
		int			GetRawPixelCount(const DWORD * data);
		int			GetRLEPixelCount(const DWORD * data);

	protected:
		TGA_HEADER	m_Header;
		DWORD		m_dwFlag;
		DWORD *		m_pdwEndPtr;
};

#endif
