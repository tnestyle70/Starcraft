#pragma once
#include "CTile.h"

enum eTileOccupy
{
	TILE_EMPTY = 0,
	TILE_OCCUPIED = 1,
	TILE_MINERAL = 2,
	TILE_GAS = 3,
	TILE_POWER = 5,
	TILE_CRIP = 10,
	TILE_RESTRICTED = 15 
};

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
	void		MarkExpansionTile(); //자원 주변 타운홀 건물 건설 가능 여부 플래그 설정
	void		Save_Tile();
	void		Load_Tile();
	void RenderGrid(HDC hDC, float fScrX, float fScrY);
	void RenderBuildingOverlay(HDC hDC, int topRow, int topCol,
		int width, int height, eRaceType type, eBuildingType buildingType ,int requiredValue = -1);
	void RenderCrip(HDC hDC);
public:
	void ClearOccupy() { m_vecOccupy.assign(TILEX * TILEY, 0); }
	bool IsOccupy(int row, int col); //건물 타일 점유 상태 확인
	void SetOccupy(int row, int col, bool occupy); //건물 타일 점유
	void SetPylonPower(int centerX, int centerY, bool power); //프로토스 - 파일런 파워
	void AddCrip(int centerX, int centerY); //저그 - 크립 설치
	bool InRange(int& row, int& col) const;
	bool IsBuildableTile(int row, int col, int requiredValue, eBuildingType type);
	bool CanBuildTileProtoss(int row, int col, int requiredValue, eBuildingType type);
	bool CanBuildRestrictedTile(int row, int col, int requiredValue);
	bool HasPylonPower(int row, int col);
	bool IsGasTile(int row, int col);
	int GetOccupyState(int row, int col);
	bool CanConstruct(int row, int col, int width, int height, int requiredValue, eBuildingType type);
	bool CanConstructProtoss(int row, int col, int width, int height, 
		int requiredValue, eBuildingType type);
	//좌표 변환
	bool WorldToCell(const Vec2& world, int& outRow, int& outCol) const;
	Vec2 CellToWorldCenter(int row, int col) const; 
	Vec2 CellToWorldTopLeft(int row, int col) const;
private:
	vector<int> m_vecOccupy; //인게임상에서 동적으로 변하는 타일 점유 상태
	vector<int> m_vecCrip; //저그 - 크립 점유 상태
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


