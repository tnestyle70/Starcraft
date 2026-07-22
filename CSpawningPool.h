#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CSpawningPool : public CBuilding, public Commandable
{
public:
	CSpawningPool();
	virtual ~CSpawningPool();
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
	void PlayCompleteSound() override;
	void SetBuildingData() override;
	void Destroy() override;
private:
	void SpawnZergUnits();
	float m_fSpawnTimer = 0.f;
	float m_fSpawnInterval = 10.f;
	int m_iSpawnCount = 0;
	void UpdateProduction();
	void ProductionComplete(eCommandID command);
};
