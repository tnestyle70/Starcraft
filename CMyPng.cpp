#include "pch.h"
#include "CMyPng.h"
#include <string>
using namespace Gdiplus;

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
    Release(); // 기존 자원 해제

    //종족 감지
    bool isZerg = (wcsstr(pFilePath, L"Zerg") != nullptr ||
        wcsstr(pFilePath, L"zerg") != nullptr);
    bool isProtoss = (wcsstr(pFilePath, L"Protoss") != nullptr ||
        wcsstr(pFilePath, L"protoss") != nullptr);

    // 1. GDI+ Bitmap 로드
    m_pImage = Gdiplus::Bitmap::FromFile(pFilePath);
    if (!m_pImage || ((Gdiplus::Bitmap*)m_pImage)->GetLastStatus() != Gdiplus::Ok) {
        if (m_pImage) { delete m_pImage; m_pImage = nullptr; }
        return;
    }

    m_iWidth = m_pImage->GetWidth();
    m_iHeight = m_pImage->GetHeight();

    // 2. DIBSection 생성
    HDC hDC = GetDC(g_hWnd);
    m_hMemDC = CreateCompatibleDC(hDC);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_iWidth;
    bmi.bmiHeader.biHeight = -m_iHeight; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pDestBits = nullptr;
    m_hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pDestBits, NULL, 0);
    ReleaseDC(g_hWnd, hDC);

    if (!m_hBitmap) return;

    BITMAP bm;
    GetObject(m_hBitmap, sizeof(bm), &bm);
    m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);

    // 3. 픽셀 데이터 잠금
    Gdiplus::Rect rect(0, 0, m_iWidth, m_iHeight);
    Gdiplus::BitmapData bmpData;
    Gdiplus::Status status = ((Gdiplus::Bitmap*)m_pImage)->LockBits(
        &rect,
        Gdiplus::ImageLockModeRead,
        PixelFormat32bppARGB, // Straight Alpha
        &bmpData);

    if (status != Gdiplus::Ok) return;

    // ========== 원본 StarCraft 유닛 색상 (실제 측정값) ==========
    const BYTE ORIGINAL_MAGENTA_R = 160;   // ← 실제 StarCraft 색상!
    const BYTE ORIGINAL_MAGENTA_B = 160;
    const BYTE ORIGINAL_MAGENTA_G = 0;

    // ========== 통계 변수 ==========
    int brightMagentaFixed = 0;  // 밝은 마젠타를 어둡게 보정
    int alreadyDarkMagenta = 0;   // 이미 어두운 마젠타 (정상)

    wchar_t debugMsg[512];

    // ========== 픽셀 변환 + 마젠타 보정 ==========
    for (int y = 0; y < m_iHeight; ++y)
    {
        DWORD* pSrcRow = (DWORD*)((BYTE*)bmpData.Scan0 + y * bmpData.Stride);
        DWORD* pDestRow = (DWORD*)((BYTE*)pDestBits + y * bm.bmWidthBytes); // 실제 Stride!

        for (int x = 0; x < m_iWidth; ++x)
        {
            DWORD pixel = pSrcRow[x];
            BYTE a = (pixel >> 24) & 0xFF;
            BYTE r = (pixel >> 16) & 0xFF;
            BYTE g = (pixel >> 8) & 0xFF;
            BYTE b = pixel & 0xFF;

            // 알파가 일정 이상이면 완전 불투명으로
            if (a > 128)  // 50% 이상 불투명이면
            {
                a = 255;  // 완전 불투명으로 만들기
            }
            else if (a > 0)  // 완전 투명은 아닌데 50% 미만이면
            {
                a = 0;  // 완전 투명으로
            }

            // Premultiply 적용 (알파=255면 자동으로 스킵됨)
            if (a < 255)
            {
                r = (r * a) / 255;
                g = (g * a) / 255;
                b = (b * a) / 255;
            }

            // 조건: R>100, G<50, B>100 (마젠타 계열)
            //       A==255 (불투명만)
            if (r > 100 && g < 50 && b > 100 && a == 255)
            {
                if (r > 150)  // 밝은 마젠타 (R>150) → 문제!
                {
                    // 원본 StarCraft 색상으로 교체
                    r = ORIGINAL_MAGENTA_R;
                    b = ORIGINAL_MAGENTA_B;
                    g = ORIGINAL_MAGENTA_G;

                    brightMagentaFixed++;
                }
                else  // 이미 어두운 마젠타 (정상)
                {
                    alreadyDarkMagenta++;
                }
            }

            // 1. 청록색 보정 (필요시)
            // RGB(43, 236, 255) 근처 색상을 통일
            if (r < 50 && g > 230 && b > 230)
            {
                // 표준 청록색으로 통일
                r = 43;
                g = 236;
                b = 255;
            }

            pDestRow[x] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }

    ((Gdiplus::Bitmap*)m_pImage)->UnlockBits(&bmpData);
}

void CMyPng::Render_Alpha(HDC hDC, int iX, int iY, bool bUseColorKey)
{
    
    if (!m_pImage)
        return;

    Graphics graphics(hDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    graphics.SetPixelOffsetMode(PixelOffsetModeHalf);

    if (bUseColorKey)
    {
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
    else
    {
        // PNG 알파 채널 그대로 사용
        graphics.DrawImage(m_pImage, iX, iY, m_iWidth, m_iHeight);
    }
}

void CMyPng::Render_Alpha(HDC hDC, int iX, int iY, int iWidth, int iHeight, bool bUseColorKey, bool conceal)
{
    if (!m_hMemDC)
        return;

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    if (conceal)
    {
        bf.SourceConstantAlpha = 100;
    }
    else
    {
        bf.SourceConstantAlpha = 255;
    }

    bf.AlphaFormat = AC_SRC_ALPHA;

    if (m_iWidth == iWidth && m_iHeight == iHeight)
    {
        // 원본 크기 그대로
        AlphaBlend(hDC, iX, iY, iWidth, iHeight,
            m_hMemDC, 0, 0, m_iWidth, m_iHeight, bf);
    }
    else
    {
        // 스케일링 필요
        int oldMode = SetStretchBltMode(hDC, COLORONCOLOR);
        AlphaBlend(hDC, iX, iY, iWidth, iHeight,
            m_hMemDC, 0, 0, m_iWidth, m_iHeight, bf);
        SetStretchBltMode(hDC, oldMode);
    }
}

void CMyPng::Render_Alpha_Flipped(HDC hDC, int iX, int iY, int iWidth, int iHeight, 
    bool bUseColorKey, bool conceal)
{
    if (!m_hMemDC)
        return;

    // 그래픽 모드 설정 (좌우반전을 위해 필요)
    int oldMode = SetGraphicsMode(hDC, GM_ADVANCED);

    // 현재 변환 저장
    XFORM oldXform;
    GetWorldTransform(hDC, &oldXform);

    // 좌우반전 변환 설정
    XFORM xForm;
    xForm.eM11 = -1.0f;  // X축 반전
    xForm.eM12 = 0.0f;
    xForm.eM21 = 0.0f;
    xForm.eM22 = 1.0f;   // Y축 유지
    xForm.eDx = (float)(iX + iWidth);  // 반전 기준점
    xForm.eDy = (float)iY;

    SetWorldTransform(hDC, &xForm);

    // AlphaBlend로 렌더링
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    if (conceal)
    {
        bf.SourceConstantAlpha = 100;
    }
    else
    {
        bf.SourceConstantAlpha = 255;
    }
    bf.AlphaFormat = AC_SRC_ALPHA;

    if (m_iWidth == iWidth && m_iHeight == iHeight)
    {
        // 원본 크기 그대로
        AlphaBlend(hDC, 0, 0, iWidth, iHeight,  // 변환 적용되므로 0,0에서 시작
            m_hMemDC, 0, 0, m_iWidth, m_iHeight, bf);
    }
    else
    {
        // 스케일링 필요
        int stretchMode = SetStretchBltMode(hDC, COLORONCOLOR);
        AlphaBlend(hDC, 0, 0, iWidth, iHeight,  // 변환 적용되므로 0,0에서 시작
            m_hMemDC, 0, 0, m_iWidth, m_iHeight, bf);
        SetStretchBltMode(hDC, stretchMode);
    }

    // 변환 복구
    SetWorldTransform(hDC, &oldXform);
    SetGraphicsMode(hDC, oldMode);
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

    // GDI+ Graphics 사용 (자동 알파 처리!)
    Graphics graphics(hDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    graphics.SetPixelOffsetMode(PixelOffsetModeHalf);

    // PNG 알파 채널 그대로 사용 (자동으로 올바르게 블렌딩됨)
    graphics.DrawImage(m_pImage, iX, iY, iWidth, iHeight);
}

void CMyPng::Render_Alpha_Custom(HDC hDC, int iX, int iY, int iWidth, int iHeight, int alpha)
{
    if (!m_hMemDC)
        return;

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = alpha;
    bf.AlphaFormat = AC_SRC_ALPHA;

    if (m_iWidth == iWidth && m_iHeight == iHeight)
    {
        // 원본 크기 그대로
        AlphaBlend(hDC, iX, iY, iWidth, iHeight,
            m_hMemDC, 0, 0, m_iWidth, m_iHeight, bf);
    }
    else
    {
        // 스케일링 필요
        int oldMode = SetStretchBltMode(hDC, COLORONCOLOR);
        AlphaBlend(hDC, iX, iY, iWidth, iHeight,
            m_hMemDC, 0, 0, m_iWidth, m_iHeight, bf);
        SetStretchBltMode(hDC, oldMode);
    }
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