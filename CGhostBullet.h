#pragma once
#include "CProjectile.h"

class CGhostBullet : public CProjectile
{
public:
    CGhostBullet();
    virtual ~CGhostBullet();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
private:
    void CreateNuclearEffect();
    void AreaDamage();
};

