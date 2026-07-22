#pragma once
#include "CUnit.h"

class CReavor : public CUnit
{
public:
	CReavor();
	virtual ~CReavor();
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
};





