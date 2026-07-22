#include "pch.h"
#include "CNuclearSilo.h"
#include "CTimeMgr.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CSelectionMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CSCV.h"
#include "CUIMgr.h"
#include "CResourceMgr.h"
#include "CSoundMgr.h"
#include "CFogMgr.h"

CNuclearSilo::CNuclearSilo()
{
}

CNuclearSilo::~CNuclearSilo()
{
}

void CNuclearSilo::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 128.f;
	m_tInfo.fCY = 128.f;
	lstrcpy(m_szGreenKey, L"FACTORY_ADDON");
	lstrcpy(m_szRedKey, L"FACTORY_ADDON");
	m_pFrameKey = L"SCIENCE_SECRET_ANIM";
	m_eRender = RENDER_WORLD;
	m_tFrame.iStart = 0;
	m_tFrame.iFrame = 0;
	m_tFrame.iEnd = 7;
	m_tFrame.dwTime = 0;
	m_tFrame.dwSpeed = 100;

	m_eOriginalRace = eRaceType::RACE_TERRAN;
	m_eCurrentRace = eRaceType::RACE_TERRAN;
}

void CNuclearSilo::SetBuildingData()
{
	m_eType = eBuildingType::NUCLEAR_SILO;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 500;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 2;
	m_iWidth = 2;
}

int CNuclearSilo::Update()
{
	int ret = CBuilding::Update();

	if (m_eState == eBuildingState::CONSTRUCT)
	{
		UpdateProduction();
		UpdateAnimation();
	}
	UpdateHotKeys();

	__super::Update_Rect();

	return ret;
}

void CNuclearSilo::Render(HDC hDC)
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

void CNuclearSilo::Release()
{
}

void CNuclearSilo::RenderSlot(HDC hDC, int slotIndex)
{
}

int CNuclearSilo::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CNuclearSilo::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);

	//0번 슬롯 핵 미사일 장전
	outSlot[0].commandID = eCommandID::NUCLEAR_MISSILE;
	outSlot[0].iconKey = TEXT("ICON_SCV");
	outSlot[0].hotkey = 'W';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//8번 : Cancle(Queue 취소)
	outSlot[8].commandID = eCommandID::CANCLE;
	outSlot[8].iconKey = TEXT("ICON_CANCLE");
	outSlot[8].hotkey = VK_ESCAPE;
	outSlot[8].clickable = true;
	outSlot[8].visible = true;
}

void CNuclearSilo::UpdateHotKeys()
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

bool CNuclearSilo::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	//if (m_eState != eBuildingState::COMPLETE)
	//	return false;

	ResourceCost cost{};

	switch (command)
	{
	case eCommandID::NUCLEAR_MISSILE:
		cost.mineral = 50;
		cost.gas = 50;
		cost.supply = 8;
		//유닛 아니므로 false
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, false))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::NUCLEAR_MISSILE, 10.f, 10.f, 50, 50 });
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

void CNuclearSilo::UpdateAnimation()
{
	if (m_bProducing)
	{
		DWORD now = GetTickCount();
		// 생산 중: 애니메이션 진행
		if (now - m_tFrame.dwTime >= m_tFrame.dwSpeed)
		{
			//m_tFrame.iFrame++;
			if (!m_bReverse)
				m_tFrame.iFrame++;
			else
				m_tFrame.iFrame--;

			if (m_tFrame.iFrame >= m_tFrame.iEnd)
				m_bReverse = true;
			else if (m_tFrame.iFrame <= 2)
				m_bReverse = false;

			m_tFrame.dwTime = now;
		}
	}
	else
	{
		m_tFrame.iFrame = 2;
	}
}

void CNuclearSilo::UpdateProduction()
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

void CNuclearSilo::ProductionComplete(eCommandID command)
{
	if (command == eCommandID::NUCLEAR_MISSILE)
	{
		//사운드 재생 + 고스트 핵 잠금 풀기
		CSoundMgr::Get_Instance()->PlayEffect(L"Advisor/NuclearMissileReady.wav", 1.f);
		CObjMgr::Get_Instance()->SetMissileReady(true);
	}
}

void CNuclearSilo::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}

void CNuclearSilo::PlayCompleteSound()
{
	CSoundMgr::Get_Instance()->PlayEffect(L"Build/tadUpd03.wav", 1.f);
}
