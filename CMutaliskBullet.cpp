#include "pch.h"
#include "CMutaliskBullet.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"

CMutaliskBullet::CMutaliskBullet()
{
}

CMutaliskBullet::~CMutaliskBullet()
{
    Release();
}

void CMutaliskBullet::Initialize()
{
    __super::Initialize();

    m_tInfo.fCX = 128.f;
    m_tInfo.fCY = 128.f;
    m_fSpeed = 300.f;      // 빠른 속도
    m_iDamage = 10;        // 기본 공격 데미지
    m_bHoming = false;     // 직선으로 날아감

    m_eRender = RENDER_WORLD;

    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 9;
    m_tFrame.dwTime = GetTickCount();
    m_tFrame.dwSpeed = 100;             // 50ms마다 프레임 전환 (빠른 애니메이션)
}

int CMutaliskBullet::Update()
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
        if (dist < 30.f)
        {
            //닿았을 때 데미지
            m_pTarget->TakeDamage(m_iDamage);
            CreateEffect();
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

    // ========== 애니메이션 프레임 업데이트 ==========
    DWORD now = GetTickCount();
    if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
    {
        m_tFrame.iStart++;
        if (m_tFrame.iStart > m_tFrame.iEnd)
            m_tFrame.iStart = 0;  // 반복

        m_tFrame.dwTime = now;
    }

    return NOEVENT;
}

void CMutaliskBullet::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    // ========== PNG 애니메이션 방식으로 렌더링 ==========
    TCHAR szKey[256];
    wsprintf(szKey, L"Mutalisk_Bullet_%d", m_tFrame.iStart);

    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
    if (pPng)
    {
        int iWidth = pPng->Get_Width();
        int iHeight = pPng->Get_Height();

        pPng->Render_Alpha(hDC,
            (int)m_tInfo.fX - iScrollX - iWidth / 2,
            (int)m_tInfo.fY - iScrollY - iHeight / 2,
            iWidth, iHeight, false);
    }
}

void CMutaliskBullet::Release()
{
}

void CMutaliskBullet::CreateEffect()
{
    //이펙트 생성
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
