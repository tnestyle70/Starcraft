#include "pch.h"
#include "CHatchery.h"
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
#include "CLava.h"

CHatchery::CHatchery()
{
}

CHatchery::~CHatchery()
{
}

void CHatchery::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 192.f;
	m_tInfo.fCY = 160.f;
	lstrcpy(m_szGreenKey, L"Hatchery");
	lstrcpy(m_szRedKey, L"COMMANDCENTER_RED");
	lstrcpy(m_szConstructKey, L"COMMANDCENTER_CONSTRUCT");
	m_pFrameKey = L"Hatchery";
	m_pLareKey = L"Lare";
	m_pHiveKey = L"HIVE";
	m_eRender = RENDER_WORLD;
	m_tFrame.iFrame = 0;
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 1;
	m_tFrame.iCol = 0;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	m_eCommandCardState = eCommandCardState::HATCHERY;

	m_eOriginalRace = eRaceType::RACE_ZERG;
	m_eCurrentRace = eRaceType::RACE_ZERG;
}

void CHatchery::PlayCompleteSound()
{
	CSoundMgr::Get_Instance()->PlayEffect(L"Overload/zovDth00.wav", 0.5f);
}

void CHatchery::SetBuildingData()
{
	m_eType = eBuildingType::HATCHERY;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 1500;
	m_iHP = m_iMaxHP;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 3;
	m_iWidth = 3;
}

int CHatchery::Update()
{
	int ret = CBuilding::Update();
	//건설이 완료되었을 경우에만 생산
	if (m_eState == eBuildingState::CONSTRUCT)
	{
		CreateLava();
		UpdateProduction();
	}
	UpdateHotKeys();

	MutateLair();
	MutateHive();

	if (m_eTeamType == eTeamType::ENEMY)
	{
		SpawnZergUnits();
	}

	__super::Update_Rect();

	return ret;
}

void CHatchery::Render(HDC hDC)
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

void CHatchery::Release()
{
}

void CHatchery::RenderSlot(HDC hDC, int slotIndex)
{
}

int CHatchery::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CHatchery::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);

	if (m_eState == eBuildingState::CONSTRUCTING)
		return;

	switch (m_eCommandCardState)
	{
	case eCommandCardState::HATCHERY:
		//6번 슬롯 Lare
		outSlot[6].commandID = eCommandID::LAIR;
		outSlot[6].iconKey = TEXT("ICON_SCV");
		outSlot[6].hotkey = 'X';
		outSlot[6].clickable = true;
		outSlot[6].visible = true;
		break;
	case eCommandCardState::LAIR:
		//6번 슬롯 Lare
		outSlot[6].commandID = eCommandID::HIVE;
		outSlot[6].iconKey = TEXT("ICON_SCV");
		outSlot[6].hotkey = 'X';
		outSlot[6].clickable = true;
		outSlot[6].visible = true;
		break;
	case eCommandCardState::HIVE:
		break;
	default:
		break;
	}
	//0번 슬롯 Lava
	outSlot[0].commandID = eCommandID::LAVA;
	outSlot[0].iconKey = TEXT("ICON_SCV");
	outSlot[0].hotkey = 'W';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//1번 슬롯 Rally
	outSlot[1].commandID = eCommandID::RALLY;
	outSlot[1].iconKey = TEXT("ICON_SCV");
	outSlot[1].hotkey = 'E';
	outSlot[1].clickable = true;
	outSlot[1].visible = true;
	//8번 : Cancle(Queue 취소)
	outSlot[8].commandID = eCommandID::CANCLE;
	outSlot[8].iconKey = TEXT("ICON_CANCLE");
	outSlot[8].hotkey = VK_ESCAPE;
	outSlot[8].clickable = true;
	outSlot[8].visible = true;
}

void CHatchery::UpdateHotKeys()
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

bool CHatchery::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	if (m_eState != eBuildingState::CONSTRUCT)
		return false;

	ResourceCost cost{};

	switch (command)
	{
	case eCommandID::LAVA:
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
	case eCommandID::LAIR:
		m_eCommandCardState = eCommandCardState::LAIR;
		m_bMutatingLair = true;
		return true;
	case eCommandID::HIVE:
		m_eCommandCardState = eCommandCardState::HIVE;
		m_bMutatingHive = true;
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

void CHatchery::SpawnZergUnits()
{
	if (m_eTeamType == eTeamType::ALLY)
		return;

	if (!CObjMgr::Get_Instance()->IsStartZergRush())
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_fSpawnTimer += dt;
	if (m_fSpawnTimer >= m_fSpawnInterval)
	{
		Vec2 pos = Get_Pos();
		//오버로드 스폰
		CSoundMgr::Get_Instance()->PlayEffect(L"Overload/OverloadBirth.wav", 0.5f);
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
		m_fSpawnTimer = 0.f;
	}
}

void CHatchery::UpdateProduction()
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

void CHatchery::ProductionComplete(eCommandID command)
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

void CHatchery::CreateLava()
{
	if (!m_bCreateLava)
		return;

	Vec2 pos = Get_Pos();
	pos.fX += 100.f;
	pos.fY -= 25.f;
	CObj* pLava = CAbstractFactory<CLava>::Create(pos.fX, pos.fY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT,pLava);
	pos.fY += 25.f;
	pLava = CAbstractFactory<CLava>::Create(pos.fX, pos.fY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pLava);
	pos.fY += 25.f;
	pLava = CAbstractFactory<CLava>::Create(pos.fX, pos.fY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pLava);
	pos.fY += 25.f;
	pLava = CAbstractFactory<CLava>::Create(pos.fX, pos.fY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pLava);
	pos.fY += 25.f;
	pLava = CAbstractFactory<CLava>::Create(pos.fX, pos.fY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pLava);
	pos.fY += 25.f;
	pLava = CAbstractFactory<CLava>::Create(pos.fX, pos.fY);
	CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pLava);

	m_bCreateLava = false;
}

void CHatchery::MutateLair()
{
	if (m_eType == eBuildingType::LAIR)
		return;
	// 4. 건설 시작
	if (m_bMutatingLair && m_eState != eBuildingState::CONSTRUCTING)
	{
		m_bMutatingLair = false;
		m_bConstructing = true;
		m_eState = eBuildingState::CONSTRUCTING;
		m_eHatcheryType = eHatcheryType::LAIR;
		m_fConstructRemain = m_fConstructDuration;
		m_iHP = 1;
		return;
	}
	else if (m_eHatcheryType == eHatcheryType::LAIR &&m_fConstructRemain <= 0.f)
	{
		m_bMutatingLair = false;
		m_pFrameKey = m_pLareKey;
		m_eHatcheryType = eHatcheryType::LAIR;
		//라바 다시 생성
		m_bCreateLava = true;
		//빌딩 타입 변경
		m_eType = eBuildingType::LAIR;
	}
}

void CHatchery::MutateHive()
{
	if (m_eType == eBuildingType::HIVE)
		return;
	// 4. 건설 시작
	if (m_bMutatingHive && m_eState != eBuildingState::CONSTRUCTING)
	{
		m_bMutatingHive = false;
		m_bConstructing = true;
		m_eState = eBuildingState::CONSTRUCTING;
		m_eHatcheryType = eHatcheryType::HIVE;
		m_fConstructRemain = m_fConstructDuration;
		m_iHP = 1;
		return;
	}
	else if (m_eHatcheryType == eHatcheryType::HIVE && m_fConstructRemain <= 0.f)
	{
		m_bMutatingHive = false;
		m_pFrameKey = m_pHiveKey;
		m_eHatcheryType = eHatcheryType::HIVE;
		//라바 다시 생성
		m_bCreateLava = true;
		//빌딩 타입 변경
		m_eType = eBuildingType::HIVE;
	}
}

void CHatchery::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}
