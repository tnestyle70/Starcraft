#include "pch.h"
#include "CCommandCenter.h"
#include "CTimeMgr.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CSelectionMgr.h"
#include "CAbstractFactory.h"
#include "CObjMgr.h"
#include "CSCV.h"
#include "CUIMgr.h"

CCommandCenter::CCommandCenter()
{
}

CCommandCenter::~CCommandCenter()
{
}

void CCommandCenter::Initialize()
{
	CBuilding::Initialize();
	m_tInfo.fCX = 128.f;
	m_tInfo.fCY = 160.f;
	m_pFrameKey = L"CommandCenter";
	m_eRender = RENDER_WORLD;
	m_tFrame.iStart = 0;
	m_tFrame.iFrame = 0;
	m_tFrame.iEnd = 0;
}

int CCommandCenter::Update()
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

void CCommandCenter::Render(HDC hDC)
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
}

void CCommandCenter::Release()
{
}

void CCommandCenter::RenderSlot(HDC hDC, int slotIndex)
{
}

int CCommandCenter::GetIconIndex(eCommandID eCommand)
{
	return 0;
}

void CCommandCenter::SetBuildingData()
{
	m_eType = eBuildingType::COMMAND_CENTER;
	//비용, 스탯
	m_tCost.mineral = 100;
	m_tCost.gas = 0;
	m_tCost.supply = 0;
	m_iMaxHP = 100;
	m_fConstructDuration = 10.f;
	//타일 단위 크기
	m_iHeight = 4;
	m_iWidth = 4;
}

void CCommandCenter::ConstructComplete()
{
}

void CCommandCenter::CommandCardSlot(std::vector<CommandSlot>& outSlot)
{
	outSlot.clear();
	outSlot.resize(9);
	//미리 값 채우기
	for (int i = 0; i < 9; ++i)
	{
		outSlot[i].slotIndex = i;
		outSlot[i].commandID = eCommandID::NONE;
		outSlot[i].iconKey = TEXT("");
		outSlot[i].hotkey = 0;
		outSlot[i].clickable = false;
		outSlot[i].visible = false;
	}
	//0번 슬롯 SCV 생산
	outSlot[0].commandID = eCommandID::SCV;
	outSlot[0].iconKey = TEXT("ICON_SCV");
	outSlot[0].hotkey = 'S';
	outSlot[0].clickable = true;
	outSlot[0].visible = true;
	//8번 : Cancle(Queue 취소)
	outSlot[7].commandID = eCommandID::CANCLE;
	outSlot[7].iconKey = TEXT("ICON_CANCLE");
	outSlot[7].hotkey = VK_ESCAPE;
	outSlot[7].clickable = true;
	outSlot[7].visible = true;
}

bool CCommandCenter::ExecuteCommand(eCommandID command, CommandContext& context)
{
	//건물이 다 지어진 이후에 건설 가능
	//if (m_eState != eBuildingState::COMPLETE)
	//	return false;

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
		m_queue.push_back({ eCommandID::SCV, 3.f, 3.f, 50, 0 });
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

void CCommandCenter::UpdateHotKeys()
{
	//S키를 눌렀을 경우 SCV 생산
	if (!CInputMgr::Get_Instance()->KeyDown(S_KEY))
		return;

	auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
	if (selected.size() != 1)
		return;

	CObj* pSelected = selected[0];
	CCommandCenter* cc = dynamic_cast<CCommandCenter*>(pSelected);
	if (!cc)
		return;
	CommandContext context{};
	cc->ExecuteCommand(eCommandID::SCV, context);
	//UIMgr 쪽에 버튼 클릭 피드백 전달(0번 슬롯)
	CUIMgr::Get_Instance()->SetButtonFeedback(0, true);
}

void CCommandCenter::UpdateProduction()
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
		ConstructComplete(done);
	}
}

void CCommandCenter::ConstructComplete(eCommandID command)
{
	if (command == eCommandID::SCV)
	{
		Vec2 pos = Get_Pos();
		pos.fX += 100.f;
		CObj* pSCV = CAbstractFactory<CSCV>::Create(pos.fX, pos.fY);
		CObjMgr::Get_Instance()->Add_Object(OBJ_UNIT, pSCV);
	}
}

void CCommandCenter::Destroy()
{
	m_eState = eBuildingState::DESTROY;
}
