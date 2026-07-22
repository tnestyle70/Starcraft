#include "pch.h"
#include "CEngineeringBay.h"
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

CEngineeringBay::CEngineeringBay()
{
}

CEngineeringBay::~CEngineeringBay()
{
}

void CEngineeringBay::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 192.f;
	m_tInfo.fCY = 160.f;
	lstrcpy(m_szGreenKey, L"ENGINEERING_BAY");
	lstrcpy(m_szRedKey, L"ENGINEERING_BAY");
	m_pFrameKey = L"ENGINEERING_BAY_ANIM";
	m_eRender = RENDER_WORLD;
	m_tFrame.iStart = 0;
	m_tFrame.iFrame = 0;
	m_tFrame.iCol = 0;
	m_tFrame.iEnd = 7;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	m_eOriginalRace = eRaceType::RACE_TERRAN;
	m_eCurrentRace = eRaceType::RACE_TERRAN;
}

void CEngineeringBay::SetBuildingData()
{
	m_eType = eBuildingType::ENGINEERING_BAY;
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

int CEngineeringBay::Update()
{
	int ret = CBuilding::Update();
	//건설 완료된 상태에서만 업그레이드 가능
	if (m_eState == eBuildingState::CONSTRUCT)
	{
		UpdateProduction();
		UpdateAnimation();
	}
	UpdateHotKeys();

	__super::Update_Rect();

	return ret;
}

void CEngineeringBay::Render(HDC hDC)
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

void CEngineeringBay::Release()
{
}

void CEngineeringBay::UpdateAnimation()
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
				m_tFrame.iFrame = 2;  // 2번 프레임으로 루프
			m_tFrame.dwTime = now;
		}
	}
	else
	{
		// idle 상태: 고정 프레임 (0 또는 1)
		m_tFrame.iFrame = 1;  // 또는 1 (idle 프레임 번호)
	}
}


void CEngineeringBay::RenderSlot(HDC hDC, int slotIndex)
{
}

int CEngineeringBay::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CEngineeringBay::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);
	//0번 슬롯 attack upgrade
	outSlot[0].commandID = eCommandID::ATTACK_UPGRADE;
	outSlot[0].iconKey = TEXT("ICON_ATTACK_UPGRADE");
	outSlot[0].hotkey = 'W';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//1번 슬롯 armor upgrade
	outSlot[1].commandID = eCommandID::ARMOR_UPGRADE;
	outSlot[1].iconKey = TEXT("ICON_ARMOR_UPGRADE");
	outSlot[1].hotkey = 'E';
	outSlot[1].clickable = true;
	outSlot[1].visible = true;
}

void CEngineeringBay::UpdateHotKeys()
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

bool CEngineeringBay::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	if (m_eState != eBuildingState::CONSTRUCT)
		return false;

	ResourceCost cost{};

	switch (command)
	{
	case eCommandID::ATTACK_UPGRADE:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이므로 인구수 검사 X
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::ATTACK_UPGRADE, 10.f, 10.f, 50, 50 });
		return true;
	case eCommandID::ARMOR_UPGRADE:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 0;
		//건물이므로 인구수 검사 X
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::ARMOR_UPGRADE, 10.f, 10.f, 50, 50 });
		return true;
	default:
		break;
	}
	return false;
}

void CEngineeringBay::UpdateProduction()
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

void CEngineeringBay::ProductionComplete(eCommandID command)
{
	if (command == eCommandID::ATTACK_UPGRADE)
	{
		//업그레이드 매니저로 한 번에 관리하기
		CObjMgr::Get_Instance()->SetAttackUpgrade(1);
	}
	else if (command == eCommandID::ARMOR_UPGRADE)
	{
		//업그레이드 매니저로 한 번에 관리하기
		CObjMgr::Get_Instance()->SetArmorUpgrade(1);
	}
}

void CEngineeringBay::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}
