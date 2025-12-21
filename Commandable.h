#pragma once
#include "Define.h"
#include "CommandSlot.h"
#include <vector>

class Commandable 
{
public:
	virtual ~Commandable() PURE;
	//유닛, 건물 선택시 slot에 보여줄 슬롯
	virtual void CommandCardSlot(std::vector<CommandSlot>& outSlot) PURE;
	//슬롯 클릭시 실행
	virtual bool ExecuteCommand(eCommandID command, CommandContext& context) PURE;
};
