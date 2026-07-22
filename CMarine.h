#pragma once
#include "CUnit.h"

class CMarine : public CUnit
{
public:
	CMarine();
	virtual ~CMarine();
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
	void UpdateGarrisoned() override;
	void UpdateDead() override;
private:
	void UpdateSteamPack();
	void FireBullet();
private:
	//½ºÆÀÆÑ
	bool m_bSteamPackActive = false;
	float m_fSteamPackUse = 0.f;
	float m_fSteamPackDuration = 3.f;
	float m_fOriginalSpeed;
	float m_fOriginalAttackSpeed;
private:
	void UpdateAI();
	CObj* FindNearestEnemyAI(float searchRadius);
};
