#pragma once
#include "CProjectile.h"

class CGBullet : public CProjectile
{
public:
    CGBullet();
    virtual ~CGBullet();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
};


