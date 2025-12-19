#pragma once
#include "CUnit.h"

class CTank : public CUnit
{
public:
	CTank();
	virtual ~CTank();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
};


