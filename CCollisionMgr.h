#pragma once

#include "CObj.h"

class CCollisionMgr
{
public:
	static void	Collision_Rect(list<CObj*> Dst, list<CObj*> Src);
	static void	Collision_Circle(list<CObj*> Dst, list<CObj*> Src);
	static bool	Check_Circle(CObj* pDst, CObj* pSrc);
	static void Check_Projectile_Collsion(list<CObj*>&projectile, list<CObj*>&enemy);
	//유닛 간 물리적 밀어내기 
	static void PushApart_Units(list<CObj*>& units, float minDistance = 30.f);
	//두 유닛이 너무 가까우면 서로 밀어냄
	static void PushApart_TwoUnits(CObj* unit1, CObj* unit2, float minDistance);
	//유닛이 벽/장애물과 겹치지 않도록 보정
	static void ResolveWallCollision(CObj* unit);
	//충돌 감지 함수
	static bool IsOverlapping(CObj* obj1, CObj* obj2, float threshold = 0.f);
	static float GetDistance(CObj* obj1, CObj* obj2);
	//특정 반경 내 유닛들 찾기
	static list<CObj*> FindUnitsInRadius(CObj* center, list<CObj*> allUnits, float radius);
};

