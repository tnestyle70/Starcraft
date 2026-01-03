#pragma once
#include "CProjectile.h"

class CBCYamato : public CProjectile
{
private:
    bool m_bCharged;           // 차징 완료 여부
    DWORD m_dwChargeStart;     // 차징 시작 시간
    const DWORD m_dwChargeTime = 2000; // 2초 차징

public:
    CBCYamato();
    virtual ~CBCYamato();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
};
