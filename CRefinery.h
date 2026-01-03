#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CRefinery : public CBuilding, public Commandable
{
public:
	CRefinery();
	virtual ~CRefinery();
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
	void SetBuildingData() override;
	void Destroy() override;
	int GetRequiredTileValue() const override { return 3; } //°¡½º
private:
	void UpdateProduction();
	void ConstructComplete(eCommandID command);
};