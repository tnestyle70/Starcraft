#pragma once
#include "CMyImage.h"
#include "CMyPng.h"
#include "CMyBmp.h"
#include <string>

class CBmpMgr 
{
private:
	CBmpMgr();
	CBmpMgr(const CBmpMgr& rhs) = delete;
	CBmpMgr& operator=(CBmpMgr& rObj) = delete;
	~CBmpMgr();
public:
	void	Insert_Bmp(const TCHAR* pFilePath, const TCHAR* pImgKey);
	void	Insert_Png(const TCHAR* pFilePath, const TCHAR* pImgKey);
	HDC		Find_Image(const TCHAR* pImgKey);
	HBITMAP Find_Bitmap(const TCHAR* pImgKey);
	CMyPng* Find_Png(const TCHAR* pImgKey);
	//애니메이션 버튼용 전용 맵 사용
	void	Insert_Png_Anim(const TCHAR* pFilePath, const TCHAR* pImgKey);
	CMyPng* Find_Png_Anim(const TCHAR* pImgKey);
	void	Release();
	//Alpha 채널 렌더
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
	map<wstring, CMyImage*> m_mapPng;
	map<const TCHAR*, CMyImage*> m_mapBit;
};

