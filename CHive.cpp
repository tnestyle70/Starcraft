#include "pch.h"
#include "CHive.h"
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
#include "COverload.h"
#include "CZergling.h"
#include "CHydralisk.h"
#include "CMutalisk.h"
#include "CHydralisk.h"
#include "CUltralisk.h"

CHive::CHive()
{
}

CHive::~CHive()
{
}

void CHive::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 192.f;
	m_tInfo.fCY = 224.f;
	lstrcpy(m_szGreenKey, L"COMMANDCENTER_GREEN");
	lstrcpy(m_szRedKey, L"COMMANDCENTER_RED");
	lstrcpy(m_szConstructKey, L"COMMANDCENTER_CONSTRUCT");
	m_pFrameKey = L"HIVE";
	m_eRender = RENDER_WORLD;
	m_tFrame.iFrame = 0;
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 1;
	m_tFrame.iCol = 0;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	m_eOriginalRace = eRaceType::RACE_ZERG;
	m_eCurrentRace = eRaceType::RACE_ZERG;
}

void CHive::SetBuildingData()
{
	m_eType = eBuildingType::HIVE;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 1500;
	m_iHP = m_iMaxHP;
	m_fConstructDuration = 1.f;
	//타일 단위 크기
	m_iHeight = 4;
	m_iWidth = 4;
}

int CHive::Update()
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
		SpawnZergUnits();
	}

	__super::Update_Rect();

	return ret;
}

void CHive::Render(HDC hDC)
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

		//CBlueMarine처럼 스크롤을 빼기
		//pPng->Render_Alpha(hDC,
		//    (int)m_tInfo.fX - iScrollX - iWidth / 2,
		//    (int)m_tInfo.fY - iScrollY - iHeight / 2, false);
		pPng->Render_Alpha(hDC,
			(int)m_tInfo.fX - iScrollX - iWidth / 2,
			(int)m_tInfo.fY - iScrollY - iHeight / 2, iWidth, iHeight, false);
	}
}

void CHive::Release()
{
}

void CHive::RenderSlot(HDC hDC, int slotIndex)
{
}

int CHive::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CHive::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);

	if (m_eState == eBuildingState::CONSTRUCTING)
		return;

	//0번 슬롯 SCV 생산
	outSlot[0].commandID = eCommandID::SCV;
	outSlot[0].iconKey = TEXT("ICON_SCV");
	outSlot[0].hotkey = 'S';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//8번 : Cancle(Queue 취소)
	outSlot[8].commandID = eCommandID::CANCLE;
	outSlot[8].iconKey = TEXT("ICON_CANCLE");
	outSlot[8].hotkey = VK_ESCAPE;
	outSlot[8].clickable = true;
	outSlot[8].visible = true;
}

void CHive::UpdateHotKeys()
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

bool CHive::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	if (m_eState != eBuildingState::CONSTRUCT)
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
		m_queue.push_back({ eCommandID::SCV, 2.f, 2.f, 50, 0 });
		return true;
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

void CHive::SpawnZergUnits()
{
	if (CObjMgr::Get_Instance()->IsLastRush()) //러쉬 카운트 초기화 이후에 다시 생성 가능하도록 설정하기
		m_iRushCount = 5;

	if (m_eTeamType == eTeamType::ALLY)
		return;

	if (!CObjMgr::Get_Instance()->IsStartZergRush())
		return;

	if (m_iRushCount >= 7)
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_fSpawnTimer += dt;
	if (m_fSpawnTimer >= m_fSpawnInterval)
	{
		Vec2 pos = Get_Pos();
		pos.fY -= 150.f;
		pos.fX -= 150.f;
		//오버로드 스폰
		//CSoundMgr::Get_Instance()->PlayEffect(L"Overload/OverloadBirth.wav", 0.5f);
		CObj* pOverload = CAbstractFactory<COverload>::Create(pos.fX, pos.fY);
		pOverload->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pOverload);
		//아군 진영으로 이동
		COverload* ppOverload = dynamic_cast<COverload*>(pOverload);
		if (ppOverload)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppOverload->PushOrder(rallyOrder);
		}
		//히드라 스폰
		pos.fX += 50.f;
		//CSoundMgr::Get_Instance()->PlayEffect(L"Hydralisk/HydraliskBirth.wav", 0.5f);
		CObj* pHydralisk = CAbstractFactory<CHydralisk>::Create(pos.fX, pos.fY);
		pHydralisk->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pHydralisk);
		//아군 진영으로 이동
		CHydralisk* ppHydralisk = dynamic_cast<CHydralisk*>(pHydralisk);
		if (ppHydralisk)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppHydralisk->PushOrder(rallyOrder);
		}
		//뮤탈리스크 스폰
		//CSoundMgr::Get_Instance()->PlayEffect(L"Mutalisk/MutaliskBirth.wav", 0.5f);
		pos.fX += 50.f;
		CObj* pMutalisk = CAbstractFactory<CMutalisk>::Create(pos.fX, pos.fY);
		pMutalisk->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pMutalisk);
		//아군 진영으로 이동
		CMutalisk* ppMutalisk = dynamic_cast<CMutalisk*>(pMutalisk);
		if (ppMutalisk)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppMutalisk->PushOrder(rallyOrder);
		}
		//저글링 스폰
		CSoundMgr::Get_Instance()->PlayEffect(L"Zergling/ZerglingBirth.wav", 0.5f);
		pos.fX += 50.f;
		CObj* pZergling = CAbstractFactory<CZergling>::Create(pos.fX, pos.fY);
		pZergling->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pZergling);
		//아군 진영으로 이동
		CZergling* ppZergling = dynamic_cast<CZergling*>(pZergling);
		if (ppZergling)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppZergling->PushOrder(rallyOrder);
		}
		//울트라 스폰
		//CSoundMgr::Get_Instance()->PlayEffect(L"Ultralisk/UltraliskBirth.wav", 0.5f);
		pos.fX += 50.f;
		CObj* pUltralisk = CAbstractFactory<CUltralisk>::Create(pos.fX, pos.fY);
		pUltralisk->SetTeamType(eTeamType::ENEMY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_ENEMY, pUltralisk);
		//아군 진영으로 이동
		CUltralisk* ppUltralisk = dynamic_cast<CUltralisk*>(pUltralisk);
		if (ppUltralisk)
		{
			Order rallyOrder;
			rallyOrder.eType = eOrderType::MOVE;
			rallyOrder.dst = { 100, 100 };
			ppUltralisk->PushOrder(rallyOrder);
		}
		m_fSpawnTimer = 0.f;
		m_iRushCount++;
	}
}

void CHive::UpdateProduction()
{
	//건설 완료시 처리(사운드, 이펙트, 기능 오픈 포함 )
	if (m_queue.empty())
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_queue.front().remainTime -= dt;

	if (m_queue.front().remainTime <= 0.f)
	{
		eCommandID done = m_queue.front().command;
		m_queue.pop_front();
		ProductionComplete(done);
	}
}

void CHive::ProductionComplete(eCommandID command)
{
	if (command == eCommandID::SCV)
	{
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"SCV/SCVBirth.wav", 1.f);

		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pSCV = CAbstractFactory<CSCV>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	}
}

void CHive::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}
