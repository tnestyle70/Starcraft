#pragma once
#include "Define.h"

class CMainUI
{
public:
    CMainUI();
    ~CMainUI();

public:
    void Initialize();
    void Render(HDC hDC);
    void Release();

public:
    void RenderFrame(HDC hDC);
    HBITMAP CreateAlphaBitmap(HDC hdc, HDC hSrcDC, int width, int height, COLORREF transparentColor);

private:
    RECT m_srcPanel;    // 원본 이미지에서의 영역
    RECT m_dstPanel;    // 화면에 그릴 영역

    HDC m_dcPanel;      // 알파 비트맵용 DC
    HBITMAP m_bmpPanel; // 알파 채널이 적용된 비트맵
    HBITMAP m_oldPanel; // 이전 비트맵 저장용

    HFONT m_hFont;      // 폰트
};