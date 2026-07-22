#include "pch.h"
#include "CBCYamato.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"

CBCYamato::CBCYamato()
    : m_bCharged(false), m_dwChargeStart(0)
{
}

CBCYamato::~CBCYamato()
{
    Release();
}

void CBCYamato::Initialize()
{
    __super::Initialize();

    m_tInfo.fCX = 96.f;
    m_tInfo.fCY = 96.f;
    m_fSpeed = 600.f;        // 차징 중에는 정지
    m_iDamage = 200;       // 야마토 캐논 데미지
    m_bHoming = true;      // 타겟 추적
    m_bCharged = true;

    m_pFrameKey = L"BCYamato";
    m_eRender = RENDER_WORLD;

    m_dwChargeStart = GetTickCount();
}

int CBCYamato::Update()
{
    if (m_bDead)
        return DEAD;

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

        // 타겟 근처 도달 (64픽셀 이내)
        if (dist < 30.f)
        {
            m_pTarget->TakeDamage(m_iDamage);
            Set_Dead();
            return DEAD;
        }
    }

    __super::Update_Rect();

    return NOEVENT;
}

void CBCYamato::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

    //16방향 스프라이트 사용
    int iScrX = 0;
    int iScrY = m_iDirection16 * 96;  // 16방향 중 현재 방향

    // 차징 중일 때는 약간 작게 렌더링 (이펙트 효과)
    if (!m_bCharged)
    {
        GdiTransparentBlt(hDC,
            iDrawX,
            iDrawY,
            (int)m_tInfo.fCX,
            (int)m_tInfo.fCY,
            hMemDC,
            iScrX,
            iScrY,
            96,
            96,
            RGB(0, 0, 0));
    }
    else
    {
        GdiTransparentBlt(hDC,
            iDrawX,
            iDrawY,
            (int)m_tInfo.fCX,
            (int)m_tInfo.fCY,
            hMemDC,
            iScrX,
            iScrY,
            96,
            96,
            RGB(0, 0, 0));
    }
}

void CBCYamato::Release()
{
}