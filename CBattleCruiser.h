#pragma once
#include "CUnit.h"

class CBattleCruiser : public CUnit
{
public:
	CBattleCruiser();
	virtual ~CBattleCruiser();
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
public:
	//공격 
	void FireBullet();
	void FireYamato();
	//에너지 
	int Get_Energy() const { return m_iEnergy; }
private:
	//에너지 관련
	int m_iEnergy;
	int m_iMaxEnergy;
	//야마토
	int m_iYamatoCost;
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};


