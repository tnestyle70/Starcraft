#pragma once
#include "CObj.h"
#include <deque>
#include <vector>

enum class eUnitState {IDLE, MOVE, ATTACK, DIE};

enum class eOrderType
{STOP, MOVE, ATTACK_MOVE, ATTACK_TARGET, HOLD};

struct Order
{
	eOrderType eType;
	Vec2 vTargetPos;
	CObj* pTarget;
};

class CUnit : public CObj
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
public:
	void IssueMove(const Vec2& worldTarget);
	void ClearOrders() { m_OrderQ.clear(); };
	void PushOrder(const Order& order) {m_OrderQ.push_back(order);};
public:
	bool UpdateMove();
public:
	int DirTo16WayIndex(Vec2& vDir);
public:
	RECT GetWorldRect() const;
	void SetSelected(bool bSelected) { m_bSelected = bSelected; }
	bool IsDead() const override { return m_bDead; }
protected:
	//명령큐
	std::deque<Order> m_OrderQ;
	bool m_bActiveOrder;
	//애니메이션 상태
	eUnitState m_eState;
	FRAME m_tFrame;
	//유닛 상태
	bool m_bSelected;
	float m_fSpeed;
	bool m_bDead;
	int m_iHP;
	int m_iMaxHP;
	//이동 + 위치
	Vec2 m_vDir;
	std::vector<Vec2> m_vecPath;
	int m_iPathIndex;
};