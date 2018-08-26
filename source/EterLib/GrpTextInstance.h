#ifndef __INC_ETERLIB_GRPTEXTINSTANCE_H__
#define __INC_ETERLIB_GRPTEXTINSTANCE_H__

#include "Pool.h"
#include "GrpText.h"

class CGraphicTextInstance
{
	public:
		typedef CDynamicPool<CGraphicTextInstance> TPool;

	public:
		enum EHorizontalAlign
		{
			HORIZONTAL_ALIGN_LEFT		= 0x01,
			HORIZONTAL_ALIGN_CENTER		= 0x02,
			HORIZONTAL_ALIGN_RIGHT		= 0x03,
		};
		enum EVerticalAlign
		{
			VERTICAL_ALIGN_TOP		= 0x10,
			VERTICAL_ALIGN_CENTER	= 0x20,
			VERTICAL_ALIGN_BOTTOM	= 0x30
		};

	public:
		static void Hyperlink_UpdateMousePos(int x, int y);
		static int  Hyperlink_GetText(char* buf, int len);

	public:
		CGraphicTextInstance();
		virtual ~CGraphicTextInstance();

		void Destroy();
		
		void Update();
		void Render(RECT * pClipRect = NULL);

		void ShowCursor();
		void HideCursor();

		void ShowOutLine();
		void HideOutLine();

		void SetColor(DWORD color);
		void SetColor(float r, float g, float b, float a = 1.0f);

		void SetOutLineColor(DWORD color);
		void SetOutLineColor(float r, float g, float b, float a = 1.0f);

		void SetHorizonalAlign(int hAlign);
		void SetVerticalAlign(int vAlign);
		void SetMax(int iMax);
		void SetTextPointer(CGraphicText* pText);
		void SetValueString(const string& c_stValue);
		void SetValue(const char* c_szValue, size_t len = -1);
		void SetPosition(float fx, float fy, float fz = 0.0f);
		void SetSecret(bool Value);
		void SetOutline(bool Value);
		void SetFeather(bool Value);
		void SetMultiLine(bool Value);
		void SetLimitWidth(float fWidth);

		void GetTextSize(int* pRetWidth, int* pRetHeight);
		const std::string& GetValueStringReference();
		WORD GetTextLineCount();

		int PixelPositionToCharacterPosition(int iPixelPosition);
		int GetHorizontalAlign();

	protected:
		void __Initialize();
		int  __DrawCharacter(CGraphicFontTexture * pFontTexture, WORD codePage, wchar_t text, DWORD dwColor);
		void __GetTextPos(DWORD index, float* x, float* y);
		int __GetTextTag(const wchar_t * src, int maxLen, int & tagLen, std::wstring & extraInfo);

	protected:
		struct SHyperlink
		{
			short sx;
			short ex;
			std::wstring text;

			SHyperlink() : sx(0), ex(0) { }
		};

	protected:
		DWORD m_dwTextColor;
		DWORD m_dwOutLineColor;

		WORD m_textWidth;
		WORD m_textHeight;

		BYTE m_hAlign;
		BYTE m_vAlign;

		WORD m_iMax;
		float m_fLimitWidth;

		bool m_isCursor;
		bool m_isSecret;
		bool m_isMultiLine;

		bool m_isOutline;
		float m_fFontFeather;

		/////

		std::string m_stText;
		D3DXVECTOR3 m_v3Position;

	private:
		bool m_isUpdate;
		bool m_isUpdateFontTexture;
		
		CGraphicText::TRef m_roText;
		CGraphicFontTexture::TPCharacterInfomationVector m_pCharInfoVector;
		std::vector<DWORD> m_dwColorInfoVector;
		std::vector<SHyperlink> m_hyperlinkVector;

	public:
		static void CreateSystem(UINT uCapacity);
		static void DestroySystem();

		static CGraphicTextInstance* New();
		static void Delete(CGraphicTextInstance* pkInst);

		static CDynamicPool<CGraphicTextInstance>		ms_kPool;
};

extern const char* FindToken(const char* begin, const char* end);
extern int ReadToken(const char* token);

#endif