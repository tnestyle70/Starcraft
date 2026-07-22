#include "pch.h"
#include "CZergling.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "Define.h"
#include <cmath>
#include "CInputMgr.h"
#include "CTileMgr.h"
#include "CFogMgr.h"
#include "CSoundMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"
#include "CSoundMgr.h"
#include "CTimeMgr.h"

CZergling::CZergling() : 
    m_iAttackFrameStart(0)
    , m_iAttackFrameEnd(3)
{
}

CZergling::~CZergling()
{
    Release();
}

void CZergling::Initialize()
{
    // Zergling 크기 및 기본 정보
    m_tInfo.fCX = 64.f;
    m_tInfo.fCY = 64.f;

    // Zergling 스탯
    m_iMaxHP = 50;
    m_iHP = m_iMaxHP;
    m_fSpeed = 130.f;           // 빠른 이동속도 (픽셀/초)

    // 공격 관련
    m_iAttackDamage = 6;
    m_fAttackRange = 50.f;

    m_fAttackInterval = 0.1f; //진짜 공격 속도 타이머!
    m_fAttackTimer = m_fAttackInterval;

    // 공격 애니메이션 프레임 (전체 4프레임 사용)
    m_iAttackFrameStart = 0;
    m_iAttackFrameEnd = 3;

    m_eOriginalRace = eRaceType::RACE_ZERG;
    m_eCurrentRace = eRaceType::RACE_ZERG;

    // 유닛 타입 설정
    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::ZERGLING;

    // 프레임 정보 초기화
    m_tFrame.iFrame = 0;        // 현재 방향 (0~15)
    m_tFrame.iStart = 0;        // 현재 프레임 (0~7)
    m_tFrame.iEnd = 7;          // 마지막 프레임
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = GetTickCount();
    m_tFrame.dwSpeed = 80;      // 빠른 애니메이션 (80ms)
}

int CZergling::Update()
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
        if (m_pTarget) //타겟이 존재하는 경우에만 타이머 돌리기
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
        // 방향 설정
        m_tFrame.iFrame = DirTo16WayIndex(m_vDir);
        // 애니메이션 진행
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
            // 방향 설정
            m_tFrame.iFrame = DirTo16WayIndex(m_vDir);
            // 애니메이션 진행
            if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
            {
                // 첫 진입시 프레임 설정
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

    case eUnitState::DIE:
        UpdateDead();
        return DEAD;

    default:
        break;
    }

    __super::Update_Rect();

    return NOEVENT;
}

void CZergling::Late_Update()
{
}

void CZergling::Render(HDC hDC)
{
    // 전장의 안개
    CUnit::Render(hDC);

    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    //상태에 따른 다른 키 생성
    TCHAR szKey[256];
    switch (m_eState)
    {
    case eUnitState::IDLE:
    case eUnitState::MOVE:
        wsprintf(szKey, L"Zergling_Move%d_Anim%d", m_tFrame.iFrame, m_tFrame.iStart);
        break;
    case eUnitState::ATTACK:
        wsprintf(szKey, L"Zergling_Attack%d_Anim%d", m_tFrame.iFrame, m_tFrame.iStart);
        break;
    case eUnitState::DIE:
        break;
    default:
        wsprintf(szKey, L"Zergling_Move%d_Anim%d", m_tFrame.iFrame, m_tFrame.iStart);
        break;
    }

    // 이미지 가져오기
    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
    if (pPng)
    {
        int iWidth = pPng->Get_Width();
        int iHeight = pPng->Get_Height();

        pPng->Render_Alpha(hDC,
            (int)m_tInfo.fX - iScrollX - iWidth / 2,
            (int)m_tInfo.fY - iScrollY - iHeight / 2, iWidth, iHeight, false);
    }
}

void CZergling::Release()
{
}

void CZergling::UpdateAI()
{
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

CObj* CZergling::FindNearestEnemyAI(float searchRadius)
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

        //공중 유닛 제외
        if (pObj->GetLayer() != eUnitLayer::GROUND)
            continue;

        //벙커에 있는 유닛 제외
        if (!pObj->IsSelectable())
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

void CZergling::FireBullet()
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
        //공격 사운드 재생
        CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingAttack.wav", 0.4f);
        //이펙트 생성
        m_fAttackSpeed = 2.5f;
        return;
    }
}

void CZergling::UpdateDead()
{
    if (m_eTeamType == eTeamType::ALLY)
    {
        CResourceMgr::Get_Instance()->SubtractSupply(1);
    }
    //이펙트와 사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingDeath.wav", 0.5f);

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