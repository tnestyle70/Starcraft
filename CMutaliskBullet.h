#pragma once
#include "CProjectile.h"

class CMutaliskBullet : public CProjectile
{
public:
    CMutaliskBullet();
    virtual ~CMutaliskBullet();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
private:
    void CreateEffect();
};


