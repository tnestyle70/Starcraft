#include "pch.h"
#include "CBarracks.h"
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
#include "CUIMgr.h"

CBarracks::CBarracks()
{
}

CBarracks::~CBarracks()
{
}

void CBarracks::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 192.f;
	m_tInfo.fCY = 160.f;
	lstrcpy(m_szGreenKey, L"BARRACK_GREEN");
	lstrcpy(m_szRedKey, L"BARRACK_RED");
	m_pFrameKey = L"Barracks";
	m_eRender = RENDER_WORLD;
	m_tFrame.iStart = 0;
	m_tFrame.iFrame = 0;
	m_tFrame.iEnd = 0;
}

int CBarracks::Update()
{
	int ret = CBuilding::Update();

	//if (m_eState == eBuildingState::COMPLETE)
	//{
	//	UpdateProduction();
	//}
	UpdateProduction();
	UpdateHotKeys();

	__super::Update_Rect();

	return ret;
}

void CBarracks::Render(HDC hDC)
{
	//고스트 모드일 경우 고스트 렌더가 되도록 설정
	if (m_bGhost)
	{
		CBuilding::Render(hDC);
		return;
	}

	int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
	int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

	int iDrawX = (int)(m_tInfo.fX - m_tInfo.fCX / 2.f - iScrollX);
	int iDrawY = (int)(m_tInfo.fY - m_tInfo.fCY / 2.f - iScrollY);

	//선택 원(예: m_bSelected가 true일 때) 추후에 bmp로 교체
	if (m_bSelected)
	{
		HBRUSH oldB = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
		HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
		HPEN oldP = (HPEN)SelectObject(hDC, pen);

		int cx = iDrawX + (int)(m_tInfo.fCX * 0.5f);
		int cy = iDrawY + (int)(m_tInfo.fCY * 0.8f);   // 발밑 느낌으로 살짝 아래
		int r = (int)(max(m_tInfo.fCX, m_tInfo.fCY) * 0.55f);

		Ellipse(hDC, cx - r, cy - r / 2, cx + r, cy + r / 2);

		SelectObject(hDC, oldP);
		SelectObject(hDC, oldB);
		DeleteObject(pen);
	}

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

	RenderProgressbar(hDC);
}

void CBarracks::Release()
{
}

void CBarracks::RenderSlot(HDC hDC, int slotIndex)
{
}

int CBarracks::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CBarracks::SetBuildingData()
{
	m_eType = eBuildingType::BARRACK;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 100;
	m_fConstructDuration = 10.f;
	//타일 단위 크기
	m_iHeight = 5;
	m_iWidth = 6;
}

void CBarracks::ConstructComplete()
{
}

void CBarracks::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	// 1. 부모 클래스의 공통 슬롯을 먼저 가져오기
	CBuilding::CommandCardSlot(outSlot);

	//0번 슬롯 MARINE 생산
	outSlot[0].commandID = eCommandID::MARINE;
	outSlot[0].iconKey = TEXT("ICON_MARINE");
	outSlot[0].hotkey = 'S';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//1번 슬롯 MEDIC 생산
	outSlot[1].commandID = eCommandID::MEDIC;
	outSlot[1].iconKey = TEXT("ICON_MEDIC");
	outSlot[1].hotkey = 'D';
	outSlot[1].clickable = true;
	outSlot[1].visible = true;
	//8번 : Cancle(Queue 취소)
	outSlot[8].commandID = eCommandID::CANCLE;
	outSlot[8].iconKey = TEXT("ICON_CANCLE");
	outSlot[8].hotkey = VK_ESCAPE;
	outSlot[8].clickable = true;
	outSlot[8].visible = true;
}

bool CBarracks::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	//if (m_eState != eBuildingState::COMPLETE)
	//	return false;

	ResourceCost cost{};

	switch (command)
	{
	case eCommandID::MARINE:
		cost.mineral = 50;
		cost.gas = 0;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::MARINE, 3.f, 3.f, 50, 0 });
		return true;
		break;
	case eCommandID::MEDIC:
		cost.mineral = 50;
		cost.gas = 0;
		cost.supply = 1;
		//유닛이니까 true로 인구수 검사
		if (!CResourceMgr::Get_Instance()->TrySpend(cost, true))
			return false;
		//생산 시간
		m_queue.push_back({ eCommandID::MEDIC, 3.f, 3.f, 50, 0 });
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

void CBarracks::UpdateHotKeys()
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

void CBarracks::UpdateProduction()
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

void CBarracks::ProductionComplete(eCommandID command)
{
	if (command == eCommandID::MARINE)
	{
		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pMarine = CAbstractFactory<CMarine>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMarine);
	}
	else if (command == eCommandID::MEDIC)
	{
		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pMedic = CAbstractFactory<CMedic>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pMedic);
	}
}

void CBarracks::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}
