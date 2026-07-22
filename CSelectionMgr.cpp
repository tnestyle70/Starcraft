#include "pch.h"
#include "CSelectionMgr.h"
#include "CUnit.h"
#include "CBuilding.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CInputMgr.h"
#include "CCommandMgr.h"
#include "CMainUI.h"
#include "CMineral.h"
#include "CGas.h"
#include "CSCV.h"
#include "CSoundMgr.h"
#include "CBunker.h"
#include "CShuttle.h"
#include "CProbe.h"

CSelectionMgr* CSelectionMgr::m_pInstance = nullptr;

CSelectionMgr::CSelectionMgr()
{
}

CSelectionMgr::~CSelectionMgr()
{
}

void CSelectionMgr::Update()
{
	//죽은 객체 지우기!!
	m_vecSelected.erase(
		remove_if(m_vecSelected.begin(), m_vecSelected.end(),
			[](CObj* pObj) {return pObj->IsDead(); }),
		m_vecSelected.end()
	);

	//건물 배치 중에는 선택 막기
	if (CCommandMgr::Get_Instance()->IsPlacing())
		return;
	if (CInputMgr::Get_Instance()->KeyDown(LEFT_MOUSE))
	{
		OnLMouseDown();
	}
	//좌클릭(선택)
	if (m_bDragging && CInputMgr::Get_Instance()->KeyPress(LEFT_MOUSE))
		OnMouseMove();
	if (CInputMgr::Get_Instance()->KeyUp(LEFT_MOUSE))
		OnLMouseUp();
	//우클릭(명령)
	if (CInputMgr::Get_Instance()->KeyDown(RIGHT_MOUSE))
		OnRMouseDown();
	if (CInputMgr::Get_Instance()->KeyUp(RIGHT_MOUSE))
		OnRMouseUp();
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
	return (w <= 10 && h <= 10);
}

void CSelectionMgr::ClearSelection()
{
	for (auto* pObj : m_vecSelected)
	{
		if (pObj)
			pObj->SetSelected(false);
	}
	m_vecSelected.clear();
	//BuildingUIInfo 숨김 추가
	BuildingUIInfo buildingInfo;
	buildingInfo.IsVisible = false;
	buildingInfo.pBuildingName = nullptr;
	buildingInfo.pCurrentUnit = nullptr;
	buildingInfo.IsProducing = false;
	buildingInfo.fProgress = 0.f;
	buildingInfo.queue.clear();
	CMainUI::Get_Instance()->SetBuildingUIInfo(buildingInfo);
	//UnitUIInfo 숨김 추가
	UnitUIInfo UnitInfo;
	UnitInfo.IsVisible = false;
	UnitInfo.pUnitName = nullptr;
	UnitInfo.iMaxHP = 0;
	UnitInfo.iHP = 0;
	UnitInfo.eType = eUnitType::NONE;
	CMainUI::Get_Instance()->SetUnitUIInfo(UnitInfo);
}

void CSelectionMgr::RemoveFromSelection(CObj* pTarget)
{
	auto it = m_vecSelected.begin();
	while (it != m_vecSelected.end())
	{
		if (*it == pTarget)
		{
			it = m_vecSelected.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CSelectionMgr::SelectSingleAt(const POINT& clientPt)
{
	bool bCtrlPressed = CInputMgr::Get_Instance()->KeyPressVK(VK_CONTROL);

	const float fScrX = CScrollMgr::Get_Instance()->Get_ScrollX();
	const float fScrY = CScrollMgr::Get_Instance()->Get_ScrollY();

	Vec2 vWorld{ clientPt.x + fScrX, clientPt.y + fScrY };

	CObj* pHit = CObjMgr::Get_Instance()->PickObjAt(vWorld);
	if (!pHit || !pHit->IsSelectable())
	{
		if (!bCtrlPressed)
		{
			ClearSelection();
		}
		return;
	}
	//ctrl 클릭
	if (bCtrlPressed)
	{
		CUnit* pClickedUnit = dynamic_cast<CUnit*>(pHit);
		if (pClickedUnit)
		{
			SelectSameTypeUnits(pClickedUnit);
			return;
		}
		//멀티 빌딩 나중에 처리!
		CBuilding* pClickedBuilding = dynamic_cast<CBuilding*>(pHit);
		if (pClickedBuilding)
		{
			SelectSameTypeBuildings(pClickedBuilding);
			return;
		}
	}
	//일반 클릭 처리
	ClearSelection();
	pHit->SetSelected(true);
	m_vecSelected.push_back(pHit);
	//사운드 재생!
	//아군 유닛일 경우만 재생!
	if (pHit->GetTeamType() != eTeamType::ALLY)
		return;
	PlaySelectedSound(pHit);
}

void CSelectionMgr::PlaySelectedSound(CObj* pSelected)
{
	CUnit* pUnit = dynamic_cast<CUnit*>(pSelected);
	if (!pUnit)
		return;
	eUnitType type = pUnit->Get_UnitType();
	//유닛의 경우에만 사운드 재생
	switch (type)
	{
	case eUnitType::SCV:
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"SCV/SCVSelect1.wav", 0.3f);
		break;
	case eUnitType::MARINE:
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Marine/MarineSelect1.wav", 0.3f);
		break;
	case eUnitType::MEDIC:
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Medic/MedicSelect1.wav", 0.3f);
		break;
	case eUnitType::FIREBAT:
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"FireBat/FireBatSelect1.wav", 0.3f);
		break;
	case eUnitType::GHOST:
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Ghost/GhostSelect1.wav", 0.3f);
		break;
	case eUnitType::VULTURE:
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Vulture/VultureSelect1.wav", 0.3f);
		break;
	case eUnitType::GOLIATH:
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Goliath/TGoYes00.wav", 0.3f);
		break;
	case eUnitType::TANK:
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Tank/TankSelect1.wav", 0.3f);
		break;
	case eUnitType::SIEGE_TANK:
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"Tank/TankSelect1.wav", 0.3f);
		break;
	case eUnitType::BATTLECRUISER:
		//사운드 재생
		CSoundMgr::Get_Instance()->PlayEffect(L"BattleCrusor/BattleCrusorSelect1.wav", 0.3f);
		break;
		//프로토스
	case eUnitType::PROBE:
		CSoundMgr::Get_Instance()->PlayEffect(L"Probe/ppryes01.wav", 0.3f);
		break;
	case eUnitType::ZEALOT:
		CSoundMgr::Get_Instance()->PlayEffect(L"zealot/PZeYes02.wav", 0.3f);
		break;
	case eUnitType::DRAGON:
		CSoundMgr::Get_Instance()->PlayEffect(L"Dragoon/PDrYes00.wav", 0.3f);
		break;
	case eUnitType::HIGH_TEMPLAR:
		CSoundMgr::Get_Instance()->PlayEffect(L"HTemplar/pteYes00.wav", 0.3f);
		break;
	case eUnitType::DARK_ARCHON:
		CSoundMgr::Get_Instance()->PlayEffect(L"DArchon/pdawht01.wav", 0.5f);
		break;
	case eUnitType::SHUTTLE:
		CSoundMgr::Get_Instance()->PlayEffect(L"shuttle/pshrdy00.wav", 0.3f);
		break;
	case eUnitType::CARRIER:
		CSoundMgr::Get_Instance()->PlayEffect(L"Carrier/PCaWht00.wav", 0.3f);
		break;
	}
}

void CSelectionMgr::SaveControlGroup(int slotNum)
{
	if (slotNum <= 3 || slotNum > 9)
		return;
	m_mapControlGroup[slotNum] = m_vecSelected;
}

void CSelectionMgr::LoadControlGroup(int slotNum, bool AddToSelection)
{
	if (slotNum <= 3 || slotNum > 9)
		return;
	auto iter = m_mapControlGroup.find(slotNum);
	if (iter == m_mapControlGroup.end())
		return;
	//유효하지 않은 오브젝트 제거하기
	vector<CObj*> validObjs;
	for (auto* pObj : iter->second)
	{
		if (pObj && !pObj->IsDead())
			validObjs.push_back(pObj);
	}
	iter->second = validObjs;
	if (validObjs.empty())
	{
		m_mapControlGroup.erase(iter);
		return;
	}
	//선택 처리
	if (AddToSelection)
	{
		//shift + 숫자 기존 선택에 추가하기
		for (auto* pObj : validObjs)
		{
			if (find(m_vecSelected.begin(), m_vecSelected.end(), pObj) == m_vecSelected.end())
			{
				m_vecSelected.push_back(pObj);
			}
		}
	}
	else 
	{
		ClearSelection();
		for (auto& pObj : validObjs)
		{
			m_vecSelected.push_back(pObj);
			pObj->SetSelected(true);
		}
		//m_vecSelected = validObjs;
	}
}

void CSelectionMgr::RemoveFromControlGroup(CObj* pDeadObj)
{
	auto iter = m_mapControlGroup.begin();
	while (iter != m_mapControlGroup.end())
	{
		//vector에서 유효하지 않은 오브젝트 제거하기
		auto& validObjs = iter->second;
		validObjs.erase(remove(validObjs.begin(), validObjs.end(), pDeadObj), validObjs.end());
		//vector가 비었으면 map에서도 제거하기
		if (validObjs.empty())
		{
			iter = m_mapControlGroup.erase(iter);
		}
		else
		{
			iter++;
		}
	}
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

	if (CMainUI::Get_Instance()->IsInUIArea(m_ptCur))
		return;

	//BuildingUIInfo 숨김 추가
	BuildingUIInfo buildingInfo;
	buildingInfo.IsVisible = false;
	buildingInfo.pBuildingName = nullptr;
	buildingInfo.pCurrentUnit = nullptr;
	buildingInfo.IsProducing = false;
	buildingInfo.fProgress = 0.f;
	buildingInfo.queue.clear();
	CMainUI::Get_Instance()->SetBuildingUIInfo(buildingInfo);
	//클릭 선택
	if (IsClickSelection(m_rcScreen))
	{
		//SelectSingleAt(m_ptCur);
		//단일 선택시에 멀티 UI 숨김 처리
		MultiUnitUIInfo multiInfo;
		multiInfo.IsVisible = false;
		multiInfo.iUnitCount = 0;
		CMainUI::Get_Instance()->SetMultiUnitUIInfo(multiInfo);
		//MultiUnitUIInfo에 대한 정보를 덮어씌우지 않게 순서 조정!
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
	
	//전체 유닛 훑어서 월드 Rect가 rcWorld와 겹치면 선택
	auto& units = CObjMgr::Get_Instance()->GetUnits();
	bool bFirstSelected = false;
	for (CUnit* u : units)
	{
		if (!u || u->IsDead() || !u->IsSelectable()) 
			continue;

		RECT ur = u->GetWorldRect(); //유닛 충돌 + 바운딩 박스(월드 좌표 기준)
		RECT inter{};
		if (IntersectRect(&inter, &rcWorld, &ur))
		{
			u->SetSelected(true);
			m_vecSelected.push_back(u);
			//처음 선택된 유닛 사운드 재생! , 적은 사운드 재생하지 않음.
			if (!bFirstSelected)
			{
				PlaySelectedSound(u);
				bFirstSelected = true;
			}
		}
	}
	//멀티 유닛 선택 처리(WireFrame 부분 추가)
	if (m_vecSelected.size() > 1)
	{
		//멀티 유닛 UI 구성
		MultiUnitUIInfo info;
		info.IsVisible = true;
		info.iUnitCount = min(12, (int)m_vecSelected.size()); //최대 16개
		for (int i = 0; i < info.iUnitCount; ++i)
		{
			CUnit* pUnit = dynamic_cast<CUnit*>(m_vecSelected[i]);
			if (pUnit)
			{
				info.units[i].eRaceType = pUnit->GetOriginalRace();
				info.units[i].eType = pUnit->Get_UnitType();
				info.units[i].iHP = pUnit->Get_HP();
				info.units[i].iMaxHP = pUnit->Get_MaxHP();
				info.units[i].pUnit = pUnit;
			}
		}
		CMainUI::Get_Instance()->SetMultiUnitUIInfo(info);
		//단일 유닛 UI 숨김,
		UnitUIInfo unitInfo;
		unitInfo.IsVisible = false;
		CMainUI::Get_Instance()->SetUnitUIInfo(unitInfo);
	}
	else if (m_vecSelected.size() == 1)
	{
		MultiUnitUIInfo info;
		info.IsVisible = false;
		CMainUI::Get_Instance()->SetMultiUnitUIInfo(info);
	}

	//아군 없으면 적 선택
	//auto& ememy = CObjMgr::Get_Instance()->GetEnemies();
	//for (CObj* u : ememy)
	//{
	//	if (!u || u->IsDead() || !u->IsSelectable())
	//		continue;

	//	RECT ur = u->GetWorldRect(); //유닛 충돌 + 바운딩 박스(월드 좌표 기준)
	//	RECT inter{};
	//	if (IntersectRect(&inter, &rcWorld, &ur))
	//	{
	//		u->SetSelected(true);
	//		m_vecSelected.push_back(u);
	//	}
	//}

	//유닛 없으면 건물 선택
	if (m_vecSelected.empty())
	{
		auto& building = CObjMgr::Get_Instance()->GetBuildings();
		for (auto* pBuilding : building)
		{
			if (!pBuilding || !pBuilding->IsSelectable()) 
				continue;

			//Ghost 건물 선택 안 됨
			CBuilding* build = dynamic_cast<CBuilding*>(pBuilding);
			if (build && build->IsGhost())
				continue;
			
			RECT br = pBuilding->GetWorldRect();
			RECT inter{};
			if (IntersectRect(&inter, &rcWorld, &br))
			{
				pBuilding->SetSelected(true);
				m_vecSelected.push_back(pBuilding);
				continue; // 건물도 여러 개 선택 가능
			}
		}
	}
}

void CSelectionMgr::SelectSameTypeUnits(CUnit* pRefUnit)
{
	if (!pRefUnit) return;
	//이전 선택 해제
	ClearSelection();
	eUnitType targetType = pRefUnit->Get_UnitType();
	const int MAX_SELECTION = 12; //최대 선택 개수 제한
	//유닛 선택 화면에 있는 유닛만 선택
	float srcX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float srcY = CScrollMgr::Get_Instance()->Get_ScrollY();
	RECT screenWorld;
	screenWorld.left = (LONG)srcX;
	screenWorld.top = (LONG)srcY;
	screenWorld.right = (LONG)(srcX + WINCX);
	screenWorld.bottom = (LONG)(srcY + WINCY);

	auto& units = CObjMgr::Get_Instance()->GetUnits();

	for (CUnit* pUnit : units)
	{
		if (!pUnit || pUnit->IsDead() || !pUnit->IsSelectable())
			continue;
		if (m_vecSelected.size() > MAX_SELECTION)
			continue;
		//동일 타입인지 확인
		if (pUnit->Get_UnitType() != targetType)
			continue;
		//화면 내에 있는지 체크
		Vec2 pos = pUnit->Get_Pos();
		if (pos.fX >= screenWorld.left && pos.fX <= screenWorld.right &&
			pos.fY >= screenWorld.top && pos.fY <= screenWorld.bottom) 
		{
			pUnit->SetSelected(true);
			m_vecSelected.push_back(pUnit);
		}
	}
	//멀티 유닛 UI업데이트
	if (m_vecSelected.size() > 1)
	{
		MultiUnitUIInfo info;
		info.IsVisible = true;
		info.iUnitCount = min(12, (int)m_vecSelected.size());
		for (int i = 0; i < info.iUnitCount; ++i)
		{
			CUnit* pUnit = dynamic_cast<CUnit*>(m_vecSelected[i]);
			if (pUnit)
			{
				info.units[i].eRaceType = pUnit->GetOriginalRace();
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
}

void CSelectionMgr::SelectSameTypeBuildings(CBuilding* pRefBuilding)
{
	if (!pRefBuilding) return;
	//이전 선택 해제
	ClearSelection();
	eBuildingType targetType = pRefBuilding->GetBuildingType();
	const int MAX_SELECTION = 12; //최대 선택 개수 제한
	//화면에 있는 건물만 선택 
	float srcX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float srcY = CScrollMgr::Get_Instance()->Get_ScrollY();
	RECT screenWorld;
	screenWorld.left = (LONG)srcX;
	screenWorld.top = (LONG)srcY;
	screenWorld.right = (LONG)(srcX + WINCX);
	screenWorld.bottom = (LONG)(srcY + WINCY);

	auto& buildings = CObjMgr::Get_Instance()->GetBuildings();

	for (CBuilding* pBuilding : buildings)
	{
		if (!pBuilding || pBuilding->IsDead() || !pBuilding->IsSelectable())
			continue;
		if (m_vecSelected.size() > MAX_SELECTION)
			continue;
		//동일 타입인지 확인
		if (pBuilding->GetBuildingType() != targetType)
			continue;
		//화면 내에 있는지 체크
		Vec2 pos = pBuilding->Get_Pos();
		if (pos.fX >= screenWorld.left && pos.fX <= screenWorld.right &&
			pos.fY >= screenWorld.top && pos.fY <= screenWorld.bottom)
		{
			pBuilding->SetSelected(true);
			m_vecSelected.push_back(pBuilding);
		}
	}
	//멀티 빌딩 UI업데이트
	if (m_vecSelected.size() > 1)
	{
		MultiBuildingUIInfo info;
		info.IsVisible = true;
		info.iBuildingCount = min(12, (int)m_vecSelected.size());
		for (int i = 0; i < info.iBuildingCount; ++i)
		{
			CBuilding* pBuilding = dynamic_cast<CBuilding*>(m_vecSelected[i]);
			if (pBuilding)
			{
				info.buildings[i].eType = pBuilding->GetBuildingType();
				info.buildings[i].iHP = pBuilding->Get_HP();
				info.buildings[i].iMaxHP = pBuilding->Get_MaxHP();
				info.buildings[i].pBuilding = pBuilding;
			}
		}
		CMainUI::Get_Instance()->SetMultiBuildingUIInfo(info);
		//단일 건물 UI 숨기기
		BuildingUIInfo bulidingInfo;
		bulidingInfo.IsVisible = false;
		CMainUI::Get_Instance()->SetBuildingUIInfo(bulidingInfo);
	}
}

void CSelectionMgr::OnRMouseDown()
{
	m_bRightClick = true;
	m_ptRStart = GetMouseClient();
}

void CSelectionMgr::OnRMouseUp()
{
	if (!m_bRightClick)
		return;
	m_bRightClick = false;
	//방향성, SelectionMgr에서 우클릭을 통합해서 처리하는 방향은 맞는데, 미네랄 쪽 nullptr 처리에서 문제가 생겨버림
	//선택된 OBJ가 없으면 무시
	if (m_vecSelected.empty()) //MainUI의 CUnit*쪽이 터지므로 여기서는 다른 처리를 해야 함 Mineral 쪽이 애초에 선택되면 안 됨?
		return;
	//월드 좌표 계산
	POINT clientPT = GetMouseClient();
	float srcX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float srcY = CScrollMgr::Get_Instance()->Get_ScrollY();
	Vec2 worldPos{ clientPT.x + srcX, clientPT.y + srcY };
	//클릭한 위치에 어떤 것이 있는지 확인
	CObj* pTarget = FindClickTarget(worldPos);
	//스마트 커맨드 실행
	IssueSmartCommand(pTarget, worldPos);
}

CObj* CSelectionMgr::FindClickTarget(const Vec2& worldPos)
{
	const float CLICK_RADIUS = 100.f; //클릭 판정 범위

	CObj* pClosest = nullptr;
	float minDist = CLICK_RADIUS;

	//벙커선택
	list<CObj*>& buildingList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_BUILDING);
	for (auto* pObj : buildingList)
	{
		if (!pObj || pObj->IsDead())
			continue;
		CBunker* pBunker = dynamic_cast<CBunker*>(pObj);
		if (pBunker) //우선은 벙커 반환
		{
			Vec2 bunkerPos = pBunker->Get_Pos();
			float dx = worldPos.fX - bunkerPos.fX;
			float dy = worldPos.fY - bunkerPos.fY;
			float dist = sqrtf(dx * dx + dy * dy);
			if (dist < CLICK_RADIUS)
			{
				return pBunker;
			}
		}
	}
	
	//셔틀 선택
	list<CObj*>& unitList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_UNIT);
	for (auto& pObj : unitList)
	{
		if (!pObj || pObj->IsDead())
			continue;
		CShuttle* pShuttle = dynamic_cast<CShuttle*>(pObj);
		if (pShuttle)
		{
			Vec2 shuttlePos = pShuttle->Get_Pos();
			float dx = worldPos.fX - shuttlePos.fX;
			float dy = worldPos.fY - shuttlePos.fY;
			float dist = sqrtf(dx * dx + dy * dy);
			if (dist < CLICK_RADIUS)
				return pShuttle;
		}
	}

	//자원
	list<CObj*> resourceList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_RESOURCE);
	for (auto* pObj : resourceList)
	{
		if (!pObj || pObj->IsDead())
			continue;
		Vec2 resourcePos = pObj->Get_Pos();
		float dx = worldPos.fX - resourcePos.fX;
		float dy = worldPos.fY - resourcePos.fY;
		float dist = sqrtf(dx * dx + dy * dy);

		if (dist < minDist) //더 가까운 것으로 발견
		{
			minDist = dist;
			pClosest = pObj;
		}
	}
	//가장 가까운 자원 반환
	if (pClosest)
		return pClosest;

	//적 유닛
	list<CObj*> enemyList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_ENEMY);
	for (auto* pObj : enemyList)
	{
		if (!pObj || pObj->IsDead())
			continue;
		Vec2 enemyPos = pObj->Get_Pos();
		float dx = worldPos.fX - enemyPos.fX;
		float dy = worldPos.fY - enemyPos.fY;
		float dist = sqrtf(dx * dx + dy * dy);
		if (dist < CLICK_RADIUS) //더 가까운 것으로 선택
		{
			minDist = dist;
			pClosest = pObj;
		}
	}
	//가장 가까운 적 반환
	if (pClosest)
		return pClosest;

	return nullptr;
}

void CSelectionMgr::IssueSmartCommand(CObj* pTarget, const Vec2& worldPos)
{
	// 선택된 유닛 중 ENEMY가 하나라도 있으면 명령 무시
	for (auto* pObj : m_vecSelected)
	{
		CUnit* pUnit = dynamic_cast<CUnit*>(pObj);
		if (pUnit && pUnit->GetTeamType() == eTeamType::ENEMY)
			return; 
	}

	if (pTarget)
	{
		//벙커 클릭이면 선택된 유닛이 마린, 파이어벳인지 확인한 이후에 loadBunker호출해서 벙커에 태위기
		CBunker* pBunker = dynamic_cast<CBunker*>(pTarget);
		if (pBunker)
		{
			for (auto* pObj : m_vecSelected)
			{
				CUnit* pUnit = dynamic_cast<CUnit*>(pObj);
				if (!pUnit || pUnit->IsDead())
					continue;
				if (pUnit->Get_UnitType() == eUnitType::MARINE ||
					pUnit->Get_UnitType() == eUnitType::FIREBAT)
				{
					//Bunker Enter 오더 주기
					Order bunkerOrder;
					bunkerOrder.eType = eOrderType::ENTER_BUNKER;
					bunkerOrder.pTarget = pBunker;
					bunkerOrder.dst = pBunker->Get_Pos();
					pUnit->PushOrder(bunkerOrder);
				}
			}
		}

		//셔틀 클릭이면 프로토스 지상 유닛 태우기
		CShuttle* pShuttle = dynamic_cast<CShuttle*>(pTarget);
		if (pShuttle)
		{
			for (auto& pObj : m_vecSelected)
			{
				CUnit* pUnit = dynamic_cast<CUnit*>(pObj);
				if (!pUnit || pUnit->IsDead())
					continue;
				if (pUnit->GetLayer() == eUnitLayer::GROUND)
				{
					//Shuttle Enter 오더 추가하기 
					Order shuttleOrder;
					shuttleOrder.eType = eOrderType::ENTER_SHUTTLE;
					shuttleOrder.pTarget = pShuttle;
					shuttleOrder.dst = pShuttle->Get_Pos();
					pUnit->PushOrder(shuttleOrder);
				}
			}
		}

		//미네랄 클릭 -> SCV, Probe GATHER 명령
		CMineral* pMineral = dynamic_cast<CMineral*>(pTarget);
		if (pMineral)
		{
			for (auto* pObj : m_vecSelected)
			{
				CSCV* pSCV = dynamic_cast<CSCV*>(pObj);
				CProbe* pProbe = dynamic_cast<CProbe*>(pObj);
				if (pSCV)
				{
					Order gatherOrder;
					gatherOrder.eType = eOrderType::GATHER;
					gatherOrder.pTarget = pMineral;
					gatherOrder.dst = pMineral->Get_Pos();

					pSCV->SetResourceType(eResourceType::MINERAL);
					pSCV->PushOrder(gatherOrder);
				}
				else if (pProbe)
				{
					Order gatherOrder;
					gatherOrder.eType = eOrderType::GATHER;
					gatherOrder.pTarget = pMineral;
					gatherOrder.dst = pMineral->Get_Pos();

					pProbe->SetResourceType(eResourceType::MINERAL);
					pProbe->PushOrder(gatherOrder);
				}
			}
			return;
		}
		//가스 클릭 -> SCV, Probe GATHER 명령
		CGas* pGas = dynamic_cast<CGas*>(pTarget);
		if (pGas)
		{
			for (auto* pObj : m_vecSelected)
			{
				CSCV* pSCV = dynamic_cast<CSCV*>(pObj);
				CProbe* pProbe = dynamic_cast<CProbe*>(pObj);
				if (pSCV)
				{
					Order gatherOrder;
					gatherOrder.eType = eOrderType::GATHER;
					gatherOrder.pTarget = pGas;
					gatherOrder.dst = pGas->Get_Pos();

					pSCV->SetResourceType(eResourceType::GAS);
					pSCV->PushOrder(gatherOrder);
				}
				else if (pProbe)
				{
					Order gatherOrder;
					gatherOrder.eType = eOrderType::GATHER;
					gatherOrder.pTarget = pGas;
					gatherOrder.dst = pGas->Get_Pos();

					pProbe->SetResourceType(eResourceType::GAS);
					pProbe->PushOrder(gatherOrder);
				}
			}
			return;
		}
		//적 유닛 클릭 -> ATTACK 명령
		list<CObj*> enemyList = CObjMgr::Get_Instance()->Get_ObjList(OBJ_ENEMY);
		bool isEnemy = false;
		for (auto* pEnemy : enemyList)
		{
			if (pEnemy == pTarget)
			{
				isEnemy = true;
				break;
			}
		}
		if (isEnemy)
		{
			for (auto* pObj : m_vecSelected)
			{
				CUnit* pUnit = dynamic_cast<CUnit*>(pObj);
				if (pUnit && !pUnit->IsDead())
				{
					Order attackOrder;
					attackOrder.eType = eOrderType::ATTACK;
					attackOrder.pTarget = pTarget;
					attackOrder.dst = pTarget->Get_Pos();
					pUnit->PushOrder(attackOrder);
				}
			}
			return;
		}
	}
	else
	{
		//빈 땅 클릭 -> MOVE
		for (auto* pObj : m_vecSelected)
		{
			CUnit* pUnit = dynamic_cast<CUnit*>(pObj);
			if (pUnit)
			{
				Order moveOrder;
				moveOrder.eType = eOrderType::MOVE;
				moveOrder.dst = worldPos;
				pUnit->PushOrder(moveOrder);
			}
		}
	}
}

void CSelectionMgr::Render(HDC hDC)
{
	if (m_bDragging)
	{
		// 드래그 중일 때 초록색 박스 그리기
		HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));  // 초록색
		HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);  // 투명 브러시

		HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

		Rectangle(hDC, m_ptStart.x, m_ptStart.y, m_ptCur.x, m_ptCur.y);

		SelectObject(hDC, hOldPen);
		SelectObject(hDC, hOldBrush);
		DeleteObject(hPen);
	}
}

void CSelectionMgr::RenderSelectionCircle(HDC hDC)
{
	int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
	int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

	for (auto* pObj : m_vecSelected)
	{
		INFO info = pObj->Get_Info();
		int iDrawX = (int)(info.fX - info.fCX / 2.f - iScrollX);
		int iDrawY = (int)(info.fY - info.fCY / 2.f - iScrollY);

		//선택 원(예: m_bSelected가 true일 때) 추후에 bmp로 교체
		if (pObj->IsSelected())
		{
			eTeamType type = pObj->GetTeamType();

			COLORREF color = (type == eTeamType::ALLY) ? RGB(0, 255, 0) : RGB(255, 0, 0);
			
			HBRUSH oldB = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
			HPEN pen = CreatePen(PS_SOLID, 2, color);
			HPEN oldP = (HPEN)SelectObject(hDC, pen);

			int cx = iDrawX + (int)(info.fCX * 0.5f);
			int cy = iDrawY + (int)(info.fCY * 0.7f);   // 발밑 느낌으로 살짝 아래

			float fRatio;

			if ((max(info.fCX, info.fCY)) >= 160)
			{
				fRatio = 0.45f;
			}
			else if ((max(info.fCX, info.fCY)) < 160 &&
				(max(info.fCX, info.fCY)) >= 96)
			{
				fRatio = 0.4f;
			}
			else
			{
				fRatio = 0.3f;
			}

			int r = (int)(max(info.fCX, info.fCY) * fRatio);

			Ellipse(hDC, cx - r, cy - r / 2, cx + r, cy + r / 2);

			SelectObject(hDC, oldP);
			SelectObject(hDC, oldB);
			DeleteObject(pen);
		}
	}
}

void CSelectionMgr::Release()
{
	m_vecSelected.clear();
}
