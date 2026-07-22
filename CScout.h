#pragma once
#include "CUnit.h"

class CScout : public CUnit
{
public:
	CScout();
	virtual ~CScout();
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
	void FireBullet();
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};





