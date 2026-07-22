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
	CUnit* m_pBuilder = nullptr;
	CBuilding* m_pGhost = nullptr;
	//사운드 재생 타이머
	float m_fSoundInterval = 2.f;
	float m_fSoundTimer = m_fSoundInterval;
	bool m_bCanPlaySound = false;
	//A땅 공격
	bool m_bAttackMove = false;
public:
	void SaveCamSlot();
	void HandleControlGroup();
	void BeginPlaceBuilding(eBuildingType type, CUnit* pBuilder);
	void CancleBuilding();
	bool IsPlacing();
	void Update();
	void Render(HDC hDC);
public:
	void IssueMove(Vec2& worldGoal);
	void IssueAttack(CObj* pTarget);
	void IssueAttackMove(Vec2& worldGoal);
	void IssueHeal(CObj* pTarget);
	void PlayMoveSound(CUnit* pUnit);
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
private:
	static CCommandMgr* m_pInstance;
};