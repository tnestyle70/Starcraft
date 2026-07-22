#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CPhotonCannon : public CBuilding, public Commandable
{
public:
	CPhotonCannon();
	virtual ~CPhotonCannon();
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
	CBuilding* m_pAddOn = nullptr;
};


