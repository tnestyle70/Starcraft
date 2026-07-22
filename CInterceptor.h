#pragma once
#include "CUnit.h"

class CCarrier;

class CInterceptor : public CUnit
{
public:
	CInterceptor();
	virtual ~CInterceptor();
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
	bool UpdateMove(Order& order) override;
	bool UpdateRAttack(Order& order) override;
	void UpdateDead() override;
private:
	void FireBullet();
public:
	void SetOwner(CCarrier* pOwner) { pOwner = m_pOwner; }
private:
	CCarrier* m_pOwner = nullptr;
	Vec2 m_vBackwardDir { 0,0 };
	float m_fBackwardDist = 100.f; //뒤로 이동할 거리
	float m_fBackwardCur = 0.f; //현재까지 후퇴한 거리
	bool m_bChasing = true; //타겟으로 돌진 여부
};






