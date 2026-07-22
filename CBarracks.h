#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CBarracks : public CBuilding, public Commandable
{
public:
	CBarracks();
	virtual ~CBarracks();
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
	void UpdateAnimation() override;
	void SetBuildingData() override;
	void Destroy() override;
private:
	void UpdateProduction();
	void ProductionComplete(eCommandID command);
private:
	void SpawnTerranEnemy(); //배럭 유닛 스폰
	float m_fSpawnTimer = 0.f;
	float m_fSpawnInterval = 10.f;
	int m_iSpawnCount = 0;
};