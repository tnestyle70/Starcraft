#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>
#include "CUnit.h"

class CBunker : public CBuilding, public Commandable
{
public:
	CBunker();
	virtual ~CBunker();
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
public:
	void UnloadUnit(); //벙커에서 유닛 내리게 하기
	void LoadUnit(CUnit* pUnit); //벙커에 유닛 수용 시키기
private:
	void ShowLoadUnits();
private:
	vector<CUnit*> m_vecLoadUnits; //벙커에 존재하는 유닛
	int m_iMaxCapacity = 4; //최대 수용 가능 유닛 수 
	//float m_fAttackRange; //벙커 공격 범위?
};
