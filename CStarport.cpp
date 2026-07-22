#include "pch.h"
#include "CStarport.h"
#include "CTimeMgr.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CSelectionMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CSCV.h"
#include "CBattleCruiser.h"
#include "CUIMgr.h"
#include "CSoundMgr.h"
#include "CTileMgr.h"

CStarport::CStarport()
{
}

CStarport::~CStarport()
{
}

void CStarport::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 128.f;
	m_tInfo.fCY = 160.f;
	lstrcpy(m_szGreenKey, L"STARPORT_GREEN");
	lstrcpy(m_szRedKey, L"STARPORT_RED");
	m_pFrameKey = L"STARPORT_ANIM";
	m_eRender = RENDER_WORLD;
	m_tFrame.iStart = 0;
	m_tFrame.iFrame = 0;
	m_tFrame.iEnd = 1;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	m_eOriginalRace = eRaceType::RACE_TERRAN;
	m_eCurrentRace = eRaceType::RACE_TERRAN;
}

void CStarport::SetBuildingData()
{
	m_eType = eBuildingType::STARPORT;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 1400;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 4;
	m_iWidth = 4;
}

int CStarport::Update()
{
	int ret = CBuilding::Update();

	if (m_eState == eBuildingState::CONSTRUCT)
	{
		UpdateProduction();
		UpdateAnimation();
	}
	UpdateHotKeys();

	if (m_eTeamType == eTeamType::ENEMY)
	{
		//테란 적 스폰
		SpawnTerranEnemy();
	}

	__super::Update_Rect();

	return ret;
}

void CStarport::Render(HDC hDC)
{
	//고스트 모드일 경우 고스트 렌더가 되도록 설정
	if (m_bGhost)
	{
		CBuilding::Render(hDC);
		return;
	}
	if (m_bConstructing) //건설 중일 경우 BuildAnim Render!
	{
		CBuilding::Render(hDC);
		return;
	}

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

void CStarport::Release()
{
}

void CStarport::RenderSlot(HDC hDC, int slotIndex)
{
}

int CStarport::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CStarport::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);
	//건설 중일 경우에는 표시 X
	if (m_eState == eBuildingState::CONSTRUCTING)
		return;

	bool bAddOn = (m_pAddOn != nullptr);

	//0번 슬롯 BATTLECRUISE 생산
	outSlot[0].commandID = eCommandID::BATTLECRUISER;
	outSlot[0].iconKey = TEXT("ICON_BATTLECRUISER");
	outSlot[0].hotkey = 'W';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	outSlot[0].lock = !bAddOn;
	//4번 슬롯 LIFT
	outSlot[4].commandID = eCommandID::MOVE;
	outSlot[4].iconKey = TEXT("ICON_GHOST");
	outSlot[4].hotkey = 'D';
	outSlot[4].clickable = true;
	outSlot[4].visible = true;
	outSlot[4].lock = false;
	//5번 슬롯 RALLY
	outSlot[5].commandID = eCommandID::RALLY;
	outSlot[5].iconKey = TEXT("ICON_GHOST");
	outSlot[5].hotkey = 'F';
	outSlot[5].clickable = true;
	outSlot[5].visible = true;
	outSlot[5].lock = false;
	//6번 슬롯 AddOn 부착
	outSlot[6].commandID = eCommandID::STARPORT_ADDON;
	outSlot[6].iconKey = TEXT("ICON_STARPORTADDON");
	outSlot[6].hotkey = 'X';
	outSlot[6].clickable = true;
	outSlot[6].visible = true;
	outSlot[6].lock = bAddOn;
	//8번 : Cancle(Queue 취소)
	outSlot[8].commandID = eCommandID::CANCLE;
	outSlot[8].iconKey = TEXT("ICON_CANCLE");
	outSlot[8].hotkey = 'V';
	outSlot[8].clickable = true;
	outSlot[8].visible = true;
}

void CStarport::UpdateHotKeys()
{
	CBuilding::UpdateHotKeys();
	/*
	//배럭 하나만 선택되었을 경우 실행
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
	*/
}

bool CStarport::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	if (m_eState == eBuildingState::GHOST ||
		m_eState == eBuildingState::CONSTRUCTING)
		return false;

	ResourceCost cost{};

	switch (command)
	{
	case eCommandID::BATTLECRUISER:
		cost.mineral = 200;
		cost.gas = 200;
		cost.supply = 3;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::BATTLECRUISER, 2.f, 2.f, 100, 100, 3 });
		return true;
	case eCommandID::STARPORT_ADDON:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이니까 false
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//생산 시간
		//m_queue.push_back({ eCommandID::BATTLECRUISER, 3.f, 3.f, 50, 0 });
		BuildAddOn();
		return true;
	case eCommandID::MOVE:
		//리프팅되지 않은 상태의 경우 startLift = true로 변경
		if (!m_bStartLifting && !m_bLifted)
		{
			m_bStartLifting = true;
			m_eState = eBuildingState::LIFT;
			return true;
		}
		//리프팅 상태인 경우 startLand = true, 상태 landing으로 변경
		if (m_bLifted)
		{
			m_bStartLanding = true;
			m_eState = eBuildingState::LANDING;
			return true;
		}
		return true;
	case eCommandID::RALLY:
		Rally();
		break;
	case eCommandID::CANCLE:
		//생산 중인 큐 취소
		if (m_queue.empty())
		{
			return false;
		}
		m_queue.pop_back();
		//환불 정책
		return true;
	default:
		break;
	}
	return false;
}

void CStarport::Rally()
{
	m_vRallyPoint = CInputMgr::Get_Instance()->GetWorldMouse();
	m_bHasRallyPoint = true;
}

void CStarport::Lift()
{
	//리프팅 시작일 경우 건물 위치 기준 targetY 설정 + eBuildingState FLYING으로 변경 
	if (m_bStartLifting)
	{
		m_eState = eBuildingState::LIFT;
		m_fTargetY = m_tInfo.fY - 100.f;
		//리프팅 사운드
		CSoundMgr::Get_Instance()->PlayEffect(L"Build/SupplyDepot.wav", 0.5f);
		m_bStartLifting = false;
		//타일 점유 해제
		ReleaseOccupy();
	}
	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_tInfo.fY -= m_fSpeed * dt;
	if (m_tInfo.fY <= m_fTargetY)
	{
		//타겟 지점에 도달했을 경우 상태 전환!
		m_eState = eBuildingState::FLYING;
		m_bLifted = true;
	}
}

void CStarport::Landing()
{
	if (m_bStartLanding)
	{
		//랜딩 가능 검증
		//짓고자 하는 위치, Y + 100를 기준으로 건설 가능 여부 체크
		int row, col;
		if (!CalcSizeTopLeft({ m_tInfo.fX, m_tInfo.fY + 100.f }, row, col))
			return;

		if (!CTileMgr::Get_Instance()->CanConstruct(row, col,
			m_iWidth, m_iHeight, 0, m_eType))
		{
			//Can't Build 사운드 재생
			CSoundMgr::Get_Instance()->PlayEffect(L"SCV/SCVCantBuild.wav", 0.5f);
			m_bStartLanding = false;
			m_eState = eBuildingState::FLYING;
			return;
		}
		m_eState = eBuildingState::LANDING;
		m_fTargetY = m_tInfo.fY + 100.f;
		//랜딩 사운드
		CSoundMgr::Get_Instance()->PlayEffect(L"Build/SupplyDepot.wav", 0.5f);
		m_bStartLanding = false;
	}
	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_tInfo.fY += m_fSpeed * dt;
	if (m_tInfo.fY >= m_fTargetY)
	{
		//타겟 지점에 도달했을 경우 상태 전환!
		m_eState = eBuildingState::CONSTRUCT;
		//플래그 초기화!
		m_bStartLifting = false;
		m_bLifted = false;
		m_bStartLanding = false;
		AppplyOccupy();
	}
}

void CStarport::BuildAddOn()
{
	CBuilding* pAddOn = CBuildingFactory::Create(eBuildingType::STARPORT_ADDON);
	//factory 기준 오른쪽에 배치
	pAddOn->Set_Pos(m_tInfo.fX + 80.f, m_tInfo.fY + 20.f);
	//건설 시작
	pAddOn->SetGhost(false);
	pAddOn->SetState(eBuildingState::CONSTRUCTING);
	pAddOn->SetHP(1);
	pAddOn->SetConstructRemain(1.f);
	pAddOn->SetConstruct(true);
	pAddOn->AppplyOccupy();
	CObjMgr::Get_Instance()->Add_Object(OBJ_BUILDING, pAddOn);
	//애드온 추가!
	m_pAddOn = pAddOn;
}

void CStarport::UpdateAnimation()
{
	bool bProducing = !m_queue.empty();

	DWORD now = GetTickCount();

	if (bProducing)
	{
		// 생산 중: 애니메이션 진행 (2 ~ iEnd 반복)
		if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
		{
			m_tFrame.iFrame++;
			if (m_tFrame.iFrame >= m_tFrame.iEnd)
				m_tFrame.iFrame = 1;  // 2번 프레임으로 루프
			m_tFrame.dwTime = now;
			//addon 쪽에도 생산 중임을 알리기!
			if (m_pAddOn)
			{
				m_pAddOn->SetProducing(true);
			}
		}
	}
	else
	{
		// idle 상태: 고정 프레임 (0 또는 1)
		m_tFrame.iFrame = 1;  // 또는 1 (idle 프레임 번호)
		//addon 쪽에도 생산 상태 알리기
		if (m_pAddOn)
		{
			m_pAddOn->SetProducing(false);
		}
	}
}

void CStarport::UpdateProduction()
{
	Vec2 pos = Get_Pos();
	pos.fY += 80.f;

	CObj* pUnit = nullptr;

	//건설 완료시 처리(사운드, 이펙트, 기능 오픈 포함 )
	if (m_queue.empty())
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_queue.front().remainTime -= dt;

	if (m_queue.front().remainTime <= 0.f)
	{
		//생산 완료 후 인구수 증가
		CResourceMgr::Get_Instance()->AddSupply(m_queue.front().unitSupply);
		eCommandID done = m_queue.front().command;
		m_queue.pop_front();
		ProductionComplete(done);
	}
}

void CStarport::ProductionComplete(eCommandID command)
{
	Vec2 pos = Get_Pos();
	pos.fY += 80.f;

	CObj* pUnit = nullptr;

	switch (command)
	{
	case eCommandID::BATTLECRUISER:
		pUnit = CAbstractFactory<CBattleCruiser>::Create(pos.fX, pos.fY);
		CSoundMgr::Get_Instance()->PlayEffect(L"BattleCrusor/BattleCrusorBirth.wav", 0.5f);
		break;
	default:
		break;
	}

	if (pUnit)
	{
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pUnit);
		CUnit* ppUnit = dynamic_cast<CUnit*>(pUnit);
		if (m_bHasRallyPoint)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = m_vRallyPoint;
			ppUnit->PushOrder(rallyOrder);
		}
	}
}

void CStarport::SpawnTerranEnemy()
{
	if (m_eTeamType == eTeamType::ALLY)
		return;

	if (!CObjMgr::Get_Instance()->IsStartTerranRush())
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_fSpawnTimer += dt;
	if (m_fSpawnTimer >= m_fSpawnInterval)
	{
		Vec2 pos = Get_Pos();
		pos.fY -= 100.f;

		/*
		//배틀크루저 스폰
		//CSoundMgr::Get_Instance()->PlayEffect(L"BattleCruiser/BattleCruiserBirth.wav", 0.5f);
		CObj* pBattlerCruiser = CAbstractFactory<CBattleCruiser>::Create(pos.fX, pos.fY);
		pBattlerCruiser->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pBattlerCruiser);
		//아군 진영으로 이동
		CBattleCruiser* ppBattlerCruiser = dynamic_cast<CBattleCruiser*>(pBattlerCruiser);
		if (ppBattlerCruiser)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppBattlerCruiser->PushOrder(rallyOrder);
		}

		*/
		//스폰 타이머 초기화
		m_fSpawnTimer = 0.f;
		m_fSpawnInterval += 10.f;
	}
}

void CStarport::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}
