#pragma once

#include "CObj.h"

class CCollisionMgr
{
public:
	static void	Collision_Rect(list<CObj*> Dst, list<CObj*> Src);
	static void	Collision_Circle(list<CObj*> Dst, list<CObj*> Src);
	static bool	Check_Circle(CObj* pDst, CObj* pSrc);
};

