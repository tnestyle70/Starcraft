#include "pch.h"
#include "CCollisionMgr.h"
/*
void CCollisionMgr::Collision_Rect(list<CObj*> Dst, list<CObj*> Src)
{
	RECT	rc{};


	for (auto& pDst : Dst)
	{
		for (auto& pSrc : Src)
		{
			if (IntersectRect(&rc, pDst->Get_Rect(), pSrc->Get_Rect()))
			{
				pDst->Set_Dead();
				pSrc->Set_Dead();
			}
		}
	}
}

void CCollisionMgr::Collision_Circle(list<CObj*> Dst, list<CObj*> Src)
{
	for (auto& pDst : Dst)
	{
		for (auto& pSrc : Src)
		{
			if (Check_Circle(pDst, pSrc))
			{
				pDst->Set_Dead();
				pSrc->Set_Dead();
			}
		}
	}
}

bool CCollisionMgr::Check_Circle(CObj* pDst, CObj* pSrc)
{

	float fWidth = abs(pDst->Get_Info().fX - pSrc->Get_Info().fX);
	float fHeight = abs(pDst->Get_Info().fY - pSrc->Get_Info().fY);

	float fDistance = sqrtf(fWidth * fWidth + fHeight * fHeight);

	float fRadius = (pDst->Get_Info().fCX + pSrc->Get_Info().fCX) * 0.5f;

	return fRadius >= fDistance;
}
*/