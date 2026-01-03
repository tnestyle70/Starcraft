#include "pch.h"
#include "CMyPng.h"

CMyPng::CMyPng()
    : m_pImage(nullptr), m_iWidth(0), m_iHeight(0)
{
    m_eType = eImageType::PNG;
}

CMyPng::~CMyPng()
{
    Release();
}

void CMyPng::Load_Image(const TCHAR* pFilePath)
{
    Release();

    // GDI+로 PNG 이미지 로드
    m_pImage = Image::FromFile(pFilePath);

    if (!m_pImage || m_pImage->GetLastStatus() != Ok)
    {
        if (m_pImage)
        {
            delete m_pImage;
            m_pImage = nullptr;
        }
        return;
    }

    // 이미지 크기 저장
    m_iWidth = m_pImage->GetWidth();
    m_iHeight = m_pImage->GetHeight();

    // MemDC 생성 (기존 BMP와의 호환성을 위해)
    HDC hDC = GetDC(g_hWnd);
    m_hMemDC = CreateCompatibleDC(hDC);

    // 32비트 RGBA 비트맵 생성
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_iWidth;
    bmi.bmiHeader.biHeight = -m_iHeight; // Top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    m_hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

    ReleaseDC(g_hWnd, hDC);

    if (!m_hBitmap)
    {
        delete m_pImage;
        m_pImage = nullptr;
        DeleteDC(m_hMemDC);
        m_hMemDC = nullptr;
        return;
    }

    m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);

    // GDI+를 사용해서 비트맵에 PNG 그리기
    Graphics graphics(m_hMemDC);
    graphics.DrawImage(m_pImage, 0, 0, m_iWidth, m_iHeight);

    return;
}

void CMyPng::Render_Alpha(HDC hDC, int iX, int iY)
{
    if (!m_pImage)
        return;

    Graphics graphics(hDC);
    // 검은색을 투명 처리하기 위한 ImageAttributes 설정
    ImageAttributes imageAttr;
    imageAttr.SetColorKey(
        Color(0, 0, 0),      // 투명 처리할 색상 시작 (검은색)
        Color(10, 10, 10),   // 투명 처리할 색상 끝 (거의 검은색까지)
        ColorAdjustTypeBitmap
    );
    // ColorKey를 적용해서 렌더링
    graphics.DrawImage(m_pImage,
        Rect(iX, iY, m_iWidth, m_iHeight),  // 목적지
        0, 0, m_iWidth, m_iHeight,          // 원본 영역
        UnitPixel,
        &imageAttr);
}

void CMyPng::Render_Alpha(HDC hDC, int iX, int iY, int iWidth, int iHeight)
{
    if (!m_pImage)
        return;

    Graphics graphics(hDC);
    // 검은색을 투명 처리
    ImageAttributes imageAttr;
    imageAttr.SetColorKey(
        Color(0, 0, 0),
        Color(10, 10, 10),
        ColorAdjustTypeBitmap
    );

    graphics.DrawImage(m_pImage,
        Rect(iX, iY, iWidth, iHeight),
        0, 0, m_iWidth, m_iHeight,
        UnitPixel,
        &imageAttr);
}

void CMyPng::Render_Alpha_Pure(HDC hDC, int iX, int iY)
{
    if (!m_pImage)
        return;

    Graphics graphics(hDC);
    graphics.DrawImage(m_pImage, iX, iY);
}

void CMyPng::Render_Alpha_Pure(HDC hDC, int iX, int iY, int iWidth, int iHeight)
{
    if (!m_pImage)
        return;

    Graphics graphics(hDC);
    graphics.DrawImage(m_pImage, iX, iY, iWidth, iHeight);
}

void CMyPng::Release()
{
    if (m_pImage)
    {
        delete m_pImage;
        m_pImage = nullptr;
    }

    if (m_hMemDC)
    {
        SelectObject(m_hMemDC, m_hOldBitmap);
        DeleteObject(m_hBitmap);
        DeleteDC(m_hMemDC);

        m_hMemDC = nullptr;
        m_hBitmap = nullptr;
        m_hOldBitmap = nullptr;
    }

    m_iWidth = 0;
    m_iHeight = 0;
}