#pragma once

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
public:
	void OnLMouseDown();
	void OnMouseMove();
	void OnLMouseUp();

	const std::vector<CUnit*>& GetSelected() const { return m_vecSelected; }
	void ClearSelection();
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

	bool m_bDragging; 
	POINT m_ptStart{};
	POINT m_ptCur{};
	RECT m_rcScreen{};

	std::vector<CUnit*> m_vecSelected;
private:
	static POINT GetMouseClient();
	static RECT NormalizeRect(POINT a, POINT b);
	static bool IsClickSelection(const RECT& r);
	void SelectSingleAt(const POINT& clientPt);
};