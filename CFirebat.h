#pragma once
#include "CUnit.h"

class CFirebat : public CUnit
{
public:
	CFirebat();
	virtual ~CFirebat();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
protected:
	void UpdateGarrisoned() override;
	void UpdateDead() override;
	void FireBullet();
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};
