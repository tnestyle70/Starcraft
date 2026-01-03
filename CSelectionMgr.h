#pragma once
#include "Define.h"

class CObj;
class CUnit;

class CSelectionMgr
{
private:
	CSelectionMgr();
	CSelectionMgr(const CSelectionMgr& rhs) = delete;
	CSelectionMgr& operator=(CSelectionMgr& rObj) = delete;
	~CSelectionMgr();
public:
	void Update();
	void Render(HDC hDC);
	void RenderSelectionCircle(HDC hDC);
public:
	void OnLMouseDown();
	void OnMouseMove();
	void OnLMouseUp();
	void SelectSameTypeUnits(CUnit* pRefUnit);

	void OnRMouseDown();
	void OnRMouseUp();
	//우클릭 타겟 분석
	CObj* FindClickTarget(const Vec2& worldPos);
	void IssueSmartCommand(CObj* pTarget, const Vec2& worldPos);

	const std::vector<CObj*>& GetSelected() const { return m_vecSelected; }
	void ClearSelection();
private:
	bool m_bDragging = false;
	POINT m_ptStart{};
	POINT m_ptCur{};
	RECT m_rcScreen{};

	std::vector<CObj*> m_vecSelected;

	//우클릭 관련
	bool m_bRightClick = false;
	POINT m_ptRStart{};
private:
	static POINT GetMouseClient();
	static RECT NormalizeRect(POINT a, POINT b);
	static bool IsClickSelection(const RECT& r);
	void SelectSingleAt(const POINT& clientPt);
public:
	static CSelectionMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CSelectionMgr;
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
	static CSelectionMgr* m_pInstance;
};