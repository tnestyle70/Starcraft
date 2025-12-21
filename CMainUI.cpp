#include "pch.h"
#include "CMainUI.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"

CMainUI::CMainUI()
    : m_dcPanel(nullptr)
    , m_bmpPanel(nullptr)
    , m_oldPanel(nullptr)
    , m_hFont(nullptr)
{
}

CMainUI::~CMainUI()
{
}

// 알파 비트맵 생성 함수
HBITMAP CMainUI::CreateAlphaBitmap(HDC hdc, HDC hSrcDC, int width, int height, COLORREF transparentColor)
{
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Top-down (음수로 설정)
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32비트 (RGBA)
    bmi.bmiHeader.biCompression = BI_RGB;

    BYTE* pBits = nullptr;
    HBITMAP hAlphaBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);

    if (!hAlphaBitmap || !pBits)
        return nullptr;

    // 원본 이미지의 픽셀 데이터를 읽어와서 알파 채널 적용
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            COLORREF pixel = GetPixel(hSrcDC, x, y);
            int index = (y * width + x) * 4;

            if (pixel == transparentColor) {
                // 투명 처리 (마젠타 색상)
                pBits[index] = 0;     // Blue
                pBits[index + 1] = 0; // Green
                pBits[index + 2] = 0; // Red
                pBits[index + 3] = 0; // Alpha = 0 (완전 투명)
            }
            else {
                // 불투명 처리
                pBits[index] = GetBValue(pixel);     // Blue
                pBits[index + 1] = GetGValue(pixel); // Green
                pBits[index + 2] = GetRValue(pixel); // Red
                pBits[index + 3] = 255;              // Alpha = 255 (완전 불투명)
            }
        }
    }

    return hAlphaBitmap;
}

void CMainUI::Initialize()
{
    // 1. 영역 설정
    m_srcPanel = { 0, 367, 800, 600 };
    int panelW = m_srcPanel.right - m_srcPanel.left;
    int panelH = m_srcPanel.bottom - m_srcPanel.top;

    m_dstPanel.left = 0;
    m_dstPanel.top = WINCY - panelH;
    m_dstPanel.right = panelW;
    m_dstPanel.bottom = WINCY;

    HDC hUIDC = CBmpMgr::Get_Instance()->Find_Image(L"MainUI");
    if (!hUIDC) return;

    // 2. 임시 DC 생성 - 원본 이미지에서 UI 영역만 추출
    HDC hTempDC = CreateCompatibleDC(hUIDC);
    HBITMAP hTempBmp = CreateCompatibleBitmap(hUIDC, panelW, panelH);
    HBITMAP hTempOld = (HBITMAP)SelectObject(hTempDC, hTempBmp);

    // 원본에서 UI 영역만 복사
    BitBlt(hTempDC, 0, 0, panelW, panelH, hUIDC, m_srcPanel.left, m_srcPanel.top, SRCCOPY);

    // 3. 알파 비트맵 생성 (마젠타 색상을 투명 처리)
    m_bmpPanel = CreateAlphaBitmap(hUIDC, hTempDC, panelW, panelH, RGB(255, 0, 255));

    // 임시 DC 해제
    SelectObject(hTempDC, hTempOld);
    DeleteObject(hTempBmp);
    DeleteDC(hTempDC);

    // 4. 알파 비트맵을 위한 DC 생성
    m_dcPanel = CreateCompatibleDC(hUIDC);
    m_oldPanel = (HBITMAP)SelectObject(m_dcPanel, m_bmpPanel);

    // 폰트 생성 (한 번만)
    m_hFont = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
}

void CMainUI::Render(HDC hDC)
{
    RenderFrame(hDC);
}

void CMainUI::RenderFrame(HDC hDC)
{
    if (!m_dcPanel) return;

    int w = m_srcPanel.right - m_srcPanel.left;
    int h = m_srcPanel.bottom - m_srcPanel.top;

    // AlphaBlend 사용 (TransparentBlt보다 훨씬 빠름!)
    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255; // 완전 불투명
    bf.AlphaFormat = AC_SRC_ALPHA; // 소스 알파 채널 사용

    AlphaBlend(hDC,
        m_dstPanel.left, m_dstPanel.top, w, h,
        m_dcPanel,
        0, 0, w, h,
        bf);
}

void CMainUI::Release()
{
    if (m_hFont)
    {
        DeleteObject(m_hFont);
        m_hFont = nullptr;
    }

    if (m_dcPanel)
    {
        SelectObject(m_dcPanel, m_oldPanel);
        DeleteObject(m_bmpPanel); // 알파 비트맵 해제
        DeleteDC(m_dcPanel);
        m_dcPanel = nullptr;
        m_bmpPanel = nullptr;
        m_oldPanel = nullptr;
    }
}