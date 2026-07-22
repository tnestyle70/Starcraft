#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CAssimilator : public CBuilding, public Commandable
{
public:
	CAssimilator();
	virtual ~CAssimilator();
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
	void Rally() override;
	void Lift() override;
	void Landing() override;
	void BuildAddOn() override;
	void SetBuildingData() override;
	void Destroy() override;
	int GetRequiredTileValue() const override { return 3; } //°¡½º 
private:
	void BuildCombatStation();
	void BuildNuclearSilo();
	void UpdateProduction();
	void ProductionComplete(eCommandID command);
	void UseScanner();
private:
	CBuilding* m_pAddOn = nullptr;
};


