#pragma once
#include "CUnit.h"

class CGhost : public CUnit
{
public:
	CGhost();
	virtual ~CGhost();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
public:
	void UpdateHotKeys() override;
	bool ExecuteCommand(eCommandID command, CommandContext& context);
	void CommandCardSlot(vector<CommandSlot>& outSlot);
protected:
	void UpdateDead() override;
	void FireBullet(); 
	void UpdateAttackTimer();
private:
	void Clocking();
	void NuclearLaunch();
	void NuclearLaunchEnemy(Vec2 worldMouse);
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};

