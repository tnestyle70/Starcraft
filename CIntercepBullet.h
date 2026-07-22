#pragma once
#include "CProjectile.h"

class CIntercepBullet : public CProjectile
{
public:
    CIntercepBullet();
    virtual ~CIntercepBullet();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
private:
    void CreateEffect();
};


