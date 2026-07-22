#include "pch.h"
#include "CUltraliskDen.h"
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
#include "CUltralisk.h"

CUltraliskDen::CUltraliskDen()
{
}

CUltraliskDen::~CUltraliskDen()
{
}

void CUltraliskDen::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 192.f;
	m_tInfo.fCY = 224.f;
	lstrcpy(m_szGreenKey, L"ULTRALISK_DEN");
	lstrcpy(m_szRedKey, L"COMMANDCENTER_RED");
	lstrcpy(m_szConstructKey, L"COMMANDCENTER_CONSTRUCT");
	m_pFrameKey = L"ULTRALISK_DEN";
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

void CUltraliskDen::PlayCompleteSound()
{
	//이펙트와 사운드 재생
	CSoundMgr::Get_Instance()->PlayEffect(L"Overload/zovDth00.wav", 0.5f);
}

void CUltraliskDen::SetBuildingData()
{
	m_eType = eBuildingType::ULTRALISK_DEN;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 1000;
	m_iHP = m_iMaxHP;
	m_fConstructDuration = 2.f;
	//타일 단위 크기
	m_iHeight = 3;
	m_iWidth = 3;
}

int CUltraliskDen::Update()
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

void CUltraliskDen::Render(HDC hDC)
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

void CUltraliskDen::Release()
{
}

void CUltraliskDen::RenderSlot(HDC hDC, int slotIndex)
{
}

int CUltraliskDen::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CUltraliskDen::CommandCardSlot(std::vector<CommandSlot>& outSlot)
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

void CUltraliskDen::UpdateHotKeys()
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

bool CUltraliskDen::ExecuteCommand(eCommandID command, CommandContext& context)
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

void CUltraliskDen::UpdateProduction()
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

void CUltraliskDen::ProductionComplete(eCommandID command)
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

void CUltraliskDen::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}

void CUltraliskDen::SpawnZergUnits()
{
	if (m_eTeamType == eTeamType::ALLY)
		return;

	if (!CObjMgr::Get_Instance()->IsStartZergRush())
		return;

	if (m_iSpawnCount == 2) //두 번만 스폰하도록 설정
		return;

	float dt = CTimeMgr::Get_Instance()->GetDT();
	m_fSpawnTimer += dt;
	if (m_fSpawnTimer >= m_fSpawnInterval)
	{
		Vec2 pos = Get_Pos();
		//울트라 스폰
		CSoundMgr::Get_Instance()->PlayEffect(L"Ultralisk/UltraliskBirth.wav", 0.5f);
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
		m_iSpawnCount++;
	}
}
