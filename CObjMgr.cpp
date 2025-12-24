#include "pch.h"
#include "CObjMgr.h"
//#include "CCollisionMgr.h"
#include "CButton.h"
#include "CUnit.h"
#include "CBuilding.h"

CObjMgr* CObjMgr::m_pInstance = nullptr;

CObjMgr::CObjMgr()
{
}

CObjMgr::~CObjMgr()
{
	Release();
}

void CObjMgr::Delete_Obj(OBJID eID)
{
	for (auto& pObj : m_ObjList[eID])
	{
		Safe_Delete(pObj);
	}

	m_ObjList[eID].clear();
}

void CObjMgr::Add_Object(OBJID eID, CObj* pObj)
{
	if (OBJ_END <= eID || pObj == nullptr)
		return;

	m_ObjList[eID].push_back(pObj);
	
	if (eID == OBJ_UNIT)
	{
		if (auto* pUnit = dynamic_cast<CUnit*>(pObj))
		{
			m_vecUnits.push_back(pUnit);
		}
	}
	if (eID == OBJ_BUILDING)
	{
		if (auto* pBuilding = dynamic_cast<CBuilding*>(pObj))
		{
			m_vecBuildings.push_back(pBuilding);
		}
	}
}

void CObjMgr::Update()
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		for (auto iter = m_ObjList[i].begin();
			iter != m_ObjList[i].end(); )
		{
			int iResult = (*iter)->Update();

			if (iResult == DEAD)
			{
				CObj* pDead = *iter;

				if (i == OBJ_UNIT)
				{
					CUnit* pDeadUnit = static_cast<CUnit*>(pDead);
					m_vecUnits.erase(remove(m_vecUnits.begin(), m_vecUnits.end(), pDeadUnit), m_vecUnits.end());
				}
				if (i == OBJ_BUILDING)
				{
					CBuilding* pDeadUnit = static_cast<CBuilding*>(pDead);
					m_vecBuildings.erase(remove(m_vecBuildings.begin(), m_vecBuildings.end(), pDeadUnit), m_vecBuildings.end());
				}

				Safe_Delete<CObj*>(pDead);
				iter = m_ObjList[i].erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}
}

void CObjMgr::Late_Update()
{
	for (size_t i = 0; i < OBJ_END; ++i)
	{
		for (auto& pObj : m_ObjList[i])
		{
			pObj->Late_Update();

			if (m_ObjList[i].empty())
				break;
			
			RENDERID	eID = pObj->Get_RenderID();
			m_RenderList[eID].push_back(pObj);
		}
	}
	//CCollisionMgr::Collision_Rect(m_ObjList[OBJ_MONSTER], m_ObjList[OBJ_MOUSE]);
	//CCollisionMgr::Collision_Circle(m_ObjList[OBJ_MONSTER], m_ObjList[OBJ_BULLET]);
}

void CObjMgr::Render(HDC hDC)
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		m_RenderList[i].sort([](CObj* pDst, CObj* pSrc)->bool
			{
				return pDst->Get_Info().fY < pSrc->Get_Info().fY;
			});

		for (auto& pObj : m_RenderList[i])
		{
			pObj->Render(hDC);
		}
		m_RenderList[i].clear();
	}
}

void CObjMgr::Release()
{
	m_vecUnits.clear();

	for (size_t i = 0; i < OBJ_END; ++i)
	{
		for_each(m_ObjList[i].begin(), m_ObjList[i].end(), Safe_Delete<CObj*>);
		m_ObjList[i].clear();
	}
}

CObj* CObjMgr::PickObjAt(const Vec2& vWorldPos)
{
	CObj* pBest = nullptr;
	float fBestDistSq = FLT_MAX;

	// 1) 유닛 먼저(정책)
	for (CUnit* pObj : m_vecUnits)
	{
		if (!pObj || pObj->IsDead()) continue;

		RECT rc = pObj->GetWorldRect();
		if (vWorldPos.fX < rc.left || vWorldPos.fX > rc.right ||
			vWorldPos.fY < rc.top || vWorldPos.fY > rc.bottom)
			continue;

		const INFO& info = pObj->Get_Info();
		float dx = vWorldPos.fX - info.fX;
		float dy = vWorldPos.fY - info.fY;
		float distSq = dx * dx + dy * dy;
		if (distSq < fBestDistSq) { fBestDistSq = distSq; pBest = pObj; }
	}

	// 2) 유닛 없으면 건물
	if (!pBest)
	{
		for (CBuilding* pObj : m_vecBuildings)
		{
			if (!pObj || pObj->IsDead()) continue;

			RECT rc = pObj->GetWorldRect();
			if (vWorldPos.fX < rc.left || vWorldPos.fX > rc.right ||
				vWorldPos.fY < rc.top || vWorldPos.fY > rc.bottom)
				continue;

			const INFO& info = pObj->Get_Info();
			float dx = vWorldPos.fX - info.fX;
			float dy = vWorldPos.fY - info.fY;
			float distSq = dx * dx + dy * dy;
			if (distSq < fBestDistSq) { fBestDistSq = distSq; pBest = pObj; }
		}
	}

	return pBest;
}

CButton CObjMgr::CreateButton(float fX, float fY, float fCX, float fCY)
{
	CButton* pButton = new CButton;

	return CButton();
}

CButton* CObjMgr::CreateButton(const wchar_t* framekey, float fX, float fY, const wchar_t* atlasKey, int iScrY)
{
	CButton* pButton = new CButton;
	pButton->Set_Pos(fX, fY);
	pButton->Set_FrameKey(framekey);

	AtlasRect normal{ 0, iScrY, 162, 33 };
	AtlasRect hover{ 162, iScrY, 162, 33 };

	pButton->SetAtlas(atlasKey, normal, hover);

	pButton->m_iZOrder = 200; //패널보다 위쪽, 텍스트보다 아래쪽으로 설정

	pButton->Initialize();
	Add_Object(OBJ_UI, pButton);

	return pButton;
}

list<CObj*>& CObjMgr::Get_ObjList(OBJID eID)
{
	return m_ObjList[eID];
}
