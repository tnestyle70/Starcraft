#include "pch.h"
#include "CUnit.h"
#include "CTimeMgr.h"
#include "CNavMgr.h"
#include "CSelectionMgr.h"
#include "CInputMgr.h"
#include "CUIMgr.h"
#include "CBuilding.h"
#include "CTileMgr.h"
#include "CObjMgr.h"

CUnit::CUnit() : m_fSpeed(0.f), m_bDead(false), m_iHP(0), m_iMaxHP(0),
	m_bActiveOrder(false)
{
	m_vDir = { 1.f, 1.f };
	m_eState = eUnitState::IDLE;
	ZeroMemory(&m_tFrame, sizeof(FRAME));
}

CUnit::~CUnit()
{
}

int CUnit::Update()
{
	if (m_bDead)
		return DEAD;

	//HotKey Update
	UpdateHotKeys();

	if (m_OrderQ.empty())
	{
		m_bActiveOrder = false;
		m_eState = eUnitState::IDLE;
		__super::Update_Rect();
		return 0;
	}
	Order& order = m_OrderQ.front();
	// 오더 시작(1회)
	if (!m_bActiveOrder)
	{
		StartOrder(order);
		m_bActiveOrder = true;
	}

	bool finished = false;

	switch (order.eType)
	{
	case eOrderType::MOVE:
	case eOrderType::MOVE_AND_BUILD:
		finished = UpdateMove(order);
		break;
	case eOrderType::CONSTRUCTING:
	{
		m_eState = eUnitState::IDLE;
		finished = false; //자동 완료 방지
		//건물 완성 여부 확인
		if (order.pBuilding && order.pBuilding->IsComplete())
		{
			finished = true;
		}
		break;
	}
	case eOrderType::STOP:
		//바로 종료
		finished = true;
		break;
	default:
		// 나머지 오더는 아직 미구현이면 그냥 종료 처리하거나 HOLD로 두면 됨
		finished = true;
		break;
	}

	if (finished)
	{
		CompleteOrder();
	}

	__super::Update_Rect();

	return 0;
}

void CUnit::UpdateHotKeys()
{
	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();

	if (selected.empty())
		return;
	//다중 유닛 멈춤 처리
	if (CInputMgr::Get_Instance()->KeyDown(S_KEY))
	{
		for (auto* unit : selected)
		{
			Commandable* command = dynamic_cast<Commandable*>(unit);
			if (command)
			{
				CommandContext ctx{};
				command->ExecuteCommand(eCommandID::STOP, ctx);
			}
		}
		return;
	}

	//선택된 유닛이 없거나 CUnit 클래스가 아닐 경우 return
	if (selected.size() != 1)
		return;
	if (selected[0] != this)
		return;
	//Commandable 확인
	Commandable* command = dynamic_cast<Commandable*>(this);
	if (!command)
		return;
	//슬롯 정보
	vector<CommandSlot> slots;
	command->CommandCardSlot(slots);

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
			command->ExecuteCommand(slots[i].commandID, context);
			break;
		}
	}
}

bool CUnit::StartBuild(Order& order)
{
	if (!order.pBuilding)
		return false;
	CBuilding* pBuilding = order.pBuilding;

	//배치 가능 여부 재확인
	if (!pBuilding->CanPlace(order.dst))
	{
		delete pBuilding;
		order.pBuilding = nullptr;
		return false;
	}

	//최종 위치 조정
	int row, col;
	if (pBuilding->CalcSizeTopLeft(order.dst, row, col))
	{
		Vec2 centerPos = CTileMgr::Get_Instance()->CellToWorldCenter(
			row + pBuilding->GetHeight() * 0.5f,
			col + pBuilding->GetWidth() * 0.5f);
		pBuilding->Set_Pos(centerPos.fX, centerPos.fY);
	}

	//고스트 해제 + 리소스 차감 + 건설 시작
	pBuilding->SetGhost(false);
	//리소스 부족으로 실패
	if (pBuilding->IsGhost())
	{
		delete pBuilding;
		order.pBuilding = nullptr;
		return false;
	}
	//타일 점유
	pBuilding->AppplyOccupy();

	//맵에 건설 중인 상태로 추가
	CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pBuilding);

	return true;
}

void CUnit::CommandCardSlot(vector<CommandSlot>& outSlot)
{
	outSlot.clear();
	outSlot.resize(9);
	//미리 값 채우기
	for (int i = 0; i < 9; ++i)
	{
		outSlot[i].slotIndex = i;
		outSlot[i].commandID = eCommandID::NONE;
		outSlot[i].iconKey = TEXT("");
		outSlot[i].hotkey = 0;
		outSlot[i].clickable = false;
		outSlot[i].visible = false;
	}
	//3 * 3 기준
	//0번 슬롯
	outSlot[0].commandID = eCommandID::MOVE;
	outSlot[0].iconKey = TEXT("ICON_MOVE");
	outSlot[0].hotkey = 'M';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//1번 슬롯
	outSlot[1].commandID = eCommandID::STOP;
	outSlot[1].iconKey = TEXT("ICON_STOP");
	outSlot[1].hotkey = 'S';
	outSlot[1].clickable = true;
	outSlot[1].visible = true;
	//2번 슬롯
	outSlot[2].commandID = eCommandID::HOLD;
	outSlot[2].iconKey = TEXT("ICON_HOLD");
	outSlot[2].hotkey = 'H';
	outSlot[2].clickable = true;
	outSlot[2].visible = true;
	//3번 슬롯
	outSlot[3].commandID = eCommandID::PATROL;
	outSlot[3].iconKey = TEXT("ICON_PATROL");
	outSlot[3].hotkey = 'P';
	outSlot[3].clickable = true;
	outSlot[3].visible = true;
	//4번 슬롯
	outSlot[4].commandID = eCommandID::ATTACK;
	outSlot[4].iconKey = TEXT("ICON_ATTACK");
	outSlot[4].hotkey = 'A';
	outSlot[4].clickable = true;
	outSlot[4].visible = true;
}

bool CUnit::ExecuteCommand(eCommandID command, CommandContext& context)
{
	switch (command)
	{
	case eCommandID::STOP:
		CompleteOrder();
		break;
	default:
		break;
	}

	return false;
}

void CUnit::CompleteOrder()
{
	if (m_OrderQ.empty())
		return;
	//끝난 오더 처리
	Order& orderFinished = m_OrderQ.front();
	//특수 처리
	switch (orderFinished.eType)
	{
	case eOrderType::MOVE_AND_BUILD:
		//이동 완료 -> 건설 시작(Constructing)
		if (!StartBuild(orderFinished))
		{
			m_OrderQ.pop_front();
			m_bActiveOrder = false;
			return;
		}
		orderFinished.eType = eOrderType::CONSTRUCTING;
		m_bActiveOrder = false;
		return;
	case eOrderType::CONSTRUCTING:
		//건설 완료 검증
		if (orderFinished.pBuilding && orderFinished.pBuilding->IsComplete())
		{
			//건설이 완료될 경우 오더 종료
			orderFinished.pBuilding = nullptr;
		}
		else
		{
			return;
		}
		break;
	}
	//오더 제거 
	m_OrderQ.pop_front();
	m_bActiveOrder = false;
	if (m_OrderQ.empty())
	{
		m_eState = eUnitState::IDLE;
	}
}

void CUnit::ClearOrder()
{
	//오더들의 포인터 정리
	for (auto& order : m_OrderQ)
	{
		delete order.pBuilding;
		order.pBuilding = nullptr;
	}
	m_OrderQ.clear();
	m_bActiveOrder = false;
	m_eState = eUnitState::IDLE;
}

bool CUnit::UpdateMove(Order& order)
{
	if (order.path.empty())
		return true;

	if (order.iPathIndex >= (int)order.path.size())
		return true;

	//현재 목표가 되는 웨이포인트
	Vec2 target = (order.iPathIndex < order.path.size()) 
		? order.path[order.iPathIndex] : order.dst;
	Vec2 diff{ target.fX - m_tInfo.fX, target.fY - m_tInfo.fY };
	float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
	float fArriveEps = 6.f;

	if (dist <= fArriveEps)
	{
		order.iPathIndex++;
		//경로 끝 -> 오더 완료
		if (order.iPathIndex >= order.path.size())
			return true;
		return false;
	}
	Vec2 dir = { diff.fX / dist, diff.fY / dist };
	//이동 방향 갱신
	m_vDir = dir;

	float dt = CTimeMgr::Get_Instance()->GetDT();

	m_tInfo.fX += dir.fX * dt * m_fSpeed;
	m_tInfo.fY += dir.fY * dt * m_fSpeed;

	return false;
}

void CUnit::StartOrder(Order& order)
{
	switch (order.eType)
	{
	case eOrderType::MOVE:
	case eOrderType::MOVE_AND_BUILD:
	{
		m_eState = eUnitState::MOVE;

		// 경로가 없으면 목적지 하나라도 넣어준다
		if (order.path.empty())
		{
			Vec2 start = { m_tInfo.fX, m_tInfo.fY };
			order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, order.dst);
			//경로 못 구했을 경우 직선 이동
			if (order.path.empty())
			{
				order.path.push_back(order.dst);
			}
		}
		order.iPathIndex = 0;
	}
	default:
		break;
	}
}

int CUnit::DirTo16WayIndex(Vec2& vDir)
{
	// (중요) 화면 좌표계가 y 아래로 증가면, 수학 좌표계로 맞추려면 -y를 쓰는 게 안전
	float ang = atan2(-vDir.fY, vDir.fX); // -PI ~ PI

	float step = PI / 8.f;        // 22.5도
	int idx = (int)floorf((ang + (PI / 16.f)) / step); // 가까운 방향 반올림
	idx = (idx % 16 + 16) % 16;   // 0~15
	return idx;
}

RECT CUnit::GetWorldRect() const
{
	RECT rc{};

	rc.left = (LONG)(m_tInfo.fX - m_tInfo.fCX * 0.5f);
	rc.top = (LONG)(m_tInfo.fY - m_tInfo.fCY * 0.5f);
	rc.right = (LONG)(m_tInfo.fX + m_tInfo.fCX * 0.5f);
	rc.bottom = (LONG)(m_tInfo.fY + m_tInfo.fCY * 0.5f);

	return rc;
}