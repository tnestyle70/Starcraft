#pragma once
#include "Define.h"

class CNavMgr
{
private:
	CNavMgr() {};
	~CNavMgr() {};
public:
	void BuildFromTile();
	vector<Vec2> RequestPathWorld(Vec2& startW, Vec2& goalW);
public:
	int index(int row, int col) { return row * TILEX + row; };
	bool IsWalkable(int row, int col) const;
	bool InRange(int row, int col) const;

	bool WorldToCell(Vec2 World, int& outR, int& outC);
	Vec2 CellToWolrdCenter(int row, int col);

	bool SnapToNearestWalkable(int& ioR, int& ioC, int iMaxRadius);
	vector<int> AStarCells(int scrR, int scrC, int goalR, int goalC);
private:
	int m_iRow;
	int m_iCol;
	vector<int> m_vecWalkable;
	vector<int> m_vecCost;
public:
	static CNavMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CNavMgr;
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
	static CNavMgr* m_pInstance;
};