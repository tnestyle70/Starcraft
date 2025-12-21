#include "pch.h"
#include "CSelectionMgr.h"
#include "CUnit.h"
#include "CBuilding.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CInputMgr.h"

CSelectionMgr* CSelectionMgr::m_pInstance = nullptr;

CSelectionMgr::CSelectionMgr()
{
}

CSelectionMgr::~CSelectionMgr()
{
}

POINT CSelectionMgr::GetMouseClient()
{
	POINT pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	return pt;
}

RECT CSelectionMgr::NormalizeRect(POINT a, POINT b)
{
	RECT r;
	r.left = min(a.x, b.x);
	r.top = min(a.y, b.y);
	r.right = max(a.x, b.x);
	r.bottom = max(a.y, b.y);
	return r;
}

bool CSelectionMgr::IsClickSelection(const RECT& r)
{
	const int w = abs(r.right - r.left);
	const int h = abs(r.bottom - r.top);
	return (w <= 4 && h <= 4);
}

void CSelectionMgr::ClearSelection()
{
	for (auto* pObj : m_vecSelected)
	{
		if (pObj)
			pObj->SetSelected(false);
	}
	m_vecSelected.clear();
}

void CSelectionMgr::SelectSingleAt(const POINT& clientPt)
{
	//이전 선택 초기화
	ClearSelection();

	const float fScrX = CScrollMgr::Get_Instance()->Get_ScrollX();
	const float fScrY = CScrollMgr::Get_Instance()->Get_ScrollY();

	Vec2 vWorld{ clientPt.x + fScrX, clientPt.y + fScrY };

	CObj* pHit = CObjMgr::Get_Instance()->PickObjAt(vWorld);
	if (pHit) 
	{
		pHit->SetSelected(true);
		m_vecSelected.push_back(pHit);
	}
}

void CSelectionMgr::Update()
{
	if (CInputMgr::Get_Instance()->KeyDown(LEFT_MOUSE))
		OnLMouseDown();

	if (m_bDragging && CInputMgr::Get_Instance()->KeyPress(LEFT_MOUSE))
		OnMouseMove();

	if (CInputMgr::Get_Instance()->KeyUp(LEFT_MOUSE))
		OnLMouseUp();
}

void CSelectionMgr::OnLMouseDown() //드래그 중
{
	m_bDragging = true;
	m_ptStart = GetMouseClient();
	m_ptCur = m_ptStart;
	m_rcScreen = NormalizeRect(m_ptStart, m_ptCur);
	//shift 누르고 있지 않으면 기존 선택 해제
	//if (!CInputMgr::Get_Instance()->KeyPress(VK_SHIFT))
	//	ClearSelection();
}

void CSelectionMgr::OnMouseMove()
{
	if (!m_bDragging) return;

	m_ptCur = GetMouseClient();
	m_rcScreen = NormalizeRect(m_ptStart, m_ptCur);
}

void CSelectionMgr::OnLMouseUp() //드래그 종료
{
	if (!m_bDragging) return;

	m_ptCur = GetMouseClient();
	m_rcScreen = NormalizeRect(m_ptStart, m_ptCur);
	m_bDragging = false;
	//클릭 선택
	if (IsClickSelection(m_rcScreen))
	{
		SelectSingleAt(m_ptCur);
		return;
	}
	//중심이 rect 안에 존재하는 유닛 전부 선택
	ClearSelection();

	//박스 선택 Client의 rect를 world의 rect로 변경
	float fScrX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrY = CScrollMgr::Get_Instance()->Get_ScrollY();

	RECT rcWorld = m_rcScreen;
	rcWorld.left += (LONG)fScrX;
	rcWorld.top += (LONG)fScrY;
	rcWorld.right += (LONG)fScrX;
	rcWorld.bottom += (LONG)fScrY;
	
	//전체 마린 훑어서 유닛 월드 Rect가 rcWorld와 겹치면 선택
	auto& units = CObjMgr::Get_Instance()->GetUnits();
	for (CUnit* u : units)
	{
		if (!u || u->IsDead()) continue;

		RECT ur = u->GetWorldRect(); //유닛 충돌 + 바운딩 박스(월드 좌표 기준)
		RECT inter{};
		if (IntersectRect(&inter, &rcWorld, &ur))
		{
			u->SetSelected(true);
			m_vecSelected.push_back(u);
		}
	}
	//유닛 없으면 건물 선택
	if (m_vecSelected.empty())
	{
		auto& building = CObjMgr::Get_Instance()->GetBuildings();
		for (auto* pBuilding : building)
		{
			if (!pBuilding) continue;
			
			RECT br = pBuilding->GetWorldRect();
			RECT inter{};
			if (IntersectRect(&inter, &rcWorld, &br))
			{
				pBuilding->SetSelected(true);
				m_vecSelected.push_back(pBuilding);
				break; // 건물은 1개만
			}
		}
	}
}

void CSelectionMgr::Render(HDC hDC)
{
	if (!m_bDragging) return;

	// 채움(칸 느낌): 해치 브러시
	HBRUSH fill = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 255, 0));
	HBRUSH oldB = (HBRUSH)SelectObject(hDC, fill);

	// 테두리: 점선 펜
	HPEN pen = CreatePen(PS_DOT, 1, RGB(0, 255, 0));
	HPEN oldP = (HPEN)SelectObject(hDC, pen);  // PS_DOT는 폭 1 이하에서 유효 :contentReference[oaicite:3]{index=3}

	Rectangle(hDC, m_rcScreen.left, m_rcScreen.top, m_rcScreen.right, m_rcScreen.bottom); 

	SelectObject(hDC, oldP);
	SelectObject(hDC, oldB);
	DeleteObject(pen);
	DeleteObject(fill);
}