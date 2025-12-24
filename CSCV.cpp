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

    m_fSpeed = 200.f;

    m_pFrameKey = L"SCV";

    m_eRender = RENDER_WORLD;
    m_eState = eUnitState::IDLE;
    m_tFrame.iFrame = 0;
    m_tFrame.iStart = 0;
    m_tFrame.iEnd = 2;
    m_tFrame.iCol = 0;
    m_tFrame.dwTime = 0;
    m_tFrame.dwSpeed = 100;
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
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
    int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

    //선택 원(예: m_bSelected가 true일 때) 추후에 bmp로 교체
    if (m_bSelected)
    {
        HBRUSH oldB = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
        HPEN oldP = (HPEN)SelectObject(hDC, pen);

        int cx = iDrawX + (int)(m_tInfo.fCX * 0.5f);
        int cy = iDrawY + (int)(m_tInfo.fCY * 0.8f);   // 발밑 느낌으로 살짝 아래
        int r = (int)(max(m_tInfo.fCX, m_tInfo.fCY) * 0.55f);

        Ellipse(hDC, cx - r, cy - r / 2, cx + r, cy + r / 2);

        SelectObject(hDC, oldP);
        SelectObject(hDC, oldB);
        DeleteObject(pen);
    }

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
    }

    return false;
}

void CSCV::CommandCardSlot(vector<CommandSlot>& outSlot)
{
    // 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
    CUnit::CommandCardSlot(outSlot);
    //CONSTRUCTING 중에는 건설 슬롯 안 보이게
    if (m_eState == eUnitState::CONSTRUCTING)
        return;
    //건설 모드 체크 CommandMgr로 변경
    if (CCommandMgr::Get_Instance()->IsPlacing())
        return;
    //6번 : Factory 생성
    outSlot[5].commandID = eCommandID::FACTORY;
    outSlot[5].iconKey = TEXT("ICON_COMMAND_CENTER");
    outSlot[5].hotkey = 'Y';
    outSlot[5].clickable = true;
    outSlot[5].visible = true;
    //7번 : CommandCenter 생성
    outSlot[6].commandID = eCommandID::COMMAND_CENTER;
    outSlot[6].iconKey = TEXT("ICON_COMMAND_CENTER");
    outSlot[6].hotkey = 'B';
    outSlot[6].clickable = true;
    outSlot[6].visible = true;
    //8번 : SupplyDepot 생성
    outSlot[7].commandID = eCommandID::SUPPLY_DEPOT;
    outSlot[7].iconKey = TEXT("ICON_SUPPLY_DEPOT");
    outSlot[7].hotkey = 'T';
    outSlot[7].clickable = true;
    outSlot[7].visible = true;
    //9번 : Barracks 생성
    outSlot[8].commandID = eCommandID::BARRACKS;
    outSlot[8].iconKey = TEXT("ICON_BARRACKS");
    outSlot[8].hotkey = 'G';
    outSlot[8].clickable = true;
    outSlot[8].visible = true;
}



