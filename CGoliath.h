#pragma once
#include "CUnit.h"

class CGoliath : public CUnit
{
public:
	CGoliath();
	virtual ~CGoliath();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
protected:
	bool UpdateAttack(Order& order) override;
	void Fire_Bullet(CObj* pTarget);
private:
	void UpdateBody();
	void UpdateHead();
	void RenderBody(HDC hDC);
	void RenderHead(HDC hDC);
private:
	//머리 
	const TCHAR* m_pHeadKey;
	Vec2 m_vHeadDir;
	int m_iHeadFrame;
	//공격
	bool m_bFiring;
	int m_iFireFrame;
	DWORD m_dwFireStart;
};

