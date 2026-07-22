#pragma once
#include "CProjectile.h"

class CDarkArchonBullet : public CProjectile
{
public:
    CDarkArchonBullet();
    virtual ~CDarkArchonBullet();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
private:
    void CreateEffect();
    bool m_bCreateEffect = false;
};

