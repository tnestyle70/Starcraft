#include "pch.h"
#include "CSteeringMgr.h"
#include "CObj.h"
#include "CUnit.h"
#include "CNavMgr.h"

CSteeringMgr* CSteeringMgr::m_pInstance = nullptr;

Vec2 CSteeringMgr::Normalize(const Vec2& v)
{
	float mag = Magnitude(v);
	if (mag > 0.00001f) //0으로 나누기 방지
		return { v.fX / mag, v.fY / mag };
	return { 0.f, 0.f };
}

float CSteeringMgr::Magnitude(const Vec2& v)
{
	return sqrtf(v.fX * v.fX + v.fY * v.fY);
}

Vec2 CSteeringMgr::Limit(const Vec2& v, float max)
{
	float mag = Magnitude(v);
	if (mag > max && mag > 0.0001f)
	{
		return { (v.fX / mag) * max, (v.fY / mag) * max };
	}
	return v;
}

float CSteeringMgr::Distance(const Vec2& a, const Vec2& b)
{
	float dx = b.fX - a.fX;
	float dy = b.fY - a.fY;
	return sqrtf(dx * dx + dy * dy);
}

list<CObj*> CSteeringMgr::FindNeighbors(CObj* me, const list<CObj*>& units, float radius)
{
	list<CObj*> neighbors;
	INFO meInfo = me->Get_Info();
	Vec2 mePos = me->Get_Pos();

	for (auto* other : units)
	{
		if (other == me) continue;

		INFO otherInfo = other->Get_Info();
		Vec2 otherPos = other->Get_Pos();
		if (Distance(mePos, otherPos) < radius)
		{
			neighbors.push_back(other);
		}
	}
	return neighbors;
}

float CSteeringMgr::Dot(const Vec2& a, const Vec2& b)
{
    return a.fX * a.fX + a.fY * a.fY;
}

// ========== 기본 Steering Behaviors 구현 ==========

Vec2 CSteeringMgr::Seek(const Vec2& position, const Vec2& velocity, const Vec2& target, float maxSpeed)
{
    // 목표를 향한 방향
    Vec2 desired = { target.fX - position.fX, target.fY - position.fY };
    desired = Normalize(desired);
    desired.fX *= maxSpeed;
    desired.fY *= maxSpeed;

    // 조향력 = 원하는 속도 - 현재 속도
    Vec2 steer = { desired.fX - velocity.fX, desired.fY - velocity.fY };

    return steer;
}

Vec2 CSteeringMgr::Arrival(const Vec2& position, const Vec2& velocity, const Vec2& target, float maxSpeed, float slowRadius)
{
    Vec2 desired = { target.fX - position.fX, target.fY - position.fY };
    float distance = Magnitude(desired);

    desired = Normalize(desired);

    // 감속 구역 내에 있으면 속도 줄이기
    if (distance < slowRadius)
    {
        float speed = maxSpeed * (distance / slowRadius);
        desired.fX *= speed;
        desired.fY *= speed;
    }
    else
    {
        desired.fX *= maxSpeed;
        desired.fY *= maxSpeed;
    }

    Vec2 steer = { desired.fX - velocity.fX, desired.fY - velocity.fY };
    return steer;
}

Vec2 CSteeringMgr::Flee(const Vec2& position, const Vec2& velocity, const Vec2& target, float maxSpeed)
{
    // Seek의 반대 방향
    Vec2 desired = { position.fX - target.fX, position.fY - target.fY };
    desired = Normalize(desired);
    desired.fX *= maxSpeed;
    desired.fY *= maxSpeed;

    Vec2 steer = { desired.fX - velocity.fX, desired.fY - velocity.fY };
    return steer;
}

// ========== 군집 Behaviors 구현 ==========

Vec2 CSteeringMgr::Separation(CObj* me, const list<CObj*>& neighbors, float radius,
    const Vec2& meVel, float maxSpeed)
{
    Vec2 steer = { 0.f, 0.f };
    int count = 0;

    INFO meInfo = me->Get_Info();
    Vec2 mePos = { meInfo.fX, meInfo.fY };

    for (auto* other : neighbors)
    {
        INFO otherInfo = other->Get_Info();
        Vec2 otherPos = { otherInfo.fX, otherInfo.fY };

        float dist = Distance(mePos, otherPos);

        if (dist > 0.0001f && dist < radius)
        {
            // 상대방으로부터 멀어지는 방향
            Vec2 diff = { mePos.fX - otherPos.fX, mePos.fY - otherPos.fY };
            diff = Normalize(diff);

            // 가까울수록 더 강하게 밀어냄
            diff.fX /= dist;
            diff.fY /= dist;

            steer.fX += diff.fX;
            steer.fY += diff.fY;
            count++;
        }
    }

    if (count > 0)
    {
        steer.fX /= count;
        steer.fY /= count;
    }
    //방향 -> desireVel -> steerForce로 변환
    Vec2 desired = Normalize(steer);
    desired.fX *= maxSpeed;
    desired.fY *= maxSpeed;

    return {desired.fX - meVel.fX, desired.fY - meVel.fY};
}

Vec2 CSteeringMgr::Alignment(CObj* me, const list<CObj*>& neighbors, float radius,
    const Vec2& meVel, float maxSpeed)
{
    Vec2 avgVelocity = { 0.f, 0.f };
    int count = 0;

    INFO meInfo = me->Get_Info();
    Vec2 mePos = { meInfo.fX, meInfo.fY };

    for (auto* other : neighbors)
    {
        INFO otherInfo = other->Get_Info();
        Vec2 otherPos = { otherInfo.fX, otherInfo.fY };

        float dist = Distance(mePos, otherPos);

        if (dist < radius)
        {
            //유닛만 velocity가 있으므로 캐스팅
            if (auto* unit = dynamic_cast<CUnit*>(other))
            {
                Vec2 vel = unit->GetVelocity();
                avgVelocity.fX += vel.fX;
                avgVelocity.fY += vel.fY;
                count++;
            }
        }
    }

    if (count <= 0)
        return { 0.f, 0.f };

    avgVelocity.fX /= count;
    avgVelocity.fY /= count;

    Vec2 desired = Normalize(avgVelocity);
    desired.fX *= maxSpeed;
    desired.fY *= maxSpeed;

    return { desired.fX - meVel.fX, desired.fY - meVel.fY };
}

Vec2 CSteeringMgr::Cohesion(CObj* me, const list<CObj*>& neighbors, float radius,
    const Vec2& meVel, float maxSpeed)
{
    Vec2 centerOfMass = { 0.f, 0.f };
    int count = 0;

    INFO meInfo = me->Get_Info();
    Vec2 mePos = { meInfo.fX, meInfo.fY };

    for (auto* other : neighbors)
    {
        INFO otherInfo = other->Get_Info();
        Vec2 otherPos = { otherInfo.fX, otherInfo.fY };

        float dist = Distance(mePos, otherPos);

        if (dist < radius)
        {
            centerOfMass.fX += otherPos.fX;
            centerOfMass.fY += otherPos.fY;
            count++;
        }
    }

    if (count <= 0)
        return { 0.f, 0.f };
    centerOfMass.fX /= count;
    centerOfMass.fY /= count;

    return Seek(mePos, meVel, centerOfMass, maxSpeed);
}

// ========== 충돌 회피 구현 ==========

Vec2 CSteeringMgr::CollisionAvoidance(const Vec2& position, const Vec2& velocity,
    const list<CObj*>& obstacles, float lookAhead)
{
    // 현재 속도 방향으로 lookAhead 거리만큼 앞을 본다
    Vec2 ahead = Normalize(velocity);
    ahead.fX *= lookAhead;
    ahead.fY *= lookAhead;

    Vec2 futurePos = { position.fX + ahead.fX, position.fY + ahead.fY };

    CObj* mostThreatening = nullptr;
    float minDist = FLT_MAX;

    // 가장 위협적인 장애물 찾기
    for (auto* obs : obstacles)
    {
        INFO obsInfo = obs->Get_Info();
        Vec2 obsPos = { obsInfo.fX, obsInfo.fY };

        float dist = Distance(futurePos, obsPos);

        if (dist < minDist)
        {
            minDist = dist;
            mostThreatening = obs;
        }
    }

    // 충돌 가능성이 있으면 회피
    if (mostThreatening && minDist < 50.f)
    {
        INFO obsInfo = mostThreatening->Get_Info();
        Vec2 obsPos = { obsInfo.fX, obsInfo.fY };

        return Flee(position, velocity, obsPos, 100.f);
    }

    return { 0.f, 0.f };
}

Vec2 CSteeringMgr::WallAvoidance(const Vec2& position, const Vec2& velocity, float lookAhead)
{
    // NavMgr를 사용하여 앞쪽 타일이 walkable한지 확인
    CNavMgr* navMgr = CNavMgr::Get_Instance();

    Vec2 ahead = Normalize(velocity);
    ahead.fX *= lookAhead;
    ahead.fY *= lookAhead;

    Vec2 futurePos = { position.fX + ahead.fX, position.fY + ahead.fY };

    int futureR, futureC;
    if (navMgr->WorldToCell(futurePos, futureR, futureC))
    {
        if (!navMgr->IsWalkable(futureR, futureC))
        {
            // 벽이 있으면 반대 방향으로 회피
            return { -ahead.fX * 2.f, -ahead.fY * 2.f };
        }
    }

    return { 0.f, 0.f };
}

// ========== 복합 Behaviors 구현 ==========

SteeringOutput CSteeringMgr::Flocking(CObj* me, const list<CObj*>& neighbors,
    const SteeringParams& params, const Vec2& meVel)
{
    SteeringOutput output;

    // 각 행동 계산
    Vec2 sep = Separation(me, neighbors, params.separationRadius, meVel, params.maxSpeed);
    Vec2 ali = Alignment(me, neighbors, params.alignmentRadius, meVel, params.maxSpeed);
    Vec2 coh = Cohesion(me, neighbors, params.cohesionRadius, meVel, params.maxSpeed);

    // 가중치 적용
    sep.fX *= params.separationWeight;
    sep.fY *= params.separationWeight;

    ali.fX *= params.alignmentWeight;
    ali.fY *= params.alignmentWeight;

    coh.fX *= params.cohesionWeight;
    coh.fY *= params.cohesionWeight;

    // 조합
    output.linear.fX = sep.fX + ali.fX + coh.fX;
    output.linear.fY = sep.fY + ali.fY + coh.fY;

    // 최대 힘 제한
    output.linear = Limit(output.linear, params.maxForce);

    return output;
}

SteeringOutput CSteeringMgr::FollowPathWithFlocking(
    CObj* me,
    const vector<Vec2>& path,
    int& currentWaypointIndex,
    const list<CObj*>& neighbors,
    const SteeringParams& params,
    const Vec2& _meVel)
{
    SteeringOutput output;

    // 수정 1: velocity는 me에서 직접 가져오기
    Vec2 meVel = me->GetVelocity();
    Vec2 mePos = me->Get_Pos();

    // 경로 체크
    if (path.empty() || currentWaypointIndex >= path.size())
    {
        // 수정 2: 경로 완료 시 급정거!
        // 빈 output이 아니라 반대 힘을 줌
        output.linear.fX = -meVel.fX * 2.0f;
        output.linear.fY = -meVel.fY * 2.0f;
        return output;
    }

    // 현재 웨이포인트
    Vec2 waypoint = path[currentWaypointIndex];

    // 웨이포인트 도달 체크
    float distToWaypoint = Distance(mePos, waypoint);

    // 수정 3: 임계값을 더 크게 (10 → 15)
    float waypointThreshold = 15.f;

    // 수정 4: 마지막 웨이포인트는 더 엄격하게
    bool bIsFinal = (currentWaypointIndex >= (int)path.size() - 1);
    if (bIsFinal)
    {
        waypointThreshold = 8.f;  // 최종 목표는 정확히
    }

    if (distToWaypoint < waypointThreshold)
    {
        currentWaypointIndex++;

        if (currentWaypointIndex >= path.size())
        {
            // 수정 5: 최종 도착 시 급정거
            output.linear.fX = -meVel.fX * 3.0f;  // 강한 역가속
            output.linear.fY = -meVel.fY * 3.0f;
            return output;
        }

        waypoint = path[currentWaypointIndex];
        distToWaypoint = Distance(mePos, waypoint);  // 재계산
        bIsFinal = (currentWaypointIndex >= (int)path.size() - 1);
    }

    // 수정 6: 마지막만 Arrival, 나머지는 Seek
    Vec2 pathForce;
    if (bIsFinal)
    {
        // 마지막 웨이포인트: 감속하며 접근
        pathForce = Arrival(mePos, meVel, waypoint,
            params.maxSpeed, params.arrivalRadius);
    }
    else
    {
        // 중간 웨이포인트: 빠르게 통과
        pathForce = Seek(mePos, meVel, waypoint, params.maxSpeed);
    }

    // 수정 7: Flocking도 현재 velocity 고려
    SteeringOutput flocking = Flocking(me, neighbors, params, meVel);

    // 조합
    output.linear.fX = pathForce.fX * params.seekWeight + flocking.linear.fX;
    output.linear.fY = pathForce.fY * params.seekWeight + flocking.linear.fY;

    // 최대 힘 제한
    output.linear = Limit(output.linear, params.maxForce);

    return output;
}