#include "pch.h"
#include "CVultureMine.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"
#include "CSoundMgr.h"

CVultureMine::CVultureMine()
{
}

CVultureMine::~CVultureMine()
{
    Release();
}

void CVultureMine::Initialize()
{
    __super::Initialize();

    m_tInfo.fCX = 16.f;
    m_tInfo.fCY = 16.f;
    m_fSpeed = 800.f;      // 빠른 속도
    m_iDamage = 100;        // 기본 공격 데미지
    m_bHoming = false;     // 직선으로 날아감

    m_fAttackRange = 100.f;
    m_fDamageRange = 100.f;

    m_pFrameKey = L"VBullet";
    m_eRender = RENDER_WORLD;
}

int CVultureMine::Update()
{
    int iResult = __super::Update();

    if (iResult == DEAD)
        return DEAD;

    if (!m_bMineInstalled) //마인이 설치되지 않은 상태일 경우 targetpos로 이동
    {
        Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };

        float distX = targetPos.fX - myPos.fX;
        float distY = targetPos.fY - myPos.fY;
        float dist = sqrtf(distX * distX + distY * distY);

        // 타겟 근처 도달시 멈춤 (32픽셀 이내)
        if (dist < 32.f)
        {
            m_bMineInstalled = true;
        }
    }
    else //마인이 설치된 상태일 경우 마인 이동 멈추고 타겟 감지!
    {
         UpdateMine();
    }
    
    return NOEVENT;
}

void CVultureMine::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(m_pFrameKey);
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

void CVultureMine::Release()
{
}

void CVultureMine::UpdateMine()
{
    list<CObj*> enemyList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_ENEMY);
    CObj* pNear = nullptr;
    float minDist = m_fAttackRange;
    for (auto* pObj : enemyList)
    {
        if (!pObj || pObj->IsDead())
            continue;
        Vec2 enemyPos = pObj->Get_Pos();
        Vec2 diff = { enemyPos.fX - m_tInfo.fX, enemyPos.fY - m_tInfo.fY };
        float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
        if (dist < minDist)
        {
            pNear = pObj;
            minDist = dist;
        }
    }
    //가까운 적이 존재할 경우 해당 적을 향해 돌진 후 이펙트와 범위 공격 가한 후 소멸
    if (pNear)
    {
        MoveToTarget(pNear);
    }
    else //타겟이 없을 경우 타겟 마우스 위치로 고정
    {
        m_tInfo.fX = targetPos.fX;
        m_tInfo.fY = targetPos.fY;
        return;
    }
}

void CVultureMine::MoveToTarget(CObj* pTarget)
{
    if (!m_bFindTarget)
    {
        //마인 공격 사운드
        CSoundMgr::Get_Instance()->PlayEffect(L"Vulture/VultureMineAttack.wav", 0.5f);
        m_bFindTarget = true;
    }

    //m_vDir을 반드시 업데이트 해줘야 함!!

    Vec2 targetPos = pTarget->Get_Pos();
    Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };

    float distX = targetPos.fX - myPos.fX;
    float distY = targetPos.fY - myPos.fY;
    float dist = sqrtf(distX * distX + distY * distY);

    m_vDir = { distX / dist, distY / dist };

    // 타겟 근처 도달 (32픽셀 이내)
    if (dist < 32.f)
    {
        CreateEffect(pTarget->Get_Pos());
        Set_Dead();
        return;
    }
}

void CVultureMine::CreateEffect(Vec2 targetPos)
{
    //사운드 재생
    
    //타겟 이펙트 생성
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        targetPos.fX, targetPos.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"SiegeTankHit", 16, 100, 108, eEffectType::COL_BASE, RGB(0, 0, 0));
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
    AreaDamage();
}

void CVultureMine::AreaDamage()
{
    auto& unitList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_UNIT);

    for (auto* pUnit : unitList)
    {
        Vec2 pos = pUnit->Get_Pos();
        float distX = pos.fX - targetPos.fX;
        float distY = pos.fY - targetPos.fY;
        float dist = sqrtf(distX * distX + distY * distY);
        if (dist <= m_fDamageRange)
        {
            //거리에 따른 데미지 감소?
            pUnit->TakeDamage(m_iDamage);
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
        if (dist <= m_fDamageRange)
        {
            //거리에 따른 데미지 감소?
            pEnemy->TakeDamage(m_iDamage);
        }
    }
}
