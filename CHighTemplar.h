#pragma once
#include "CUnit.h"

class CHighTemplar : public CUnit
{
public:
	CHighTemplar();
	virtual ~CHighTemplar();
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
	void UseStorm();
	void AreaDamage(Vec2 stormPos);
	void Wrap();
private:
	int m_iShadFrame;
	int m_iShadFrameEnd;
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};







