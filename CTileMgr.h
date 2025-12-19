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
	void		Picking_Tile(POINT pt, int iOption, int iCost);
	void		Save_Tile();
	void		Load_Tile();
	void RenderGrid(HDC hDC, float fScrX, float fScrY);
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


