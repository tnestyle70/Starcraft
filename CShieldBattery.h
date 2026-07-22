#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CShieldBattery : public CBuilding, public Commandable
{
public:
	CShieldBattery();
	virtual ~CShieldBattery();
public:
	void Initialize() override;
	int Update() override;
	void Render(HDC hDC) override;
	void Release();
	void RenderPylonPower(HDC hDC) override;
	//ICommandable
	void RenderSlot(HDC hDC, int slotIndex);
	int GetIconIndex(eCommandID eCommand);
	void CommandCardSlot(std::vector<CommandSlot>& outSlot) override;
	bool ExecuteCommand(eCommandID command, CommandContext& context) override;
protected:
	void SetBuildingData() override;
	void Destroy() override;
private:
	void UpdateRefill();
private:
	float m_fShieldRange = 150.f;
	float m_fShieldInterval = 0.5f;
	float m_fShieldTimer = m_fShieldInterval;
	int m_iShieldRefill = 5;
	CBuilding* m_pAddOn = nullptr;
};

