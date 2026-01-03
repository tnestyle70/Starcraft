#pragma once
#include "Define.h"
#include "CScene.h"

class CMenuRace : public CScene
{
public:
	CMenuRace();
	virtual ~CMenuRace();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
private:
	void InitializeAnim();
	void InitializeButton();
private:
	HBITMAP m_hResizedBG;
	HDC m_hResizedDC;
};
