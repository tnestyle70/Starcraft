#pragma once

#include "Define.h"

class CScrollMgr
{
private:
	CScrollMgr();
	CScrollMgr(const CScrollMgr& rhs) = delete;
	CScrollMgr& operator=(CScrollMgr& rObj) = delete;
	~CScrollMgr();

public:
	void Update();
	void		Scroll_Lock();

public:
	static CScrollMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CScrollMgr;
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
	//À¯´ÖÀÇ ÁÂÇ¥¸¦ ¿ùµå ÁÂÇ¥·Î º¯È¯
	static Vec2 ScreenToWorld(POINT pt);
	static POINT WorldToScreen(const Vec2& w);

	void Update_EdgeScroll(float fDeltaTime);
	void ClampToWorld();

	float		Get_ScrollX() { return m_fScrollX; }
	float		Get_ScrollY() { return m_fScrollY; }

	void		Set_ScrollX(float fX) { m_fScrollX += fX; }
	void		Set_ScrollY(float fY) { m_fScrollY += fY; }

private:
	static CScrollMgr* m_pInstance;

	float m_fScrollX;
	float m_fScrollY;
};


