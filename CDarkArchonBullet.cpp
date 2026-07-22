#include "pch.h"
#include "CDarkArchonBullet.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"

CDarkArchonBullet::CDarkArchonBullet()
{
}

CDarkArchonBullet::~CDarkArchonBullet()
{
    Release();
}

void CDarkArchonBullet::Initialize()
{
    __super::Initialize();

    m_tInfo.fCX = 192.f;
    m_tInfo.fCY = 192.f;
    //m_fSpeed = 100.f;      // 빠른 속도
    m_iDamage = 25;        // 기본 공격 데미지
    m_bHoming = false;     // 직선으로 날아감

    m_eRender = RENDER_WORLD;

    m_iDamage = 25;

    m_tFrame.iFrame = m_iDirection16;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 3;
    m_tFrame.dwTime = GetTickCount();
    m_tFrame.dwSpeed = 100;             // 50ms마다 프레임 전환 (빠른 애니메이션)
}

int CDarkArchonBullet::Update()
{
    int iResult = __super::Update();

    if (iResult == DEAD)
        return DEAD;
    
    if (!m_bCreateEffect)
    {
        CreateEffect();
        m_bCreateEffect = true;
    }
    // ========== 애니메이션 프레임 업데이트 ==========
    DWORD now = GetTickCount();
    if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
    {
        m_tFrame.iStart++;
        if (m_tFrame.iStart > m_tFrame.iEnd)
        {
            m_pTarget->TakeDamage(m_iDamage);
            Set_Dead();
            return DEAD;
        }
        m_tFrame.dwTime = now;
    }

    // 방향 업데이트 (발사체가 회전할 경우)
    m_tFrame.iFrame = m_iDirection16;

    return NOEVENT;
}

void CDarkArchonBullet::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - iScrollY);

    //실체 렌더링
    int dir16 = DirTo17WayIndex(m_vDir);        // 방향 0~16
    bool bFlip = false;

    // 좌우 반전 처리
    if (dir16 <= 8)  // 9~16은 0~8의 반전
    {
        if (dir16 == 8)
            dir16 = 16; //정면
        else
            dir16 = dir16 * 2 + (m_tFrame.iStart % 2);
    }
    else
    {
        int mirrorDir = 16 - dir16;  // 15→1, 14→2, ..., 9→7
        dir16 = mirrorDir * 2 + (m_tFrame.iStart % 2);
        bFlip = true;
    }

    // 스프라이트 키 생성 (Import 키 구조와 동일)
    TCHAR szKey[256];
    wsprintf(szKey, L"DarkArchon_Shockwave%d_Anim%d", dir16, m_tFrame.iStart);

    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
    if (pPng)
    {
        int iWidth = pPng->Get_Width();
        int iHeight = pPng->Get_Height();

        if (bFlip)
            pPng->Render_Alpha_Flipped(hDC,
                (int)m_tInfo.fX - iScrollX - iWidth / 2,
                (int)m_tInfo.fY - iScrollY - iHeight / 2, iWidth, iHeight, false);
        else
            pPng->Render_Alpha(hDC,
                (int)m_tInfo.fX - iScrollX - iWidth / 2,
                (int)m_tInfo.fY - iScrollY - iHeight / 2,
                iWidth, iHeight, false, false);
    }
}

void CDarkArchonBullet::Release()
{
}

void CDarkArchonBullet::CreateEffect()
{
    Vec2 targetPos = m_pTarget->Get_Pos();
    //이펙트 생성
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        targetPos.fX, targetPos.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"DarkArchon_Shockball_",
            6, 80, 80, eEffectType::COL_BASE, RGB(0, 0, 0), 0.1f, false,
            eEffectImageType::PNG);
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
}
