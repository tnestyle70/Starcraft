#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

enum class eHatcheryType
{
	HATCHERY, LAIR, HIVE
};

class CHatchery : public CBuilding, public Commandable
{
public:
	CHatchery();
	virtual ~CHatchery();
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
	void SpawnZergUnits(); //오버로드 스폰
	float m_fSpawnTimer = 0.f;
	float m_fSpawnInterval = 5.f;
	void UpdateProduction();
	void ProductionComplete(eCommandID command);
private:
	void CreateLava();
	bool m_bCreateLava = true;
	eHatcheryType m_eHatcheryType = eHatcheryType::HATCHERY;
	void MutateLair();
	void MutateHive();
	bool m_bMutatingLair = false;
	bool m_bMutatingHive = false;
	const TCHAR* m_pLareKey = nullptr;
	const TCHAR* m_pHiveKey = nullptr;
};