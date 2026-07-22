#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CScienceSecret : public CBuilding, public Commandable
{
public:
	CScienceSecret();
	virtual ~CScienceSecret();
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
	void PlayCompleteSound() override;
private:
	void UseScanner();
	void UpdateAnimation() override;
	void UpdateProduction();
	void ConstructComplete(eCommandID command);
private:
	bool m_bReverse = false;
};

