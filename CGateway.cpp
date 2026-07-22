#include "pch.h"
#include "CGateway.h"
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
#include "CZealot.h"
#include "CDragon.h"
#include "CHighTemplar.h"
#include "CDarkTemplar.h"
#include "CEffect.h"
#include "CDarkArchon.h"

CGateway::CGateway()
{
}

CGateway::~CGateway()
{
}

void CGateway::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 128.f;
	m_tInfo.fCY = 160.f;
	lstrcpy(m_szGreenKey, L"Gateway");
	lstrcpy(m_szRedKey, L"COMMANDCENTER_RED");
	lstrcpy(m_szConstructKey, L"COMMANDCENTER_CONSTRUCT");
	m_pFrameKey = L"Gateway";
	m_eRender = RENDER_WORLD;
	m_tFrame.iFrame = 0;
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 1;
	m_tFrame.iCol = 0;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	m_eOriginalRace = eRaceType::RACE_PROTOSS;
	m_eCurrentRace = eRaceType::RACE_PROTOSS;
}

void CGateway::SetBuildingData()
{
	m_eType = eBuildingType::GATEWAY;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 1000;
	m_iMaxShield = 1000;
	m_iShield = m_iMaxShield;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 3;
	m_iWidth = 3;
}

int CGateway::Update()
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
		//프로토스 적 스폰
		SpawnProtossEnemy();
	}

	__super::Update_Rect();

	return ret;
}

void CGateway::Render(HDC hDC)
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

	// 이미지 가져오기
	CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(m_pFrameKey);

	if (pPng)
	{
		int iWidth = pPng->Get_Width();
		int iHeight = pPng->Get_Height();

		pPng->Render_Alpha(hDC,
			(int)m_tInfo.fX - iScrollX - iWidth / 2,
			(int)m_tInfo.fY - iScrollY - iHeight / 2, iWidth, iHeight, false);
	}
}

void CGateway::Release()
{
}

void CGateway::RenderSlot(HDC hDC, int slotIndex)
{
}

int CGateway::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CGateway::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);

	if (m_eState == eBuildingState::CONSTRUCTING)
		return;

	bool bAddOn = (m_pAddOn != nullptr);

	//0번 슬롯 질럿 생산
	outSlot[0].commandID = eCommandID::ZEALOT;
	outSlot[0].iconKey = TEXT("ICON_SCV");
	outSlot[0].hotkey = 'W';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//1번 슬롯 드라군
	outSlot[1].commandID = eCommandID::DRAGOON;
	outSlot[1].iconKey = TEXT("ICON_SCV");
	outSlot[1].hotkey = 'E';
	outSlot[1].clickable = true;
	outSlot[1].visible = true;
	outSlot[1].lock = false;
	//2번 슬롯 다크 템플러
	outSlot[2].commandID = eCommandID::HIGH_TEMPLAR;
	outSlot[2].iconKey = TEXT("ICON_SCV");
	outSlot[2].hotkey = 'R';
	outSlot[2].clickable = true;
	outSlot[2].visible = true;
	outSlot[2].lock = false;
	//3번 슬롯 다크 템플러
	outSlot[3].commandID = eCommandID::DARK_TEMPLAR;
	outSlot[3].iconKey = TEXT("ICON_SCV");
	outSlot[3].hotkey = 'S';
	outSlot[3].clickable = true;
	outSlot[3].visible = true;
	outSlot[3].lock = false;
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

void CGateway::UpdateHotKeys()
{
	CBuilding::UpdateHotKeys();
}

bool CGateway::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	if (m_eState == eBuildingState::GHOST ||
		m_eState == eBuildingState::CONSTRUCTING)
		return false;

	ResourceCost cost{};

	bool bWarpGateReady = CObjMgr::Get_Instance()->IsWarpGateReady();

	switch (command)
	{
	case eCommandID::ZEALOT:
		cost.mineral = 50;
		cost.gas = 0;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		if(bWarpGateReady)
			m_queue.push_back({ eCommandID::ZEALOT, 0.2f, 0.2f, 50, 0 });
		else
			m_queue.push_back({ eCommandID::ZEALOT, 2.f, 2.f, 50, 0 });
		return true;
	case eCommandID::DRAGOON:
		cost.mineral = 50;
		cost.gas = 0;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		if (bWarpGateReady)
			m_queue.push_back({ eCommandID::DRAGOON, 0.2f, 0.2f, 50, 0 });
		else
			m_queue.push_back({ eCommandID::DRAGOON, 2.f, 2.f, 50, 0 });
		return true;
	case eCommandID::HIGH_TEMPLAR:
		cost.mineral = 50;
		cost.gas = 0;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		if (bWarpGateReady)
			m_queue.push_back({ eCommandID::HIGH_TEMPLAR, 0.2f, 0.2f, 50, 0 });
		else
			m_queue.push_back({ eCommandID::HIGH_TEMPLAR, 2.f, 2.f, 50, 50 });
		return true;
	case eCommandID::DARK_TEMPLAR:
		cost.mineral = 50;
		cost.gas = 0;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		if (bWarpGateReady)
			m_queue.push_back({ eCommandID::DARK_TEMPLAR, 0.2f, 0.2f, 50, 0 });
		else
			m_queue.push_back({ eCommandID::DARK_TEMPLAR, 2.f, 2.f, 50, 50 });
		return true;
	case eCommandID::SCANNER:
		cost.mineral = 50;
		cost.gas = 0;
		cost.supply = 0;
		//유닛 아니므로 false
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		UseScanner();
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
		break;
	default:
		break;
	}
	return false;
}

void CGateway::Rally()
{
	//Rally를 누른 시점의 마우스 위치를 저장
	m_vRallyPoint = CInputMgr::Get_Instance()->GetWorldMouse();
	m_bHasRallyPoint = true;
}

void CGateway::Lift()
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

void CGateway::Landing()
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

void CGateway::BuildAddOn()
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

void CGateway::UpdateProduction()
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
		eCommandID done = m_queue.front().command;
		m_queue.pop_front();

		//차원 관문 업그레이드에 따른 다른 생산 방식 설정하기
		bool bWarpGateReady = CObjMgr::Get_Instance()->IsWarpGateReady();
		if (!bWarpGateReady)
			ProductionComplete(done);
		else
			WarpGateComplete(done);
	}
}

void CGateway::ProductionComplete(eCommandID command)
{
	if (command == eCommandID::ZEALOT)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"zealot/pzeRdy00.wav", 0.5f);

		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pZeaot = CAbstractFactory<CZealot>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pZeaot);
		if (m_bHasRallyPoint)
		{
			//랠리 포인트로 이동
			CZealot* ppZealot = dynamic_cast<CZealot*>(pZeaot);
			if (ppZealot)
			{
				Order rallyOrder;
				rallyOrder.eType = eOrderType::MOVE;
				rallyOrder.dst = m_vRallyPoint;
				ppZealot->PushOrder(rallyOrder);
			}
		}
	}
	if (command == eCommandID::DRAGOON)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Dragoon/pdrRdy00.wav", 0.5f);

		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pDragoon = CAbstractFactory<CDragon>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDragoon);
		if (m_bHasRallyPoint)
		{
			//랠리 포인트로 이동
			CDragon* ppDragoon = dynamic_cast<CDragon*>(pDragoon);
			if (ppDragoon)
			{
				Order rallyOrder;
				rallyOrder.eType = eOrderType::MOVE;
				rallyOrder.dst = m_vRallyPoint;
				ppDragoon->PushOrder(rallyOrder);
			}
		}
	}
	if (command == eCommandID::HIGH_TEMPLAR)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"HTemplar/pteRdy00.wav", 0.5f);

		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pHighTemplar = CAbstractFactory<CHighTemplar>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pHighTemplar);
		if (m_bHasRallyPoint)
		{
			//랠리 포인트로 이동
			CHighTemplar* ppHighTemplar = dynamic_cast<CHighTemplar*>(pHighTemplar);
			if (ppHighTemplar)
			{
				Order rallyOrder;
				rallyOrder.eType = eOrderType::MOVE;
				rallyOrder.dst = m_vRallyPoint;
				ppHighTemplar->PushOrder(rallyOrder);
			}
		}
	}
	if (command == eCommandID::DARK_TEMPLAR)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"HTemplar/PDtRdy01.wav", 0.5f);

		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pDarkTemplar = CAbstractFactory<CDarkTemplar>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDarkTemplar);
		if (m_bHasRallyPoint)
		{
			//랠리 포인트로 이동
			CDarkTemplar* ppDarkTemplar = dynamic_cast<CDarkTemplar*>(pDarkTemplar);
			if (ppDarkTemplar)
			{
				Order rallyOrder;
				rallyOrder.eType = eOrderType::MOVE;
				rallyOrder.dst = m_vRallyPoint;
				ppDarkTemplar->PushOrder(rallyOrder);
			}
		}
	}
}

void CGateway::WarpGateComplete(eCommandID command)
{
	Vec2 worldMouse = CInputMgr::Get_Instance()->GetWorldMouse();

	if (command == eCommandID::ZEALOT)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"zealot/pzeRdy00.wav", 0.5f);
		//이펙트 생성
		CObj* pEffect = CAbstractFactory<CEffect>::Create(worldMouse.fX, worldMouse.fY);
		pEffect->Initialize();
		CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
		if (pEffectObj)
		{
			pEffectObj->Set_Effect(L"Arbiter_Recall_",
				11, 100, 100, eEffectType::COL_BASE, RGB(0, 0, 0), 0.1f, false,
				eEffectImageType::PNG);
		}
		CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
		//차원 관문
		CObj* pZeaot = CAbstractFactory<CZealot>::Create(worldMouse.fX, worldMouse.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pZeaot);
	}
	if (command == eCommandID::DRAGOON)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Dragoon/pdrRdy00.wav", 0.5f);
		//이펙트 생성
		CObj* pEffect = CAbstractFactory<CEffect>::Create(worldMouse.fX, worldMouse.fY);
		pEffect->Initialize();
		CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
		if (pEffectObj)
		{
			pEffectObj->Set_Effect(L"Arbiter_Recall_",
				11, 100, 100, eEffectType::COL_BASE, RGB(0, 0, 0), 0.1f, false,
				eEffectImageType::PNG);
		}
		CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
		CObj* pDragoon = CAbstractFactory<CDragon>::Create(worldMouse.fX, worldMouse.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDragoon);
	}
	if (command == eCommandID::HIGH_TEMPLAR)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"HTemplar/pteRdy00.wav", 0.5f);
		//이펙트 생성
		CObj* pEffect = CAbstractFactory<CEffect>::Create(worldMouse.fX, worldMouse.fY);
		pEffect->Initialize();
		CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
		if (pEffectObj)
		{
			pEffectObj->Set_Effect(L"Arbiter_Recall_",
				11, 100, 100, eEffectType::COL_BASE, RGB(0, 0, 0), 0.1f, false,
				eEffectImageType::PNG);
		}
		CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
		CObj* pHighTemplar = CAbstractFactory<CHighTemplar>::Create(worldMouse.fX, worldMouse.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pHighTemplar);
	}
	if (command == eCommandID::DARK_TEMPLAR)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"HTemplar/PDtRdy01.wav", 0.5f);
		//이펙트 생성
		CObj* pEffect = CAbstractFactory<CEffect>::Create(worldMouse.fX, worldMouse.fY);
		pEffect->Initialize();
		CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
		if (pEffectObj)
		{
			pEffectObj->Set_Effect(L"Arbiter_Recall_",
				11, 100, 100, eEffectType::COL_BASE, RGB(0, 0, 0), 0.1f, false,
				eEffectImageType::PNG);
		}
		CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
		CObj* pDarkTemplar = CAbstractFactory<CDarkTemplar>::Create(worldMouse.fX, worldMouse.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pDarkTemplar);
	}
}

void CGateway::UseScanner()
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

void CGateway::SpawnProtossEnemy()
{
	if (m_eTeamType == eTeamType::ALLY)
		return;

	if (!CObjMgr::Get_Instance()->IsStartProtossRush())
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_fSpawnTimer += dt;
	if (m_fSpawnTimer >= m_fSpawnInterval)
	{
		Vec2 pos = Get_Pos();
		pos.fY -= 100.f;
		//질럿 스폰
		//CSoundMgr::Get_Instance()->PlayEffect(L"BattleCruiser/BattleCruiserBirth.wav", 0.5f);
		CObj* pZealot = CAbstractFactory<CZealot>::Create(pos.fX, pos.fY);
		pZealot->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pZealot);
		//아군 진영으로 이동
		CZealot* ppZealot = dynamic_cast<CZealot*>(pZealot);
		if (ppZealot)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppZealot->PushOrder(rallyOrder);
		}
		//드라군 스폰
		pos.fX += 50.f;
		CObj* pDragon = CAbstractFactory<CDragon>::Create(pos.fX, pos.fY);
		pDragon->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pDragon);
		//아군 진영으로 이동
		CDragon* ppDragon = dynamic_cast<CDragon*>(pDragon);
		if (ppDragon)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppDragon->PushOrder(rallyOrder);
		}

		/*
		//다크 템플러 스폰
		pos.fX += 50.f;
		CObj* pDarkTemplar = CAbstractFactory<CDarkTemplar>::Create(pos.fX, pos.fY);
		pDarkTemplar->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pDarkTemplar);
		//아군 진영으로 이동
		CDarkTemplar* ppDarkTemplar = dynamic_cast<CDarkTemplar*>(pDarkTemplar);
		if (ppDarkTemplar)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppDarkTemplar->PushOrder(rallyOrder);
		}
		//하이 템플러 스폰
		pos.fX += 50.f;
		CObj* pHighTemplar = CAbstractFactory<CHighTemplar>::Create(pos.fX, pos.fY);
		pHighTemplar->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pHighTemplar);
		//아군 진영으로 이동
		CHighTemplar* ppHighTemplar = dynamic_cast<CHighTemplar*>(pHighTemplar);
		if (ppHighTemplar)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppHighTemplar->PushOrder(rallyOrder);
		}
		//다크 아칸 스폰
		pos.fX += 50.f;
		CObj* pDarkArchon = CAbstractFactory<CDarkArchon>::Create(pos.fX, pos.fY);
		pDarkArchon->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pDarkArchon);
		//아군 진영으로 이동
		CDarkArchon* ppDarkArchon = dynamic_cast<CDarkArchon*>(pDarkArchon);
		if (ppDarkArchon)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppDarkArchon->PushOrder(rallyOrder);
		}
		*/

		//스폰 타이머 초기화
		m_fSpawnTimer = 0.f;
		m_fSpawnInterval += 10.f;
	}
}

void CGateway::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}

void CGateway::UpdateDestroy()
{
	//타일 점유 해제
	ReleaseOccupy();
	//사운드 재생
	//CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingDeath.wav", 1.f);
	//타겟 이펙트 생성
	CObj* pEffect = CAbstractFactory<CEffect>::Create(
		m_tInfo.fX, m_tInfo.fY);
	pEffect->Initialize();
	CEffect* pEffectObj = dynamic_cast<CEffect*>(pEffect);
	if (pEffectObj)
	{
		pEffectObj->Set_Effect(L"Protoss_Building_Destroy_",
			14, 252, 200, eEffectType::COL_BASE, RGB(0, 0, 0), 0.05f, false,
			eEffectImageType::PNG);
	}
	CObjMgr::Get_Instance()->Add_Object(OBJ_PROJECTILE, pEffect);
	CObjMgr::Get_Instance()->IncreaseDestroyCount();
}

void CGateway::BuildCombatStation()
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

void CGateway::BuildNuclearSilo()
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
