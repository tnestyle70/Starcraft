#include "pch.h"
#include "CIntercepBullet.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"

CIntercepBullet::CIntercepBullet()
{
}

CIntercepBullet::~CIntercepBullet()
{
    Release();
}

void CIntercepBullet::Initialize()
{
    __super::Initialize();

    m_tInfo.fCX = 32.f;
    m_tInfo.fCY = 32.f;
    m_fSpeed = 300.f;      // 빠른 속도
    m_iDamage = 25;        // 기본 공격 데미지
    m_bHoming = false;     // 직선으로 날아감

    m_eRender = RENDER_WORLD;

    //m_tFrame.iFrame = m_iDirection16;
    //m_tFrame.iStart = 0;
    //m_tFrame.iEnd = 0;
    //m_tFrame.dwTime = GetTickCount();
    //m_tFrame.dwSpeed = 100;             // 50ms마다 프레임 전환 (빠른 애니메이션)
}

int CIntercepBullet::Update()
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
            //CreateEffect();
            Set_Dead();
            return DEAD;
        }
    }

    return NOEVENT;
}

void CIntercepBullet::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - iScrollY);

    int fileIndex = 16;  // 기본값: 정면 (180도)
    bool bFlip = false;

    int dir16 = m_tFrame.iFrame;  // 0~15

    if (dir16 <= 8)  // 0~8: 오른쪽 반구 (0~180도)
    {
        // dir16 = 0 → 파일 0~1
        // dir16 = 1 → 파일 2~3
        // dir16 = 8 → 파일 16 (정면)
        if (dir16 == 8)
            fileIndex = 16;  // 정면
        else
            fileIndex = dir16 * 2 + (m_tFrame.iStart % 2);
    }
    else  // 9~15: 왼쪽 반구 (181~359도) - 좌우반전
    {
        // dir16 = 9 → dir16 = 7의 반전 → 파일 14~15
        // dir16 = 15 → dir16 = 1의 반전 → 파일 2~3
        int mirrorDir = 16 - dir16;  // 15→1, 14→2, ..., 9→7
        fileIndex = mirrorDir * 2 + (m_tFrame.iStart % 2);
        bFlip = true;
    }

    TCHAR szKey[256];
    wsprintf(szKey, L"Interceptor_Bullet_%d", fileIndex);

    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
    if (pPng)
    {
        int iWidth = pPng->Get_Width();
        int iHeight = pPng->Get_Height();

        if (bFlip)
            pPng->Render_Alpha_Flipped(hDC,
                (int)m_tInfo.fX - iScrollX - iWidth / 2,
                (int)m_tInfo.fY - iScrollY - iHeight / 2,
                iWidth, iHeight, false, false);
        else
            pPng->Render_Alpha(hDC,
                (int)m_tInfo.fX - iScrollX - iWidth / 2,
                (int)m_tInfo.fY - iScrollY - iHeight / 2,
                iWidth, iHeight, false, false);
    }   
}

void CIntercepBullet::Release()
{
}

void CIntercepBullet::CreateEffect()
{
    //이펙트 생성
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        m_tInfo.fX, m_tInfo.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"Hydralisk_HitEffect_",
            7, 128, 128, eEffectType::COL_BASE, RGB(0, 0, 0), 0.05f, false,
            eEffectImageType::PNG);
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
}
