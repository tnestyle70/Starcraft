#pragma once
#include "CObj.h"
#include "CSteeringMgr.h"
#include "Commandable.h"
#include <deque>
#include <vector>

enum class eResourceType
{
	NONE, MINERAL, GAS
};

class CBuilding;

enum class eUnitState 
{
	IDLE, MOVE, CONSTRUCTING, GARRISONED, BIRTH
 ,GATHER, RETURN_RESOURCE, ATTACK, DIE
};

class CUnit : public Commandable, public CObj
{
public:
	CUnit();
	virtual ~CUnit();
public:
	virtual void Initialize() PURE;
	int	 Update() override;
	virtual void Late_Update()	PURE;
	void Render(HDC hDC) override;
	virtual void Release()		PURE;
public: //Commandable의 커맨드 카드 슬롯 구현
	virtual void UpdateHotKeys();
	virtual bool StartBuild(Order& order);
	virtual bool UpdateConstructing(Order& order);
	virtual bool UpdateGather(Order& order);
	virtual bool UpdateReturn(Order& order);
	CObj* FindNearestCommandCenter();
	void CommandCardSlot(vector<CommandSlot>& outSlot);
	bool ExecuteCommand(eCommandID command, CommandContext& context);
	void PushOrder(const Order& order) {m_OrderQ.push_back(order);};
	void CompleteOrder();
	void ClearOrder();
public:
	bool EnterBunker(Order& order);
	bool EnterShuttle(Order& order);
	virtual bool UpdateMove(Order& order);
	void StartOrder(Order& order);
	void RemoveOrdersWithTarget(CObj* pTarget);
	void Heal(int iHeal) override;
protected:
	virtual bool UpdateAttack(Order& order);
	virtual bool UpdateAttackMove(Order& order);
	virtual bool UpdateRAttack(Order& order);
	virtual CObj* FindNearestEnemy(float searchRadius);
	virtual void UpdateDead();
public:
	void SetAttackMove(bool attackmove) { m_bAttackMove = attackmove; }
protected: //공격 관련 멤버 변수
	bool m_bAttackMove = false;
	int m_iAttackDamage;
	float m_fAttackRange;
	float m_fAttackSpeed;
	float m_fAttackCoolTime = 0.f;
	DWORD m_dwAttackCoolTime;
	DWORD m_dwLastAttack;
public:
	void SetConceal(bool conceal) { m_bConceal = conceal; }
protected:
	bool m_bConceal = false;
public:
	int DirTo16WayIndex(Vec2& vDir);
	int DirTo17WayIndex(Vec2& vDir);
	int DirTo8WayIndex(Vec2& vDir);
	RECT GetWorldRect() const;
public:
	void UpdateUnitUIInfo();
protected:
	const TCHAR* GetUnitName();
	int GetIconIndex(eCommandID command);
public:
	int Get_HP() { return m_iHP; }
	int Get_MaxHP() { return m_iMaxHP; }
	int Get_MP() { return m_iMP; }
	int Get_MaxMP() { return m_iMaxMP; }
	int Get_Shield() { return m_iShield; }
	int Get_MaxShield() { return m_iMaxShield; }
	eUnitType Get_UnitType() 
	{
		return m_eType;
	}
	Vec2 GetVelocity() { return m_velocity; }
	void TakeDamage(int iAttackDamage) override;
	void RefillShield(int iRefill);
	int GetSightRange() { return m_iSightRange; }
	void SetSightRange(int range) { m_iSightRange = range; }
public:
	//벙커
	virtual void UpdateGarrisoned();
	void SetUnitState(eUnitState state) { m_eState = state; }
	void SetVisible(bool visible) { m_bVisible = visible; }
	void SetGarrisoned(bool garrison) { m_bGarrisoned = garrison; }
	bool m_bGarrisoned = false;
	bool m_bVisible = true;
	//벙커 공격 속도
	float m_fBunkerAttackInterval = 1.f;
public:
	//steering 관련 함수
	void UpdateSteering(const list<CObj*>& allUnits, float fDeltaTime);
	void SetSteeringParams(const SteeringParams& params) { m_steeringParams = params; }
	SteeringParams& GetSteeringParams() { return m_steeringParams; }
protected:
	eCommandCardState m_eCommandCardState;
	//명령큐
	std::deque<Order> m_OrderQ;
	CObj* m_pTarget = nullptr;
	bool m_bActiveOrder;
	//애니메이션 상태
	eUnitState m_eState;
	eUnitType m_eType;
	FRAME m_tFrame;
	int m_iAttackFrameStart;
	int m_iAttackFrameEnd;
	float m_fAttackTimer = 0.f;
	float m_fAttackInterval = 0.5f; 
	bool m_bAttack = false;
	//유닛 상태
	float m_fSpeed;
	int m_iHP;
	int m_iMaxHP;
	int m_iMP;
	int m_iMaxMP;
	int m_iShield;
	int m_iMaxShield;
	//라바 Birth
	bool m_bBirthing = false;
	//Body, Head 분리 유닛용
	int m_iFireFrame;
	bool m_bFiring;
	//이동 + 위치
	Vec2 m_vDir;
	std::vector<Vec2> m_vecPath;
	int m_iPathIndex;
	//steering 멤버 변수 
	Vec2 m_acceleration;
	SteeringParams m_steeringParams;
	int m_currentWayPointIndex;
	//시야 
	int m_iSightRange;
};