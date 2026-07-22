#define NOMINMAX
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
#include "CMainUI.h"
#include "CFogMgr.h"
#include "CScrollMgr.h"
#include <string>
#include "CBunker.h"
#include "CShuttle.h"

CUnit::CUnit() : m_fSpeed(0.f), m_iHP(0), m_iMaxHP(0), m_iMP(0), m_iMaxMP(0), m_iShield(0), m_iMaxShield(0),
	m_bActiveOrder(false), m_acceleration({0.f, 0.f}), m_currentWayPointIndex(0),
	m_iAttackDamage(0), m_fAttackRange(0.f), m_fAttackSpeed(0.f), m_dwLastAttack(0),
	m_iAttackFrameStart(0), m_iAttackFrameEnd(0), m_iSightRange(14)
	,m_bFiring(false), m_iFireFrame(0)
{
	m_vDir = { 1.f, 1.f };
	m_eState = eUnitState::IDLE;
	ZeroMemory(&m_tFrame, sizeof(FRAME));
	//Steering 파라미터 초기화
	m_steeringParams = SteeringParams();
	m_velocity = { 0.f,0.f };
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

	//유닛 정보 업데이트
	UpdateUnitUIInfo();

	if (m_OrderQ.empty())
	{
		if (m_eState == eUnitState::GARRISONED)
		{
			return NOEVENT;
			//UpdateGarrisoned();
		}
		else if (m_bBirthing&&m_eState == eUnitState::BIRTH)
		{
			return NOEVENT;
		}
		else
		{
			m_bActiveOrder = false;
			m_eState = eUnitState::IDLE;
		}
		return NOEVENT;
	}

	Order& order = m_OrderQ.front(); //CommandMgr 쪽에서 받은 AttackMove 오더를 바탕으로 A땅 공격 시작!

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
	case eOrderType::GATHER:
		finished = UpdateGather(order);
		break;
	case eOrderType::RETURN_RESOURCE:
		finished = UpdateReturn(order);
		break;
	case eOrderType::ATTACK:
		finished = UpdateRAttack(order);
		break;
	case eOrderType::ATTACK_MOVE:
		finished = UpdateAttackMove(order); 
		break;
	case eOrderType::CONSTRUCTING:
	{
		finished = UpdateConstructing(order);
		break;
	}
	case eOrderType::ENTER_BUNKER:
	{
		finished = EnterBunker(order);
		break;
	}
	case eOrderType::ENTER_SHUTTLE:
	{
		finished = EnterShuttle(order);
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

void CUnit::Render(HDC hDC)
{
	//전장의 안개
	Vec2 pos = Get_Pos();
	int row, col;
	if (!CTileMgr::Get_Instance()->WorldToCell(pos, row, col))
		return;

	eFogState fogState = CFogMgr::Get_Instance()->GetFogState(row, col);

	// UNKNOWN 영역이면 렌더링 안 함
	if (fogState == eFogState::UNKNOWN)
		return;

	// EXPLORED 영역이면 적 유닛은 안 보임
	//if (fogState == eFogState::EXPLORED && GetTeam() != eTeam::PLAYER)
	//	return;

	// ========== 화면 밖 체크 (최적화) ==========
	float scrX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float scrY = CScrollMgr::Get_Instance()->Get_ScrollY();

	if (m_tInfo.fX + m_tInfo.fCX * 0.5f < scrX ||
		m_tInfo.fX - m_tInfo.fCX * 0.5f > scrX + WINCX ||
		m_tInfo.fY + m_tInfo.fCY * 0.5f < scrY ||
		m_tInfo.fY - m_tInfo.fCY * 0.5f > scrY + WINCY)
	{
		return;
	}
}

void CUnit::StartOrder(Order& order)
{
	switch (order.eType)
	{
	case eOrderType::MOVE:
	case eOrderType::MOVE_AND_BUILD:
	{
		//if (m_eTeamType == eTeamType::ENEMY)
		//	return;

		m_eState = eUnitState::MOVE; //애니메이션용 UnitState
		if (order.path.empty()) //A star 경로 생성
		{
			Vec2 start = { m_tInfo.fX, m_tInfo.fY };
			order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, order.dst);
			//경로 못 구했을 경우 직선 이동
			if (order.path.empty())
			{
				order.path.push_back(order.dst); //RAttack과 동일한 직선 이동
			}
		}
		order.iPathIndex = 0;
		break;
	}
	case eOrderType::GATHER:
	{
		m_eState = eUnitState::MOVE;
		//미네랄 위치로 path 설정
		if (order.pTarget)
		{
			Vec2 start = { m_tInfo.fX, m_tInfo.fY };
			Vec2 resourcePos = order.pTarget->Get_Pos();
			order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, resourcePos);
			if (order.path.empty())
			{
				order.path.push_back(resourcePos);
			}
			order.iPathIndex = 0;
		}
		break;
	}
	case eOrderType::RETURN_RESOURCE:
	{
		m_eState = eUnitState::MOVE;
		//가장 가까운 커맨드 센터, 넥서스 위치로 path 설정
		if (order.pTarget)
		{
			Vec2 start = { m_tInfo.fX, m_tInfo.fY };
			Vec2 targetPos = order.pTarget->Get_Pos();
			order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, targetPos);
			if (order.path.empty())
			{
				order.path.push_back(targetPos);
			}
			order.iPathIndex = 0;
		}
		break;
	}
	case eOrderType::ATTACK:
	{
		//CommandMgr에서 타겟 찾아서 반환한 상태 -> 해당 유닛이 공격 범위 내에 있으면 공격 아니면 이동
		//책임과 분리 -> StartOrder에서는 애니메이션 프레임용 UnitState만 Move로 변경해주고 실질적인 이동은 UpdateRAttack에서 담당
		m_eState = eUnitState::MOVE;
		break;
	}
	case eOrderType::ATTACK_MOVE:
	{
		//CommandMgr에서 dst 받아서 A Star로 경로 생성 -> 경로 따라가면서 사거리 내에 있는 적 처치, 처치 후 계속 이동 -> AttackMove 상태유지
		m_eState = eUnitState::MOVE; //UnitState도 Move로 설정해줘서 프레임 업데이트
		if (order.path.empty()) 
		{
			Vec2 start = { m_tInfo.fX, m_tInfo.fY }; //A Star 경로 생성
			order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, order.dst);
			if (order.path.empty()) //경로가 없으면 직선 이동! RAttack과 동일한 방식
			{
				order.path.push_back(order.dst);
			}
		}
		order.iPathIndex = 0;
		break;
	}
	default:
		break;
	}
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
			//건설이 완료될 경우 오더 종료
			orderFinished.pBuilding = nullptr;
		else
			return;
		break;
	}
	if (m_bAttackMove)
	{
		return; //AttackMove상태면 오더 제거하지 않고 return
	}
	//오더 제거
	m_OrderQ.pop_front();
	m_bActiveOrder = false;
	if (m_OrderQ.empty())
	{
		m_eState = eUnitState::IDLE;
	}
}

bool CUnit::UpdateMove(Order& order)
{
	//Move, 이동 하나만에 대한 처리 담당
	if (order.path.empty()) //path empty는 도착을 의미함
	{
		m_eState = eUnitState::IDLE;
		return true; //Move 완료
	}
	//경로 인덱스 범위 추가 안전 체크 로직
	if (order.iPathIndex >= (int)order.path.size())
	{
		m_eState = eUnitState::IDLE;
		return true; 
	}
	// 현재 목표가 되는 웨이포인트
	Vec2 target = order.path[order.iPathIndex]; //현재 목표로 설정한 path {}{}{}{} 좌표들 중에서 몇 번 째인지, iPathIndex
	Vec2 current = { m_tInfo.fX, m_tInfo.fY };
	// vector<Vec2> path {}{}{}{}중에서 iPathIndex Vec2 위치 기준 거리 계산
	Vec2 diff = { target.fX - current.fX, target.fY - current.fY };
	float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
	// 도착 체크
	float fArriveEps = 6.f;

	if (dist <= fArriveEps) //Move 완료, 도착!
	{
		order.iPathIndex++; //다음 경로로 이동하기 위해 PathIndex 증가 시키기!
		// 경로 끝 -> 오더 완료 - pathIndex가 vector<Vec2> path {}{}{}의 사이즈보다 크거나 같은 경우는 도착을 의미함
		if (order.iPathIndex >= (int)order.path.size())
		{
			// 최종 도착 -> 정지
			m_eState = eUnitState::IDLE;
			return true;
		}
		// 다음 웨이포인트로 이동
		return false; //UpdateMove 상태 유지하면서 계속 이동!
	}
	// 현재 위치와 vector<Vec2> path {}{}{} ipathindex의 위치를 통한 방향 * 속도만큼 이동!
	Vec2 dir = { diff.fX / dist, diff.fY / dist };
	// 방향 갱신
	m_vDir = dir;
	// 위치 업데이트
	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_tInfo.fX += dir.fX * dt * m_fSpeed;
	m_tInfo.fY += dir.fY * dt * m_fSpeed;
	//목적지 도달 못했으므로 계속 이동!
	return false;
}

bool CUnit::UpdateAttack(Order& order)
{
	//AttackMove의 흐름, 타겟 확인 -> 타겟 없으면 설정한 웨이 포인트로 이동 return false로 계속 웨이포인트로 이동
	//타겟 확인 -> 타겟 있으면 RAttack과 동일하게 사거리 내 타겟 공격 -> 타겟 죽었고 웨이 포인트가 남아있으면 계속 이동
	//타겟 없거나 죽었고 웨이 포인트도 없으면 AttackMove Order 유지하면서 계속 사거리 내 적 탐색(터렛 같은 느낌)
	
	//현재 위치 기준 타겟 확인
	m_pTarget = FindNearestEnemy(m_fAttackRange);
	if (m_pTarget)
	{
		order.eType = eOrderType::ATTACK;
		order.pTarget = m_pTarget;
		return false; //dst, path, ipathindex에 대한 정보는 유지가 핵심
	}
	return false;
	/*
	if (m_pTarget) //타겟이 존재할 경우 타겟 공격, 타겟이 사거리에서 멀어지면 따라가야하므로 Order Attack 생성
	{
		Order orderAttack;
		orderAttack.eType = eOrderType::ATTACK;
		orderAttack.pTarget = m_pTarget;
		orderAttack.dst = m_pTarget->Get_Pos();
		orderAttack.path.clear();
		orderAttack.iPathIndex = 0;

		this->SetAttackMove(false); //A땅 공격 true
		this->ClearOrder();
		this->PushOrder(orderAttack);

		//여기서 종료를 해버리면 CompleteOrder로 인해서 오더가 비워져버림
		return true; //Attack 오더 push하고 오더 종료
	}
	else //타겟이 존재하지 않을 경우 AttackMove 계속해서 호출
	{
		Order orderAttackMove;
		orderAttackMove.eType = eOrderType::ATTACK_MOVE;
		orderAttackMove.dst = order.dst; //기존 오더의 목적지 설정
		orderAttackMove.pTarget = nullptr;
		orderAttackMove.path.clear();
		orderAttackMove.iPathIndex = 0;
		//오더 비우고 A땅 이동 오더 실행!
		this->SetAttackMove(true); //A땅 공격 true
		this->ClearOrder();
		this->PushOrder(orderAttackMove);

		return true; //AttackMove 오더 push 후 종료
	}
	*/
}

bool CUnit::UpdateAttackMove(Order& order)
{
	if (m_bAttackMove)
	{
		m_pTarget = FindNearestEnemy(m_fAttackRange);
		if (m_pTarget)
		{
			if (!m_pTarget->IsSelectable())
				return false;
			order.eType = eOrderType::ATTACK;
			order.pTarget = m_pTarget;
			return false; //dst, path, ipathindex에 대한 정보는 유지가 핵심
		}
	}

	if (order.path.empty())
	{
		m_velocity.fX = 0.f;
		m_velocity.fY = 0.f;
		m_eState = eUnitState::IDLE;
		return false;  // A땅은 대기
	}
	if (order.iPathIndex >= (int)order.path.size())
	{
		m_velocity.fX = 0.f;
		m_velocity.fY = 0.f;
		m_eState = eUnitState::IDLE;
		return false;  // A땅은 도착해도 계속
	}

	Vec2 target = order.path[order.iPathIndex];
	Vec2 current = { m_tInfo.fX, m_tInfo.fY };
	Vec2 diff = { target.fX - current.fX, target.fY - current.fY };
	float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);

	if (dist <= 6.f)
	{
		order.iPathIndex++;
		if (order.iPathIndex >= (int)order.path.size())
		{
			m_velocity.fX = 0.f;
			m_velocity.fY = 0.f;
			m_eState = eUnitState::IDLE;
			return false;  // 도착
		}
		return false;
	}

	// 이동!
	Vec2 dir = { diff.fX / dist, diff.fY / dist };
	m_vDir = dir;
	m_velocity.fX = dir.fX * m_fSpeed;
	m_velocity.fY = dir.fY * m_fSpeed;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_tInfo.fX += dir.fX * dt * m_fSpeed;
	m_tInfo.fY += dir.fY * dt * m_fSpeed;

	return false;
}

//Unit에서 m_pTarget만 업데이트해주고 Idle, attack에서 공격 속도 체크와 bullet 발사만 수행
bool CUnit::UpdateRAttack(Order& order) 
{
	//타겟이 죽었거나 사라진 경우
	if (!order.pTarget || order.pTarget->IsDead())
	{
		if (m_bAttackMove)
		{
			order.eType = eOrderType::ATTACK_MOVE;
			order.pTarget = nullptr;
			return false;
		}
		else
		{
			m_eState = eUnitState::IDLE;
			return false; //오더 완료
		}
	}

	//공격 중이 아닐 때만 타겟 설정
	if (!m_bAttack && order.pTarget)
	{
		if (!order.pTarget->IsSelectable())
			return false;
		m_pTarget = order.pTarget;
	}

	float fDT = CTimeMgr::Get_Instance()->GetDT();

	Vec2 targetPos = order.pTarget->Get_Pos();
	Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
	//타겟까지의 거리
	Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
	float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
	//공격 사거리 체크
	if (dist <= m_fAttackRange)
	{
		//Move -> Idle로 상태를 변환해서 공격 타이머가 돌 수 있도록 설정한다.
		if (m_eState == eUnitState::MOVE)
		{
			m_eState = eUnitState::IDLE;
		}
		//타겟 방향 보기
		if (dist > 0.1f)
		{
			m_vDir = { diff.fX / dist, diff.fY / dist };
		}
		return false;
	}
	else
	{
		//타겟이 사거리 내에 존재하지 않을 경우 이동
		Vec2 dir = { diff.fX / dist, diff.fY / dist };
		m_vDir = dir;
		float fDT = CTimeMgr::Get_Instance()->GetDT();
		m_tInfo.fX += dir.fX * fDT * m_fSpeed;
		m_tInfo.fY += dir.fY * fDT * m_fSpeed;
		return false;
	}
}

void CUnit::UpdateHotKeys()
{
	// CONSTRUCTING 상태에서는 일반 커맨드 사용 불가
	if (m_eState == eUnitState::CONSTRUCTING)
		return;

	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();

	if (selected.empty()) //선택되지 않았으면 return 
		return;
	//selected가 unit이 아니면 HotKey 업데이트하지 않음!
	for (auto* obj : selected)
	{
		CUnit* pUnit = dynamic_cast<CUnit*>(obj);
		if (!pUnit)
			return;
	}

	//첫번째 선택된 유닛만 UI 업데이트
	if (selected[0] != this)
		return;

	//Commandable 확인
	Commandable* command = dynamic_cast<Commandable*>(this);
	if (!command)
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

	//다중 유닛 선택시 타입이 다르면 return
	if (selected.size() > 1)
	{
		eUnitType firstType = static_cast<CUnit*>(selected.front())->Get_UnitType();
		for (auto* obj : selected)
		{
			if (static_cast<CUnit*>(obj)->Get_UnitType() != firstType)
				return;  // 다른 타입 섞여있으면 커맨드 슬롯 안 보임
		}
	}

	//커맨드 카드 슬롯 정보 띄우기
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
			//선택된 유닛 전부 명령 실행
			for (auto* obj : selected)
			{
				Commandable* cmd = dynamic_cast<Commandable*>(obj);
				if (cmd)
				{
					CommandContext context{};
					cmd->ExecuteCommand(slots[i].commandID, context);
				}
			}
			break;
		}
	}
}

bool CUnit::StartBuild(Order& order)
{
	if (!order.pBuilding)
		return false;

	CBuilding* pBuilding = order.pBuilding;
	//건물에게 건설 시작 요청(건물이 알아서 설계하도록 한다.)
	if (!pBuilding->StartConstruct(order.dst))
	{
		delete pBuilding; //건설 불가능할 경우 pBuilding과 order.pBuilding 초기화
		order.pBuilding = nullptr;
		return false;
	}
	//건설 중인 상태 -> 애니메이션으로 상태 나누기!!!!!!!!!!
	//CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pBuilding);
	return true;
}

bool CUnit::UpdateConstructing(Order& order)
{
	return false;
}

bool CUnit::UpdateGather(Order& order)
{
	//SCV에서 구현 Unit은 베이스 클래스 
	return false;
}

bool CUnit::UpdateReturn(Order& order)
{
	//SCV에서 구현 Unit은 베이스 클래스
	return false;
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

void CUnit::ClearOrder()
{
	//오더들의 포인터 정리
	for (auto& order : m_OrderQ)
	{
		if (order.pBuilding)
		{
			//아직 월드에 올라가기 전(이동 + 빌드 대기) 고스트만 유닛이 소유한 것으로 판단
			if (order.eType == eOrderType::MOVE_AND_BUILD && order.pBuilding->IsGhost())
			{
				delete order.pBuilding;
			}
		}
		//CONSTRUCTING은 월드에 이미 올라가 있으므로 delete하면 안 됨
		order.pBuilding = nullptr;
		order.pTarget = nullptr;
	}
	m_OrderQ.clear();
	m_bActiveOrder = false;
	m_eState = eUnitState::IDLE;
}

void CUnit::TakeDamage(int iAttackDamage)
{
	eRaceType type = CGameDataMgr::Get_Instance()->Get_PlayerRace();
	if (type == eRaceType::RACE_PROTOSS)
	{
		//쉴드,HP 로직 -> 쉴드가 0이상인 경우 쉴드량과 damage를 비교해서 오버면 HP까지 까이도록 아니면 쉴드 흡수
		if (m_iShield > 0)
		{
			//쉴드가 데미지를 다 막을 수 있는 경우
			if (m_iShield >= iAttackDamage)
			{
				m_iShield -= iAttackDamage;
			}
			else
			{
				int remainDamage = iAttackDamage - m_iShield;
				m_iShield = 0;
				m_iHP -= remainDamage;
			}
		}
		else
		{
			m_iHP -= iAttackDamage;
		}
		if (m_iHP < 0) m_iHP = 0;
		if (m_iHP == 0) Set_Dead();
	}
	else
	{
		m_iHP -= iAttackDamage;
		if (m_iHP < 0) m_iHP = 0;
		if (m_iHP == 0) Set_Dead();
	}
}

void CUnit::RefillShield(int iRefill)
{
	if (m_iMaxShield == 0)
		return;
	m_iShield += iRefill;
	if (m_iShield >= m_iMaxShield)
		m_iShield = m_iMaxShield;
}

void CUnit::UpdateGarrisoned()
{
}

bool CUnit::EnterBunker(Order& order)
{
	float dx = order.dst.fX - m_tInfo.fX;
	float dy = order.dst.fY - m_tInfo.fY;
	float dist = sqrtf(dx * dx + dy * dy);
	if (dist < 100.f) //벙커에 도착했을 경우
	{
		CBunker* pBunker = dynamic_cast<CBunker*>(order.pTarget);
		if (pBunker)
		{
			pBunker->LoadUnit(this);
			m_OrderQ.pop_front();
			return true;
		}
	}
	else
		return false;
}

bool CUnit::EnterShuttle(Order& order)
{
	float dx = order.dst.fX - m_tInfo.fX;
	float dy = order.dst.fY - m_tInfo.fY;
	float dist = sqrtf(dx * dx + dy * dy);
	if (dist < 100.f) //벙커에 도착했을 경우
	{
		CShuttle* pShuttle = dynamic_cast<CShuttle*>(order.pTarget);
		if (pShuttle)
		{
			pShuttle->LoadUnit(this);
			m_OrderQ.pop_front();
			return true;
		}
	}
	else
		return false;
}

CObj* CUnit::FindNearestEnemy(float searchRadius)
{
	Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };
	list<CObj*>& enemyList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_ENEMY);
	CObj* pNear = nullptr;
	float minDist = searchRadius;
	for (auto* pObj : enemyList)
	{
		if (!pObj || pObj->IsDead())
			continue;
		Vec2 enemyPos = pObj->Get_Pos();
		Vec2 diff = { enemyPos.fX - m_tInfo.fX, enemyPos.fY - m_tInfo.fY };
		float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
		if (dist < minDist)
		{
			minDist = dist;
			pNear = pObj;
		}
	}
	return pNear;
}

void CUnit::UpdateDead() //각기 다른 Dead Effect와 사운드 재생하도록 가상함수로 선언 
{
}

void CUnit::RemoveOrdersWithTarget(CObj* pTarget)
{
	//현재 오더가 죽은 타겟을 가르킬 경우 해당 타겟과의 포인터 연결 끊기
	if (!m_OrderQ.empty())
	{
		Order& curOrder = m_OrderQ.front();
		if (curOrder.pTarget == pTarget)
		{
			if (m_bAttackMove) //AttackMove 상태에서 타겟이 죽었을 경우 오더 지우지 않고 타입만 변경
			{
				m_pTarget = nullptr;
				curOrder.pTarget = nullptr;
				curOrder.eType = eOrderType::ATTACK_MOVE;
				return;
			}
			else
			{
				////RAttack도 타겟 정리
				//m_pTarget = nullptr;
				//curOrder.pTarget = nullptr;
				//오더 클리어
				m_OrderQ.pop_front();
				m_eState = eUnitState::IDLE;
				//HeadFrame 리셋
				m_iFireFrame = 0;
				m_bFiring = false;
			}
		}
	}
	//대기 중인 모든 오더에서도 제거하기
	auto it = m_OrderQ.begin();
	while (it != m_OrderQ.end())
	{
		if (it->pTarget == pTarget)
		{
			it = m_OrderQ.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CUnit::Heal(int amount)
{
	if (m_bDead || m_eState == eUnitState::DIE)
		return;
	m_iHP += amount;
	//maxHP 제한 
	if (m_iHP > m_iMaxHP)
		m_iHP = m_iMaxHP;
}

CObj* CUnit::FindNearestCommandCenter()
{
	Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };
	list<CObj*> buildingList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_BUILDING);
	CObj* pNear = nullptr;
	float minDist = FLT_MAX;

	for (auto* pObj : buildingList)
	{
		if (!pObj || pObj->IsDead())
			continue;
		//CC인지 확인
		CBuilding* pBuilding = dynamic_cast<CBuilding*>(pObj);
		if (!pBuilding)
			continue;
		if (pBuilding->GetBuildingType() != eBuildingType::COMMAND_CENTER)
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

void CUnit::UpdateSteering(const list<CObj*>& allUnits, float fDeltaTime)
{
	//오더가 없거나 MOVE가 아닐 경우 스킵
	if (m_OrderQ.empty())
	{
		m_velocity.fX *= 0.9f;
		m_velocity.fY *= 0.9f;
		return;
	}
	Order& order = m_OrderQ.front();
	//Move 또는 Move_and_Build 오더만 steering 적용
	if (order.eType != eOrderType::MOVE && order.eType != eOrderType::MOVE_AND_BUILD)
		return;
	//경로가 없으면 스킵
	if (order.path.empty())
		return;
	//속도 설정
	m_steeringParams.maxSpeed = m_fSpeed;

	//1.이웃 탐색 반경 : spe / ali / coh 중 최대값
	float neighborRadius = max(
		m_steeringParams.separationRadius,
		max(m_steeringParams.alignmentRadius, m_steeringParams.cohesionRadius)
	);
	//1. 주변 이웃 찾기
	list<CObj*> neighbors = CSteeringMgr::Get_Instance()->FindNeighbors(this,
		allUnits, neighborRadius);

	//steering force 계산
	SteeringOutput steering = CSteeringMgr::Get_Instance()->FollowPathWithFlocking(
		this, order.path, order.iPathIndex, neighbors, m_steeringParams, m_velocity);
	//경로 끝났으면 즉시 멈추기
	if (order.iPathIndex >= (int)order.path.size())
	{
		m_velocity = { 0.f, 0.f };
		m_acceleration = { 0.f, 0.f };
		//마지막 웨이포인트에 스냅까지
		Vec2 last = order.path.back();
		m_tInfo.fX = last.fX;
		m_tInfo.fY = last.fY;
		return;
	}
	//물리 시뮬레이션
	//가속도 = Force / Mass(Mass = 1)로 가정
	m_acceleration = steering.linear;
	//속도 업데이트
	m_velocity.fX += m_acceleration.fX * fDeltaTime;
	m_velocity.fY += m_acceleration.fY * fDeltaTime;
	m_velocity = CSteeringMgr::Get_Instance()->Limit(m_velocity,
		m_steeringParams.maxSpeed);
	// 실제 속도가 계산되는지
	OutputDebugString(L"Velocity: ");
	OutputDebugString(std::to_wstring(m_velocity.fX).c_str());
	//위치 업데이트
	m_tInfo.fX += m_velocity.fX * fDeltaTime;
	m_tInfo.fY += m_velocity.fY * fDeltaTime;
	//방향 업데이트(애니메이션용)
	if (CSteeringMgr::Get_Instance()->Magnitude(m_velocity) > 0.1f)
	{
		m_vDir = CSteeringMgr::Get_Instance()->Normalize(m_velocity);
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

int CUnit::DirTo17WayIndex(Vec2& vDir)
{
	if (vDir.fX == 0.f && vDir.fY == 0.f)
		return 0;

	float ang = atan2f(-vDir.fY, -vDir.fX); // -PI ~ PI

	//-90도 보정 추가
	ang -= PI * 0.5f;
	if (ang >= PI * 2.f)
		ang -= PI * 2.f;

	float step = (PI * 2.f) / 17.f;        // 21.176도
	int idx = (int)floorf((ang + step * 0.5f) / step); // 반올림
	idx = (idx % 17 + 17) % 17;   // 0~16
	return idx;
}

int CUnit::DirTo8WayIndex(Vec2& vDir)
{
	if (vDir.fX == 0.f && vDir.fY == 0.f)
		return 0;  // 정면

	float ang = atan2f(-vDir.fY, vDir.fX);
	if (ang < 0.f)
		ang += PI * 2.f;

	// 위쪽(12시)이 0번
	ang -= PI * 0.5f;
	if (ang < 0.f)
		ang += PI * 2.f;

	// 8방향으로 변환
	float step = (PI * 2.f) / 8.f;  // 45도
	int idx = (int)((ang + step * 0.5f) / step);

	return idx % 8;  // 0~7
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

void CUnit::UpdateUnitUIInfo()
{
	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
	//해당 유닛이 선택되어있는 상태인지 확인
	bool bThisSelected = (selected.size() == 1 && selected[0] == this);
	if (!bThisSelected)
	{
		UnitUIInfo info;
		info.pUnitName = GetUnitName();
		info.eType = m_eType;
		info.IsVisible = false;
		info.iHP = 0;
		info.iMaxHP = 0;
		info.iMP = 0;
		info.iMaxMP = 0;
		return;
	}
	//선택된 상태라면 항상 유닛 정보 표시
	UnitUIInfo info;
	info.pUnitName = GetUnitName();
	info.eType = m_eType;
	info.eRaceType = GetOriginalRace();
	info.IsVisible = true;
	info.iHP = m_iHP;
	info.iMaxHP = m_iMaxHP;
	info.iMP = m_iMP;
	info.iMaxMP = m_iMaxMP;
	info.iShield = m_iShield;
	info.iMaxShield = m_iMaxShield;

	CMainUI::Get_Instance()->SetUnitUIInfo(info);
}

const TCHAR* CUnit::GetUnitName()
{
	switch (m_eType)
	{
	case eUnitType::SCV:
		return L"SCV";
	case eUnitType::MARINE:
		return L"Marine";
	case eUnitType::MEDIC:
		return L"Medic";
	case eUnitType::GHOST:
		return L"Ghost";
	case eUnitType::FIREBAT:
		return L"Firebat";
	case eUnitType::VULTURE:
		return L"Vulture";
	case eUnitType::TANK:
		return L"Tank";
	case eUnitType::SIEGE_TANK:
		return L"Siege Tank";
	case eUnitType::GOLIATH:
		return L"Goliath";
	case eUnitType::BATTLECRUISER:
		return L"BattleCruiser";
		//저그 
	case eUnitType::LAVA:
		return L"Lava";
	case eUnitType::DRONE:
		return L"Drone";
	case eUnitType::ZERGLING:
		return L"Zergling";
	case eUnitType::HYDRALISK:
		return L"Hydralisk";
	case eUnitType::ULTRALISK:
		return L"Ultralisk";
	case eUnitType::MUTALISK:
		return L"Mutalisk";
	case eUnitType::OVERLOAD:
		return L"Overload";
		//프로토스
	case eUnitType::PROBE:
		return L"Probe";
	case eUnitType::ZEALOT:
		return L"Zealot";
	case eUnitType::DRAGON:
		return L"Dragoon";
	case eUnitType::HIGH_TEMPLAR:
		return L"High Templar";
	case eUnitType::DARK_TEMPLAR:
		return L"Dark Templar";
	case eUnitType::SHUTTLE:
		return L"Shuttle";
	case eUnitType::CARRIER:
		return L"Carrier";
	case eUnitType::OBSERVER:
		return L"Observer";
	case eUnitType::SCOUT:
		return L"Scout";
	case eUnitType::REAVOR:
		return L"Reavor";
	case eUnitType::CORSAIR:
		return L"Corsair";
	case eUnitType::DARK_ARCHON:
		return L"Dark Archon";
	case eUnitType::ARBITER:
		return L"Arbiter";
	default:
		return L"Unit";
	}
}

int CUnit::GetIconIndex(eCommandID command)
{
	switch (command)
	{
	case eCommandID::SCV:
		return IconIndex::SCV;
	case eCommandID::MARINE:
		return IconIndex::MARINE;
	case eCommandID::MEDIC:
		return IconIndex::MEDIC;
	case eCommandID::GHOST:
		return IconIndex::GHOST;
	case eCommandID::FIREBAT:
		return IconIndex::FIREBAT;
	case eCommandID::VULTURE:
		return IconIndex::VULTURE;
	case eCommandID::TANK:
		return IconIndex::TANK;
	case eCommandID::SIEGE_TANK:
		return IconIndex::SIEGE_TANK;
	case eCommandID::GOLIATH:
		return IconIndex::GOLIATH;
	case eCommandID::BATTLECRUISER:
		return IconIndex::BATTLECRUISER;
		break;
	default:
		return -1;
	}

	return 0;
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