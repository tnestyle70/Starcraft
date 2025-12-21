#pragma once
#include "CObj.h"
#include "Commandable.h"
#include <deque>
#include <vector>

enum class eUnitState {IDLE, MOVE, ATTACK, DIE};

enum class eOrderType
{STOP, MOVE, ATTACK_MOVE, ATTACK_TARGET, HOLD};

struct Order
{
	eOrderType eType;
	Vec2 dst;
	//월드 좌표 way point
	vector<Vec2> path;
	int iPathIndex;
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
	void CommandCardSlot(vector<CommandSlot>& outSlot) override;
	bool ExecuteCommand(eCommandID command, CommandContext& context) override;
public:
	void IssueMove();
	void IssueStop();
	void IssueHold();
	void IssuePatrol();
	void IssueAttackMove();
	void ClearOrders() { m_OrderQ.clear(); };
	void PushOrder(const Order& order) {m_OrderQ.push_back(order);};
public:
	bool UpdateMove(Order& order);
	void PopOrder();
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