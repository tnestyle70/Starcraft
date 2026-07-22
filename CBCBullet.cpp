#include "pch.h"
#include "CBCBullet.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"

CBCBullet::CBCBullet()
{
}

CBCBullet::~CBCBullet()
{
    Release();
}

void CBCBullet::Initialize()
{
    __super::Initialize();

    m_tInfo.fCX = 128.f;
    m_tInfo.fCY = 128.f;
    m_fSpeed = 1000.f;      // 빠른 속도
    m_iDamage = 15;        // 기본 공격 데미지
    m_bHoming = false;     // 직선으로 날아감

    m_pFrameKey = L"BCBullet";
    m_eRender = RENDER_WORLD;
}

int CBCBullet::Update()
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
            m_pTarget->TakeDamage(m_iDamage);
            Set_Dead();
            return DEAD;
        }
    }

    return NOEVENT;
}

void CBCBullet::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

    // ? 16방향 스프라이트 사용
    int iScrX = 0;                      // 애니메이션이 없으면 0
    int iScrY = m_iDirection16 * 128;   // 16방향 중 현재 방향

    GdiTransparentBlt(hDC,
        iDrawX,
        iDrawY,
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hMemDC,
        iScrX,      // ← 0
        iScrY,      // ← m_iDirection16 * 128 (0, 128, 256, ...)
        128,
        128,
        RGB(0, 0, 0));
}

void CBCBullet::Release()
{
}
