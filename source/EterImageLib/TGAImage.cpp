#include "StdAfx.h"

#include <assert.h>

#include "../eterBase/MappedFile.h"
#include "TGAImage.h"

CTGAImage::CTGAImage() : m_dwFlag(0)
{
}

CTGAImage::~CTGAImage()
{
}

CTGAImage::CTGAImage(CImage &image) : m_dwFlag(0)
{
	int w = image.GetWidth();
	int h = image.GetHeight();

	Create(w, h);

	DWORD * pdwDest = GetBasePointer();
	memcpy(pdwDest, image.GetBasePointer(), w * h * sizeof(DWORD));
	FlipTopToBottom();
}

void CTGAImage::Create(int width, int height)
{
	memset(&m_Header, 0, sizeof(m_Header));

	m_Header.imgType	= 2;
	m_Header.width		= (short) width;
	m_Header.height		= (short) height;
	m_Header.colorBits	= 32;
	m_Header.desc		= 0x08;	// alpha channel 있음

	CImage::Create(width, height);
}

bool CTGAImage::LoadFromMemory(int iSize, const BYTE * c_pbMem)
{
	memcpy(&m_Header, c_pbMem, 18);
	c_pbMem += 18;
	iSize -= 18;

	CImage::Create(m_Header.width, m_Header.height);

	UINT hxw = m_Header.width * m_Header.height;
	BYTE r, g, b, a;
	DWORD i;

	DWORD * pdwDest = GetBasePointer();

	switch (m_Header.imgType)
	{
		case 3:	// 알파만 있는 것 (1bytes per pixel, 거의 안쓰임)
			{
				for (i = 0; i < hxw; ++i)
				{
					a = (char) *(c_pbMem++);
					pdwDest[i] = (a << 24) | (a << 16) | (a << 8) | a;
				}
			}
			break;

		case 2:	// 압축 안된 TGA
			{
				if (m_Header.colorBits == 16)	// 16bit
				{
					for (i = 0; i < hxw; ++i)
					{
						WORD w;

						memcpy(&w, c_pbMem, sizeof(WORD));
						c_pbMem += sizeof(WORD);
						iSize -= sizeof(WORD);
						
						b = (BYTE) (w & 0x1F);
						g = (BYTE) ((w >> 5) & 0x1F);
						r = (BYTE) ((w >> 10) & 0x1F);
						
						b <<= 3;
						g <<= 3;
						r <<= 3;
						a = 0xff;
						
						pdwDest[i] = (a << 24) | (r << 16) | (g << 8) | b;
					}
				}
				else if (m_Header.colorBits == 24)	// 24bit
				{
					for (i = 0; i < hxw; ++i)
					{
						r = (BYTE) *(c_pbMem++); --iSize;
						g = (BYTE) *(c_pbMem++); --iSize;
						b = (BYTE) *(c_pbMem++); --iSize;
						a = 0xff;
						
						pdwDest[i] = (a << 24) | (r << 16) | (g << 8) | b;
					}
				}
				else if (m_Header.colorBits == 32)	// 32bit
				{
					int size = GetWidth();
					size *= GetHeight() * 4;

					memcpy(pdwDest, c_pbMem, size);
					c_pbMem += size;
					iSize -= size;
				}
			}
			break;

		case 10: // 압축 된 TGA (RLE)
			{
				BYTE rle;

				if (m_Header.colorBits == 24)
				{
					i = 0;
					while (i < hxw)
					{
						rle = (BYTE) *(c_pbMem++); --iSize;

						if (rle < 0x80)	// 압축 안된 곳
						{
							rle++;

							while (rle)
							{
								b = (BYTE) *(c_pbMem++); --iSize;
								g = (BYTE) *(c_pbMem++); --iSize;
								r = (BYTE) *(c_pbMem++); --iSize;
								a = 0xff;
								pdwDest[i++] = (a << 24) | (r << 16) | (g << 8) | b;

								if (i > hxw)
								{
									assert(!"RLE overflow");
									printf("RLE overflow");
									return false;
								}
								--rle;
							}
						}
						else
						{
							// 압축 된 곳
							rle -= 127;

							b = (BYTE) *(c_pbMem++); --iSize;
							g = (BYTE) *(c_pbMem++); --iSize;
							r = (BYTE) *(c_pbMem++); --iSize;
							a = 0xff;

							while (rle)
							{
								pdwDest[i++] = (a << 24) | (r << 16) | (g << 8) | b;

								if (i > hxw)
								{
									assert(!"RLE overflow");
									printf("RLE overflow");
									return false;
								}
								--rle;
							}
						}
					}
				}
				else if (m_Header.colorBits == 32)
				{
					i = 0;
					while (i < hxw)
					{
						rle = (BYTE) *(c_pbMem++); --iSize;
						
						if (rle < 0x80)
						{
							rle++;
							
							while (rle)
							{
								b = (BYTE) *(c_pbMem++); --iSize;
								g = (BYTE) *(c_pbMem++); --iSize;
								r = (BYTE) *(c_pbMem++); --iSize;
								a = (BYTE) *(c_pbMem++); --iSize;
								pdwDest[i++] = (a << 24) | (r << 16) | (g << 8) | b;
								
								if (i > hxw)
								{
									assert(!"RLE overflow");
									printf("RLE overflow");
									return false;
								}
								--rle;
							}
						}
						else
						{
							rle -= 127;
							
							b = (BYTE) *(c_pbMem++); --iSize;
							g = (BYTE) *(c_pbMem++); --iSize;
							r = (BYTE) *(c_pbMem++); --iSize;
							a = (BYTE) *(c_pbMem++); --iSize;
							
							while (rle)
							{
								pdwDest[i++] = (a << 24) | (r << 16) | (g << 8) | b;
								
								if (i > hxw)
								{
									assert(!"RLE overflow");
									printf("RLE overflow");
									return false;
								}

								--rle;
							}
						}
					}
				}
			}
			break;
	}
	
	if (!(m_Header.desc & 0x20))
	{
		FlipTopToBottom();
	}

	return true;
}

bool CTGAImage::LoadFromDiskFile(const char * c_szFileName)
{
	CMappedFile file;

	const BYTE * c_pbMap;

	if (!file.Create(c_szFileName, (const void **) &c_pbMap, 0, 0))
		return false;

	return LoadFromMemory(file.Size(), c_pbMap);
}

int CTGAImage::GetRLEPixelCount(const DWORD * data)
{
    int r = 0;
	DWORD pixel;
    
    r = 1;
	
    if (data >= m_pdwEndPtr)
        return 0;
    
	pixel = *data;
    
    while ((r < 127) && (data < m_pdwEndPtr))
    {
		if (pixel != *(++data))
			return r;
		
        r++;
    }
	
	return r;
}

int CTGAImage::GetRawPixelCount(const DWORD * data)
{
    int i = 0;
    
    if (data >= m_pdwEndPtr)
        return 0;
	
    while ((data < m_pdwEndPtr) && (i < 127))
    {
		int rle = GetRLEPixelCount(data);
		
		if (rle >= 4)
			break;

        data++;
        i++;
    }
	
    return i;
}

void CTGAImage::SetCompressed(bool isCompress)
{
	if (isCompress)
		m_Header.imgType = 10;
	else
		m_Header.imgType = 2;
}

void CTGAImage::SetAlphaChannel(bool isExist)
{
	if (isExist)
		m_Header.desc |= 0x08;
	else
		m_Header.desc &= ~0x08;
}

bool CTGAImage::SaveToDiskFile(const char* c_szFileName)
{
	FILE * fp = fopen(c_szFileName, "wb");
	
	if (!fp)
		return false;

	fwrite(&m_Header, 18, 1, fp);
	
	if (m_Header.imgType == 10)	// RLE 압축으로 저장
	{
		DWORD * data = GetBasePointer();
		
		while (data < m_pdwEndPtr)
		{
			int rle = GetRLEPixelCount(data);
			
			if (rle < 4)
			{
				int raw = GetRawPixelCount(data);
				
				if (raw == 0)
					break;
				
				fputc(raw - 1, fp);
				
				while (raw)
				{
					fwrite(data, sizeof(DWORD), 1, fp);
					data++;
					raw--;
				}
			}
			else
			{
				fputc((rle - 1) | 0x80, fp);
				fwrite(data, sizeof(DWORD), 1, fp);
				data += rle;
			}
		}
	}
	else
	{
		int size = GetWidth();
		size *= GetHeight() * 4;
		fwrite(GetBasePointer(), size, 1, fp);
	}

	fclose(fp);
	return true;
}

TGA_HEADER & CTGAImage::GetHeader()
{
	return m_Header;
}
