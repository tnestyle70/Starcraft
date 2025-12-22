#pragma once
#include "CObj.h"
#include <tchar.h>

//커맨드 카드 종류
enum class eCommandID
{
	//기본 커맨드
	MOVE = 0,
	STOP = 1,
	ATTACK = 2,
	PATROL = 3,
	HOLD = 4,
	CANCLE = 5,
	RALLY = 6,
	GATHER = 7,
	BUILD = 8,
	REPARI = 9,
	// 유닛 아이콘 (10~)
	SCV = 11,
	MARINE = 12,
	MEDIC = 13,
	TANK = 16,
	BATTLECRUISER = 20,
	//건물 아이콘
	COMMAND_CENTER = 22,
	SUPPLY_DEPOT = 23,
	BARRACKS = 24,
	NONE = 100
};

//커맨드 카드 1칸
struct CommandSlot
{
	int slotIndex; // 3 * 3 커맨드 카드 슬롯 인덱스
	eCommandID commandID;
	const TCHAR* iconKey; 
	int hotkey;
	bool clickable; //클릭 가능 여부 
	bool visible; //표시 여부
};

//명령 실행에 필요한 상황 정보
struct CommandContext
{
	Vec2 worldPos;
	//공격, 수리, 치료 같은 대상 지정 커맨드
	CObj* pTarget; 
	//필요시 확장
};