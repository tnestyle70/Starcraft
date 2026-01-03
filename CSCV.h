#pragma once
#include "CUnit.h"
#include "CBuilding.h"

enum class eResourceType
{
	NONE, MINERAL, GAS
};

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
	bool ExecuteCommand(eCommandID command, CommandContext& context);
	void CommandCardSlot(vector<CommandSlot>& outSlot);
	void SetResourceType(eResourceType eType) { m_eResourceType = eType; }
protected:
	bool UpdateGather(Order& order) override;
	bool UpdateReturn(Order& order) override;
private:
	CBuilding* m_pGhostBuilding;
	bool m_bBuildingMode;
	//자원
	CObj* m_pTargetResource = nullptr;
	eResourceType m_eResourceType = eResourceType::NONE;
	int m_iCarryingResource; //들고 있는 미네랄 양
	DWORD m_dwGatherStartTime; //자원 채집 시작 시간
	static const int MINERLA_PER_ONCE = 8; //한 번에 채집 가능한 미네랄 
	static const int GAS_PER_ONCE = 8; //한 번에 채집 가능한 가스
	static const DWORD GATHER_TIME = 1000; //채집 시간
};


