#pragma once
#include "CUnit.h"

class CBattleCruiser : public CUnit
{
public:
	CBattleCruiser();
	virtual ~CBattleCruiser();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
};


