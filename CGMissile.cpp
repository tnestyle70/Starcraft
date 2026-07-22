#include "pch.h"
#include "CGMissile.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"
#include "CTimeMgr.h"

CGMissile::CGMissile()
{
}

CGMissile::~CGMissile()
{
    Release();
}

void CGMissile::Initialize()
{
    __super::Initialize();

    m_tInfo.fCX = 36.f;
    m_tInfo.fCY = 36.f;
    m_fSpeed = 200.f;      // 빠른 속도
    m_iDamage = 25;        // 기본 공격 데미지
    m_bHoming = false;     // 직선으로 날아감

    m_eRender = RENDER_WORLD;

    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 9;
    m_tFrame.dwTime = GetTickCount();
    m_tFrame.dwSpeed = 100;             // 50ms마다 프레임 전환 (빠른 애니메이션)
}

int CGMissile::Update()
{
    int iResult = __super::Update();

    if (iResult == DEAD)
        return DEAD;
    float dt = CTimeMgr::Get_Instance()->GetDT();

    // 시각 효과용 - 타겟 근처 도달하면 소멸
    if (m_pTarget && !m_pTarget->IsDead())
    {
        Vec2 targetPos = m_pTarget->Get_Pos();
        Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };

        float distX = targetPos.fX - myPos.fX;
        float distY = targetPos.fY - myPos.fY;
        float dist = sqrtf(distX * distX + distY * distY);

        m_fEffectTimer += dt;
        if (m_fEffectTimer >= m_fEffectInterval)
        {
            CreateEffect();
            m_fEffectTimer = 0.f;
        }
        // 타겟 근처 도달 (32픽셀 이내)
        if (dist < 30.f)
        {
            m_pTarget->TakeDamage(m_iDamage);
            Set_Dead();
            return DEAD;
        }
    }
    else
    {
        // 타겟이 없거나 이미 죽은 경우
        TCHAR debugMsg[256];
        swprintf_s(debugMsg, L"[Projectile] No valid target - HasTarget:%d IsDead:%d\n",
            m_pTarget ? 1 : 0,
            m_pTarget ? (m_pTarget->IsDead() ? 1 : 0) : -1);
        OutputDebugString(debugMsg);
    }

    return NOEVENT;
}

void CGMissile::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    // ========== PNG 애니메이션 방식으로 렌더링 ==========

    int iDir = DirTo16WayIndex(m_vDir);

    TCHAR szKey[256];
    wsprintf(szKey, L"Missile_%d", iDir);

    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
    if (pPng)
    {
        int iWidth = pPng->Get_Width();
        int iHeight = pPng->Get_Height();

        pPng->Render_Alpha(hDC,
            (int)m_tInfo.fX - iScrollX - iWidth / 2,
            (int)m_tInfo.fY - iScrollY - iHeight / 2,
            true);
    }
}

void CGMissile::Release()
{
}

void CGMissile::CreateEffect()
{
    //뮤탈리스크 Hit Effect 생성하면서 날아가기!
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        m_tInfo.fX, m_tInfo.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"Mutalisk_HitEffect_",
            9, 128, 128, eEffectType::COL_BASE, RGB(0, 0, 0), 0.05f, false,
            eEffectImageType::PNG);
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
}
