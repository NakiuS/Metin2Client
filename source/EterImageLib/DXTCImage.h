#ifndef AFX_IMAGE_DXTC_H__4B89D8D0_7857_11D4_9630_00A0C996DE3D__INCLUDED_
#define AFX_IMAGE_DXTC_H__4B89D8D0_7857_11D4_9630_00A0C996DE3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

enum EPixFormat
{
	PF_ARGB,
	PF_DXT1,
	PF_DXT2,
	PF_DXT3,
	PF_DXT4,
	PF_DXT5,
	PF_UNKNOWN
};

#define MAX_MIPLEVELS 12

#ifndef DUMMYUNIONNAMEN
#if defined(__cplusplus) || !defined(NONAMELESSUNION)
#define DUMMYUNIONNAMEN(n)
#else
#define DUMMYUNIONNAMEN(n)      u##n
#endif
#endif

typedef struct _XDDPIXELFORMAT
{
	DWORD       dwSize;                 // size of structure
	DWORD       dwFlags;                // pixel format flags
	DWORD       dwFourCC;               // (FOURCC code)

    union
    {
        DWORD   dwRGBBitCount;          // how many bits per pixel
        DWORD   dwYUVBitCount;          // how many bits per pixel
        DWORD   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
        DWORD   dwAlphaBitDepth;        // how many bits for alpha channels
        DWORD   dwLuminanceBitCount;    // how many bits per pixel
        DWORD   dwBumpBitCount;         // how many bits per "buxel", total
        DWORD   dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture
		// format list and if DDPF_D3DFORMAT is set
    } DUMMYUNIONNAMEN(1);
    union
    {
        DWORD   dwRBitMask;             // mask for red bit
        DWORD   dwYBitMask;             // mask for Y bits
        DWORD   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
        DWORD   dwLuminanceBitMask;     // mask for luminance bits
        DWORD   dwBumpDuBitMask;        // mask for bump map U delta bits
        DWORD   dwOperations;           // DDPF_D3DFORMAT Operations
    } DUMMYUNIONNAMEN(2);
    union
    {
        DWORD   dwGBitMask;             // mask for green bits
        DWORD   dwUBitMask;             // mask for U bits
        DWORD   dwZBitMask;             // mask for Z bits
        DWORD   dwBumpDvBitMask;        // mask for bump map V delta bits
        struct
        {
            WORD    wFlipMSTypes;       // Multisample methods supported via flip for this D3DFORMAT
            WORD    wBltMSTypes;        // Multisample methods supported via blt for this D3DFORMAT
        } MultiSampleCaps;
		
    } DUMMYUNIONNAMEN(3);
    union
    {
        DWORD   dwBBitMask;             // mask for blue bits
        DWORD   dwVBitMask;             // mask for V bits
        DWORD   dwStencilBitMask;       // mask for stencil bits
        DWORD   dwBumpLuminanceBitMask; // mask for luminance in bump map
    } DUMMYUNIONNAMEN(4);
    union
    {
        DWORD   dwRGBAlphaBitMask;      // mask for alpha channel
        DWORD   dwYUVAlphaBitMask;      // mask for alpha channel
        DWORD   dwLuminanceAlphaBitMask;// mask for alpha channel
        DWORD   dwRGBZBitMask;          // mask for Z channel
        DWORD   dwYUVZBitMask;          // mask for Z channel
    } DUMMYUNIONNAMEN(5);
} XDDPIXELFORMAT;


class CDXTCImage
{
	public:
		CDXTCImage();
		virtual ~CDXTCImage();

		void	Initialize();
		void	Clear();
		
	public:
		const BYTE *		m_pbCompBufferByLevels[MAX_MIPLEVELS];
		std::vector<BYTE>	m_bCompVector[MAX_MIPLEVELS];

		int					m_nCompSize;
		int					m_nCompLineSz;

		char				m_strFormat[32];
		EPixFormat			m_CompFormat;
		
		long				m_lPitch;
		DWORD				m_dwMipMapCount;
		bool				m_bMipTexture;	// texture has mipmaps?
		DWORD				m_dwFlags;
		
		int					m_nWidth;		// in pixels of uncompressed image 
		int					m_nHeight;
		
		XDDPIXELFORMAT		m_xddPixelFormat;

		bool LoadFromFile(const char * filename);		// true if success
		bool LoadFromMemory(const BYTE * c_pbMap);
		bool LoadHeaderFromMemory(const BYTE * c_pbMap);
		bool Copy(int miplevel, BYTE * pbDest, long lDestPitch);

		void Decompress(int miplevel, DWORD * pdwDest);
		void DecompressDXT1(int miplevel, DWORD * pdwDest);
		void DecompressDXT3(int miplevel, DWORD * pdwDest);
		void DecompressDXT5(int miplevel, DWORD * pdwDest);
		void DecompressARGB(int miplevel, DWORD * pdwDest);

		VOID DecodePixelFormat(CHAR* strPixelFormat, XDDPIXELFORMAT* pddpf);

		void Unextract(BYTE * pbDest, int iWidth, int iHeight, int iPitch);
		/*
		struct TimingInfo;			// defined in Image_DXTC.cpp
		void RunTimingSession();	// run a few methods & time the code

		// must use dxt5 texture
		void Time_Decomp5_01(int ntimes, TimingInfo * info);
		void Time_Decomp5_02(int ntimes, TimingInfo * info);
		void Time_Decomp5_03(int ntimes, TimingInfo * info);
		void Time_Decomp5_04(int ntimes, TimingInfo * info);
		*/
};

#endif // #ifndef AFX_IMAGE_DXTC_H__4B89D8D0_7857_11D4_9630_00A0C996DE3D__INCLUDED_
