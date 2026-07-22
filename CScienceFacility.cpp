#include "pch.h"
#include "CScienceFacility.h"
#include "CTimeMgr.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CSelectionMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CSCV.h"
#include "CMarine.h"
#include "CMedic.h"
#include "CGhost.h"
#include "CFirebat.h"
#include "CUIMgr.h"
#include "CSoundMgr.h"

CScienceFacility::CScienceFacility()
{
}

CScienceFacility::~CScienceFacility()
{
}

void CScienceFacility::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 128.f;
	m_tInfo.fCY = 96.f;
	lstrcpy(m_szGreenKey, L"SCIENCE_FACILITY");
	lstrcpy(m_szRedKey, L"SCIENCE_FACILITY");
	m_pFrameKey = L"SCIENCE_FACILITY_ANIM";
	m_eRender = RENDER_WORLD;
	m_tFrame.iStart = 0;
	m_tFrame.iFrame = 0;
	m_tFrame.iCol = 0;
	m_tFrame.iEnd = 3;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	m_eOriginalRace = eRaceType::RACE_TERRAN;
	m_eCurrentRace = eRaceType::RACE_TERRAN;
}

void CScienceFacility::SetBuildingData()
{
	m_eType = eBuildingType::SCIENCE_FACILITY;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 1000;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 3;
	m_iWidth = 3;
}

int CScienceFacility::Update()
{
	int ret = CBuilding::Update();
	//건설 완료된 상태에서만 유닛 생산
	if (m_eState == eBuildingState::CONSTRUCT)
	{
		UpdateProduction();
		UpdateAnimation();
	}
	UpdateHotKeys();

	__super::Update_Rect();

	return ret;
}

void CScienceFacility::Render(HDC hDC)
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
		RGB(255, 0, 255));
}

void CScienceFacility::Release()
{
}

void CScienceFacility::BuildAddOn()
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

void CScienceFacility::UpdateAnimation()
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
		}
	}
	else
	{
		// idle 상태: 고정 프레임 (0 또는 1)
		m_tFrame.iFrame = 1;  // 또는 1 (idle 프레임 번호)
	}
}


void CScienceFacility::RenderSlot(HDC hDC, int slotIndex)
{
}

int CScienceFacility::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CScienceFacility::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);
	//건설 중일 경우에는 표시 X
	if (m_eState == eBuildingState::CONSTRUCTING)
		return;

	//0번 슬롯 EMP
	outSlot[0].commandID = eCommandID::EMP;
	outSlot[0].iconKey = TEXT("ICON_MARINE");
	outSlot[0].hotkey = 'W';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//1번 슬롯 IRRADIATE
	outSlot[1].commandID = eCommandID::IRRADIATE;
	outSlot[1].iconKey = TEXT("ICON_MEDIC");
	outSlot[1].hotkey = 'E';
	outSlot[1].clickable = true;
	outSlot[1].visible = true;
	//2번 슬롯 TITAN 생산
	outSlot[2].commandID = eCommandID::TITAN;
	outSlot[2].iconKey = TEXT("ICON_FIREBAT");
	outSlot[2].hotkey = 'R';
	outSlot[2].clickable = true;
	outSlot[2].visible = true;
	//6번 슬롯 Science Secret Addon 
	outSlot[6].commandID = eCommandID::SCIENCE_SECRET;
	outSlot[6].iconKey = TEXT("ICON_GHOST");
	outSlot[6].hotkey = 'X';
	outSlot[6].clickable = true;
	outSlot[6].visible = true;
	//6번 슬롯 Science Phycics Addon 
	outSlot[7].commandID = eCommandID::SCIENCE_PHYSICS;
	outSlot[7].iconKey = TEXT("ICON_GHOST");
	outSlot[7].hotkey = 'C';
	outSlot[7].clickable = true;
	outSlot[7].visible = true;
	//8번 : Cancle(Queue 취소)
	outSlot[8].commandID = eCommandID::CANCLE;
	outSlot[8].iconKey = TEXT("ICON_CANCLE");
	outSlot[8].hotkey = VK_ESCAPE;
	outSlot[8].clickable = true;
	outSlot[8].visible = true;
}

void CScienceFacility::UpdateHotKeys()
{
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
}

bool CScienceFacility::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	if (m_eState != eBuildingState::CONSTRUCT)
		return false;

	ResourceCost cost{};

	switch (command)
	{
	case eCommandID::SCIENCE_SECRET:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이니까 false
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		BuildScienceSecret();
		return true;
	case eCommandID::SCIENCE_PHYSICS:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이니까 false
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		BuildSciencePhysics();
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
	default:
		break;
	}
	return false;
}

void CScienceFacility::UpdateProduction()
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

void CScienceFacility::ProductionComplete(eCommandID command)
{
	if (command == eCommandID::EMP)
	{
		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pMarine = CAbstractFactory<CMarine>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarine);
	}
	else if (command == eCommandID::IRRADIATE)
	{
		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pMedic = CAbstractFactory<CMedic>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMedic);
	}
	else if (command == eCommandID::TITAN)
	{
		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pFirebat = CAbstractFactory<CFirebat>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pFirebat);
	}
}

void CScienceFacility::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}

void CScienceFacility::BuildScienceSecret()
{
	CBuilding* pAddOn = CBuildingFactory::Create(eBuildingType::SCIENCE_SECRET);
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

void CScienceFacility::BuildSciencePhysics()
{
	CBuilding* pAddOn = CBuildingFactory::Create(eBuildingType::SCIENCE_PHYSICS);
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
