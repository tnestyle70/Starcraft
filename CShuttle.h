#pragma once
#include "CUnit.h"

class CShuttle : public CUnit
{
public:
	CShuttle();
	virtual ~CShuttle();
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
	void LoadUnit(CUnit* pUnit);
	void UnloadUnit();
private:
	void ShowLoadUnits();
private:
	vector<CUnit*> m_vecLoadUnits; //¼ÅÆ²¿¡ Å¾½ÂÇÑ À¯´Ö¼ö
public:
	void RenderPower(HDC hDC);
private:
	void FireBullet();
	bool m_bPower = false;
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};


