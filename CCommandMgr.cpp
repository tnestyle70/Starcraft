#include "pch.h"
#include "CCommandMgr.h"
#include "CSelectionMgr.h"
#include "CNavMgr.h"
#include "CObjMgr.h"
#include "CScrollMgr.h"
#include "CTileMgr.h"
#include "CInputMgr.h"
#include "CAbstractFactory.h"
#include "CSoundMgr.h"
#include "CTimeMgr.h"
#include "CCursorMgr.h"
#include "CMedic.h"
#include "CSCV.h"

CCommandMgr* CCommandMgr::m_pInstance = nullptr;

CCommandMgr::CCommandMgr()
{
}

CCommandMgr::~CCommandMgr()
{
}

static Vec2 GetMouseWorld()
{
    POINT pt{};
    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);
    //스크롤 반영
    float sx = CScrollMgr::Get_Instance()->Get_ScrollX();
    float sy = CScrollMgr::Get_Instance()->Get_ScrollY();
    return Vec2{ sx + pt.x, sy + pt.y };
}

void CCommandMgr::BeginPlaceBuilding(eBuildingType type, CUnit* pBuilder)
{
    CancleBuilding();
    m_eMode = eCommandMode::PLAEC_BUILDING;
    m_ePlaceType = type;
    m_pBuilder = pBuilder;
    //ghost 생성
    m_pGhost = CBuildingFactory::Create(type);
    m_pGhost->SetGhost(true);
}

void CCommandMgr::CancleBuilding()
{
    if (m_pGhost)
    {
        delete m_pGhost;
        m_pGhost = nullptr;
    }
    m_eMode = eCommandMode::NONE;
    m_pBuilder = nullptr;
}

bool CCommandMgr::IsPlacing()
{
    return m_eMode == eCommandMode::PLAEC_BUILDING;
}

void CCommandMgr::Update()
{
    //우클릭 명령 업데이트
    if (CInputMgr::Get_Instance()->KeyDown(RIGHT_MOUSE))
    {
        //커서 상태 변경
        CCursorMgr::Get_Instance()->SetClickEffect();
        //월드 마우스 좌표 가지고 오기
        Vec2 worldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
        CObj* pClickedEnemy = CObjMgr::Get_Instance()->PickEnemyAt(worldMouse);
        CObj* pClickedUnit = CObjMgr::Get_Instance()->PickUnitAt(worldMouse);
        if (pClickedEnemy)
        {
            //우클릭 공격 구현!
            CCommandMgr::Get_Instance()->IssueAttack(pClickedEnemy);
        }
        else if (pClickedUnit)
        {
            CCommandMgr::Get_Instance()->IssueHeal(pClickedUnit);
        }
        else
        {
            CCommandMgr::Get_Instance()->IssueMove(worldMouse); //기본 이동
        }
    }
    //A키 + Left Mouse 클릭으로 A땅 공격 구현
    if (CInputMgr::Get_Instance()->KeyDown(A_KEY))
    {
        m_bAttackMove = true;
    }
    if (m_bAttackMove && CInputMgr::Get_Instance()->KeyDown(LEFT_MOUSE))
    {
        Vec2 worldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
        CObj* pClickedObj = CObjMgr::Get_Instance()->PickObjAt(worldMouse);
            IssueAttackMove(worldMouse); //AttackMove에 경로만 넣어주기
        m_bAttackMove = false;
    }
    //카메라 화면 저장
    SaveCamSlot();
    //부대 지정
    HandleControlGroup();

    //사운드 타이머 업데이트
    float dt = CTimeMgr::Get_Instance()->GetDT();
    m_fSoundTimer += dt;
    if (m_fSoundTimer >= m_fSoundInterval)
    {
        m_bCanPlaySound = true;
    }
    //배치 모드가 아닐 경우 return 
    if (m_eMode != eCommandMode::PLAEC_BUILDING)
        return;

    if (!m_pGhost) 
    {
        m_eMode = eCommandMode::NONE;
        m_pBuilder = nullptr;
        return;
    }

    //마우스 월드 좌표
    Vec2 mouseWorld = GetMouseWorld();

    //배치 스냅 결과 확인
    int r = -1, c = -1;
    if (m_pGhost->CalcSizeTopLeft(mouseWorld, r, c))
    {
        //고스트 내부에 row/col 저장
        m_pGhost->SetPlace(r, c);
        //스냅된 타일 top-left 건물의 중심 좌표로 변환해서 고스트 위치 세팅
        Vec2 tile = CTileMgr::Get_Instance()->CellToWorldTopLeft(r, c);
        Vec2 center = tile;
        center.fX += m_pGhost->GetWidth() * TILECX * 0.5f;
        center.fY += m_pGhost->GetHeight() * TILECY * 0.5f;
        m_pGhost->Set_Pos(center.fX, center.fY);
    }
    else
    {
        //범위 밖일 경우 마우스 따라가게 하기
        m_pGhost->Set_Pos(mouseWorld.fX, mouseWorld.fY);
    }
    //배치 가능 여부 mouseWorld를 기준으로 판단
    bool can;
    if (CInputMgr::Get_Instance()->KeyDown(LEFT_MOUSE))
    {
        can = m_pGhost->CanPlace(mouseWorld);
        m_pGhost->SetCanPlace(can);
    }
    //건설 명령
    if (CInputMgr::Get_Instance()->KeyDown(LEFT_MOUSE))
    {
        if (can && m_pBuilder)
        {           
            CBuilding* pBuilding = m_pGhost;
            Vec2 buildPos = pBuilding->Get_Pos();
            m_pGhost = nullptr; //pBuilding으로 소유권 이전 시키기
            m_eMode = eCommandMode::NONE;

            // SCV에게 건설 오더 추가
            Order order;
            order.eType = eOrderType::MOVE_AND_BUILD;
            order.dst = buildPos;
            order.pBuilding = pBuilding;
            order.iPathIndex = 0;

            Vec2 start = m_pBuilder->Get_Pos();
            order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, buildPos);
            if (order.path.empty())
            {
                order.path.push_back(buildPos);
            }

            m_pBuilder->ClearOrder();
            m_pBuilder->PushOrder(order);
            m_eMode = eCommandMode::NONE;
            m_pBuilder = nullptr;

            /*
            //소유권 넘기기
            CBuilding* pBuilding = m_pGhost;
            m_pGhost = nullptr;

            pBuilding->SetGhost(false);
            pBuilding->AppplyOccupy();
            pBuilding->SetBuilder(m_pBuilder);
            //모드 정리 
            m_eMode = eCommandMode::NONE;
            m_pBuilder = nullptr;
            */
        }
        else
        {
            //Can't Build 사운드 재생
            eRaceType type = m_pGhost->GetOriginalRace();
            if (type == eRaceType::RACE_TERRAN)
            {
                CSoundMgr::Get_Instance()->PlayEffect(L"SCV/SCVCantBuild.wav", 1.f);
                return;
            }
            else if (type == eRaceType::RACE_PROTOSS)
            {
                CSoundMgr::Get_Instance()->PlayEffect(L"Advisor/BuildErr.wav", 1.f);
                return;
            }
            else
            {
                CSoundMgr::Get_Instance()->PlayEffect(L"Advisor/BuildErr.wav", 1.f);
                return;
            }
        }
    }
    if (CInputMgr::Get_Instance()->KeyDown(RIGHT_MOUSE) ||
        CInputMgr::Get_Instance()->KeyDownVK(VK_ESCAPE))
    {
        CancleBuilding();
        return;
    }
}

void CCommandMgr::Render(HDC hDC)
{
    if (m_pGhost)
        m_pGhost->Render(hDC);
}

void CCommandMgr::SaveCamSlot()
{
    //Shift + num으로 카메라 슬롯에 저장
    bool shift = CInputMgr::Get_Instance()->KeyPressVK(VK_LSHIFT);

    if (CInputMgr::Get_Instance()->KeyDownVK('1'))
    {
        if (shift)
            CScrollMgr::Get_Instance()->SaveCamSlot(1);
        else
            CScrollMgr::Get_Instance()->UpdateCamSlot(1);
    }
    if (CInputMgr::Get_Instance()->KeyDownVK('2'))
    {
        if (shift)
            CScrollMgr::Get_Instance()->SaveCamSlot(2);
        else
            CScrollMgr::Get_Instance()->UpdateCamSlot(2);
    }
    if (CInputMgr::Get_Instance()->KeyDownVK('3'))
    {
        if (shift)
            CScrollMgr::Get_Instance()->SaveCamSlot(3);
        else
            CScrollMgr::Get_Instance()->UpdateCamSlot(3);
    }
}

void CCommandMgr::HandleControlGroup()
{
    bool ctrl = CInputMgr::Get_Instance()->KeyPressVK(VK_CONTROL);
    bool shift = CInputMgr::Get_Instance()->KeyPressVK(VK_SHIFT);

    for (int i = 0; i <= 9; ++i)
    {
        char key = (i == 0) ? '0' : ('1' + i - 1);

        if (CInputMgr::Get_Instance()->KeyDownVK(key))
        {
            if (ctrl)
                CSelectionMgr::Get_Instance()->SaveControlGroup(i);
            else
                CSelectionMgr::Get_Instance()->LoadControlGroup(i, shift);
            break;
        }
    }
}

void CCommandMgr::IssueMove(Vec2& worldGoal)
{
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    if (selected.empty())
        return;
    int unitCount = selected.size();

    for (auto& pObj : selected)
    {
        eTeamType type = pObj->GetTeamType();
        if (type == eTeamType::ENEMY)
            return;
    }

    if (unitCount == 1)
    {
        //단일 유닛 : 직진
        CUnit* pUnit = dynamic_cast<CUnit*>(selected[0]);

        //이동 사운드 재생
        PlayMoveSound(pUnit);
        if (pUnit)
        {
            Vec2 start = pUnit->Get_Pos();
            //AStart를 통해 계산한 위치를 반환 받아서 CUnit 쪽에 넘겨주기 
            vector<Vec2> path = CNavMgr::Get_Instance()->RequestPathWorld(start, worldGoal);

            Order order;
            order.eType = eOrderType::MOVE;
            order.dst = worldGoal;
            order.path = move(path);
            order.iPathIndex = 0;
            if (order.path.empty())
            {
                order.path.push_back(worldGoal);
                order.iPathIndex = 0;
            }
            //우클릭 이동일 경우 기존 오더 비우기
            pUnit->SetAttackMove(false); //AttackMove모드 해제 
            pUnit->ClearOrder();
            pUnit->PushOrder(order);

            return;
        }
        else //건물도 order 넣어서 이동 가능하게 하기
        {
            CBuilding* pBuilding = dynamic_cast<CBuilding*>(selected[0]);
            if (!pBuilding)
                return;
            Vec2 start = pBuilding->Get_Pos();
            //AStart를 통해 계산한 위치를 반환 받아서 CUnit 쪽에 넘겨주기 
            vector<Vec2> path = CNavMgr::Get_Instance()->RequestPathWorld(start, worldGoal);

            Order order;
            order.eType = eOrderType::MOVE;
            order.dst = worldGoal;
            order.path = move(path);
            order.iPathIndex = 0;
            if (order.path.empty())
            {
                order.path.push_back(worldGoal);
                order.iPathIndex = 0;
            }
            //우클릭 이동일 경우 기존 오더 비우기
            pBuilding->ClearOrder();
            pBuilding->PushOrder(order);
            return;
        }
    }
    //격자형 배치
    else
    {
        //첫번째 유닛 이동 사운드 재생
        CUnit* pUnit = dynamic_cast<CUnit*>(selected[0]);
        PlayMoveSound(pUnit);

        int cols = (int)ceil(sqrt(unitCount));  // 한 줄에 몇 개
        float spacing = 25.f;  // 유닛 간 간격

        int index = 0;
        for (auto* obj : selected)
        {
            CUnit* unit = dynamic_cast<CUnit*>(obj);
            if (!unit) continue;

            int row = index / cols;
            int col = index % cols;

            // 중앙 정렬을 위한 오프셋
            float offsetX = (col - cols / 2.0f) * spacing;
            float offsetY = (row - unitCount / cols / 2.0f) * spacing;

            Vec2 formationGoal;
            formationGoal.fX = worldGoal.fX + offsetX;
            formationGoal.fY = worldGoal.fY + offsetY;

            Order order;
            order.eType = eOrderType::MOVE;
            order.dst = formationGoal;
            order.path.clear();

            unit->SetAttackMove(false);
            unit->ClearOrder();
            unit->PushOrder(order);

            index++;
        }
    }
    //원형 배치
    /*
    else
    {
        float formationRadius = 15.f + (unitCount * 2.f);
        float angleStep = 360.f / unitCount;

        int index = 0;
        for (auto* obj : selected)
        {
            CUnit* unit = dynamic_cast<CUnit*>(obj);
            if (!unit)
                continue;

            // 원형 배치
            float angle = angleStep * index;
            float angleRad = angle * 3.14159f / 180.f;

            Vec2 formationGoal;
            formationGoal.fX = worldGoal.fX + cosf(angleRad) * formationRadius;
            formationGoal.fY = worldGoal.fY + sinf(angleRad) * formationRadius;

            Order order;
            order.eType = eOrderType::MOVE;
            order.dst = formationGoal;  // 각자 다른 목표!
            order.path.clear();

            unit->ClearOrder();
            unit->PushOrder(order);

            index++;
        }
    }
    */
}

void CCommandMgr::IssueAttack(CObj* pTarget) //우클릭 공격
{
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    if (selected.empty() || !pTarget)
        return;

    for (auto& pObj : selected)
    {
        if (!pObj->IsSelectable())
            return;
        eTeamType type = pObj->GetTeamType();
        if (type == eTeamType::ENEMY)
            return;
    }

    for (auto* pObj : selected)
    {
        CUnit* pUnit = dynamic_cast<CUnit*>(pObj);
        if (!pUnit) continue;

        Order order;
        order.eType = eOrderType::ATTACK;
        order.pTarget = pTarget;
        order.dst = pTarget->Get_Pos();
        order.path.clear();
        order.iPathIndex = 0;

        pUnit->SetAttackMove(false);
        pUnit->ClearOrder();
        pUnit->PushOrder(order);
    }
}

void CCommandMgr::IssueAttackMove(Vec2& worldGoal) //A땅 공격
{
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();

    if (selected.empty())
        return;

    for (auto& pObj : selected)
    {
        if (!pObj->IsSelectable())
            return;
        eTeamType type = pObj->GetTeamType();
        if (type == eTeamType::ENEMY)
            return;
    }

    for (auto* obj : selected) //선택된 유닛에 대한 A땅 공격 구현
    {
        CUnit* pUnit = dynamic_cast<CUnit*>(obj);
        if (!pUnit) continue;

        Order order;
        order.eType = eOrderType::ATTACK_MOVE;
        order.dst = worldGoal;
        order.pTarget = nullptr;
        order.path.clear();
        order.iPathIndex = 0;
        //오더 비우고 A땅 이동 오더 실행!
        pUnit->SetAttackMove(true); //A땅 공격 true
        pUnit->ClearOrder();
        pUnit->PushOrder(order);
    }
}

void CCommandMgr::IssueHeal(CObj* pTarget)
{
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    if (selected.empty() || !pTarget)
        return;
    for (auto* pObj : selected)
    {
        CMedic* pMedic = dynamic_cast<CMedic*>(pObj);
        if (!pMedic) continue;

        Order order;
        order.eType = eOrderType::ATTACK;
        order.pTarget = pTarget;
        order.dst = pTarget->Get_Pos();
        order.path.clear();
        order.iPathIndex = 0;

        pMedic->SetAttackMove(false); //A땅 공격 true
        pMedic->ClearOrder();
        pMedic->PushOrder(order);
    }
}   

void CCommandMgr::PlayMoveSound(CUnit* pUnit)
{
    if (!m_bCanPlaySound)
        return;

    if (!pUnit || pUnit->IsDead())
        return;

    eUnitType type = pUnit->Get_UnitType();

    switch (type)
    {
    case eUnitType::SCV:
        CSoundMgr::Get_Instance()->PlayEffect(L"SCV/SCVMove2.wav", 0.4f);
        break;
    case eUnitType::MARINE:
        CSoundMgr::Get_Instance()->PlayEffect(L"Marine/MarineMove1.wav", 0.4f);
        break;
    case eUnitType::MEDIC:
        CSoundMgr::Get_Instance()->PlayEffect(L"Medic/MedicMove1.wav", 0.4f);
        break;
    case eUnitType::FIREBAT:
        CSoundMgr::Get_Instance()->PlayEffect(L"FireBat/FireBatMove2.wav", 0.4f);
        break;
    case eUnitType::GHOST:
        CSoundMgr::Get_Instance()->PlayEffect(L"Ghost/GhostMove1.wav", 0.4f);
        break;
    case eUnitType::VULTURE:
        CSoundMgr::Get_Instance()->PlayEffect(L"Vulture/VultureMove3.wav", 0.4f);
        break;
    case eUnitType::TANK:
        CSoundMgr::Get_Instance()->PlayEffect(L"Tank/TankMove1.wav", 0.4f);
        break;
    case eUnitType::GOLIATH:
        CSoundMgr::Get_Instance()->PlayEffect(L"Goliath/TGoYes02.wav", 0.4f);
        break;
    case eUnitType::BATTLECRUISER:
        CSoundMgr::Get_Instance()->PlayEffect(L"BattleCrusor/BattleCrusorMove2.wav", 0.4f);
        break;
        //프로토스
    case eUnitType::PROBE:
        CSoundMgr::Get_Instance()->PlayEffect(L"Probe/ppryes00.wav", 0.4f);
        break;
    default:
        break;
    }
    m_bCanPlaySound = false;
    m_fSoundTimer = 0.f;
}
