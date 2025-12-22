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

CSCV::CSCV() : m_pGhostBuilding(nullptr), m_bBuildingMode(false)
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

    //핫키 업데이트
    UpdateHotKeys();
    //건물 모드 업데이트
    UpdateBuilding();

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

    if (m_pGhostBuilding && m_bBuildingMode)
        m_pGhostBuilding->Render(hDC);
}

void CSCV::Release()
{
    //고스트 건물 해제 
    if (m_pGhostBuilding)
    {
        delete m_pGhostBuilding;
        m_pGhostBuilding = nullptr;
    }
}

void CSCV::UpdateHotKeys()
{
    //SCV 유닛 하나만 선택되었을 경우 실행
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    if (selected.size() != 1)
        return;
    //선택된 객체가 this인지 확인
    if (selected[0] != this)
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

void CSCV::UpdateBuilding()
{
    //건설 모드가 아니면 리턴
    if (!m_bBuildingMode || !m_pGhostBuilding)
        return;
    //마우스 월드 좌표
    Vec2 worldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    //고스트 건물 위치 업데이트
    m_pGhostBuilding->Set_Pos(worldMouse.fX, worldMouse.fY);
    //타일 좌표로 변환
    int row, col;
    if (m_pGhostBuilding->CalcSizeTopLeft(worldMouse, row, col))
    {
        m_pGhostBuilding->SetPlace(row, col);
    }
    //고스트 건물 업데이트(배치 가능 여부 체크)
    m_pGhostBuilding->Update();
    //ESC키, 우클릭으로 건설 취소
    if (CInputMgr::Get_Instance()->KeyDownVK(VK_ESCAPE))
    {
        CancelBuilding();
        return;
    }
    if (CInputMgr::Get_Instance()->KeyDown(RIGHT_MOUSE))
    {
        CancelBuilding();
        return;
    }
    //좌클릭 건물 배치
    if (CInputMgr::Get_Instance()->KeyDown(LEFT_MOUSE))
    {
        if (m_pGhostBuilding->CanPlace(worldMouse))
        {
            //SCV 이동 로직 수행하고 PlaceBuilding 진행
            PlaceBuilding(worldMouse);
        }
    }
}

bool CSCV::ExecuteCommand(eCommandID command, CommandContext& context)
{
    ResourceCost cost{};

    switch (command)
    {
    case eCommandID::COMMAND_CENTER:
        //커맨드 센터 빌딩 시작
        StartBuildMode(eBuildingType::COMMAND_CENTER);
        break;
    case eCommandID::SUPPLY_DEPOT:
        //보급고 빌딩 시작
        StartBuildMode(eBuildingType::SUPPLY_DEPOT);
        break;
    case eCommandID::BARRACKS:
        //배럭 빌딩 시작
        StartBuildMode(eBuildingType::BARRACK);
        break;
    default:
        break;
    }

    return false;
}

void CSCV::CommandCardSlot(vector<CommandSlot>& outSlot)
{
    // 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
    CUnit::CommandCardSlot(outSlot);
    //건설 모드일 경우 커맨드 카드 표시X
    if (m_bBuildingMode)
        return;
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

void CSCV::StartBuildMode(eBuildingType buildingType)
{
    //이미 건설 모드일 경우 취소
    if (m_pGhostBuilding)
        return;
    //건물 타입에 따른 고스트 건물 생성(알파 비트맵 마스크 씌우기)
    CBuilding* pBuilding = nullptr;
    switch (buildingType)
    {
    case eBuildingType::COMMAND_CENTER:
        pBuilding = new CCommandCenter();
        break;
    case eBuildingType::BARRACK:
        pBuilding = new CBarracks();
        break;
    case eBuildingType::FACTORY:
        pBuilding = new CFactory();
        break;
    case eBuildingType::STARPORT:
        pBuilding = new CStarport();
        break;
    case eBuildingType::SUPPLY_DEPOT:
        pBuilding = new CSupplyDepot();
        break;
    default:
        break;
    }
    if (!pBuilding)
        return;
    //건물 초기화
    pBuilding->Initialize();
    //고스트 모드로 설정
    pBuilding->SetGhost(true);
    //빌더 설정
    pBuilding->SetBuilder(this);
    //마우스 위치로 이동
    Vec2 worldMouse = CInputMgr::Get_Instance()->GetWorldMouse();
    pBuilding->Set_Pos(worldMouse.fX, worldMouse.fY);
    //고스트 건물 설정
    m_pGhostBuilding = pBuilding;
    m_bBuildingMode = true;
}

void CSCV::CancelBuilding()
{
    //고스트 건물 삭제
    if (m_pGhostBuilding)
    {
        delete m_pGhostBuilding;
        m_pGhostBuilding = nullptr;
    }
    m_bBuildingMode = false;
}

void CSCV::PlaceBuilding(const Vec2& worldPos)
{
    if (!m_pGhostBuilding)
        return;
    //목적지에 도착을 했을 경우 건물 빌드하도록 설계
    //해당 위치로 이동
    Vec2 start = Get_Pos();
    //AStart를 통해 계산한 위치를 반환 받아서 CUnit 쪽에 넘겨주기 
    Vec2 world = worldPos;
    vector<Vec2> path = CNavMgr::Get_Instance()->RequestPathWorld(start, world);
    Order order;
    order.eType = eOrderType::MOVE_AND_BUILD;
    order.dst = worldPos;
    order.path = move(path);
    order.pBuilding = m_pGhostBuilding;
    order.iPathIndex = 0;
    if (order.path.empty())
    {
        order.path.push_back(worldPos);
        order.iPathIndex = 0;
    }
    dynamic_cast<CUnit*>(this)->PushOrder(order);
    //도착시에 건물 짓기
    //소유권 order의 pBuilding에 넘기고 해제
    m_pGhostBuilding = nullptr;
    m_bBuildingMode = false;
}

void CSCV::FinalizeBuild(Order& order)
{
    if (!order.pBuilding)
        return;
    CBuilding* pBuilding = order.pBuilding;

    pBuilding->SetGhost(false);
    //고스트 모드 확인 
    if (pBuilding->IsGhost())
    {
        delete pBuilding;
        order.pBuilding = nullptr;
        return;
    }
    //타일 점유
    pBuilding->AppplyOccupy();
    //최종 배치
    int row, col;
    if (pBuilding->CalcSizeTopLeft(order.dst, row, col))
    {
        Vec2 centerPos = CTileMgr::Get_Instance()->CellToWorldCenter(
            row + pBuilding->GetHeight() * 0.5f,
            col + pBuilding->GetWidth() * 0.5f);
        pBuilding->Set_Pos(centerPos.fX, centerPos.fY);
    }
    CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pBuilding);
    //소유권 이전(포인터 정리)
    order.pBuilding = nullptr;
}
