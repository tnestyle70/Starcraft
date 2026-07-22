#pragma once
#include "CUnit.h"

class CZealot : public CUnit
{
public:
    CZealot();
    virtual ~CZealot();

public:
    void Initialize() override;
    int  Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
protected:
    void UpdateHotKeys() override;
    bool ExecuteCommand(eCommandID command, CommandContext& context);
    void CommandCardSlot(vector<CommandSlot>& outSlot);
protected:
    void UpdateDead() override;
private:
    void FireBullet();
    void Rush();
private:
    float m_fOriginalSpeed;
private: //AI
    void UpdateAI();
    CObj* FindNearestEnemyAI(float searchRadius);
};
