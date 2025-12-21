#pragma once
#include "CObj.h"
#include <tchar.h>

//커맨드 카드 종류
enum class eCommandID 
{
	NONE = 0, 
	//유닛 공통
	MOVE, STOP, HOLD, PATROL, ATTACKMOVE,
	//빌딩
	CANCLE,
	//배럭
	TRAIN_MARIN, TRAIN_MEDIC,
	//커맨드 센터
	TRAIN_SCV
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