#include "pch.h"
#include "CProbe.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CInputMgr.h"
#include "CSelectionMgr.h"
#include "CUIMgr.h"
#include "CResourceMgr.h"
#include "CCommandCenter.h"
#include "CBarracks.h"
#include "CFactory.h"
#include "CStarport.h"
#include "CSupplyDepot.h"
#include "CTileMgr.h"
#include "CObjMgr.h"
#include "CNavMgr.h"
#include "CCommandMgr.h"
#include "CMineral.h"
#include "CEffect.h"
#include "CAbstractFactory.h"
#include "CTimeMgr.h"
#include "CSoundMgr.h"

CProbe::CProbe()
{
}

CProbe::~CProbe()
{
    Release();
}

void CProbe::Initialize()
{
    m_tInfo.fCX = 72.f; //SCV 한 칸 크기
    m_tInfo.fCY = 72.f;
    m_iMaxHP = 50;
    m_iHP = m_iMaxHP;
    m_iMaxShield = 50;
    m_iShield = m_iMaxShield;

    m_fSpeed = 100.f;

    m_pFrameKey = L"PROBE_";

    m_eOriginalRace = eRaceType::RACE_PROTOSS;
    m_eCurrentRace = eRaceType::RACE_PROTOSS;

    m_eRender = RENDER_WORLD;
    m_eCommandCardState = eCommandCardState::MAIN;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::PROBE;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 2;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
    //자원
    m_dwGatherStartTime = 0;
}

int CProbe::Update()
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

    //CONSTRUCTING 상태면 애니메이션만 처리하고 끝
    if (m_eState == eUnitState::CONSTRUCTING)
    {
        // 건설 중 애니메이션 (예: 망치질 모션)
        m_tFrame.iStart = 0;  // 또는 건설 전용 프레임
        __super::Update_Rect();
        return NOEVENT;
    }

    DWORD now = GetTickCount();

    switch (m_eState)
    {
    case eUnitState::IDLE:
        m_tFrame.iStart = 0;
        break;
    case eUnitState::MOVE:
        m_tFrame.iFrame = DirTo17WayIndex(m_vDir);
        break;
    case eUnitState::CONSTRUCTING:
        m_tFrame.iCol = 2;
        m_tFrame.iFrame = DirTo17WayIndex(m_vDir);

        if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
        {
            m_tFrame.iStart++;
            if (m_tFrame.iStart > m_tFrame.iEnd)
                m_tFrame.iStart = 0;
            m_tFrame.dwTime = now;
        }
        break;
    case eUnitState::GATHER:
        m_tFrame.iCol = 2;
        m_tFrame.iFrame = DirTo17WayIndex(m_vDir);

        if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
        {
            m_tFrame.iStart++;
            if (m_tFrame.iStart > m_tFrame.iEnd)
                m_tFrame.iStart = 0;
            m_tFrame.dwTime = now;
        }
        break;
    case eUnitState::RETURN_RESOURCE:
        //복귀하면서 미네랄 들고 오기 
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

void CProbe::Late_Update()
{
    //이전에 선택되지 않았고, 이번 프레임에 선택되었다면 eCommandCardState = Main으로 전환!
    m_bCurSelected = false;
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    for (auto* obj : selected)
    {
        if (obj == this)
        {
            m_bCurSelected = true;
            break;
        }
    }
    if (!m_bPrevSelected && m_bCurSelected)
    {
        m_eCommandCardState = eCommandCardState::MAIN;
    }
    m_bPrevSelected = m_bCurSelected;

    //선택이 되었을 경우 마우스 방향의 애니메이션 재생
    if (!m_bSelected) return;
    //마우스 방향 안 보기!
    if (m_eState != eUnitState::IDLE) return;
    //if (m_eState == eUnitState::MOVE) return;
    //if (m_eState == eUnitState::ATTACK) return;
    //if (m_eState == eUnitState::IDLE) return;
    //if (m_eState == eUnitState::GATHER) return;
    //if (m_eState == eUnitState::RETURN_RESOURCE) return;
    //if (m_eState == eUnitState::CONSTRUCTING) return;

    Vec2 vWorldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 vDir{ vWorldMouse.fX - m_tInfo.fX, vWorldMouse.fY - m_tInfo.fY };

    int iDir17 = DirTo17WayIndex(vDir);
    m_tFrame.iFrame = iDir17;
}

void CProbe::Render(HDC hDC)
{
    if (!m_bVisible) //벙커에 탑승해있는 상태일 경우 렌더링X
        return;

    //전장의 안개 
    CUnit::Render(hDC);

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
    wsprintf(szKey, L"Probe_Move_%d", fileIndex);

    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szKey);
    if (pPng)
    {
        int iWidth = pPng->Get_Width();
        int iHeight = pPng->Get_Height();

        if (bFlip)
            pPng->Render_Alpha_Flipped(hDC, 
                (int)m_tInfo.fX - iScrollX - iWidth / 2, 
                (int)m_tInfo.fY - iScrollY - iHeight / 2, iWidth, iHeight, false, m_bConceal);
        else
            pPng->Render_Alpha(hDC, 
                (int)m_tInfo.fX - iScrollX - iWidth / 2,
                (int)m_tInfo.fY - iScrollY - iHeight / 2,
                iWidth, iHeight, false, m_bConceal);
    }

    //자원 렌더링
    TCHAR szResKey[128];
    if (m_bCarryingResource)
    {
        if (m_eResourceType == eResourceType::MINERAL)
            wsprintf(szResKey, L"MINERAL");
        if (m_eResourceType == eResourceType::GAS)
            wsprintf(szResKey, L"GAS");
        CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(szResKey);
        if (!pPng) return;
        int width = pPng->Get_Width();
        int height = pPng->Get_Height();
        pPng->Render_Alpha(hDC, iDrawX - 4.f, iDrawY - 20.f,
            width, height, false);
    }
}

void CProbe::Release()
{
}

void CProbe::UpdateHotKeys()
{
    CUnit::UpdateHotKeys();
    //SCV 유닛 하나만 선택되었을 경우 실행
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    if (selected.size() != 1)
        return;
    //선택된 객체가 this인지 확인
    if (selected[0] != this)
        return;
    //건설 중이면 키 입력 안 먹게
    if (m_eState == eUnitState::CONSTRUCTING)
        return;

    //슬롯 정보
    vector<CommandSlot> slots;
    this->CommandCardSlot(slots);
    //각 슬롯의 단축키 확인
    for (int i = 0; i < slots.size(); ++i)
    {
        if (!slots[i].visible || !slots[i].clickable)
            continue;
        //단축키가 눌렸는지 확인
        if (CInputMgr::Get_Instance()->KeyDownVK(slots[i].hotkey))
        {
            CUIMgr::Get_Instance()->SetButtonFeedback(i, true);
            //명령 실행
            CommandContext context{};
            this->ExecuteCommand(slots[i].commandID, context);
        }
    }
}

bool CProbe::ExecuteCommand(eCommandID command, CommandContext& context)
{
    ResourceCost cost{};
    //먼저 부모 명령 실행
    if (CUnit::ExecuteCommand(command, context))
        return true;

    // CCommandMgr로 위임, 부모 명령 실행 이후 진행
    switch (command)
    {
    case eCommandID::NORMAL_BUILDING:
        m_eCommandCardState = eCommandCardState::NORMAL_BUILD;
        break;
    case eCommandID::ADVANCED_BUILDING:
        m_eCommandCardState = eCommandCardState::ADVANCED_BUILD;
        break;
    case eCommandID::CANCLE:
        m_eCommandCardState = eCommandCardState::MAIN;
        break;
        //NBuild
    case eCommandID::NEXUS:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::NEXUS, this);
        break;
    case eCommandID::PYLON:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::PYLON, this);
        break;
    case eCommandID::ASSIMILATOR:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::ASSIMILATOR, this);
        break;
    case eCommandID::GATEWAY:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::GATEWAY, this);
        break;
    case eCommandID::FORGE:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::FORGE, this);
        break;
    case eCommandID::PHOTON_CANNON:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::PHOTON_CANNON, this);
        break;
    case eCommandID::CYBERNETICS_CORE:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::CYBERNETICS_CORE, this);
        break;
    case eCommandID::SHIELD_BATTERY:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::SHIELD_BATTERY, this);
        break;
        //ABuild
    case eCommandID::ROBOTICS_FACILITY:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::ROBOTICS_FACILITY, this);
        break;
    case eCommandID::STARGATE:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::STARGATE, this);
        break;
    case eCommandID::CITADEL_OF_ADUN:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::CITADEL_OF_ADUN, this);
        break;
    case eCommandID::ROBOTICS_SUPPORT_BAY:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::ROBOTICS_SUPPORT_BAY, this);
        break;
    case eCommandID::FLEET_BEACON:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::FLEET_BEACON, this);
        break;
    case eCommandID::TEMPLAR_ARCHIVES:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::TEMPLAR_ARCHIVES, this);
        break;
    case eCommandID::OBSERVATORY:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::OBSERVATORY, this);
        break;
    case eCommandID::ARBITER_TRIBUNAL:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::ARBITER_TRIBUNAL, this);
        break;
    }

    return false;
}

void CProbe::CommandCardSlot(vector<CommandSlot>& outSlot)
{
    //CONSTRUCTING 중에는 건설 슬롯 안 보이게
    if (m_eState == eUnitState::CONSTRUCTING)
        return;
    //건설 모드 체크 CommandMgr로 변경
    if (CCommandMgr::Get_Instance()->IsPlacing())
        return;

    bool bGateBuilt = CObjMgr::Get_Instance()->IsGateBuilt();
    bool bRoboticsBuilt = CObjMgr::Get_Instance()->IsRoboticsBuilt();
    bool bStargateBuilt = CObjMgr::Get_Instance()->IsStargateBuilt();

    switch (m_eCommandCardState)
    {
    case eCommandCardState::MAIN:
        CUnit::CommandCardSlot(outSlot);
        //7번 : 일반 건설 모드로 이동
        outSlot[6].commandID = eCommandID::NORMAL_BUILDING;
        outSlot[6].hotkey = 'B';
        outSlot[6].clickable = true;
        outSlot[6].visible = true;
        //7번 : 고급 건설 모드로 이동
        outSlot[7].commandID = eCommandID::ADVANCED_BUILDING;
        outSlot[7].hotkey = 'V';
        outSlot[7].clickable = true;
        outSlot[7].visible = true;
        //9번 : Cancle -> Main으로 복귀
        outSlot[8].commandID = eCommandID::CANCLE;
        outSlot[8].hotkey = VK_ESCAPE;
        outSlot[8].clickable = true;
        outSlot[8].visible = true;
        break;
    case eCommandCardState::NORMAL_BUILD:
        //Build 모드 일 경우에는 처음부터 새로 Commandslot구성
        outSlot.clear();
        outSlot.resize(9);
        for (int i = 0; i < 9; ++i)
        {
            outSlot[i].slotIndex = i;
            outSlot[i].commandID = eCommandID::NONE;
            outSlot[i].iconKey = TEXT("");
            outSlot[i].hotkey = 0;
            outSlot[i].clickable = false;
            outSlot[i].visible = false;
        }
        //0번 : Nexus 생성
        outSlot[0].commandID = eCommandID::NEXUS;
        outSlot[0].hotkey = 'W';
        outSlot[0].clickable = true;
        outSlot[0].visible = true;
        //1번 : Pylon 생성
        outSlot[1].commandID = eCommandID::PYLON;
        outSlot[1].hotkey = 'E';
        outSlot[1].clickable = true;
        outSlot[1].visible = true;
        //2번 : Assimilator 생성
        outSlot[2].commandID = eCommandID::ASSIMILATOR;
        outSlot[2].hotkey = 'R';
        outSlot[2].clickable = true;
        outSlot[2].visible = true;
        //3번 : Gateway 생성
        outSlot[3].commandID = eCommandID::GATEWAY;
        outSlot[3].hotkey = 'S';
        outSlot[3].clickable = true;
        outSlot[3].visible = true;
        //4번 : Forge 생성
        outSlot[4].commandID = eCommandID::FORGE;
        outSlot[4].hotkey = 'D';
        outSlot[4].clickable = true;
        outSlot[4].visible = true;
        outSlot[4].lock = !bGateBuilt;
        //5번 : Photo Cannon 생성
        outSlot[5].commandID = eCommandID::PHOTON_CANNON;
        outSlot[5].hotkey = 'F';
        outSlot[5].clickable = true;
        outSlot[5].visible = true;
        outSlot[5].lock = !bGateBuilt;
        //outSlot[5].lock = !bBarrackBuilt;
        //6번 : Cybernetics Core 생성
        outSlot[6].commandID = eCommandID::CYBERNETICS_CORE;
        outSlot[6].hotkey = 'X';
        outSlot[6].clickable = true;
        outSlot[6].visible = true;
        outSlot[6].lock = !bGateBuilt;
        //outSlot[6].lock = !bBarrackBuilt;
        //7번 : Shield Battery 생성
        outSlot[7].commandID = eCommandID::SHIELD_BATTERY;
        outSlot[7].hotkey = 'C';
        outSlot[7].clickable = true;
        outSlot[7].visible = true;
        outSlot[7].lock = !bGateBuilt;
        //outSlot[7].lock = !bBarrackBuilt;
        //9번 : Cancle -> Main으로 복귀
        outSlot[8].commandID = eCommandID::CANCLE;
        outSlot[8].hotkey = VK_ESCAPE;
        outSlot[8].clickable = true;
        outSlot[8].visible = true;
        break;
    case eCommandCardState::ADVANCED_BUILD:
        //Build 모드 일 경우에는 처음부터 새로 Commandslot구성
        outSlot.clear();
        outSlot.resize(9);
        for (int i = 0; i < 9; ++i)
        {
            outSlot[i].slotIndex = i;
            outSlot[i].commandID = eCommandID::NONE;
            outSlot[i].hotkey = 0;
            outSlot[i].clickable = false;
            outSlot[i].visible = false;
        }
        //0번 : Robotics Facility 생성
        outSlot[0].commandID = eCommandID::ROBOTICS_FACILITY;
        outSlot[0].hotkey = 'W';
        outSlot[0].clickable = true;
        outSlot[0].visible = true;
        outSlot[0].lock = !bGateBuilt;
        //1번 : Starport 생성
        outSlot[1].commandID = eCommandID::STARGATE;
        outSlot[1].hotkey = 'E';
        outSlot[1].clickable = true;
        outSlot[1].visible = true;
        outSlot[1].lock = !bRoboticsBuilt;
        //2번 : ScienceFacility 생성
        outSlot[2].commandID = eCommandID::CITADEL_OF_ADUN;
        outSlot[2].hotkey = 'R';
        outSlot[2].clickable = true;
        outSlot[2].visible = true;
        outSlot[2].lock = !bRoboticsBuilt;
        //3번 : Armory 생성
        outSlot[3].commandID = eCommandID::ROBOTICS_SUPPORT_BAY;
        outSlot[3].hotkey = 'S';
        outSlot[3].clickable = true;
        outSlot[3].visible = true;
        outSlot[3].lock = !bRoboticsBuilt;
        //1번 : Starport 생성
        outSlot[4].commandID = eCommandID::FLEET_BEACON;
        outSlot[4].hotkey = 'D';
        outSlot[4].clickable = true;
        outSlot[4].visible = true;
        outSlot[4].lock = !bStargateBuilt;
        //2번 : ScienceFacility 생성
        outSlot[5].commandID = eCommandID::TEMPLAR_ARCHIVES;
        outSlot[5].hotkey = 'F';
        outSlot[5].clickable = true;
        outSlot[5].visible = true;
        outSlot[5].lock = !bRoboticsBuilt;
        //3번 : Armory 생성
        outSlot[6].commandID = eCommandID::OBSERVATORY;
        outSlot[6].hotkey = 'X';
        outSlot[6].clickable = true;
        outSlot[6].visible = true;
        outSlot[6].lock = !bRoboticsBuilt;
        //3번 : Armory 생성
        outSlot[7].commandID = eCommandID::ARBITER_TRIBUNAL;
        outSlot[7].hotkey = 'C';
        outSlot[7].clickable = true;
        outSlot[7].visible = true;
        outSlot[7].lock = !bStargateBuilt;
        //9번 : Cancle -> Main으로 복귀
        outSlot[8].commandID = eCommandID::CANCLE;
        outSlot[8].hotkey = VK_ESCAPE;
        outSlot[8].clickable = true;
        outSlot[8].visible = true;
        break;
    default:
        break;
    }
}

bool CProbe::UpdateConstructing(Order& order)
{
    //타겟 건물 저장
    CBuilding* pBuilding = order.pBuilding;
    if (!pBuilding)
    {
        m_eState = eUnitState::IDLE;
        return true;
    }
    //건물 완료됐으면 종료
    if (pBuilding->IsComplete())
    {
        m_eState = eUnitState::IDLE;
        return true;
    }

    Vec2 buildingPos = pBuilding->Get_Pos();
    Vec2 myPos = Get_Pos();
    Vec2 diff = { buildingPos.fX - myPos.fX, buildingPos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);

    //건물 건설 범위 설정
    const float CONSTRUCTION_RANGE = 80.f;

    float dt = CTimeMgr::Get_Instance()->GetDT();

    if (dist <= CONSTRUCTION_RANGE - 10.f) //건물과 너무 가까우면 뒤로 이동
    {
        if (dist > 0)
        {
            m_vDir = { -diff.fX / dist, -diff.fY / dist };
            m_tInfo.fX += m_vDir.fX * dt * m_fSpeed;
            m_tInfo.fY += m_vDir.fY * dt * m_fSpeed;
        }
        return false;
    }

    //빌딩 방향 보기
    if (dist > 0.1f)
    {
        m_vDir = { diff.fX / dist, diff.fY / dist };
    }

    //빌딩 이펙트 생성
    if (m_fEffectCoolTime > 0.f)
    {
        m_fEffectCoolTime -= CTimeMgr::Get_Instance()->GetDT();
        if (m_fEffectCoolTime < 0.f)
        {
            m_fEffectCoolTime = 0.f;
        }
    }

    //if (m_fEffectCoolTime <= 0.f)
    //{
    //    m_fEffectCoolTime = EFFECT_INTERVAL;
    //    CObj* pEffect = CAbstractFactory<CEffect>::Create(
    //        m_tInfo.fX, m_tInfo.fY);
    //    pEffect->Initialize();
    //    CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
    //    if (pEffectObj)
    //    {
    //        pEffectObj->Set_Effect(L"SCV_EFFECT", 10, 48, 48,
    //            eEffectType::ROW_BASE, RGB(255, 0, 255), 0.02f);
    //    }
    //    CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
    //}

    return false;
}

bool CProbe::UpdateGather(Order& order)
{
    //타겟 자원이 없거나 사라진 경우 예외 처리
    if (!order.pTarget || order.pTarget->IsDead())
    {
        m_eState = eUnitState::IDLE;
        m_pTargetResource = nullptr;
        return true;
    }

    //타겟 자원 저장
    if (!m_pTargetResource)
        m_pTargetResource = order.pTarget;

    Vec2 resourcePos = order.pTarget->Get_Pos();
    Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };
    Vec2 diff = { resourcePos.fX - myPos.fX, resourcePos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);

    if (dist <= 40.f)
    {

        m_eState = eUnitState::GATHER;
        //자원 방향 보기
        if (dist > 0.1f)
        {
            m_vDir = { diff.fX / dist, diff.fY / dist };
        }
        //사운드 재생
        //CSoundMgr::Get_Instance()->PlayEffect(L"SCV/SCVGather1.wav", 0.5f);
        //미네랄일 경우에만 이펙트 생성
        if (m_eResourceType == eResourceType::MINERAL)
        {
            //타겟 이펙트 생성
            if (m_fEffectCoolTime > 0.f)
            {
                m_fEffectCoolTime -= CTimeMgr::Get_Instance()->GetDT();
                if (m_fEffectCoolTime < 0.f)
                    m_fEffectCoolTime = 0.f;
            }
            if (m_fEffectCoolTime <= 0.f)
            {
                m_fEffectCoolTime = EFFECT_INTERVAL;
                CObj* pEffect = CAbstractFactory<CEffect>::Create(
                    m_tInfo.fX, m_tInfo.fY);
                pEffect->Initialize();
                CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
                if (pEffectObj)
                {
                    pEffectObj->Set_Effect(L"SCV_EFFECT", 10, 48, 48,
                        eEffectType::ROW_BASE, RGB(255, 0, 255), 0.02f);
                }
                CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
            }
        }
        else
        {
            m_bVisible = false;
        }
    
        //채집 시작 시간
        if (m_dwGatherStartTime == 0)
        {
            m_dwGatherStartTime = GetTickCount();
        }
        //채집 시간 체크
        DWORD now = GetTickCount();
        if (now - m_dwGatherStartTime >= GATHER_TIME)
        {
            //채집 완료
            m_iCarryingResource = MINERLA_PER_ONCE;
            m_dwGatherStartTime = 0;
            //가장 가까운 넥서스 센터 찾기
            CObj* pNexus = FindNearestNexus();
            if (pNexus) //RETURN 오더 생성
            {
                Order returnOrder;
                returnOrder.eType = eOrderType::RETURN_RESOURCE;
                returnOrder.pTarget = pNexus;
                returnOrder.dst = pNexus->Get_Pos();
                //현재 GATHER 오더 완료하고 RETURN 오더 추가
                m_OrderQ.pop_front();
                m_OrderQ.push_front(returnOrder);
                m_bActiveOrder = false;
                m_eState = eUnitState::RETURN_RESOURCE;
                return false; //다음 오더 진행
            }
            else
            {
                m_iCarryingResource = 0;
                return true;
            }
        }
        //채집 중인 상태
        return false;
    }
    else
    {
        m_eState = eUnitState::MOVE;
        //경로가 없으면 생성
        if (order.path.empty())
        {
            Vec2 start = { m_tInfo.fX, m_tInfo.fY };
            order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, resourcePos);
            if (order.path.empty())
            {
                order.path.push_back(resourcePos);
            }
            order.iPathIndex = 0;
        }
        return UpdateMove(order);
    }
}

bool CProbe::UpdateReturn(Order& order)
{
    //넥서스가 없거나 사라진 경우
    if (!order.pTarget || order.pTarget->IsDead())
    {
        m_eState = eUnitState::IDLE;
        m_iCarryingResource = 0;
        m_pTargetResource = nullptr;
        return true;
    }

    //자원 채집 시작 플래그
    m_bCarryingResource = true;

    Vec2 nexusPos = order.pTarget->Get_Pos();
    Vec2 myPos = Get_Pos();
    Vec2 diff = { nexusPos.fX - myPos.fX, nexusPos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);

    if (m_eResourceType == eResourceType::GAS)
        m_bVisible = true;

    if (dist <= 70.f)
    {
        if (m_eResourceType == eResourceType::MINERAL)
        {
            CResourceMgr::Get_Instance()->AddMineral(m_iCarryingResource);
            m_iCarryingResource = 0;
            //다시 같은 자원 채집하러 이동
            if (m_pTargetResource && !m_pTargetResource->IsDead())
            {
                Order gather;
                gather.eType = eOrderType::GATHER;
                gather.pTarget = m_pTargetResource;
                gather.dst = m_pTargetResource->Get_Pos();
                //현재 return 오더 완료했고 새 gather 오더 추가
                m_OrderQ.pop_front();
                m_OrderQ.push_front(gather);
                m_bActiveOrder = false;
                m_eState = eUnitState::MOVE;
                //자원 채집 완료
                m_bCarryingResource = false;
                return false;
            }
        }
        else if (m_eResourceType == eResourceType::GAS)
        {
            CResourceMgr::Get_Instance()->AddGas(m_iCarryingResource);
            m_iCarryingResource = 0;
            //다시 같은 자원 채집하러 이동
            if (m_pTargetResource && !m_pTargetResource->IsDead())
            {
                Order gather;
                gather.eType = eOrderType::GATHER;
                gather.pTarget = m_pTargetResource;
                gather.dst = m_pTargetResource->Get_Pos();
                //현재 return 오더 완료했고 새 gather 오더 추가
                m_OrderQ.pop_front();
                m_OrderQ.push_front(gather);
                m_bActiveOrder = false;
                m_eState = eUnitState::MOVE;
                //자원 채집 완료
                m_bCarryingResource = false;
                return false;
            }
        }
        else
        {
            m_pTargetResource = nullptr;
            return true; //미네랄이 존재하지 않으면 return
        }
    }
    else
    {
        m_eState = eUnitState::RETURN_RESOURCE;
        //경로가 존재하지 않으면 생성
        if (order.path.empty())
        {
            Vec2 start = Get_Pos();
            order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, nexusPos);
            if (order.path.empty())
            {
                order.path.push_back(nexusPos);
            }
            order.iPathIndex = 0;
        }
    }
    //이동 처리
    return UpdateMove(order);
}

void CProbe::UpdateDead()
{
    if (m_eTeamType == eTeamType::ALLY)
    {
        CResourceMgr::Get_Instance()->SubtractSupply(1);
    }
    //이펙트와 사운드 재생
    CSoundMgr::Get_Instance()->PlayEffect(L"Probe/pprdth00.wav", 1.f);

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

CObj* CProbe::FindNearestNexus()
{
    Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };
    list<CObj*> buildingList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_BUILDING);
    CObj* pNear = nullptr;
    float minDist = FLT_MAX;

    for (auto* pObj : buildingList)
    {
        if (!pObj || pObj->IsDead())
            continue;
        //넥서스인지 확인
        CBuilding* pBuilding = dynamic_cast<CBuilding*>(pObj);
        if (!pBuilding)
            continue;
        if (pBuilding->GetBuildingType() != eBuildingType::NEXUS)
            continue;
        Vec2 buildingPos = pObj->Get_Pos();
        Vec2 diff = { buildingPos.fX - myPos.fX, buildingPos.fY - myPos.fY };
        float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
        if (dist < minDist)
        {
            minDist = dist;
            pNear = pObj;
        }
    }
    return pNear;
}

void CProbe::UpdateAI()
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

CObj* CProbe::FindNearestEnemyAI(float searchRadius)
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



