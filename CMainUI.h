#pragma once
#include "Define.h"

class CMainUI
{
public:
    CMainUI();
    ~CMainUI();

public:
    void Initialize();
    //int Update();
    void Render(HDC hDC);
    void Release();

public:
    // 외부(게임 로직)에서 UI에 데이터 주입할 수 있게(SelectionMgr/PlayerState 연결용)
    //void SetResources(int mineral, int gas, int supplyUsed, int supplyMax);

private:
    void RenderFrame(HDC hDC);
    //void RenderResources(HDC hDC);
    //void RenderMinimap(HDC hDC);
    //void RenderUnitInfo(HDC hDC);
    //void RenderCommandCard(HDC hDC);

private:
    // 패널(정적 프레임) 렌더 영역: MainUI.bmp에서 하단만 잘라쓴다.
    RECT m_srcPanel{};
    RECT m_dstPanel{};

    // 리소스 값(나중에 PlayerMgr/ResourceMgr랑 연결)
    int m_mineral = 50;
    int m_gas = 0;
    int m_supplyUsed = 4;
    int m_supplyMax = 9;

    HFONT m_hFont = nullptr;
private:
    HDC     m_dcPanel = nullptr; // (준비된 패널: 투명영역=검정)
    HDC     m_dcMask = nullptr; // (마스크: 투명=흰색(1), 불투명=검정(0))
    HDC     m_dcInvMask = nullptr; // (반전 마스크: 투명=0, 불투명=1)

    HBITMAP m_bmpPanel = nullptr;
    HBITMAP m_bmpMask = nullptr;
    HBITMAP m_bmpInvMask = nullptr;

    HGDIOBJ m_oldPanel = nullptr;
    HGDIOBJ m_oldMask = nullptr;
    HGDIOBJ m_oldInvMask = nullptr;
};
