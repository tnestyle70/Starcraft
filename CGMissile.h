#pragma once
#include "CProjectile.h"

class CGMissile : public CProjectile
{
public:
    CGMissile();
    virtual ~CGMissile();

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


