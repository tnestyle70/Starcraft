#pragma once
#include "CUnit.h"

class CTank : public CUnit
{
public:
	CTank();
	virtual ~CTank();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
public:
	//시즈 모드
	void ToggleSiegeMode();
	bool IsSiegeMode() const { return m_bSiegeMode; }
protected:
	bool UpdateRAttack(Order& order) override;
	void UpdateDead() override;
private:
	void UpdateBody();
	void UpdateHead();
	void RenderBody(HDC hDC);
	void RenderHead(HDC hDC);
public:
	void UpdateHotKeys() override;
	bool ExecuteCommand(eCommandID command, CommandContext& context);
	void CommandCardSlot(vector<CommandSlot>& outSlot);
private:
	void FireNormalMode();
	void FireSiegeMode();
private:	
	//포탑
	Vec2 m_vHeadDir;
	int m_iHeadFrame;
	//공격
	DWORD m_dwFireStart;
	int m_iFireFrame;
	int m_iFireFrameTimer = 0;
	int m_iFireFrameDuration = 2;
	//시즈 모드 
	bool m_bTransforming;
	bool m_bSiegeMode;
	float m_fSiegeRange;
	float m_fTankRange;
	const TCHAR* m_pHeadKey;
	const TCHAR* m_pTankEffectKey;
	const TCHAR* m_pSiegeEffectKey;
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};

