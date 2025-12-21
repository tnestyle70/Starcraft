#pragma once
#include "CTile.h"

class CTileMgr
{
private:
	CTileMgr();
	CTileMgr(const CTileMgr& rhs) = delete;
	CTileMgr& operator=(CTileMgr& rObj) = delete;
	~CTileMgr();

public:
	void		Initialize();
	void		Update();
	void		Late_Update();
	void		Render(HDC hDC);
	void		Release();

public:
	CObj* GetTile(int row, int col);
	void		Picking_Tile(POINT pt, int iOption, int iCost);
	void		Save_Tile();
	void		Load_Tile();
	void RenderGrid(HDC hDC, float fScrX, float fScrY);
public:
	void ClearOccupy() { m_vecOccupy.assign(TILEX * TILEY, 0); }
	bool IsOccupy(int row, int col);
	void SetOccupy(int row, int col, bool occupy);
	bool InRange(int& row, int& col) const;
	bool IsBuildableTile(int row, int col);
	bool CanConstruct(int row, int col, int width, int height);
	//좌표 변환
	bool WorldToCell(const Vec2& world, int& outRow, int& outCol) const;
	Vec2 CellToWorldCenter(int row, int col) const; 
	Vec2 CellToWorldTopLeft(int row, int col) const;
private:
	vector<int> m_vecOccupy;//0 건설 가능, 1 건설 불가
public:
	static CTileMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CTileMgr;
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
	static CTileMgr* m_pInstance;
	vector<CObj*>	 m_vecTile;
};


