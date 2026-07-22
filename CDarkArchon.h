#pragma once
#include "CUnit.h"

class CDarkArchon : public CUnit
{
public:
	CDarkArchon();
	virtual ~CDarkArchon();
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
	void MindControl();
private:
	int m_iAuraFrame = 0;
	int m_iAuraFrameEnd = 14;
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};



