#include "pch.h"
#include "CCollisionMgr.h"
#include "CNavMgr.h"
#include "CUnit.h"
#include "CTimeMgr.h"

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

void CCollisionMgr::Check_Projectile_Collsion(list<CObj*>& projectile, list<CObj*>& enemy)
{
	
}

void CCollisionMgr::PushApart_Units(list<CObj*>& units, float minDistance)
{
	//O(n^2) 알고리즘 -> 최적화가 필요할 때 Spatial Hashing 사용
	for (auto it1 = units.begin(); it1 != units.end(); ++it1)
	{
		auto it2 = it1;
		++it2;
		for (; it2 != units.end(); ++it2)
		{
			CObj* unit1 = *it1;
			CObj* unit2 = *it2;
			//Dead면 스킵
			if (unit1->IsDead() || unit2->IsDead())
				continue;
			PushApart_TwoUnits(unit1, unit2, minDistance);
		}
	}
}

void CCollisionMgr::PushApart_TwoUnits(CObj* unit1, CObj* unit2, float minDistance)
{
	INFO info1 = unit1->Get_Info();
	INFO info2 = unit2->Get_Info();

	float dx = info2.fX - info1.fX;
	float dy = info2.fY - info1.fY;
	float distance = sqrtf(dx * dx + dy * dy);
	//너무 가까우면 밀어내기
	if (distance < minDistance && distance > 0.00001f)
	{
		//정규화된 방향 벡터
		float nx = dx / distance;
		float ny = dy / distance;
		//겹친 양 계산
		float overlap = minDistance - distance;
		//각 유닛을 절반씩 밀어냄
		float pushAmount = overlap * 0.5f;
		//unit1은 반대 방향
		info1.fX -= nx * pushAmount;
		info1.fY -= ny * pushAmount;
		unit1->Set_Pos(info1.fX, info1.fY);
		//unit2는 같은 방향
		info2.fX += nx * pushAmount;
		info2.fY += ny * pushAmount;
		unit2->Set_Pos(info2.fX, info2.fY);
		if (auto* u1 = dynamic_cast<CUnit*>(unit1)) u1->SetVelocity({ 0.f,0.f });
		if (auto* u2 = dynamic_cast<CUnit*>(unit2)) u2->SetVelocity({ 0.f,0.f });
	}
}

static bool IsWalkablePos(CNavMgr* nav, float x, float y)
{
	int r, c;
	if (!nav->WorldToCell({ x, y }, r, c))
		return false;
	return nav->IsWalkable(r, c);
}

void CCollisionMgr::ResolveWallCollision(CObj* unit)
{
	if (!unit || unit->IsDead())
		return;

	CNavMgr* nav = CNavMgr::Get_Instance();

	INFO info = unit->Get_Info();
	Vec2 cur{ info.fX, info.fY };

	// 중심점 기준으로 막힌 셀이 아니면 일단 통과
	if (IsWalkablePos(nav, cur.fX, cur.fY))
		return;

	// prev 계산 (가능하면 속도 기반으로 "직전 프레임 위치" 복원)
	Vec2 prev = cur;
	Vec2 vel{ 0.f, 0.f };
	float dt = CTimeMgr::Get_Instance()->GetDT();

	CUnit* u = dynamic_cast<CUnit*>(unit);
	if (u)
	{
		vel = u->GetVelocity();              // 없으면 CUnit에 getter 추가
		prev.fX = cur.fX - vel.fX * dt;
		prev.fY = cur.fY - vel.fY * dt;
	}

	// prev가 walkable이면, 여기서부터 축 분리 슬라이딩
	Vec2 resolved = prev;

	// 1) X 이동만 시도: (cur.x, prev.y)
	if (IsWalkablePos(nav, cur.fX, prev.fY))
	{
		resolved.fX = cur.fX;
	}
	else
	{
		// x방향으로 벽에 박힘 → x속도만 제거 (벽에 "붙는" 원인 제거)
		if (u) vel.fX = 0.f;
	}

	// 2) Y 이동 시도: (resolved.x, cur.y)
	if (IsWalkablePos(nav, resolved.fX, cur.fY))
	{
		resolved.fY = cur.fY;
	}
	else
	{
		if (u) vel.fY = 0.f;
	}

	// 3) 그래도 막혀있으면, 너무 멀리 스냅하지 말고 "짧게"만 스냅
	if (!IsWalkablePos(nav, resolved.fX, resolved.fY))
	{
		int r, c;
		if (nav->WorldToCell({ cur.fX, cur.fY }, r, c))
		{
			int snapR = r, snapC = c;

			// 5칸은 너무 커서 벽에 붙는/튕김이 과장됨 → 1~2 권장
			if (nav->SnapToNearestWalkable(snapR, snapC, 2))
			{
				resolved = nav->CellToWolrdCenter(snapR, snapC);
				if (u) vel = { 0.f, 0.f };
			}
			else
			{
				// 진짜 답 없으면 그냥 현위치 고정 + 속도 죽이기
				resolved = prev;
				if (u) vel = { 0.f, 0.f };
			}
		}
	}
	unit->Set_Pos(resolved.fX, resolved.fY);
	if (u) u->SetVelocity(vel);
}

bool CCollisionMgr::IsOverlapping(CObj* obj1, CObj* obj2, float threshold)
{
	float dist = GetDistance(obj1, obj2);
	INFO info1 = obj1->Get_Info();
	INFO info2 = obj2->Get_Info();
	//두 유닛의 반경 합
	float combinedRadius = (info1.fCX + info2.fCX) * 0.5f + threshold;
	return dist < combinedRadius;
}

float CCollisionMgr::GetDistance(CObj* obj1, CObj* obj2)
{
	INFO info1 = obj1->Get_Info();
	INFO info2 = obj2->Get_Info();
	float dx = info2.fX - info1.fX;
	float dy = info2.fY - info1.fY;

	return sqrtf(dx * dx + dy * dy);
}

list<CObj*> CCollisionMgr::FindUnitsInRadius(CObj* center, list<CObj*> allUnits, float radius)
{
	list<CObj*> result;

	if (!center || center->IsDead())
		return result;

	INFO centerInfo = center->Get_Info();

	for (auto* unit : allUnits)
	{
		if (unit == center || unit->IsDead())
			continue;

		INFO unitInfo = unit->Get_Info();
		float dx = unitInfo.fX - centerInfo.fX;
		float dy = unitInfo.fY - centerInfo.fY;
		float distance = sqrtf(dx * dx + dy * dy);

		if (distance < radius)
		{
			result.push_back(unit);
		}
	}

	return result;
}
