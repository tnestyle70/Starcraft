#pragma once
#include "CProjectile.h"

class CHydraliskBullet : public CProjectile
{
public:
    CHydraliskBullet();
    virtual ~CHydraliskBullet();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
private:
    void CreateEffect();
};

