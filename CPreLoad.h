#pragma once
#include "CScene.h"

class CPreLoad : public CScene
{
public:
    CPreLoad();
    virtual ~CPreLoad();
public:
    void Initialize() override;
    int Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
private:
    HWND        m_hVideo = nullptr;
    float m_fLogoDuration = 3.f;
};

