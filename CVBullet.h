#pragma once
#include "CProjectile.h"

class CVBullet : public CProjectile
{
public:
    CVBullet();
    virtual ~CVBullet();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
};

