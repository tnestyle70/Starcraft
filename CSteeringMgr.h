#pragma once
#include "Define.h"
#include <list>

class CObj;

//streering 행동 결과 구조체
struct SteeringOutput
{
	Vec2 linear; //선형 가속도
	float angular; //회전 가속도
	SteeringOutput() : linear({ 0.f, 0.f }), angular(0.f) {}

	void Reset()
	{
		linear.fX = 0.f;
		linear.fY = 0.f;
		angular = 0.f;
	}
};

//Steering 파라미터
struct SteeringParams
{
	float maxSpeed; //최대 속도
	float maxForce; //최대 조항력
	float separationRadius; //분리 반경
	float alignmentRadius; //정렬 반경
	float cohesionRadius; //응집 반경
	float arrivalRadius; //도착 감속 반경
	//가중치
	float separationWeight;
	float alignmentWeight;
	float cohesionWeight;
	float seekWeight;
	SteeringParams() : maxSpeed(100.f), maxForce(200.f),
		separationRadius(60.f), alignmentRadius(80.f),
		cohesionRadius(100.f), arrivalRadius(15.f),
		separationWeight(2.f) //분리가 핵심
		, alignmentWeight(0.5f), cohesionWeight(0.5f),
		seekWeight(2.f) {}
};

class CSteeringMgr
{
private:
	CSteeringMgr() {};
	~CSteeringMgr() {};
public:
	//목표를 향해 직진
	static Vec2 Seek(const Vec2& position, const Vec2& velocity,
		const Vec2& target, float maxSpeed);
	//목표에 가까워지면 감속
	static Vec2 Arrival(const Vec2& position, const Vec2& velocity,
		const Vec2& target, float maxSpeed, float slowRadius);
	//목표로부터 도망
	static Vec2 Flee(const Vec2& position, const Vec2& velocity,
		const Vec2& target, float maxSpeed);
	//군집 이동(FLocking)
	//Separation : 주변 유닛들 밀어내기
	static Vec2 Separation(CObj* me, const list<CObj*>& neighbors,
		float radius, const Vec2& meVel, float maxSpeed);
	//Alignment : 주변 유닛들과 방향 맞추기
	static Vec2 Alignment(CObj* me, const list<CObj*>& neighbors,
		float radius, const Vec2& meVel, float maxSpeed);
	//Cohesion : 주변 유닛들의 중심 모으기
	static Vec2 Cohesion(CObj* me, const list<CObj*>& neighbors,
		float radius, const Vec2& meVel, float maxSpeed);
	//충돌 회피
	//CollisionAvoidance : 장애물 회피
	static Vec2 CollisionAvoidance(const Vec2& position, const Vec2& velocity,
		const list <CObj*>& obstacles, float lookAhead);
	// WallAvoidance: 벽 회피 (NavMgr의 walkable 정보 활용)
	static Vec2 WallAvoidance(const Vec2& position, const Vec2& velocity,
		float lookAhead);
	// Flocking: Separation + Alignment + Cohesion 조합
	static SteeringOutput Flocking(CObj* me, const list<CObj*>& neighbors,
		const SteeringParams& params, const Vec2& meVel);

	// PathFollowing: A* 경로 따라가기 + Flocking
	SteeringOutput FollowPathWithFlocking(
		CObj* me,
		const vector<Vec2>& path,
		int& currentWaypointIndex,
		const list<CObj*>& neighbors,
		const SteeringParams& params,
		const Vec2& _meVel);

	// ========== 유틸리티 함수 ==========

	// 벡터 정규화
	static Vec2 Normalize(const Vec2& v);

	// 벡터 길이
	static float Magnitude(const Vec2& v);

	// 벡터 길이 제한
	static Vec2 Limit(const Vec2& v, float max);

	// 두 점 사이 거리
	static float Distance(const Vec2& a, const Vec2& b);

	// 주변 이웃 찾기 (특정 반경 내)
	static list<CObj*> FindNeighbors(CObj* me, const list<CObj*>& units,
		float radius);
	//Dot 유틸리티
	static float Dot(const Vec2& a, const Vec2& b);

public:
	static CSteeringMgr* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CSteeringMgr;
		}
		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	static CSteeringMgr* m_pInstance;
};