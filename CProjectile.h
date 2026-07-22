#pragma once
#include "CObj.h"

class CProjectile : public CObj
{
public:
	CProjectile();
	virtual ~CProjectile();
public:
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
public:
	void ResolveCollision();
	void ClearTarget();
public:
	void Set_Target_Pos(Vec2 pos) { targetPos = pos; }
	void Set_Target(CObj* pTarget) { m_pTarget = pTarget; }
	void Set_Owner(CObj* pOwner) { m_pOwner = pOwner; }
	void Set_Dir(Vec2 vDir) { m_vDir = vDir; }
	void Set_Damage(int iDamage) { m_iDamage = iDamage; }
	CObj* Get_Target() { return m_pTarget; }
	int Get_Damage() { return m_iDamage; }
	void Set_Direction16(int iDir) { m_iDirection16 = iDir; }
	void Set_Homing(bool homing) { m_bHoming = homing; }
protected:
	int DirTo16WayIndex(Vec2& vDir);
	int DirTo17WayIndex(Vec2& vDir);
	int m_iDirection16;
protected:
	Vec2 m_vDir; //발사 방향
	float m_fSpeed; //투사체 속도
	int m_iDamage; //데미지
	CObj* m_pTarget; //타겟(추적용)
	CObj* m_pOwner; //발사한 유닛
	bool m_bHoming = true; //유도 미사일 여부
	Vec2 targetPos; //타겟 위치
};