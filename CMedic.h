#pragma once
#include "CUnit.h"

class CMedic : public CUnit
{
public:
	CMedic();
	virtual ~CMedic();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
};


