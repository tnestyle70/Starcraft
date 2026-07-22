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
public:
	void SetStart(bool start) { m_bStart = start; }
private:
	void LoadResource();
	void InitializeAnim();
	void InitializeButton();
private:
	HBITMAP m_hResizedBG;
	HDC m_hResizedDC;
	//게임 시작 딜레이
	float m_fStartDelay = 0.f;
	float m_fSoundDelay = 0.f;
	float m_fSoundDuration = 0.5f;
	float m_fDelayDuartion = 5.f;
	bool m_bStart = false;
	bool m_bSoundPlay = false;
};
