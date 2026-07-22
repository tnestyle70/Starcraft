#include "pch.h"
#include "CGhostBullet.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CTimeMgr.h"
#include "CEffect.h"
#include "CSoundMgr.h"
#include "CAbstractFactory.h"

CGhostBullet::CGhostBullet()
{
}

CGhostBullet::~CGhostBullet()
{
    Release();
}

void CGhostBullet::Initialize()
{
    __super::Initialize();

    m_tInfo.fCX = 48.f;
    m_tInfo.fCY = 40.f;
    m_fSpeed = 100.f;     
    m_iDamage = 5000;        
    m_bHoming = false;     // 직선으로 날아감

    m_pFrameKey = L"GHOST_NUCLEAR_BULLET";
    m_eRender = RENDER_WORLD;
}

int CGhostBullet::Update()
{
    int iResult = __super::Update();

    if (iResult == DEAD)
        return DEAD;

    Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };
    //타겟 기준 아래로만 떨어짐!
    m_tInfo.fY += m_fSpeed * CTimeMgr::Get_Instance()->GetDT();
    //타겟 위치 도달 체크
    float distY = targetPos.fY - m_tInfo.fY;
    if (distY <= 0.f)
    {
        m_tInfo.fY = targetPos.fY;
        //폭발 이펙트 생성
        CreateNuclearEffect();
        Set_Dead(); //폭발 이펙트 생성 후 소멸
        return DEAD;
    }
      
    return NOEVENT;
}

void CGhostBullet::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

    int iScrX = 0;         
    int iScrY = 0;

    GdiTransparentBlt(hDC,
        iDrawX,
        iDrawY,
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hMemDC,
        iScrX,      
        iScrY,    
        m_tInfo.fCX,
        m_tInfo.fCY,
        RGB(0, 0, 0));
}

void CGhostBullet::Release()
{
}

void CGhostBullet::CreateNuclearEffect()
{
    //폭발 사운드
    CSoundMgr::Get_Instance()->PlayEffect(L"Ghost/NuclearHit.wav", 0.3f);
    //이펙트 생성
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        m_tInfo.fX, m_tInfo.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"GHOST_NUCLEAR_EFFECT",
            26, 252, 252, eEffectType::COL_BASE, RGB(0, 0, 0));
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
    //범위 데미지
    AreaDamage();
}

void CGhostBullet::AreaDamage()
{
    const float NUCLEAR_RADIUS = 500.f;
    const int NUKE_DAMAGE = 500;

    auto& unitList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_UNIT);

    for (auto* pUnit : unitList)
    {
        Vec2 pos = pUnit->Get_Pos();
        float distX = pos.fX - targetPos.fX;
        float distY = pos.fY - targetPos.fY;
        float dist = sqrtf(distX * distX + distY * distY);
        if (dist <= NUCLEAR_RADIUS)
        {
            //거리에 따른 데미지 감소?
            pUnit->TakeDamage(NUKE_DAMAGE);
        }
    }

    //적
    auto& enemyList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_ENEMY);

    for (auto* pEnemy : enemyList)
    {
        Vec2 pos = pEnemy->Get_Pos();
        float distX = pos.fX - targetPos.fX;
        float distY = pos.fY - targetPos.fY;
        float dist = sqrtf(distX * distX + distY * distY);
        if (dist <= NUCLEAR_RADIUS)
        {
            //거리에 따른 데미지 감소?
            pEnemy->TakeDamage(NUKE_DAMAGE);
        }
    }
}
