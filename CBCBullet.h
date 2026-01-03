#pragma once
#include "CProjectile.h"

class CBCBullet : public CProjectile
{
public:
    CBCBullet();
    virtual ~CBCBullet();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
};
