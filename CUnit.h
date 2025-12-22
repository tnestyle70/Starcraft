#pragma once
#include "CObj.h"
#include "Commandable.h"
#include <deque>
#include <vector>

class CBuilding;

enum class eUnitState {IDLE, MOVE, ATTACK, DIE};

enum class eOrderType
{MOVE, STOP, MOVE_AND_BUILD, CONSTRUCTING ,ATTACK_MOVE, ATTACK_TARGET, HOLD};

struct Order
{
	eOrderType eType;
	Vec2 dst;
	//월드 좌표 way point
	vector<Vec2> path;
	int iPathIndex;
	CBuilding* pBuilding = nullptr;
	Order() : eType(eOrderType::MOVE), iPathIndex(0) {}
};

class CUnit : public Commandable, public CObj
{
public:
	CUnit();
	virtual ~CUnit();
public:
	virtual void Initialize()	PURE;
	int	 Update() override;
	virtual void Late_Update()	PURE;
	virtual void Render(HDC hDC)PURE;
	virtual void Release()		PURE;
public: //Commandable의 커맨드 카드 슬롯 구현
	virtual void UpdateHotKeys();
	virtual bool StartBuild(Order& order);
	void CommandCardSlot(vector<CommandSlot>& outSlot);
	bool ExecuteCommand(eCommandID command, CommandContext& context);
	void PushOrder(const Order& order) {m_OrderQ.push_back(order);};
	void CompleteOrder();
	void ClearOrder();
public:
	bool UpdateMove(Order& order);
	void StartOrder(Order& order);
public:
	int DirTo16WayIndex(Vec2& vDir);
	RECT GetWorldRect() const;
protected:
	//명령큐
	std::deque<Order> m_OrderQ;
	bool m_bActiveOrder;
	//애니메이션 상태
	eUnitState m_eState;
	FRAME m_tFrame;
	//유닛 상태
	float m_fSpeed;
	bool m_bDead;
	int m_iHP;
	int m_iMaxHP;
	//이동 + 위치
	Vec2 m_vDir;
	std::vector<Vec2> m_vecPath;
	int m_iPathIndex;
};