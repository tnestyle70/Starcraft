#pragma once
#include "CProjectile.h"

class CDragoonBall : public CProjectile
{
public:
    CDragoonBall();
    virtual ~CDragoonBall();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
private:
    void CreateEffect();
};


