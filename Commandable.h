#pragma once
#include "Define.h"
#include "CommandSlot.h"
#include <vector>

class CObj;
class CBuilding;

enum class eOrderType
{
	MOVE, 
	STOP, 
	GATHER, 
	RETURN_RESOURCE, 
	ATTACK, 
	ATTACK_MOVE,
	MOVE_AND_BUILD, 
	CONSTRUCTING, 
	HOLD, 
	ENTER_BUNKER,
	ENTER_SHUTTLE
};

struct Order
{
	eOrderType eType;
	Vec2 dst; //목적지 위치
	vector<Vec2> path; //A star를 통해 찾은 경로 {}{}{}{}
	int iPathIndex; //웨이포인트의 인덱스 1 -> 2 -> 3 이런 식으로 path 내에 있는 인덱스
	CObj* pTarget = nullptr;
	CBuilding* pBuilding = nullptr;
	Order() : iPathIndex(0) {}
	//Order() : eType(eOrderType::MOVE), iPathIndex(0) {}
};

enum class eCommandCardState
{
	MAIN,
	NORMAL_BUILD,
	ADVANCED_BUILD,
	NORMAL_TANK,
	SIEGE_TANK,
	HATCHERY,
	LAIR,
	HIVE
};

class Commandable 
{
public:
	virtual ~Commandable() PURE;
	//유닛, 건물 선택시 slot에 보여줄 슬롯
	virtual void CommandCardSlot(std::vector<CommandSlot>& outSlot) PURE;
	//슬롯 클릭시 실행
	virtual bool ExecuteCommand(eCommandID command, CommandContext& context) PURE;
};
