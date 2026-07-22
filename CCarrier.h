#pragma once
#include "CUnit.h"

class CInterceptor;

class CCarrier : public CUnit
{
public:
	CCarrier();
	virtual ~CCarrier();
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
	bool UpdateAttackMove(Order& order) override;
	bool UpdateRAttack(Order& order) override;
public:
	void InterceptorDestroyed(CInterceptor* pIntercep);
private:
	void RetreatInterceptor();
	void FireBullet();
	vector<CInterceptor*> m_vecInterceptor;
	bool m_bSpawnIntercep = false;
};





