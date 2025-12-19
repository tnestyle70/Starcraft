#pragma once
#include "CUnit.h"

class CMarine : public CUnit
{
public:
	CMarine();
	virtual ~CMarine();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
};

