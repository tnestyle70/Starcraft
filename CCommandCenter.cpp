#include "pch.h"
#include "CCommandCenter.h"
#include "CTimeMgr.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CSelectionMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CSCV.h"
#include "CUIMgr.h"
#include "CSoundMgr.h"
#include "CFogMgr.h"
#include "CTileMgr.h"
#include "CMarine.h"
#include "CFirebat.h"
#include "CGhost.h"
#include "CVulture.h"
#include "CTank.h"
#include "CGoliath.h"
#include "CBattleCruiser.h"

CCommandCenter::CCommandCenter()
{
}

CCommandCenter::~CCommandCenter()
{
}

void CCommandCenter::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 128.f;
	m_tInfo.fCY = 160.f;
	lstrcpy(m_szGreenKey, L"COMMANDCENTER_GREEN");
	lstrcpy(m_szRedKey, L"COMMANDCENTER_RED");
	lstrcpy(m_szConstructKey, L"COMMANDCENTER_CONSTRUCT");
	m_pFrameKey = L"CommandCenter";
	m_eRender = RENDER_WORLD;
	m_tFrame.iFrame = 0;
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 1;
	m_tFrame.iCol = 0;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	m_eOriginalRace = eRaceType::RACE_TERRAN;
	m_eCurrentRace = eRaceType::RACE_TERRAN;
}

void CCommandCenter::SetBuildingData()
{
	m_eType = eBuildingType::COMMAND_CENTER;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 1500;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 4;
	m_iWidth = 4;
}

int CCommandCenter::Update()
{
	int ret = CBuilding::Update();
	//건설이 완료되었을 경우에만 생산
	if (m_eState == eBuildingState::CONSTRUCT)
	{
		UpdateProduction();
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

void CCommandCenter::Render(HDC hDC)
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

void CCommandCenter::Release()
{
}

void CCommandCenter::RenderSlot(HDC hDC, int slotIndex)
{
}

int CCommandCenter::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CCommandCenter::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
    // 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
    CBuilding::CommandCardSlot(outSlot);

	if (m_eState == eBuildingState::CONSTRUCTING)
		return;

	bool bAddOn = (m_pAddOn != nullptr);

	//0번 슬롯 SCV 생산
	outSlot[0].commandID = eCommandID::SCV;
	outSlot[0].iconKey = TEXT("ICON_SCV");
	outSlot[0].hotkey = 'W';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//2번 슬롯 SCANNER
	//outSlot[2].commandID = eCommandID::SCANNER;
	//outSlot[2].iconKey = TEXT("ICON_SCV");
	//outSlot[2].hotkey = 'R';
	//outSlot[2].clickable = true;
	//outSlot[2].visible = true;
	//outSlot[2].lock = !bAddOn;
	//4번 슬롯 LIFT
	outSlot[4].commandID = eCommandID::MOVE;
	outSlot[4].iconKey = TEXT("ICON_SCV");
	outSlot[4].hotkey = 'D';
	outSlot[4].clickable = true;
	outSlot[4].visible = true;
	outSlot[4].lock = false;
	//5번 슬롯 RALLY
	outSlot[5].commandID = eCommandID::RALLY;
	outSlot[5].iconKey = TEXT("ICON_SCV");
	outSlot[5].hotkey = 'F';
	outSlot[5].clickable = true;
	outSlot[5].visible = true;
	outSlot[5].lock = false;
	//6번 슬롯 COMBAT_STATION
	outSlot[6].commandID = eCommandID::COMBAT_STATION;
	outSlot[6].iconKey = TEXT("ICON_SCV");
	outSlot[6].hotkey = 'X';
	outSlot[6].clickable = true;
	outSlot[6].visible = true;
	outSlot[6].lock = bAddOn;
	//7번 슬롯 NUCLEAR_SILO
	outSlot[7].commandID = eCommandID::NUCLEAR_SILO;
	outSlot[7].iconKey = TEXT("ICON_SCV");
	outSlot[7].hotkey = 'C';
	outSlot[7].clickable = true;
	outSlot[7].visible = true;
	outSlot[7].lock = false;
	//8번 : Cancle(Queue 취소)
	outSlot[8].commandID = eCommandID::CANCLE;
	outSlot[8].iconKey = TEXT("ICON_CANCLE");
	outSlot[8].hotkey = 'V';
	outSlot[8].clickable = true;
	outSlot[8].visible = true;
}

void CCommandCenter::UpdateHotKeys()
{
	CBuilding::UpdateHotKeys();
	/*
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
	*/
}

bool CCommandCenter::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	if (m_eState == eBuildingState::GHOST ||
		m_eState == eBuildingState::CONSTRUCTING)
		return false;

	ResourceCost cost{};

	switch (command)
	{
	case eCommandID::SCV:
		cost.mineral = 50;
		cost.gas = 0;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::SCV, 1.f, 1.f, 50, 0, 1 });
		return true;
	//case eCommandID::SCANNER:
	//	cost.mineral = 50;
	//	cost.gas = 0;
	//	cost.supply = 0;
	//	//유닛 아니므로 false
	//	if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
	//		return false;
	//	UseScanner();
	//	return true;
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
	case eCommandID::COMBAT_STATION:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이므로 인구수 false로 설정
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//BuildAddOn 함수 호출!
		BuildCombatStation();
		return true;
	case eCommandID::NUCLEAR_SILO:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이므로 인구수 false로 설정
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//BuildAddOn 함수 호출!
		BuildNuclearSilo();
		return true;
	case eCommandID::CANCLE:
		//생산 중인 큐 취소
		if (m_queue.empty())
		{
			return false;
		}
		m_queue.pop_back();
		//환불 정책
		return true;
		break;
	default:
		break;
	}
	return false;
}

void CCommandCenter::Rally()
{
	//Rally를 누른 시점의 마우스 위치를 저장
	m_vRallyPoint = CInputMgr::Get_Instance()->GetWorldMouse();
	m_bHasRallyPoint = true;
}

void CCommandCenter::Lift()
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

void CCommandCenter::Landing()
{
	if (m_bStartLanding)
	{
		//랜딩 가능 검증
		//짓고자 하는 위치, Y + 100를 기준으로 건설 가능 여부 체크

		int row, col;
		if (!CalcSizeTopLeft({ m_tInfo.fX, m_tInfo.fY + 100.f }, row, col))
			return;

		if(!CTileMgr::Get_Instance()->CanConstruct(row, col,
			m_iWidth, m_iHeight, 0, m_eType))
		{
			//Can't Build 사운드 재생
			CSoundMgr::Get_Instance()->PlayEffect(L"SCV/SCVCantBuild.wav", 1.f);
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

void CCommandCenter::BuildAddOn()
{
	CBuilding* pAddOn = CBuildingFactory::Create(eBuildingType::COMBAT_STATION);
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

void CCommandCenter::UpdateProduction()
{
	//건설 완료시 처리(사운드, 이펙트, 기능 오픈 포함 )
	if (m_queue.empty())
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_queue.front().remainTime -= dt;
	
	if (m_queue.front().remainTime <= 0.f)
	{
		//생산 완료 후 인구수 증가
		CResourceMgr::Get_Instance()->AddSupply(m_queue.front().unitSupply);
		//오더 완료
		eCommandID done = m_queue.front().command;
		m_queue.pop_front();
		ProductionComplete(done);
	}
}

void CCommandCenter::ProductionComplete(eCommandID command)
{
	if (command == eCommandID::SCV)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"SCV/SCVBirth.wav", 0.5f);

		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pSCV = CAbstractFactory<CSCV>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
		if (m_bHasRallyPoint)
		{
			//랠리 포인트로 이동
			CSCV* ppSCV = dynamic_cast<CSCV*>(pSCV);
			if (ppSCV)
			{
				Order rallyOrder;
				rallyOrder.eType = eOrderType::MOVE;
				rallyOrder.dst = m_vRallyPoint;
				ppSCV->PushOrder(rallyOrder);
			}
		}
	}
}

void CCommandCenter::UseScanner()
{
	//사운드 재생
	CSoundMgr::Get_Instance()->PlayEffect(L"Build/TCsSca00.wav", 1.f);
	//월드 마우스 좌표 (픽셀)
	Vec2 worldMouse = CInputMgr::Get_Instance()->GetWorldMouse();

	//픽셀 좌표 → 타일 인덱스로 변환
	int tileCol = (int)(worldMouse.fX / 32);
	int tileRow = (int)(worldMouse.fY / 32);

	//스캐너 스윕 (반경 10타일)
	CFogMgr::Get_Instance()->SetScan(tileRow, tileCol, 10, true);
}

void CCommandCenter::SpawnTerranEnemy()
{
	if (CObjMgr::Get_Instance()->IsLastRush()) //러쉬 카운트 초기화 이후에 다시 생성 가능하도록 설정하기
		m_iRushCount = 2;

	if (m_eTeamType == eTeamType::ALLY)
		return;

	if (!CObjMgr::Get_Instance()->IsStartTerranRush())
		return;

	if (m_iRushCount >= 5)
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_fSpawnTimer += dt;
	if (m_fSpawnTimer >= m_fSpawnInterval)
	{
		Vec2 pos = Get_Pos();
		pos.fY -= 100.f;
		////SCV 스폰
		////CSoundMgr::Get_Instance()->PlayEffect(L"SCV/SCVBirth.wav", 0.5f);
		//CObj* pSCV = CAbstractFactory<CSCV>::Create(pos.fX, pos.fY);
		//pSCV->SetTeamType(eTeamType::ENEMY);
		//CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pSCV);
		////아군 진영으로 이동
		//CSCV* ppSCV = dynamic_cast<CSCV*>(pSCV);
		//if (ppSCV)
		//{
		//	Order rallyOrder;
		//	rallyOrder.eType = eOrderType::MOVE;
		//	rallyOrder.dst = { 100, 100 };
		//	ppSCV->PushOrder(rallyOrder);
		//}
		
		//마린 스폰
		CSoundMgr::Get_Instance()->PlayEffect(L"Marine/MarineBirth.wav", 0.5f);
		CObj* pMarine = CAbstractFactory<CMarine>::Create(pos.fX, pos.fY);
		pMarine->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pMarine);
		//아군 진영으로 이동
		CMarine* ppMarine = dynamic_cast<CMarine*>(pMarine);
		if (ppMarine)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppMarine->PushOrder(rallyOrder);
		}

		//파이어뱃 스폰
		pos.fX += 25.f;
		//CSoundMgr::Get_Instance()->PlayEffect(L"Firebat/FirebatBirth.wav", 0.5f);
		CObj* pFirebat = CAbstractFactory<CFirebat>::Create(pos.fX, pos.fY);
		pFirebat->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pFirebat);
		//아군 진영으로 이동
		CFirebat* ppFirebat = dynamic_cast<CFirebat*>(pFirebat);
		if (ppFirebat)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppFirebat->PushOrder(rallyOrder);
		}

		//고스트 스폰
		pos.fX += 25.f;
		//CSoundMgr::Get_Instance()->PlayEffect(L"Ghost/GhostBirth.wav", 0.5f);
		CObj* pGhost = CAbstractFactory<CGhost>::Create(pos.fX, pos.fY);
		pGhost->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pGhost);
		//아군 진영으로 이동
		CGhost* ppGhost = dynamic_cast<CGhost*>(pGhost);
		if (ppGhost)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppGhost->PushOrder(rallyOrder);
		}
		//벌쳐 스폰
		pos.fX += 25.f;
		//CSoundMgr::Get_Instance()->PlayEffect(L"Marine/MarineBirth.wav", 0.5f);
		CObj* pVulture = CAbstractFactory<CVulture>::Create(pos.fX, pos.fY);
		pVulture->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pVulture);
		//아군 진영으로 이동
		CVulture* ppVulture = dynamic_cast<CVulture*>(pVulture);
		if (ppVulture)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppVulture->PushOrder(rallyOrder);
		}

		//탱크 스폰
		pos.fX += 25.f;
		//CSoundMgr::Get_Instance()->PlayEffect(L"Firebat/FirebatBirth.wav", 0.5f);
		CObj* pTank = CAbstractFactory<CTank>::Create(pos.fX, pos.fY);
		pTank->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pTank);
		//아군 진영으로 이동
		CTank* ppTank = dynamic_cast<CTank*>(pTank);
		if (ppTank)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppTank->PushOrder(rallyOrder);
		}

		//골리앗 스폰
		pos.fX += 25.f;
		//CSoundMgr::Get_Instance()->PlayEffect(L"Ghost/GhostBirth.wav", 0.5f);
		CObj* pGoliath = CAbstractFactory<CGoliath>::Create(pos.fX, pos.fY);
		pGoliath->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pGoliath);
		//아군 진영으로 이동
		CGoliath* ppGoliath = dynamic_cast<CGoliath*>(pGoliath);
		if (ppGoliath)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppGoliath->PushOrder(rallyOrder);
		}

		//배틀크루저 스폰
		pos.fX += 25.f;
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

		m_fSpawnTimer = 0.f;
		m_iRushCount++;

		if (CObjMgr::Get_Instance()->IsLastRush()) //러쉬 카운트 초기화 이후에 다시 생성 가능하도록 설정하기
		{
			CObjMgr::Get_Instance()->SetLastRush(false);
		}
	}
}

void CCommandCenter::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}

void CCommandCenter::BuildCombatStation()
{
	CBuilding* pAddOn = CBuildingFactory::Create(eBuildingType::COMBAT_STATION);
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

void CCommandCenter::BuildNuclearSilo()
{
	CBuilding* pAddOn = CBuildingFactory::Create(eBuildingType::NUCLEAR_SILO);
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
