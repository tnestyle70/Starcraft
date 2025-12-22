#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CStarport : public CBuilding, public Commandable
{
public:
	CStarport();
	virtual ~CStarport();
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
	void ConstructComplete() override;
	void Destroy() override;
private:
	void UpdateProduction();
	void ConstructComplete(eCommandID command);
};