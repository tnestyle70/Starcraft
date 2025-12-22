#pragma once
#include "CUnit.h"
#include "CBuilding.h"

class CSCV : public CUnit
{
public:
	CSCV();
	virtual ~CSCV();
public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
public:
	void UpdateHotKeys() override;
	void UpdateBuilding();
	bool ExecuteCommand(eCommandID command, CommandContext& context);
	void CommandCardSlot(vector<CommandSlot>& outSlot);
private:
	//건물 모드
	void StartBuildMode(eBuildingType buildingType);
	void CancelBuilding();
	void PlaceBuilding(const Vec2& worldPos);
	//CUnit쪽 깡통 가상 함수 오버라이딩
	virtual void FinalizeBuild(Order& order) override;
private:
	CBuilding* m_pGhostBuilding;
	bool m_bBuildingMode;
};


