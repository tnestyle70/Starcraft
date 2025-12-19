#pragma once
#include "Define.h"
#include "CScene.h"

class CStage : public CScene
{
public:
	CStage();
	virtual ~CStage();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
public:
	void ClampMouse(HWND hWnd);
};