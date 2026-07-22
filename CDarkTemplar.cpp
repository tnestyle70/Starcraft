#include "pch.h"
#include "CDarkTemplar.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CSoundMgr.h"
#include "CTimeMgr.h"
#include "CEffect.h"
#include "CObjMgr.h"
#include "CAbstractFactory.h"
#include "CUIMgr.h"
#include "CSelectionMgr.h"
#include "CEffect.h"
#include "CDarkArchon.h"

CDarkTemplar::CDarkTemplar()
{
}

CDarkTemplar::~CDarkTemplar()
{
    Release();
}

void CDarkTemplar::Initialize()
{
    m_tInfo.fCX = 64.f; //메딕 한 칸 크기
    m_tInfo.fCY = 64.f;
    m_iMaxHP = 150;
    m_iHP = m_iMaxHP;
    m_iMaxMP = 125;
    m_iMP = m_iMaxMP;
    m_iMaxShield = 125;
    m_iShield = m_iMaxShield;
    m_fSpeed = 100.f;
    //공격 관련 변수
    m_iAttackDamage = 10;
    m_fAttackRange = 40.f;
    m_fAttackSpeed = 1.0f;
    //공격 애니메이션 프레임 
    m_iAttackFrameStart = 7;
    m_iAttackFrameEnd = 12;

    m_fAttackInterval = 0.7f; //진짜 공격 속도!
    m_fAttackTimer = m_fAttackInterval;

    // 공격 애니메이션 프레임 (전체 4프레임 사용)
    m_iAttackFrameStart = 0;
    m_iAttackFrameEnd = 9;

    //m_pFrameKey = L"Medic";

    m_eOriginalRace = eRaceType::RACE_PROTOSS;
    m_eCurrentRace = eRaceType::RACE_PROTOSS;

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::DARK_TEMPLAR;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 3;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 60;
}

int CDarkTemplar::Update()
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
        m_tFrame.iFrame = DirTo17WayIndex(m_vDir);

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
        {   //방향에 따른 행 설정
            m_tFrame.iFrame = DirTo17WayIndex(m_vDir);
            //애니메이션 진행
            if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
            {
                //첫 진입시 프레임 설정
                if (m_iAttackFrameStart == 0)
                {
                    FireBullet();
                    m_iAttackFrameStart++;
                }
                else
                {
                    m_iAttackFrameStart++;
                    if (m_iAttackFrameStart > m_iAttackFrameEnd)
                    {
                        m_iAttackFrameStart = 0;  // 반복
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

void CDarkTemplar::Late_Update()
{
    //선택이 되었을 경우 마우스 방향의 애니메이션 재생
    if (!m_bSelected) return;
    //마우스 방향 안 보기!
    if (m_eState == eUnitState::MOVE) return;
    if (m_eState == eUnitState::ATTACK) return;
    if (m_eState == eUnitState::IDLE) return;

    Vec2 vWorldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 vDir{ vWorldMouse.fX - m_tInfo.fX, vWorldMouse.fY - m_tInfo.fY };

    int iDir17 = DirTo17WayIndex(vDir);
    m_tFrame.iFrame = iDir17;
}

void CDarkTemplar::Render(HDC hDC)
{
    if (!m_bVisible) //벙커에 탑승해있는 상태일 경우 렌더링X
        return;

    // 전장의 안개
    CUnit::Render(hDC);

    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - iScrollY);

    int dir16 = m_tFrame.iFrame;        // 방향 0~16
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
    switch (m_eState)
    {
    case eUnitState::IDLE:
    case eUnitState::MOVE:
        wsprintf(szKey, L"DarkTemplar_Move%d_Anim%d", dir16, m_tFrame.iStart);
        break;
    case eUnitState::ATTACK:
        wsprintf(szKey, L"DarkTemplar_Attack%d_Anim%d", dir16 , m_iAttackFrameStart);
        break;
    case eUnitState::DIE:
        break;
    default:
        wsprintf(szKey, L"DarkTemplar_Move%d_Anim%d", dir16, m_tFrame.iStart);
        break;
    }
    //wsprintf(szKey, L"DarkTemplar_Move%d_Anim%d", dir16, m_tFrame.iStart);

    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
    if (pPng)
    {
        int iWidth = pPng->Get_Width();
        int iHeight = pPng->Get_Height();

        if (bFlip)
            pPng->Render_Alpha_Flipped(hDC,
                (int)m_tInfo.fX - iScrollX - iWidth / 2,
                (int)m_tInfo.fY - iScrollY - iHeight / 2, 
                iWidth, iHeight, false, true);
        else
            pPng->Render_Alpha(hDC,
                (int)m_tInfo.fX - iScrollX - iWidth / 2,
                (int)m_tInfo.fY - iScrollY - iHeight / 2,
                iWidth, iHeight, false, true);
    }
}

void CDarkTemplar::Release()
{
}

void CDarkTemplar::UpdateHotKeys()
{
    CUnit::UpdateHotKeys();
}

bool CDarkTemplar::ExecuteCommand(eCommandID command, CommandContext& context)
{
    //먼저 부모 명령 실행
    if (CUnit::ExecuteCommand(command, context))
        return true;

    // CCommandMgr로 위임, 부모 명령 실행 이후 진행
    switch (command)
    {
    case eCommandID::WRAP:
        Wrap();
        break;
    }
    return false;
}

void CDarkTemplar::CommandCardSlot(vector<CommandSlot>& outSlot)
{
    CUnit::CommandCardSlot(outSlot);
    //9번 : Wrap 사용
    outSlot[8].commandID = eCommandID::WRAP;
    outSlot[8].hotkey = 'V';
    outSlot[8].clickable = true;
    outSlot[8].visible = true;
    outSlot[8].lock = false;
}

void CDarkTemplar::UpdateDead()
{
    if (m_eTeamType == eTeamType::ALLY)
    {
        CResourceMgr::Get_Instance()->SubtractSupply(1);
    }
    //이펙트와 사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"BattleCrusor/BattleCrusorDeath.wav", 1.f);

    //타겟 이펙트 생성
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        m_tInfo.fX, m_tInfo.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"DarkTemplar_Die_",
            7, 128, 128, eEffectType::COL_BASE, RGB(0, 0, 0), 0.05f, false,
            eEffectImageType::PNG);
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
}

void CDarkTemplar::FireBullet()
{
    if (!m_pTarget) //안전장치
        return;

    //이펙트와 사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"DarkTemplar/uzefir00.wav", 0.5f);

    Vec2 targetPos = m_pTarget->Get_Pos();
    Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
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
        return;
    }
}

void CDarkTemplar::Wrap()
{
    if (m_bMerging)
        return; //이미 합체 중이면 다시 합체 하지 않기

    m_bMerging = true;

    //일정 거리 내에 다른 다크 아칸이 존재할 경우 해당 아칸과의 합체를 진행한다.
    list<CObj*>& unitList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_UNIT);
    CObj* pNear = nullptr;
    float fMinDist = 10000.f;
    for (auto& pObj : unitList)
    {
        CDarkTemplar* pDArchon = dynamic_cast<CDarkTemplar*>(pObj);
        if(pDArchon) //가장 가까이에 존재하는 다크 템플러와 함체를 진행한다.
        {
            //본인은 제외
            if (pDArchon == this)
                continue;
            //이미 합체 중이면 제외
            if (pDArchon->IsMerging())
                continue;
            Vec2 targetPos = pDArchon->Get_Pos();
            Vec2 diff = { targetPos.fX - m_tInfo.fX, targetPos.fY - m_tInfo.fY };
            float dist = { diff.fX * diff.fX + diff.fY * diff.fY };
            if (dist < fMinDist)
            {
                pNear = pDArchon;
                fMinDist = dist;
            }
        }
    }
    if (!pNear)
        return;
    pNear->Set_Dead();
    Set_Dead();

    //이펙트 생성
    CObj* pEffect = CAbstractFactory<CEffect>::Create(
        m_tInfo.fX, m_tInfo.fY);
    pEffect->Initialize();
    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    if (pEffectObj)
    {
        pEffectObj->Set_Effect(L"DarkArchon_Create_",
            16, 120, 120, eEffectType::COL_BASE, RGB(0, 0, 0), 0.1f, false,
            eEffectImageType::PNG);
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
    //다크아칸 생성
    //사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"DArchon/pdardy00.wav", 1.f);
    Vec2 pos = Get_Pos();
    CObj* pDarkArchon = CAbstractFactory<CDarkArchon>::Create(pos.fX, pos.fY);
    CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDarkArchon);
}

void CDarkTemplar::UpdateAI()
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
        //Order move;
        //move.eType = eOrderType::MOVE;
        //move.dst = { 0,0 };  // ← 문제 2: 매 프레임 (0,0) 명령 추가!
        //m_OrderQ.push_back(move);  // ← 큐가 무한정 쌓임!
    }
}

CObj* CDarkTemplar::FindNearestEnemyAI(float searchRadius)
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
