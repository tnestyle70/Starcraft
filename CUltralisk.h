#pragma once
#include "CUnit.h"

class CUltralisk : public CUnit
{
public:
    CUltralisk();
    virtual ~CUltralisk();

public:
    void Initialize() override;
    int  Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
protected:
    void UpdateDead() override;
    CObj* FindNearestEnemyAI(float searchRadius);
private:
    void UpdateAI();
    void FireBullet();
private:
    // 공격 애니메이션 프레임
    int     iAttackFrameStart;
    int     iAttackFrameEnd;
};
