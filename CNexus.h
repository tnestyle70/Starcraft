#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CNexus : public CBuilding, public Commandable
{
public:
	CNexus();
	virtual ~CNexus();
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
private:
	void BuildCombatStation();
	void BuildNuclearSilo();
	void UpdateProduction();
	void ProductionComplete(eCommandID command);
	void UseScanner();
private:
	CBuilding* m_pAddOn = nullptr;
private:
	void SpawnProtossEnemy();
	float m_fSpawnTimer = 0.f;
	float m_fSpawnInterval = 5.f;
	int m_iRushCount = 0;
};
