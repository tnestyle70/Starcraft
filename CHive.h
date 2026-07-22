#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CHive : public CBuilding, public Commandable
{
public:
	CHive();
	virtual ~CHive();
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
private:
	void SpawnZergUnits(); //오버로드 스폰
	float m_fSpawnTimer = 0.f; 
	float m_fSpawnInterval = 3.f;
	int m_iRushCount = 0;
	void UpdateProduction();
	void ProductionComplete(eCommandID command);
};