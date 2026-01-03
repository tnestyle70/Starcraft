#pragma once
#include "CUnit.h"

class CVulture : public CUnit
{
public:
	CVulture();
	virtual ~CVulture();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
protected:
	bool UpdateAttack(Order& order) override;
public:
	void Fire_Bullet(CObj* pTarget);
};



