#pragma once
#include "CScene.h"

class CEdit : public CScene
{
public:
	CEdit();
	virtual ~CEdit();
private:
	void		Key_Input();

public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
};

