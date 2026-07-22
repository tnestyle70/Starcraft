#include "pch.h"
#include "CBunker.h"
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
#include "CSelectionMgr.h"
#include "CMainUI.h"

CBunker::CBunker()
{
}

CBunker::~CBunker()
{
}

void CBunker::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 96.f;
	m_tInfo.fCY = 128.f;
	lstrcpy(m_szGreenKey, L"BUNKER");
	lstrcpy(m_szRedKey, L"BUNKER");
	m_pFrameKey = L"BUNKER_ANIM";
	m_eRender = RENDER_WORLD;
	m_tFrame.iStart = 0;
	m_tFrame.iFrame = 1;
	m_tFrame.iCol = 0;
	m_tFrame.iEnd = 1;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	m_eOriginalRace = eRaceType::RACE_TERRAN;
	m_eCurrentRace = eRaceType::RACE_TERRAN;
}

void CBunker::SetBuildingData()
{
	m_eType = eBuildingType::BUNKER;
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

int CBunker::Update()
{
	int ret = CBuilding::Update();

	//벙커에 탑승한 유닛들 위치 고정
	for (auto& pUnit : m_vecLoadUnits)
	{
		pUnit->Set_Pos(m_tInfo.fX, m_tInfo.fY);
	}
	//탑승한 유닛 보여주기
	ShowLoadUnits();

	UpdateHotKeys();

	__super::Update_Rect();

	return ret;
}

void CBunker::Render(HDC hDC)
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

void CBunker::Release()
{
}

void CBunker::RenderSlot(HDC hDC, int slotIndex)
{
}

int CBunker::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CBunker::LoadUnit(CUnit* pUnit)
{
	m_vecLoadUnits.push_back(pUnit);

	pUnit->SetVisible(false);
	pUnit->SetGarrisoned(true);
	pUnit->SetSelectable(false);
	pUnit->SetUnitState(eUnitState::GARRISONED);
	pUnit->Set_Pos(m_tInfo.fX, m_tInfo.fY);
}

void CBunker::ShowLoadUnits()
{
	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
	if (selected.empty() || selected[0] != this)
		return;
	MultiUnitUIInfo info;
	info.IsVisible = true;
	info.IsLoadUnit = true;
	info.iUnitCount = (int)m_vecLoadUnits.size();
	for (int i = 0; i < info.iUnitCount; ++i)
	{
		CUnit* pUnit = dynamic_cast<CUnit*>(m_vecLoadUnits[i]);
		if (pUnit)
		{
			info.units[i].eRaceType = m_eOriginalRace;
			info.units[i].eType = pUnit->Get_UnitType();
			info.units[i].iHP = pUnit->Get_HP();
			info.units[i].iMaxHP = pUnit->Get_MaxHP();
			info.units[i].pUnit = pUnit;
		}
	}
	CMainUI::Get_Instance()->SetMultiUnitUIInfo(info);
	//단일 유닛 UI 숨기기
	UnitUIInfo unitInfo;
	unitInfo.IsVisible = false;
	CMainUI::Get_Instance()->SetUnitUIInfo(unitInfo);
}

void CBunker::UnloadUnit()
{
	if (m_vecLoadUnits.empty())
		return;
	float fOffSet = 0.f;
	for (auto* pUnit : m_vecLoadUnits)
	{
		if (!pUnit)
			continue;
		pUnit->SetVisible(true);
		pUnit->SetGarrisoned(false);
		pUnit->SetSelectable(true);
		pUnit->SetUnitState(eUnitState::IDLE);
		pUnit->Set_Pos(m_tInfo.fX + fOffSet, m_tInfo.fY  + 50.f);
		fOffSet += 20.f;
	}

	m_vecLoadUnits.clear();
}

void CBunker::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);
	//건설 중일 경우에는 표시 X
	if (m_eState == eBuildingState::CONSTRUCTING)
		return;

	//6번 슬롯
	if (m_eState == eBuildingState::CONSTRUCT)
	{
		outSlot[5].commandID = eCommandID::RALLY;
		outSlot[5].iconKey = TEXT("ICON_RALLY");
		outSlot[5].hotkey = 'F';
		outSlot[5].clickable = true;
		outSlot[5].visible = true;
	}
}

void CBunker::UpdateHotKeys()
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

bool CBunker::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	if (m_eState != eBuildingState::CONSTRUCT)
		return false;

	switch (command)
	{
	case eCommandID::RALLY:
		UnloadUnit(); //유닛 벙커에서 내리기
		return true;
	default:
		break;
	}
	return false;
}

void CBunker::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}