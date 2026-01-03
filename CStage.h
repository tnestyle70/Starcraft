#pragma once
#include "Define.h"
#include "CScene.h"

class CObj;

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
	void LoadImageResource();
	void CreateObject();
	void ClampMouse(HWND hWnd);
	CObj* FindObjectAtPosition(Vec2& worldPos);
private:
	void CreateResource();
private:
	bool m_bAttackMoveMode = false;
};