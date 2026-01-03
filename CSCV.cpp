#include "pch.h"
#include "CSCV.h"
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

CSCV::CSCV() 
{
}

CSCV::~CSCV()
{
    Release();
}

void CSCV::Initialize()
{
    m_tInfo.fCX = 72.f; //SCV 한 칸 크기
    m_tInfo.fCY = 72.f;
    m_iMaxHP = 50;
    m_iHP = m_iMaxHP;
    m_fSpeed = 150.f;

    m_pFrameKey = L"SCV";

    m_eRender = RENDER_WORLD;
    m_eCommandCardState = eCommandCardState::MAIN;
    m_eState = eUnitState::IDLE;
    m_eType = eUnitType::SCV;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 2;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
    //자원
    m_dwGatherStartTime = 0;
}

int CSCV::Update()
{
    int iResult = CUnit::Update();

    if (iResult == DEAD)
        return DEAD;

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
        m_tFrame.iFrame = DirTo16WayIndex(m_vDir);

        if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
        {
            m_tFrame.iStart++;
            if (m_tFrame.iStart > m_tFrame.iEnd)
                m_tFrame.iStart = 0;
            m_tFrame.dwTime = now;
        }
        break;
    case eUnitState::GATHER:
        //땜질 effect 추가하기
        m_tFrame.iFrame = DirTo16WayIndex(m_vDir);

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
        break;
    case eUnitState::DIE:
        break;
    default:
        break;
    }

    __super::Update_Rect();

    return NOEVENT;
}

void CSCV::Late_Update()
{
    //선택이 되었을 경우 마우스 방향의 애니메이션 재생
    if (!m_bSelected) return;
    //이동 중이면 마우스 방향 애니메이션 재생 멈추기
    if (m_eState != eUnitState::IDLE) return;

    Vec2 vWorldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    Vec2 vDir{ vWorldMouse.fX - m_tInfo.fX, vWorldMouse.fY - m_tInfo.fY };

    int iDir16 = DirTo16WayIndex(vDir);
    m_tFrame.iFrame = iDir16;
}

void CSCV::Render(HDC hDC)
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

void CSCV::Release()
{
}

void CSCV::UpdateHotKeys()
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

bool CSCV::ExecuteCommand(eCommandID command, CommandContext& context)
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
    case eCommandID::COMMAND_CENTER:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::COMMAND_CENTER, this);
        break;
    case eCommandID::BARRACKS:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::BARRACKS, this);
        break;
    case eCommandID::FACTORY:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::FACTORY, this);
        break;
    case eCommandID::STARPORT:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::STARPORT, this);
        break;
    case eCommandID::SUPPLY_DEPOT:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::SUPPLY_DEPOT, this);
        break;
    case eCommandID::REFINERY:
        CCommandMgr::Get_Instance()->BeginPlaceBuilding(eBuildingType::REFINERY, this);
        break;
    }

    return false;
}

void CSCV::CommandCardSlot(vector<CommandSlot>& outSlot)
{
    //CONSTRUCTING 중에는 건설 슬롯 안 보이게
    if (m_eState == eUnitState::CONSTRUCTING)
        return;
    //건설 모드 체크 CommandMgr로 변경
    if (CCommandMgr::Get_Instance()->IsPlacing())
        return;

    switch (m_eCommandCardState)
    {
    case eCommandCardState::MAIN:
        CUnit::CommandCardSlot(outSlot);
        //7번 : 일반 건설 모드로 이동
        outSlot[6].commandID = eCommandID::NORMAL_BUILDING;
        outSlot[6].iconKey = TEXT("ICON_NORMAL_BUILDING");
        outSlot[6].hotkey = 'B';
        outSlot[6].clickable = true;
        outSlot[6].visible = true;
        //7번 : 고급 건설 모드로 이동
        outSlot[7].commandID = eCommandID::ADVANCED_BUILDING;
        outSlot[7].iconKey = TEXT("ICON_ADVANCED_BUILDING");
        outSlot[7].hotkey = 'V';
        outSlot[7].clickable = true;
        outSlot[7].visible = true;
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
        //0번 : CommandCenter 생성
        outSlot[0].commandID = eCommandID::COMMAND_CENTER;
        outSlot[0].iconKey = TEXT("ICON_COMMAND_CENTER");
        outSlot[0].hotkey = 'B';
        outSlot[0].clickable = true;
        outSlot[0].visible = true;
        //1번 : SupplyDepot 생성
        outSlot[1].commandID = eCommandID::SUPPLY_DEPOT;
        outSlot[1].iconKey = TEXT("ICON_SUPPLY_DEPOT");
        outSlot[1].hotkey = 'H';
        outSlot[1].clickable = true;
        outSlot[1].visible = true;
        //2번 : Refinary 생성
        outSlot[2].commandID = eCommandID::REFINERY;
        outSlot[2].iconKey = TEXT("ICON_REFINARY");
        outSlot[2].hotkey = 'T';
        outSlot[2].clickable = true;
        outSlot[2].visible = true;
        //3번 : Barracks 생성
        outSlot[3].commandID = eCommandID::BARRACKS;
        outSlot[3].iconKey = TEXT("ICON_BARRACKS");
        outSlot[3].hotkey = 'G';
        outSlot[3].clickable = true;
        outSlot[3].visible = true;
        //9번 : Cancle -> Main으로 복귀
        outSlot[8].commandID = eCommandID::CANCLE;
        outSlot[8].iconKey = TEXT("ICON_CANCLE");
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
            outSlot[i].iconKey = TEXT("");
            outSlot[i].hotkey = 0;
            outSlot[i].clickable = false;
            outSlot[i].visible = false;
        }
        //6번 : Factory 생성
        outSlot[0].commandID = eCommandID::FACTORY;
        outSlot[0].iconKey = TEXT("ICON_FACTORY");
        outSlot[0].hotkey = 'Y';
        outSlot[0].clickable = true;
        outSlot[0].visible = true;
        //9번 : Starport 생성
        outSlot[1].commandID = eCommandID::STARPORT;
        outSlot[1].iconKey = TEXT("ICON_STARPORT");
        outSlot[1].hotkey = 'N';
        outSlot[1].clickable = true;
        outSlot[1].visible = true;
        //9번 : Cancle -> Main으로 복귀
        outSlot[8].commandID = eCommandID::CANCLE;
        outSlot[8].iconKey = TEXT("ICON_CANCLE");
        outSlot[8].hotkey = VK_ESCAPE;
        outSlot[8].clickable = true;
        outSlot[8].visible = true;
        break;
    default:
        break;
    }
}

bool CSCV::UpdateGather(Order& order)
{
    //타겟 자원이 없거나 사라진 경우 예외 처리
    if (!order.pTarget || order.pTarget->IsDead())
    {
        m_eState = eUnitState::IDLE;
        m_pTargetResource = nullptr;
        return true;
    }

    //타겟 미네랄 저장
    if (!m_pTargetResource)
        m_pTargetResource = order.pTarget;

    Vec2 resourcePos = order.pTarget->Get_Pos();
    Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };
    Vec2 diff = { resourcePos.fX - myPos.fX, resourcePos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
    
    if (dist <= 50.f)
    {
        m_eState = eUnitState::GATHER;
        //자원 방향 보기
        if (dist > 0.1f)
        {
            m_vDir = { diff.fX / dist, diff.fY / dist };
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
            //가장 가까운 커맨드 센터 찾기
            CObj* pCC = FindNearestCommandCenter();
            if (pCC) //RETURN 오더 생성
            {
                Order returnOrder;
                returnOrder.eType = eOrderType::RETURN_RESOURCE;
                returnOrder.pTarget = pCC;
                returnOrder.dst = pCC->Get_Pos();
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

bool CSCV::UpdateReturn(Order& order)
{
    //커맨드 센터가 없거나 사라진 경우
    if (!order.pTarget || order.pTarget->IsDead())
    {
        m_eState = eUnitState::IDLE;
        m_iCarryingResource = 0;
        m_pTargetResource = nullptr;
        return true;
    }
    Vec2 ccPos = order.pTarget->Get_Pos();
    Vec2 myPos = Get_Pos();
    Vec2 diff = { ccPos.fX - myPos.fX, ccPos.fY - myPos.fY };
    float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
    
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
            order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, ccPos);
            if (order.path.empty())
            {
                order.path.push_back(ccPos);
            }
            order.iPathIndex = 0;
        }
    }
    //이동 처리
    return UpdateMove(order);
}



