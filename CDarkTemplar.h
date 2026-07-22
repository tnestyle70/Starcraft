#pragma once
#include "CUnit.h"

class CDarkTemplar : public CUnit
{
public:
	CDarkTemplar();
	virtual ~CDarkTemplar();
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
public:
	bool IsMerging() { return m_bMerging; }
private:
	void FireBullet();
	void Wrap();
	float m_fWrapDistance = 50.f; //합체 가능 사거리
	//합체 플래그를 사용해서 아칸 한 마리만 생성되도록 한다.
	bool m_bMerging = false;
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};



