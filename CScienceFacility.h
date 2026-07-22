#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CScienceFacility : public CBuilding, public Commandable
{
public:
	CScienceFacility();
	virtual ~CScienceFacility();
public:
	void Initialize() override;
	int Update() override;
	void Render(HDC hDC) override;
	void Release();
	//ICommandable
	void RenderSlot(HDC hDC, int slotIndex);
	int GetIconIndex(eCommandID eCommand);
	void CommandCardSlot(std::vector<CommandSlot>& outSlot) override;
	bool ExecuteCommand(eCommandID command, CommandContext& context) override;
	void UpdateHotKeys();
protected:
	void BuildAddOn() override;
	void UpdateAnimation() override;
	void SetBuildingData() override;
	void Destroy() override;
private:
	void BuildScienceSecret();
	void BuildSciencePhysics();
	void UpdateProduction();
	void ProductionComplete(eCommandID command);
private:
	CBuilding* m_pAddOn = nullptr;
};

