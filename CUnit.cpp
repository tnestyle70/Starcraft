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

CUnit::CUnit() : m_fSpeed(0.f), m_iHP(0), m_iMaxHP(0),
	m_bActiveOrder(false), m_acceleration({0.f, 0.f}), m_currentWayPointIndex(0),
	m_iAttackDamage(0), m_fAttackRange(0.f), m_fAttackSpeed(0.f), m_dwLastAttack(0),
	iAttackFrameStart(0), iAttackFrameEnd(0), m_iSightRange(14)
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
	case eOrderType::GATHER:
		finished = UpdateGather(order);
		break;
	case eOrderType::RETURN_RESOURCE:
		finished = UpdateReturn(order);
		break;
	case eOrderType::ATTACK:
		finished = UpdateAttack(order);
		break;
	case eOrderType::ATTACK_MOVE:
	{
		CObj* pEnemy = FindNearestEnemy(m_fAttackRange);
		if (pEnemy)
		{
			//공격 모드로 전환
			order.eType = eOrderType::ATTACK;
			order.pTarget = pEnemy;
			finished = UpdateAttack(order);
		}
		else
		{
			//적이 없을 경우 계속 이동
			finished = UpdateMove(order);
		}
		break;
	}
	case eOrderType::CONSTRUCTING:
	{
		m_eState = eUnitState::CONSTRUCTING;	
		finished = false;
		if (order.pBuilding && order.pBuilding->IsComplete())
		{
			finished = true;
			m_eState = eUnitState::IDLE;  
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

void CUnit::UpdateHotKeys()
{
	// CONSTRUCTING 상태에서는 일반 커맨드 사용 불가
	if (m_eState == eUnitState::CONSTRUCTING)
		return;

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
		//가장 가까운 커맨드 센터 위치로 path 설정
		if (order.pTarget)
		{
			Vec2 start = { m_tInfo.fX, m_tInfo.fY };
			Vec2 ccPos = order.pTarget->Get_Pos();
			order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, ccPos);
			if (order.path.empty())
			{
				order.path.push_back(ccPos);
			}
			order.iPathIndex = 0;
		}
		break;
	}
	case eOrderType::ATTACK:
	{
		m_eState = eUnitState::MOVE;  // 일단 접근
		break;
	}
	case eOrderType::ATTACK_MOVE:
	{
		m_eState = eUnitState::MOVE;

		// 경로 설정
		if (order.path.empty())
		{
			Vec2 start = { m_tInfo.fX, m_tInfo.fY };
			order.path = CNavMgr::Get_Instance()->RequestPathWorld(start, order.dst);
			if (order.path.empty())
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

bool CUnit::StartBuild(Order& order)
{
	if (!order.pBuilding)
		return false;

	CBuilding* pBuilding = order.pBuilding;
	//건물에게 건설 시작 요청(건물이 알아서 설계하도록 한다.)
	if (!pBuilding->StartConstruct(order.dst))
	{
		delete pBuilding;
		order.pBuilding = nullptr;
		return false;
	}
	//건설 중인 상태 -> 애니메이션으로 상태 나누기!!!!!!!!!!
	CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pBuilding);
	return true;
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
	m_iHP -= iAttackDamage;
	if (m_iHP < 0) m_iHP = 0;
	if (m_iHP == 0) Set_Dead();
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

bool CUnit::UpdateMove(Order& order)
{
	if (order.path.empty())
	{
		// 경로 없음 -> 정지
		m_velocity.fX = 0.f;
		m_velocity.fY = 0.f;
		return true;
	}

	if (order.iPathIndex >= (int)order.path.size())
	{
		// 경로 완료 -> 정지
		m_velocity.fX = 0.f;
		m_velocity.fY = 0.f;
		return true;
	}

	// 현재 목표가 되는 웨이포인트
	Vec2 target = order.path[order.iPathIndex];
	Vec2 current = { m_tInfo.fX, m_tInfo.fY };

	// 거리 계산
	Vec2 diff = { target.fX - current.fX, target.fY - current.fY };
	float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);

	// 도착 체크
	float fArriveEps = 6.f;

	if (dist <= fArriveEps)
	{
		order.iPathIndex++;

		// 경로 끝 -> 오더 완료
		if (order.iPathIndex >= (int)order.path.size())
		{
			// 최종 도착 -> 정지
			m_velocity.fX = 0.f;
			m_velocity.fY = 0.f;
			return true;
		}

		// 다음 웨이포인트로
		return false;
	}

	// 브루드워 방식: 직진!
	Vec2 dir = { diff.fX / dist, diff.fY / dist };

	// 방향 갱신
	m_vDir = dir;

	// velocity 설정 (충돌 처리에서 참조)
	m_velocity.fX = dir.fX * m_fSpeed;
	m_velocity.fY = dir.fY * m_fSpeed;

	// 위치 업데이트
	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_tInfo.fX += dir.fX * dt * m_fSpeed;
	m_tInfo.fY += dir.fY * dt * m_fSpeed;

	return false;
}

bool CUnit::UpdateAttack(Order& order)
{
	//타겟이 죽었거나 사라진 경우
	if (!order.pTarget || order.pTarget->IsDead())
	{
		m_eState = eUnitState::IDLE;
		return true; //오더 완료
	}
	Vec2 targetPos = order.pTarget->Get_Pos();
	Vec2 myPos{ m_tInfo.fX, m_tInfo.fY };
	//타겟까지의 거리
	Vec2 diff = { targetPos.fX - myPos.fX, targetPos.fY - myPos.fY };
	float dist = sqrtf(diff.fX * diff.fX + diff.fY * diff.fY);
	//공격 사거리 체크
	if (dist <= m_fAttackRange)
	{
		m_eState = eUnitState::ATTACK;
		//타겟 방향 보기
		if (dist > 0.1f)
		{
			m_vDir = { diff.fX / dist, diff.fY / dist };
		}
		//공격 쿨타임 체크
		DWORD now = GetTickCount();
		DWORD attackCoolTime = (DWORD)(1000.f / m_fAttackSpeed);
		if (now - m_dwLastAttack >= attackCoolTime)
		{
			order.pTarget->TakeDamage(m_iAttackDamage);
			m_dwLastAttack = now;
		}
		return false;
	}
	else
	{
		//타겟이 사거리 내에 존재하지 않을 경우 이동
		m_eState = eUnitState::MOVE;
		Vec2 dir = { diff.fX / dist, diff.fY / dist };
		m_vDir = dir;
		float fDT = CTimeMgr::Get_Instance()->GetDT();
		m_tInfo.fX += dir.fX * fDT * m_fSpeed;
		m_tInfo.fY += dir.fY * fDT * m_fSpeed;
		return false;
	}
}

CObj* CUnit::FindNearestEnemy(float searchRadius)
{
	Vec2 myPos = { m_tInfo.fX, m_tInfo.fY };
	//모든 적 유닛 - 우선은 빌딩 배치 후 적 마린, 커맨드 센터 추가
	list<CObj*> enemyList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_ENEMY);
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
		return;
	}
	//선택된 상태라면 항상 유닛 정보 표시
	UnitUIInfo info;
	info.pUnitName = GetUnitName();
	info.eType = m_eType;
	info.IsVisible = true;
	info.iHP = m_iHP;
	info.iMaxHP = m_iMaxHP;

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