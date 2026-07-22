#pragma once
#include "CProjectile.h"

class CVultureMine : public CProjectile
{
public:
    CVultureMine();
    virtual ~CVultureMine();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
private:
    void UpdateMine();
    void MoveToTarget(CObj* pTarget);
    void CreateEffect(Vec2 targetPos);
    void AreaDamage();
private:
    float m_fAttackRange = 0.f;
    float m_fDamageRange = 0.f;
    bool m_bMineInstalled = false;
    bool m_bFindTarget = false;
};

