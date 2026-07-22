#pragma once
#include "CUnit.h"

class CDragon : public CUnit
{
public:
	CDragon();
	virtual ~CDragon();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
protected:
	void UpdateHotKeys() override;
	bool ExecuteCommand(eCommandID command, CommandContext& context);
	void CommandCardSlot(vector<CommandSlot>& outSlot);
protected:
	void UpdateDead() override;
private:
	void Blink();
	float m_fBlinkRange = 100.f; 
	void FireBullet();
	int m_iAttackStartFrame = 0;
	int m_iAttackEndFrame = 0;
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};




