#include "pch.h"
#include "CGas.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"

CGas::CGas()
{
}

CGas::~CGas()
{
}

void CGas::Initialize()
{
    m_tInfo.fCX = 32.f;
    m_tInfo.fCY = 32.f;
    m_bSelectable = false;
    m_eRender = RENDER_WORLD;
}

int CGas::Update()
{
    if (m_bDead)
        return DEAD;

    __super::Update_Rect();

    return 0;
}

void CGas::Late_Update()
{
}

void CGas::Render(HDC hDC)
{

    //int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    //int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    //// 디버그용 미네랄 영역 표시 (32x32)
    //Rectangle(hDC,
    //    (int)(m_tRect.left - iScrollX),
    //    (int)(m_tRect.top - iScrollY),
    //    (int)(m_tRect.right - iScrollX),
    //    (int)(m_tRect.bottom - iScrollY));

    /*
    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);
    if (!hMemDC)
        return;

    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    GdiTransparentBlt(hDC,
        (int)(m_tRect.left - iScrollX),
        (int)(m_tRect.top - iScrollY),
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hMemDC,
        0, 0,
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        RGB(0, 255, 0));
        */
}

void CGas::Release()
{
}

