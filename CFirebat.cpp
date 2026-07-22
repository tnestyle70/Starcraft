#include "pch.h"
#include "CFirebat.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CTimeMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CSoundMgr.h"
#include "CGameDataMgr.h"

CFirebat::CFirebat()
{
}

CFirebat::~CFirebat()
{
    Release();
}

void CFirebat::Initialize()
{
    m_tInfo.fCX = 32.f; //파이어벳 한 칸 크기
    m_tInfo.fCY = 32.f;
    m_iMaxHP = 100;
    m_iHP = m_iMaxHP;
    m_fSpeed = 120.f;
    //공격 관련 변수
    m_iAttackDamage = 6;
    m_fAttackRange = 60.f;
    m_fAttackSpeed = 1.0f;
    //공격 애니메이션 프레임 - 8~9까지가 공격 애니메이션 프레임 
    m_iAttackFrameStart = 8;
    m_iAttackFrameEnd = 9;

    m_fAttackInterval = 0.5f; //진짜 공격 속도!
    m_fBunkerAttackInterval = 0.7f;
    m_fAttackTimer = m_fAttackInterval;

    m_pFrameKey = L"FIREBAT";

    m_eOriginalRace = eRaceType::RACE_TERRAN;
    m_eCurrentRace = eRaceType::RACE_TERRAN;

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::FIREBAT;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 7;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
}

int CFirebat::Update()
{
    int iResult = CUnit::Update();

    if (iResult == DEAD)
        m_eState = eUnitState::DIE;

    //적일 경우 AI 업데이트, 아니면 조작하면서 게임 플레이 가능하도록 만들기
    if (m_eTeamType == eTeamType::ENEMY)
    {
        //AI업데이트
        UpdateAI();
    }

    DWORD now = GetTickCount();
    float dt = CTimeMgr::Get_Instance()->GetDT();

    switch (m_eState)
    {
    case eUnitState::IDLE:
        m_tFrame.iStart = 0;
        if (m_pTarget)
        {
            m_fAttackTimer += dt;
            if (m_fAttackTimer >= m_fAttackInterval)
            {
                m_eState = eUnitState::ATTACK;
                m_fAttackTimer = 0.f;
                m_bAttack = true;
            }
        }
        break;
    case eUnitState::MOVE:
        m_tFrame.iFrame = DirTo16WayIndex(m_vDir);

        if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
        {
            m_tFrame.iStart++;
            if (m_tFrame.iStart > m_tFrame.iEnd)
                m_tFrame.iStart = 0;
            m_tFrame.dwTime = now;
        }
        break;
    case eUnitState::ATTACK:
        if (m_bAttack)
        {
            //방향에 따른 행 설정
            m_tFrame.iFrame = DirTo16WayIndex(m_vDir);
            //애니메이션 진행
            if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
            {
                //첫 진입시 프레임 설정
                if (m_tFrame.iStart < m_iAttackFrameStart ||
                    m_tFrame.iStart > m_iAttackFrameEnd)
                {
                    m_tFrame.iStart = m_iAttackFrameStart;
                }
                else
                {
                    m_tFrame.iStart++;
                    if (m_tFrame.iStart > m_iAttackFrameEnd)
                    {
                        m_tFrame.iStart = m_iAttackFrameStart;  // 반복
                        FireBullet();
                        m_bAttack = false;
                        m_eState = eUnitState::IDLE;
                    }
                }
                m_tFrame.dwTime = now;
            }
        }
        break;
    case eUnitState::GARRISONED:
        UpdateGarrisoned();
        break;
    case eUnitState::DIE:
        UpdateDead();
        return DEAD;
    default:
        break;
    }

    __super::Update_Rect();

    return NOEVENT;
}

void CFirebat::Late_Update()
{
    //선택이 되었을 경우 마우스 방향의 애니메이션 재생
    if (!m_bSelected) return;
    //마우스 방향 안 보기!
    if (m_eState == eUnitState::MOVE) return;
    if (m_eState == eUnitState::ATTACK) return;
    if (m_eState == eUnitState::IDLE) return;

    Vec2 vWorldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 vDir{ vWorldMouse.fX - m_tInfo.fX, vWorldMouse.fY - m_tInfo.fY };

    int iDir16 = DirTo16WayIndex(vDir);
    m_tFrame.iFrame = iDir16;
}

void CFirebat::Render(HDC hDC)
{
    //전장의 안개 
    CUnit::Render(hDC);

    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

    int iScrX = m_tFrame.iStart * (int)m_tInfo.fCX;
    int iScrY = m_tFrame.iFrame * (int)m_tInfo.fCY;

    GdiTransparentBlt(hDC,
        iDrawX,
        iDrawY,
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hMemDC,
        iScrX,
        iScrY,
        (int)m_tInfo.fCX,		// 복사할 이미지의 가로 사이즈
        (int)m_tInfo.fCY,		// 복사할 이미지의 세로 사이즈
        RGB(0, 255, 0));
}

void CFirebat::Release()
{
}

void CFirebat::UpdateGarrisoned()
{
    //이동은 벙커 쪽에서 고정이므로 선택 상태 X, 그냥 타겟 찾아서 공격하도록 한다.
    //벙커 업데이트
    CObj* pEnemy = FindNearestEnemy(m_fAttackRange + 100.f);
    float dt = CTimeMgr::Get_Instance()->GetDT();
    if (pEnemy)
    {
        m_fAttackTimer += dt;
        if (m_fAttackTimer >= m_fBunkerAttackInterval)
        {
            Vec2 targetPos = pEnemy->Get_Pos();
            Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
            float fDT = CTimeMgr::Get_Instance()->GetDT();
            //타겟까지의 거리
            Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
            float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
            //타겟 방향 보기
            if (dist > 0.1f)
            {
                m_vDir = { diff.fX / dist, diff.fY / dist };
            }
            //히트스캔
            pEnemy->TakeDamage(m_iAttackDamage);
            //사운드 재생
            CSoundMgr::Get_Instance()->PlayEffect(L"FireBat/FireBatAttack1.wav", 0.3f);
            //방향 설정
            int dir16 = DirTo16WayIndex(m_vDir);
            int effectDir = dir16 * 2;
            //타겟 이펙트 생성
            CObj* pEffect = CAbstractFactory<CEffect>::Create(
                m_tInfo.fX, m_tInfo.fY);
            pEffect->Initialize();
            CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
            if (pEffectObj)
            {
                pEffectObj->Set_DirectionalEffect(L"FIREBAT_HIT_EFFECT",
                    32, 5, 224, 224, effectDir, RGB(0, 255, 0));
            }
            CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);

            m_fAttackTimer = 0.f;

            return;
        }
    }
}

void CFirebat::FireBullet()
{
    if (!m_pTarget) //안전장치
        return;

    Vec2 targetPos = m_pTarget->Get_Pos();
    Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
    float fDT = CTimeMgr::Get_Instance()->GetDT();
    //타겟까지의 거리
    Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
    //공격 사거리 체크
    if (dist <= m_fAttackRange)
    {
        //타겟 방향 보기
        if (dist > 0.1f)
        {
            m_vDir = { diff.fX / dist, diff.fY / dist };
        }
        //히트스캔
        m_pTarget->TakeDamage(m_iAttackDamage);
        //사운드 재생
        CSoundMgr::Get_Instance()->PlayEffect(L"FireBat/FireBatAttack1.wav", 0.8f);
        //방향 설정
        int dir16 = DirTo16WayIndex(m_vDir);
        int effectDir = dir16 * 2;
        //타겟 이펙트 생성
        CObj* pEffect = CAbstractFactory<CEffect>::Create(
            m_tInfo.fX, m_tInfo.fY);
        pEffect->Initialize();
        CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
        if (pEffectObj)
        {
            pEffectObj->Set_DirectionalEffect(L"FIREBAT_HIT_EFFECT",
                32, 5, 224, 224, effectDir, RGB(0, 255, 0));
        }
        CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
        return;
    }
}

void CFirebat::UpdateAI()
{
    if (m_eTeamType == eTeamType::ALLY)
        return;
    //주변 적 찾기
    CObj* pEnemy = FindNearestEnemyAI(m_fAttackRange);
    //주변에 적이 존재할 경우, 오더 다 멈추고 공격 Order 추가
    if (pEnemy)
    {
        if (!m_OrderQ.empty())
        {
            m_OrderQ.pop_front();
        }
        Order attack;
        attack.eType = eOrderType::ATTACK;
        attack.pTarget = pEnemy;
        attack.dst = { pEnemy->Get_Info().fX, pEnemy->Get_Info().fY };

        m_OrderQ.push_back(attack);
    }
    else
    {
        //Order stop;
        //stop.eType = eOrderType::STOP;
        //m_OrderQ.push_back(stop);
        //Order move;
        //move.eType = eOrderType::MOVE;
        //move.dst = { 0,0 };  // ← 문제 2: 매 프레임 (0,0) 명령 추가!
        //m_OrderQ.push_back(move);  // ← 큐가 무한정 쌓임!
    }
}

CObj* CFirebat::FindNearestEnemyAI(float searchRadius)
{
    float fMinDistance = FLT_MAX;
    CObj* pNearestEnemy = nullptr;

    // 탐지 범위 (사거리 + 추가 탐지 범위)
    float fDetectionRange = m_fAttackRange + 300.f; //300 감지 범위

    // 모든 유닛 검사
    auto& unitList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_UNIT);
    for (auto& pObj : unitList)
    {
        // 자기 자신 제외
        if (pObj == this)
            continue;

        // 죽은 유닛 제외
        if (pObj->Is_Dead())
            continue;

        //벙커에 있는 유닛 제외
        if (!pObj->IsSelectable())
            continue;

        //공중 유닛 제외
        if (pObj->GetLayer() == eUnitLayer::AIR)
            continue;

        // 거리 계산
        float fDX = pObj->Get_Info().fX - m_tInfo.fX;
        float fDY = pObj->Get_Info().fY - m_tInfo.fY;
        float fDistance = sqrtf(fDX * fDX + fDY * fDY);

        // 탐지 범위 밖이면 제외
        if (fDistance > fDetectionRange)
            continue;

        // 가장 가까운 적 갱신
        if (fDistance < fMinDistance)
        {
            fMinDistance = fDistance;
            pNearestEnemy = pObj;
        }
    }
    return pNearestEnemy;
}

void CFirebat::UpdateDead()
{
    if (m_eTeamType == eTeamType::ALLY)
    {
        CResourceMgr::Get_Instance()->SubtractSupply(1);
    }
    //이펙트와 사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"FireBat/FireBatDeath1.wav", 1.f);

    //타겟 이펙트 생성
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        m_tInfo.fX, m_tInfo.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"MARINE_DEATH_EFFECT",
            7, 100, 50, eEffectType::COL_BASE, RGB(255, 255, 0));
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
}
