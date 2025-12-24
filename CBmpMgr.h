#pragma once

#include "CMyBmp.h"

class CBmpMgr
{
private:
	CBmpMgr();
	CBmpMgr(const CBmpMgr& rhs) = delete;
	CBmpMgr& operator=(CBmpMgr& rObj) = delete;
	~CBmpMgr();

public:
	void	Insert_Bmp(const TCHAR* pFilePath, const TCHAR* pImgKey);
	HDC		Find_Image(const TCHAR* pImgKey);
	HBITMAP Find_Bitmap(const TCHAR* pImgKey);
	void	Release();
	//Alpha Ã¤³Î ·»´õ
	void Render_Alpha(const TCHAR* pImageKey, const TCHAR* pAlphaKey,
		HDC hDC, int x, int y, int width, int height);
	void Render_Alpha_Simple(const TCHAR* pImageKey,
		HDC hDC, int x, int y, int width, int height);
	void Render_Alpha_Tint(const TCHAR* pImageKey, const TCHAR* pAlphaKey,
		HDC hDC, int x, int y, int width, int height, COLORREF tintColor);

public:
	static CBmpMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CBmpMgr;
		}

		return m_pInstance;
	}

	static void	Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	static CBmpMgr* m_pInstance;

	map<const TCHAR*, CMyBmp*>		m_mapBit;

};

