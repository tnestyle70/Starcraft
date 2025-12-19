#include "pch.h"
#include "CButton.h"
#include "CBmpMgr.h"
#include "CSceneMgr.h"
#include "CInputMgr.h"
#include "CObjMgr.h"

CButton::CButton() : m_bHover(false)
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

	if (PtInRect(&m_tRect, ptMouse))
	{
		if (CInputMgr::Get_Instance()->KeyPress(LEFT_MOUSE))
		{
			if (!lstrcmp(L"Start", m_pFrameKey))
				CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_STAGE);

			else if (!lstrcmp(L"Edit", m_pFrameKey))
				CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_EDIT);

			else if (!lstrcmp(L"Exit", m_pFrameKey))
				DestroyWindow(g_hWnd);

			return;
		}

		m_iDrawID = 1;
	}
	else
	{
		m_iDrawID = 0;
	}
}

void CButton::Render(HDC hDC)
{
	HDC	hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

	BitBlt(hDC,
		m_tRect.left,
		m_tRect.top,
		(int)m_tInfo.fCX,
		(int)m_tInfo.fCY,
		hMemDC,
		(int)m_tInfo.fCX * m_iDrawID,
		0,
		SRCCOPY);
	/*
	GdiTransparentBlt(hDC,
		m_tRect.left,
		m_tRect.top,
		(int)m_tInfo.fCX,
		(int)m_tInfo.fCY,
		hMemDC,
		(int)m_tInfo.fCX * m_iDrawID,
		0,
		(int)m_tInfo.fCX,		// 복사할 이미지의 가로 사이즈
		(int)m_tInfo.fCY,		// 복사할 이미지의 세로 사이즈
		RGB(255, 255, 255));
		*/
}

void CButton::Release()
{
}

void CButton::SetAtlas(const wchar_t* atlasKey, const AtlasRect& normal, const AtlasRect& hover)
{
	m_pAtlasKey = atlasKey;
	m_rcNormal = normal;
	m_rcHover = hover;

	// 판정 Rect와 렌더 크기를 동일하게 맞추기
	m_tInfo.fCX = (float)normal.iWidth;
	m_tInfo.fCY = (float)normal.iHeight;
}
