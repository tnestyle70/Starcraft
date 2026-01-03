#pragma once
#include "CObj.h"
#include "CSteeringMgr.h"
#include "Commandable.h"
#include <deque>
#include <vector>

class CBuilding;

enum class eUnitState 
{IDLE, MOVE, CONSTRUCTING 
 ,GATHER, RETURN_RESOURCE, ATTACK, DIE};

enum class eOrderType
{ MOVE, STOP, GATHER, RETURN_RESOURCE, ATTACK, ATTACK_MOVE,
MOVE_AND_BUILD, CONSTRUCTING, HOLD };

enum class eCommandCardState
{
	MAIN, NORMAL_BUILD, ADVANCED_BUILD, NORMAL_TANK, SIEGE_TANK
};

struct Order 
{
	eOrderType eType;
	Vec2 dst;
	//월드 좌표 way point
	vector<Vec2> path;
	int iPathIndex;
	CObj* pTarget = nullptr;
	CBuilding* pBuilding = nullptr;
	Order() : eType(eOrderType::MOVE), iPathIndex(0) {}
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
	virtual bool UpdateGather(Order& order);
	virtual bool UpdateReturn(Order& order);
	CObj* FindNearestCommandCenter();
	void CommandCardSlot(vector<CommandSlot>& outSlot);
	bool ExecuteCommand(eCommandID command, CommandContext& context);
	void PushOrder(const Order& order) {m_OrderQ.push_back(order);};
	void CompleteOrder();
	void ClearOrder();
public:
	bool UpdateMove(Order& order);
	void StartOrder(Order& order);
protected:
	virtual bool UpdateAttack(Order& order);
	CObj* FindNearestEnemy(float searchRadius);
protected: //공격 관련 멤버 변수
	int m_iAttackDamage;
	float m_fAttackRange;
	float m_fAttackSpeed;
	DWORD m_dwAttackCoolTime;
	DWORD m_dwLastAttack;
public:
	int DirTo16WayIndex(Vec2& vDir);
	RECT GetWorldRect() const;
protected:
	void UpdateUnitUIInfo();
	const TCHAR* GetUnitName();
	int GetIconIndex(eCommandID command);
public:
	int Get_HP() { return m_iHP; }
	int Get_MaxHP() { return m_iMaxHP; }
	eUnitType Get_UnitType() { return m_eType; }
	Vec2 GetVelocity() { return m_velocity; }
	void TakeDamage(int iAttackDamage) override;
	int GetSightRange() { return m_iSightRange; }
	void SetSightRange(int range) { m_iSightRange = range; }
public:
	//steering 관련 함수
	void UpdateSteering(const list<CObj*>& allUnits, float fDeltaTime);
	void SetSteeringParams(const SteeringParams& params) { m_steeringParams = params; }
	SteeringParams& GetSteeringParams() { return m_steeringParams; }
protected:
	eCommandCardState m_eCommandCardState;
	//명령큐
	std::deque<Order> m_OrderQ;
	bool m_bActiveOrder;
	//애니메이션 상태
	eUnitState m_eState;
	eUnitType m_eType;
	FRAME m_tFrame;
	int iAttackFrameStart;
	int iAttackFrameEnd;
	//유닛 상태
	float m_fSpeed;
	int m_iHP;
	int m_iMaxHP;
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