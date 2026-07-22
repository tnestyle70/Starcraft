#pragma once
#include "CUnit.h"

class CArbiter : public CUnit
{
public:
	CArbiter();
	virtual ~CArbiter();
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
	void Recall(); //¸®ÄÝ
	float m_fRecallRange = 100.f;
	void UpdateConceal(); //ÀºÆó
	float m_fConcealRange = 200.f;
	float m_fConcealInterval = 0.3f;
	float m_fConcealTimer = m_fConcealInterval;
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};




