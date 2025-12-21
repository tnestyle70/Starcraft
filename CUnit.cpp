#include "pch.h"
#include "CUnit.h"
#include "CTimeMgr.h"
#include "CNavMgr.h"

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
		finished = UpdateMove(order);
		break;
	default:
		// 나머지 오더는 아직 미구현이면 그냥 종료 처리하거나 HOLD로 두면 됨
		finished = true;
		break;
	}

	if (finished)
	{
		PopOrder();
	}

	__super::Update_Rect();

	return 0;
}

void CUnit::CommandCardSlot(vector<CommandSlot>& outSlot)
{
	outSlot.clear();
	//3 * 3 기준
	//0번 슬롯
	CommandSlot s;
	s.slotIndex = 0; 
	s.commandID = eCommandID::MOVE;
	s.iconKey = TEXT("ICON_MOVE"); // TEXT 안에는 L 붙이지 말기
	s.hotkey = 'M';
	s.clickable = true;
	s.visible = true; //칸 없음, 보이되 비활성(잠금), 동작 가능한 버튼 상태 나누기
	outSlot.push_back(s);
	//1번 슬롯
	s.slotIndex = 0;
	s.commandID = eCommandID::STOP;
	s.iconKey = TEXT("ICON_STOP");
	s.hotkey = 'S';
	s.clickable = true;
	s.visible = true;
	outSlot.push_back(s);
	//2번 슬롯
	s.slotIndex = 0;
	s.commandID = eCommandID::HOLD;
	s.iconKey = TEXT("ICON_HOLD");
	s.hotkey = 'S';
	s.clickable = true;
	s.visible = true;
	outSlot.push_back(s);
	//3번 슬롯
	s.slotIndex = 0;
	s.commandID = eCommandID::PATROL;
	s.iconKey = TEXT("ICON_PATROL");
	s.hotkey = 'S';
	s.clickable = true;
	s.visible = true;
	outSlot.push_back(s);
	//4번 슬롯
	s.slotIndex = 0;
	s.commandID = eCommandID::ATTACK;
	s.iconKey = TEXT("ICON_ATTACKMOVE");
	s.hotkey = 'S';
	s.clickable = true;
	s.visible = true;
	outSlot.push_back(s);
}

bool CUnit::ExecuteCommand(eCommandID command, CommandContext& context)
{
	switch (command)
	{
	case eCommandID::MOVE:
		IssueMove();
		break;
	case eCommandID::STOP:
		IssueStop();
		break;
	case eCommandID::HOLD:
		IssueHold();
		break;
	case eCommandID::PATROL:
		IssuePatrol();
		break;
	case eCommandID::ATTACK:
		IssueAttackMove();
		break;
	default:
		break;
	}

	return false;
}

void CUnit::IssueMove()
{
	/*
	//직접 이동 호출
	ClearOrders();

	Order order;
	order.eType = eOrderType::MOVE;
	order.dst = worldTarget;
	order.path.clear();

	//A* 경로 생성
	Vec2 start{ m_tInfo.fX, m_tInfo.fY };
	order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, order.dst);
	//경로 없으면 직선 이동
	if (order.path.empty())
	{
		order.path.push_back(order.dst);
	}
	PushOrder(order);
	*/
}

void CUnit::IssueStop()
{
}

void CUnit::IssueHold()
{
}

void CUnit::IssuePatrol()
{
}

void CUnit::IssueAttackMove()
{
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
	//도착 판정 기준
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

void CUnit::PopOrder()
{
	//현재 오더 제거 -> 오더가 끝났을 경우 상태 정리 -> 다음 오더가 있을 경우 즉시 시작
	if (m_OrderQ.empty()) return;

	//현재 오더 가져오기
	Order finished = m_OrderQ.front();
	m_OrderQ.pop_front();
	//오더 타입별 정리
	switch (finished.eType)
	{
	case eOrderType::MOVE:
	{
		m_vecPath.clear();
		m_iPathIndex = 0;
		break;
	}
	default:
		break;
	}
	//다음 오더 처리
	if (m_OrderQ.empty())
	{
		m_eState = eUnitState::IDLE;
		return;
	}
	StartOrder(m_OrderQ.front());
}

void CUnit::StartOrder(Order& order)
{
	switch (order.eType)
	{
	case eOrderType::MOVE:
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