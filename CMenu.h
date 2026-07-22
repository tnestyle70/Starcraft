#pragma once
#include "Define.h"
#include "CScene.h"
#include <thread>
#include <atomic>

class CMenu : public CScene
{
public:
	CMenu();
	virtual ~CMenu();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
private:
	void LoadResource();
	void InitializeAnim();
	void InitializeButton();
private:
	float m_fLogoTime = 0.f;
	float m_fLogoDuration = 3.f;
	bool m_bLogo = true;
private:
	std::thread m_LoadThread;
	std::atomic<bool> m_bLoadComplete;
};