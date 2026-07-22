#include "pch.h"
#include "CButton.h"
#include "CBmpMgr.h"
#include "CSceneMgr.h"
#include "CInputMgr.h"
#include "CObjMgr.h"
#include "CSoundMgr.h"

CButton::CButton() :
	m_bHover(false)
	,m_eButtonType(eButtonType::BMP)
	,m_pNormalKey(nullptr)
	,m_pHoverKey(nullptr)
{
}

CButton::~CButton()
{
}

void CButton::Initialize()
{
	m_eRender = RENDER_UI;
}

int CButton::Update()
{
	__super::Update_Rect();

	return 0;
}

void CButton::Late_Update()
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	m_bHover = PtInRect(&m_tRect, ptMouse);

	if (m_bHover)
	{
		m_iDrawID = 0;

		if (CInputMgr::Get_Instance()->KeyPress(LEFT_MOUSE))
		{
			if (!lstrcmp(L"Start", m_pFrameKey) ||
				!lstrcmp(L"Btn_Single_Normal", m_pNormalKey))
			{
				CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU_RACE);
			}
			else if (!lstrcmp(L"Edit", m_pFrameKey) ||
				!lstrcmp(L"Btn_Editor_Normal", m_pNormalKey))
			{
				CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_EDIT);
			}
			else if (!lstrcmp(L"Exit", m_pFrameKey) ||
				!lstrcmp(L"Btn_Exit_Normal", m_pNormalKey))
			{
				DestroyWindow(g_hWnd);
			}
			else if (!lstrcmp(L"Exit", m_pFrameKey) ||
				!lstrcmp(L"Btn_Cancle_Normal", m_pNormalKey))
			{
				CSoundMgr::Get_Instance()->PlaySound(L"Mouse/MouseDown.wav", SOUND_UI, 0.4f);
				CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU);
			}
			//else if (!lstrcmp(L"MENU_BTN", m_pFrameKey))
			//{
			//	CSoundMgr::Get_Instance()->PlaySound(L"Mouse/MouseDown.wav", SOUND_UI, 0.4f);
			//	CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU);
			//}
			else if (!lstrcmp(L"WIN_TEXT", m_pFrameKey))
			{
				CSoundMgr::Get_Instance()->PlaySound(L"Mouse/MouseDown.wav", SOUND_UI, 0.4f);
				CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU);
			}
		}
	}
	else
	{
		m_iDrawID = 0;
	}
}

void CButton::Render(HDC hDC)
{
	CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(m_pNormalKey);
	if (pPng)
	{
		int renderX = m_tRect.left;
		int renderY = m_tRect.top;
		pPng->Render_Alpha(hDC, renderX, renderY);
	}
	HDC	hBmpDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);
	if (hBmpDC)
	{
		GdiTransparentBlt(hDC,
			m_tRect.left,
			m_tRect.top,
			(int)m_tInfo.fCX,
			(int)m_tInfo.fCY,
			hBmpDC,
			(int)m_tInfo.fCX * m_iDrawID,
			0,
			(int)m_tInfo.fCX,		// 복사할 이미지의 가로 사이즈
			(int)m_tInfo.fCY,		// 복사할 이미지의 세로 사이즈
			RGB(0, 0, 0));
	}
}

void CButton::Release()
{
}

void CButton::Set_PngImages(const TCHAR* pNormalKey, const TCHAR* pHoverKey)
{
	m_eButtonType = eButtonType::PNG;
	m_pNormalKey = pNormalKey;
	m_pHoverKey = pHoverKey;

	// PNG 크기로 버튼 크기 설정
	CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(pNormalKey);
	if (pPng)
	{
		m_tInfo.fCX = (float)pPng->Get_Width();
		m_tInfo.fCY = (float)pPng->Get_Height();
	}
}

void CButton::Set_BmpFrameKey(const TCHAR* pFrameKey)
{
	m_eButtonType = eButtonType::BMP;
	m_pFrameKey = pFrameKey;

	//BMP 크기로 버튼 크기 설정
	HBITMAP hBmp = CBmpMgr::Get_Instance()->Find_Bitmap(m_pFrameKey);
	if (hBmp)
	{
		BITMAP bm;
		GetObject(hBmp, sizeof(BITMAP), &bm);
		m_tInfo.fCX = (float)bm.bmWidth;
		m_tInfo.fCY = (float)bm.bmHeight;
	}
}

//void CButton::SetAtlas(const wchar_t* atlasKey, const AtlasRect& normal, const AtlasRect& hover)
//{
//	m_pAtlasKey = atlasKey;
//	m_rcNormal = normal;
//	m_rcHover = hover;
//
//	// 판정 Rect와 렌더 크기를 동일하게 맞추기
//	m_tInfo.fCX = (float)normal.iWidth;
//	m_tInfo.fCY = (float)normal.iHeight;
//}
