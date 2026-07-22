#pragma once
#include "CProjectile.h"

class CTurretBullet : public CProjectile
{
public:
    CTurretBullet();
    virtual ~CTurretBullet();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
private:
    void CreateEffect();
    float m_fEffectTimer = 0.f;
    float m_fEffectInterval = 0.1f;
};


