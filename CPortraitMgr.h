#pragma once
#include "Define.h"
#include "CUnit.h"
#include "CBuilding.h"

class CPortraitMgr
{
private:
	CPortraitMgr() {};
	~CPortraitMgr() {};
public:
	void Initialize();
	void Update();
	void RenderPortraitBMP(HDC hDC, eUnitType type);
	void RenderPortraitBMPRow(HDC hDC, eUnitType type);
	void RenderPortraitPNG(HDC hDC, eUnitType type, eRaceType raceType);
	//void RenderUnitPortraitPNG(HDC hDC, eUnitType type);
private:
	float m_fAnimTimer = 0.1f; 
	float m_fAnimDuration = 0.1f;
	bool m_bReverse = false;
	int m_iCurrentFrame = 0;
	int m_iEndFrame = 44;
	int m_iWidth = 60; //가로 세로 60 * 54 고정
	int m_iHeight = 54;
	int m_iPortraitX = 518;
	int m_iPortraitY = 518;
public:
	static CPortraitMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CPortraitMgr;
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
	static CPortraitMgr* m_pInstance;
};