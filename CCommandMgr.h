#pragma once
#include "CUnit.h"
#include "CBuilding.h"

enum class eCommandMode
{
	NONE, PLAEC_BUILDING
};

class CCommandMgr
{
private:
    CCommandMgr();
    CCommandMgr(const CCommandMgr& rhs) = delete;
    CCommandMgr& operator=(CCommandMgr& rObj) = delete;
    ~CCommandMgr();
private:
	eCommandMode m_eMode = eCommandMode::NONE;
	eBuildingType m_ePlaceType;
	class CUnit* m_pBuilder = nullptr;
	CBuilding* m_pGhost = nullptr;
public:
	void BeginPlaceBuilding(eBuildingType type, CUnit* pBuilder);
	void CancleBuilding();
	bool IsPlacing();
	void Update();
	void Render(HDC hDC);
public:
	static CCommandMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CCommandMgr;
		}

		return m_pInstance;
	}
	static void	Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
public:
	void IssueMove(Vec2& worldGoal);
	void IssueAttack(CObj* pTarget);
	void IssueAttackMove(Vec2& worldGoal);
private:
	static CCommandMgr* m_pInstance;
};