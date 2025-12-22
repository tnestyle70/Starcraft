#pragma once
#include "CUnit.h"

class CSCV : public CUnit
{
public:
	CSCV();
	virtual ~CSCV();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
public:
	void UpdateHotKeys() override;
	void UpdateBuilding();
	bool ExecuteCommand(eCommandID command, CommandContext& context);
	void CommandCardSlot(vector<CommandSlot>& outSlot);
};


