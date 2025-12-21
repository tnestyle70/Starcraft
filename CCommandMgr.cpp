#include "pch.h"
#include "CCommandMgr.h"
#include "CSelectionMgr.h"
#include "CNavMgr.h"
#include "CObjMgr.h"
#include "CScrollMgr.h"
#include "CTileMgr.h"
#include "CInputMgr.h"
#include "CAbstractFactory.h"

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

void CCommandMgr::Update()
{
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
    bool can = m_pGhost->CanPlace(mouseWorld);
    m_pGhost->SetCanPlace(can);
    
    //입력 처리
    if (CInputMgr::Get_Instance()->KeyDown(LEFT_MOUSE))
    {
        if (can)
        {
            //소유권 넘기기
            CBuilding* pBuilding = m_pGhost;
            m_pGhost = nullptr;
            //모드 정리 
            m_eMode = eCommandMode::NONE;
            m_pBuilder = nullptr;

            pBuilding->SetGhost(false);
            pBuilding->AppplyOccupy();
            pBuilding->SetBuilder(m_pBuilder);
            CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pBuilding);
            /*
            //실제 건물 생성
            CBuilding* pBuilding = CBuildingFactory::Create(m_ePlaceType);
            if (pBuilding)
            {
                //고스트와 동일 위치로 배치
                Vec2 pos = m_pGhost->Get_Pos();
                pBuilding->Set_Pos(pos.fX, pos.fY);
                //월드 위치를 cell로 변환해서 전달
                Vec2 cell = CTileMgr::Get_Instance()->WorldToCell(m_pGhost->Get_Pos());
                pBuilding->SetBuilder(m_pBuilder);
                //타일 점유 확정 짓기
                pBuilding->AppplyOccupy();
                //월드에 등록하기
                CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pBuilding);
                //건설 진행 애니메이션 스프라이트
            }
            //고스트 제거 + 모드 종료
            CancleBuilding();
            */
        }
    }
    if (CInputMgr::Get_Instance()->KeyDown(RIGHT_MOUSE))
    {
        CancleBuilding();
    }
}

void CCommandMgr::Render(HDC hDC)
{
    if (m_pGhost)
        m_pGhost->Render(hDC);
}

void CCommandMgr::IssueMove(Vec2& worldGoal)
{
	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
	if (selected.empty()) return;

    for (CUnit* pUnit : selected)
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
        pUnit->ClearOrders();
        pUnit->PushOrder(order);
    }
    return;
}
