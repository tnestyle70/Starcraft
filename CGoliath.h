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
	bool UpdateRAttack(Order& order) override;
	void UpdateDead() override;
	void JudgeLayer();
	void FireBullet();
	void FireMissile();
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
	int m_iFireFrame;
	int m_iFireFrameTimer = 0;
	int m_iFireFrameDuration = 2;
	DWORD m_dwFireStartTime;
	DWORD m_dwFireDuration = 200;
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};

