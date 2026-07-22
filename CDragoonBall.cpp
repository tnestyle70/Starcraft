#include "pch.h"
#include "CDragoonBall.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"

CDragoonBall::CDragoonBall()
{
}

CDragoonBall::~CDragoonBall()
{
    Release();
}

void CDragoonBall::Initialize()
{
    __super::Initialize();

    m_tInfo.fCX = 32.f;
    m_tInfo.fCY = 32.f;
    m_fSpeed = 300.f;      // 빠른 속도
    m_iDamage = 25;        // 기본 공격 데미지
    m_bHoming = false;     // 직선으로 날아감

    m_eRender = RENDER_WORLD;

    m_iDamage = 50;

    m_tFrame.iFrame = m_iDirection16;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 4;
    m_tFrame.dwTime = GetTickCount();
    m_tFrame.dwSpeed = 100;             // 50ms마다 프레임 전환 (빠른 애니메이션)
}

int CDragoonBall::Update()
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
            CreateEffect();
            Set_Dead();
            return DEAD;
        }
    }
    // ========== 애니메이션 프레임 업데이트 ==========
    DWORD now = GetTickCount();
    if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
    {
        m_tFrame.iStart++;
        if (m_tFrame.iStart > m_tFrame.iEnd)
            m_tFrame.iStart = 0;  // 반복

        m_tFrame.dwTime = now;
    }

    // 방향 업데이트 (발사체가 회전할 경우)
    m_tFrame.iFrame = m_iDirection16;

    return NOEVENT;
}

void CDragoonBall::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Dragoon_Ball");

    GdiTransparentBlt(hDC,
        iDrawX,
        iDrawY,
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hMemDC,
        0,      // ← 0
        0,      // ← m_iDirection16 * 128 (0, 128, 256, ...)
        32,
        32,
        RGB(255, 255, 255));
}

void CDragoonBall::Release()
{
}

void CDragoonBall::CreateEffect()
{
    //이펙트 생성
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        m_tInfo.fX, m_tInfo.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"Dragoon_BallEx_",
            13, 40, 40, eEffectType::COL_BASE, RGB(0, 0, 0), 0.05f, false,
            eEffectImageType::PNG);
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
}
