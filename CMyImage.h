#pragma once
#include "Define.h"

enum class eImageType
{
	BMP, PNG
};

class CMyImage
{
public:
    CMyImage();
    virtual ~CMyImage() {};

public:
    virtual void Load_Image(const TCHAR* pFilePath) PURE;
    virtual void Release() PURE;

    HDC Get_MemDC() const { return m_hMemDC; }
    HBITMAP Get_Bitmap() { return m_hBitmap; }
    eImageType Get_Type() const { return m_eType; }

protected:
    HDC         m_hMemDC;
    HBITMAP     m_hBitmap;
    HBITMAP     m_hOldBitmap;
    eImageType  m_eType;
};