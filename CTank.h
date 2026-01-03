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
	bool UpdateAttack(Order& order) override;
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
	//포탑
	Vec2 m_vHeadDir;
	int m_iHeadFrame;
	//공격
	bool m_bFiring;
	int m_iFireFrame;
	DWORD m_dwFireStart;
	//시즈 모드 
	bool m_bTransforming;
	bool m_bSiegeMode;
	float m_fSiegeRange;
	float m_fTankRange;
	const TCHAR* m_pHeadKey;  
};

