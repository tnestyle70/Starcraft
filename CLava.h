#pragma once
#include "CUnit.h"

class CLava : public CUnit
{
public:
    CLava();
    virtual ~CLava();

public:
    void Initialize() override;
    int  Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
public:
    void UpdateHotKeys() override;
    bool ExecuteCommand(eCommandID command, CommandContext& context);
    void CommandCardSlot(vector<CommandSlot>& outSlot);
protected:
    void UpdateDead() override;
private:
    void StartBirth(eUnitType type);
    void CompleteBirth();
    eUnitType m_eBirthUnit;
    int m_iBirthStart = 0;
    int m_iBirthEnd = 7;
};

