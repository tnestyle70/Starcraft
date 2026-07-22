#include "pch.h"
#include "CVulture.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CVBullet.h"
#include "CTimeMgr.h"
#include "CObjMgr.h"
#include "CSoundMgr.h"
#include "CEffect.h"
#include "CAbstractFactory.h"
#include "CSelectionMgr.h"
#include "CUIMgr.h"
#include "CVultureMine.h"

CVulture::CVulture()
{
}

CVulture::~CVulture()
{
    Release();
}

void CVulture::Initialize()
{
    m_tInfo.fCX = 100.f; //벌쳐 사이즈
    m_tInfo.fCY = 100.f;
    m_iMaxHP = 250;
    m_iHP = m_iMaxHP;
    m_fSpeed = 150.f;

    //공격 변수 초기화 
    m_fAttackRange = 192.f;
    m_dwAttackCoolTime = 1000;
    m_dwLastAttack = 0;
    m_iAttackDamage = 10;
    m_fAttackSpeed = 1.f;

    m_fAttackInterval = 1.f;
    m_fAttackTimer = m_fAttackInterval;

    //시야 범위 
    m_iSightRange = 14;

    m_pFrameKey = L"Vulture";

    m_eOriginalRace = eRaceType::RACE_TERRAN;
    m_eCurrentRace = eRaceType::RACE_TERRAN;

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::VULTURE;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 0;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
}

int CVulture::Update()
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
        break;
    case eUnitState::ATTACK:
        if (m_bAttack)
        {
            //방향에 따른 행 설정
            m_tFrame.iFrame = DirTo16WayIndex(m_vDir);
            FireBullet();
            m_bAttack = false;
            m_eState = eUnitState::IDLE;
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

void CVulture::Late_Update()
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

void CVulture::Render(HDC hDC)
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
        RGB(255, 0, 255));
}

void CVulture::Release()
{
}

void CVulture::UpdateHotKeys()
{
    CUnit::UpdateHotKeys();
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();

    if (selected.empty())
        return;

    // 이 마린이 선택된 유닛 중 하나인지 확인
    bool isSelected = false;
    for (auto* obj : selected) {
        if (obj == this) {
            isSelected = true;
            break;
        }
    }

    if (!isSelected)
        return;

    // 첫 번째 유닛만 UI 업데이트 (중복 방지)
    if (selected[0] != this)
        return;

    // 슬롯 정보
    vector<CommandSlot> slots;
    this->CommandCardSlot(slots);

    // 단축키 확인
    for (int i = 0; i < slots.size(); ++i)
    {
        if (!slots[i].visible || !slots[i].clickable)
            continue;

        if (CInputMgr::Get_Instance()->KeyDownVK(slots[i].hotkey))
        {
            CUIMgr::Get_Instance()->SetButtonFeedback(i, true);
            CommandContext context{};
            ExecuteCommand(slots[i].commandID, context);
        }
    }
}

bool CVulture::ExecuteCommand(eCommandID command, CommandContext& context)
{
    if (CUnit::ExecuteCommand(command, context))
        return true;

    switch (command)
    {
    case eCommandID::MINE:
        LandMine();
        break;
    }
    return false;
}

void CVulture::CommandCardSlot(vector<CommandSlot>& outSlot)
{
    CUnit::CommandCardSlot(outSlot);
    //7번 : 마인 설치
    outSlot[6].commandID = eCommandID::MINE;
    outSlot[6].iconKey = TEXT("ICON_STEAMPACK");
    outSlot[6].hotkey = 'X';
    outSlot[6].clickable = true;
    outSlot[6].visible = true;
}

void CVulture::FireBullet()
{
    if (!m_pTarget) //안전장치
        return;

    Vec2 targetPos = m_pTarget->Get_Pos();
    Vec2 myPos = Get_Pos();
    //타겟까지의 거리
    Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
    //공격 사거리 체크
    if (dist <= m_fAttackRange)
    {        //타겟 방향 보기
        if (dist > 0.1f)
        {
            m_vDir = { diff.fX / dist, diff.fY / dist };
        }
        //사운드 재생
        CSoundMgr::Get_Instance()->PlayEffect(L"Vulture/VultureAttack1.wav", 0.8f);
        // 벌쳐 현재 방향 가져오기 (16방향)
        int iCurrentDir = m_tFrame.iFrame;
        // 투사체 생성
        CVBullet* pBullet = new CVBullet;
        pBullet->Initialize();
        pBullet->Set_Pos(m_tInfo.fX, m_tInfo.fY);
        pBullet->Set_Target(m_pTarget);
        pBullet->Set_Owner(this);
        pBullet->Set_Homing(true);
        pBullet->Set_Dir(m_vDir);
        // 투사체도 16방향 인덱스 설정
        pBullet->Set_Direction16(iCurrentDir);
        // ObjMgr에 추가
        CObjMgr::Get_Instance()->Add_Object(OBJID::OBJ_PROJECTILE, pBullet);
    }
}

void CVulture::LandMine()
{
    CSoundMgr::Get_Instance()->PlayEffect(L"Vulture/VultureMineIn.wav", 0.5f);
    //마인 설치!
    Vec2 worldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 diff = { worldMouse.fX - m_tInfo.fX, worldMouse.fY - m_tInfo.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
    if (dist > 0.1f)
        m_vDir = { diff.fX / dist, diff.fY / dist };

    //마인 생성
    CVultureMine* pMine = new CVultureMine;
    pMine->Initialize();
    pMine->Set_Pos(m_tInfo.fX, m_tInfo.fY); //타겟 위치로 생성
    pMine->Set_Target_Pos(worldMouse);
    pMine->Set_Owner(this);
    pMine->Set_Dir(m_vDir);
    CObjMgr::Get_Instance()->Add_Object(OBJID::OBJ_PROJECTILE, pMine);
}

void CVulture::UpdateDead()
{
    if (m_eTeamType == eTeamType::ALLY)
    {
        CResourceMgr::Get_Instance()->SubtractSupply(1);
    }
    //이펙트와 사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"Vulture/VultureDeath.wav", 1.f);

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

void CVulture::UpdateAI()
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

CObj* CVulture::FindNearestEnemyAI(float searchRadius)
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

