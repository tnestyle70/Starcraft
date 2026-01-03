#pragma once
#include "CMyImage.h"

class CMyPng : public CMyImage
{
public:
    CMyPng();
    virtual ~CMyPng();

public:
    void Load_Image(const TCHAR* pFilePath) override;
    void Release() override;

    // PNG 전용 렌더링 (알파 블렌딩 지원)
    void Render_Alpha(HDC hDC, int iX, int iY);
    void Render_Alpha(HDC hDC, int iX, int iY, int iWidth, int iHeight);
    void Render_Alpha_Pure(HDC hDC, int iX, int iY);
    void Render_Alpha_Pure(HDC hDC, int iX, int iY, int iWidth, int iHeight);

    int Get_Width() const { return m_iWidth; }
    int Get_Height() const { return m_iHeight; }

private:
    Image* m_pImage;
    int         m_iWidth;
    int         m_iHeight;
};

