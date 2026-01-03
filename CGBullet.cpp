#include "pch.h"
#include "CGBullet.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"

CGBullet::CGBullet()
{
}

CGBullet::~CGBullet()
{
    Release();
}

void CGBullet::Initialize()
{
    __super::Initialize();

    m_tInfo.fCX = 32.f;
    m_tInfo.fCY = 32.f;
    m_fSpeed = 400.f;      // 느린 속도
    m_iDamage = 25;        // 기본 공격 데미지
    m_bHoming = false;     // 직선으로 날아감

    m_pFrameKey = L"GBullet";
    m_eRender = RENDER_WORLD;
}

int CGBullet::Update()
{
    int iResult = __super::Update();

    if (iResult == DEAD)
        return DEAD;

    // 시각 효과용 - 타겟 근처 도달하면 소멸
    if (m_pTarget && !m_pTarget->IsDead())
    {
        Vec2 targetPos = m_pTarget->Get_Pos();
        Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };

        float distX = targetPos.fX - myPos.fX;
        float distY = targetPos.fY - myPos.fY;
        float dist = sqrtf(distX * distX + distY * distY);

        // 타겟 근처 도달 (32픽셀 이내)
        if (dist < 32.f)
        {
            Set_Dead();
            return DEAD;
        }
    }

    return NOEVENT;
}

void CGBullet::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(m_pFrameKey);
    if (pPng)
    {
        pPng->Render_Alpha(hDC, iDrawX, iDrawY, 
            ((int)m_tInfo.fCX * 1.5), ((int)m_tInfo.fCY) * 1.5);
    }
}

void CGBullet::Release()
{
}
