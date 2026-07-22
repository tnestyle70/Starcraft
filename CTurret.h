#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CTurret : public CBuilding, public Commandable
{
public:
	CTurret();
	virtual ~CTurret();
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
	void UpdateAttack() override;
	void FireBullet() override;
	CObj* FindNearestEnemy() override;
	void SetBuildingData() override;
	void Destroy() override;
private:
	void UpdateAnimation() override;
	void UpdateProduction();
	void ConstructComplete(eCommandID command);
};