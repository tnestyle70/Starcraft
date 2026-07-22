#include "pch.h"
#include "CAcademy.h"
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

CAcademy::CAcademy()
{
}

CAcademy::~CAcademy()
{
}

void CAcademy::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 96.f;
	m_tInfo.fCY = 128.f;
	lstrcpy(m_szGreenKey, L"ACADEMY");
	lstrcpy(m_szRedKey, L"ACADEMY");
	m_pFrameKey = L"ACADEMY_ANIM";
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

void CAcademy::SetBuildingData()
{
	m_eType = eBuildingType::ACADEMY;
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

int CAcademy::Update()
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

void CAcademy::Render(HDC hDC)
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

void CAcademy::Release()
{
}

void CAcademy::UpdateAnimation()
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


void CAcademy::RenderSlot(HDC hDC, int slotIndex)
{
}

int CAcademy::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CAcademy::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);
	//0번 슬롯 attack upgrade
	outSlot[0].commandID = eCommandID::U238;
	outSlot[0].iconKey = TEXT("ICON_ATTACK_UPGRADE");
	outSlot[0].hotkey = 'W';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//1번 슬롯 armor upgrade
	outSlot[1].commandID = eCommandID::STEAMPACK;
	outSlot[1].iconKey = TEXT("ICON_ARMOR_UPGRADE");
	outSlot[1].hotkey = 'E';
	outSlot[1].clickable = true;
	outSlot[1].visible = true;
	//0번 슬롯 attack upgrade
	outSlot[3].commandID = eCommandID::RESTORATION;
	outSlot[3].iconKey = TEXT("ICON_ATTACK_UPGRADE");
	outSlot[3].hotkey = 'S';
	outSlot[3].clickable = true;
	outSlot[3].visible = true;
	//1번 슬롯 armor upgrade
	outSlot[4].commandID = eCommandID::OPTICAL_FLARE;
	outSlot[4].iconKey = TEXT("ICON_ARMOR_UPGRADE");
	outSlot[4].hotkey = 'D';
	outSlot[4].clickable = true;
	outSlot[4].visible = true;
	//0번 슬롯 attack upgrade
	outSlot[6].commandID = eCommandID::CADUCEUS_REACTOR;
	outSlot[6].iconKey = TEXT("ICON_ATTACK_UPGRADE");
	outSlot[6].hotkey = 'X';
	outSlot[6].clickable = true;
	outSlot[6].visible = true;
}

void CAcademy::UpdateHotKeys()
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

bool CAcademy::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	if (m_eState != eBuildingState::CONSTRUCT)
		return false;

	ResourceCost cost{};

	switch (command)
	{
	case eCommandID::U238:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이므로 인구수 검사 X
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::U238, 10.f, 10.f, 50, 0 });
		return true;
	case eCommandID::STEAMPACK:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이므로 인구수 검사 X
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::STEAMPACK, 10.f, 10.f, 50, 50 });
		return true;
	case eCommandID::RESTORATION:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이므로 인구수 검사 X
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::RESTORATION, 10.f, 10.f, 50, 0 });
		return true;
	case eCommandID::OPTICAL_FLARE:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이므로 인구수 검사 X
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::OPTICAL_FLARE, 10.f, 10.f, 50, 0 });
		return true;
	case eCommandID::CADUCEUS_REACTOR:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이므로 인구수 검사 X
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::CADUCEUS_REACTOR, 10.f, 10.f, 50, 0 });
		return true;
	default:
		break;
	}
	return false;
}

void CAcademy::UpdateProduction()
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

void CAcademy::ProductionComplete(eCommandID command)
{
	if (command == eCommandID::U238)
	{
		//마린 사거리 업그레이드 
		CSoundMgr::Get_Instance()->PlayEffect(L"Advisor/UpgradeComplete.wav", 1.f);
	}
	else if (command == eCommandID::STEAMPACK)
	{
		//스팀팩 업그레이드
		//사운드 재생 + 스팀팩 잠금 풀기
		CSoundMgr::Get_Instance()->PlayEffect(L"Advisor/UpgradeComplete.wav", 1.f);
		CObjMgr::Get_Instance()->SetStimPackReady(true);
	}
	else if (command == eCommandID::RESTORATION)
	{
		CSoundMgr::Get_Instance()->PlayEffect(L"Advisor/UpgradeComplete.wav", 1.f);
	}
	else if (command == eCommandID::OPTICAL_FLARE)
	{
		CSoundMgr::Get_Instance()->PlayEffect(L"Advisor/UpgradeComplete.wav", 1.f);
	}
	else if (command == eCommandID::CADUCEUS_REACTOR)
	{
		CSoundMgr::Get_Instance()->PlayEffect(L"Advisor/UpgradeComplete.wav", 1.f);
	}
}

void CAcademy::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}
