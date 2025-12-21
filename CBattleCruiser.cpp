#include "pch.h"
#include "CBattleCruiser.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"

CBattleCruiser::CBattleCruiser()
{
}

CBattleCruiser::~CBattleCruiser()
{
    Release();
}

void CBattleCruiser::Initialize()
{
    m_tInfo.fCX = 120.f; //마린 한 칸 크기
    m_tInfo.fCY = 120.f;

    m_fSpeed = 200.f;

    m_pFrameKey = L"BattleCruiser";

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 0;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
}

int CBattleCruiser::Update()
{
    int iResult = CUnit::Update();

    if (iResult == DEAD)
        return DEAD;

    DWORD now = GetTickCount();

    switch (m_eState)
    {
    case eUnitState::IDLE:
        m_tFrame.iStart = 0;
        break;
    case eUnitState::MOVE:
        m_tFrame.iFrame = DirTo16WayIndex(m_vDir);
        break;
    case eUnitState::ATTACK:
        break;
    case eUnitState::DIE:
        break;
    default:
        break;
    }

    __super::Update_Rect();

    return NOEVENT;
}

void CBattleCruiser::Late_Update()
{
    //선택이 되었을 경우 마우스 방향의 애니메이션 재생
    if (!m_bSelected) return;
    //이동 중이면 마우스 방향 애니메이션 재생 멈추기
    if (m_eState != eUnitState::IDLE) return;

    Vec2 vWorldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 vDir{ vWorldMouse.fX - m_tInfo.fX, vWorldMouse.fY - m_tInfo.fY };

    int iDir16 = DirTo16WayIndex(vDir);
    m_tFrame.iFrame = iDir16;
}

void CBattleCruiser::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    //선택 원(예: m_bSelected가 true일 때) 추후에 bmp로 교체
    if (m_bSelected)
    {
        HBRUSH oldB = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
        HPEN oldP = (HPEN)SelectObject(hDC, pen);

        int cx = iDrawX + (int)(m_tInfo.fCX * 0.5f);
        int cy = iDrawY + (int)(m_tInfo.fCY * 0.8f);   // 발밑 느낌으로 살짝 아래
        int r = (int)(max(m_tInfo.fCX, m_tInfo.fCY) * 0.55f);

        Ellipse(hDC, cx - r, cy - r / 2, cx + r, cy + r / 2);

        SelectObject(hDC, oldP);
        SelectObject(hDC, oldB);
        DeleteObject(pen);
    }

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

    int iScrX = m_tFrame.iStart * (int)m_tInfo.fCX;
    int iScrY = m_tFrame.iFrame * (int)m_tInfo.fCY;

    GdiTransparentBlt(hDC,
        iDrawX,
        iDrawY,
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hMemDC,
        iScrX,
        iScrY,
        (int)m_tInfo.fCX,		// 복사할 이미지의 가로 사이즈
        (int)m_tInfo.fCY,		// 복사할 이미지의 세로 사이즈
        RGB(0, 0, 0));
}

void CBattleCruiser::Release()
{
}
