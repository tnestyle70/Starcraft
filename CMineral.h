#pragma once
#include "CObj.h"

class CMineral : public CObj
{
public:
	CMineral();
	virtual ~CMineral();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
};