#pragma once
#include "CBuilding.h"	
#include "Commandable.h"
#include <deque>
#include <vector>

class CCommandCenter : public CBuilding, public Commandable
{
public:
	CCommandCenter();
	virtual ~CCommandCenter();
public:
	void Initialize() override;
	int Update() override;
	void Render(HDC hDC) override;
	void Release();
	//ICommandable
	void CommandCardSlot(std::vector<CommandSlot>& outSlot) override;
	bool ExecuteCommand(eCommandID command, CommandContext& context) override;
protected:
	void SetBuildingData() override;
	void ConstructComplete() override;
	void Destroy() override;
private:
	void UpdateProduction();
	void ConstructComplete(eCommandID command);
private:
	std::deque<ProdJob> m_queue;
};