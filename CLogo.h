#pragma once
#include "CScene.h"
#include <thread>

enum class eLogoState
{
    VIDEO, LOGO
};

class CLogo :
    public CScene
{
public:
    CLogo();
    virtual ~CLogo();

public:
    void Initialize() override;
    int Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
private:
    HWND        m_hVideo = nullptr;
    eLogoState m_eState = eLogoState::VIDEO;
    float m_fLogoDuration = 3.f;
    bool m_bSceneChanging = false;
private:
    std::thread m_LoadThread;
    std::atomic<bool> m_bLoadComplete;
};
