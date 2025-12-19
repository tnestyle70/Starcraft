#include "pch.h"
#include "CScrollMgr.h"

CScrollMgr* CScrollMgr::m_pInstance = nullptr;

CScrollMgr::CScrollMgr() : m_fScrollX(0.f), m_fScrollY(0.f)
{
}

CScrollMgr::~CScrollMgr()
{
}

void CScrollMgr::Scroll_Lock()
{
	if (0.f < m_fScrollX)
		m_fScrollX = 0.f;

	if (0.f < m_fScrollY)
		m_fScrollY = 0.f;

	if (WINCX - 4096.f > m_fScrollX)
		m_fScrollX = WINCX - 4096.f;

	if (WINCY - 4096.f > m_fScrollY)
		m_fScrollY = WINCY - 4096.f;
}

Vec2 CScrollMgr::ScreenToWorld(POINT pt)
{
    float fScrX = Get_Instance()->Get_ScrollX();
    float fScrY = Get_Instance()->Get_ScrollY();

    return Vec2{pt.x + fScrX, pt.y + fScrY};
}

POINT CScrollMgr::WorldToScreen(const Vec2& w)
{
    float fScrX = Get_Instance()->Get_ScrollX();
    float fScrY = Get_Instance()->Get_ScrollY();

    return POINT{ (LONG)(w.fX - fScrX), (LONG)(w.fY - fScrY) };
}

void CScrollMgr::Update_EdgeScroll(float dt)
{
    POINT pt{};
    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);

    int dirX = 0, dirY = 0;

    if (pt.x <= EDGE_MARGIN) dirX = -1;
    else if (pt.x >= WINCX - EDGE_MARGIN) dirX = 1;

    if (pt.y <= EDGE_MARGIN) dirY = -1;
    else if (pt.y >= WINCY - EDGE_MARGIN) dirY = 1;

    m_fScrollX += dirX * SCROLL_SPEED * dt;
    m_fScrollY += dirY * SCROLL_SPEED * dt;

    ClampToWorld();
}

void CScrollMgr::ClampToWorld()
{
    // 스크롤은 "월드 좌표에서 카메라(화면)가 어디를 보느냐"로 생각하면 편함
    const float maxX = (float)(WORLD_W - WINCX);
    const float maxY = (float)(WORLD_H - WINCY);

    if (m_fScrollX < 0.f) m_fScrollX = 0.f;
    if (m_fScrollY < 0.f) m_fScrollY = 0.f;

    if (m_fScrollX > maxX) m_fScrollX = maxX;
    if (m_fScrollY > maxY) m_fScrollY = maxY;
}
