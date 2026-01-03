#pragma once
#include "CObj.h"

//struct AtlasRect { int iX, iY, iWidth, iHeight; };

class CButton :  public CObj
{
public:
    CButton();
    virtual ~CButton();
public:
    void Initialize() override;
    int Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
public:
    //설정 메서드
    void Set_PngImages(const TCHAR* pNormalKey, const TCHAR* pHoverKey);
    void Set_BmpFrameKey(const TCHAR* pFrameKey);
private:
    enum class eButtonType { BMP, PNG };
    eButtonType m_eButtonType;

    const TCHAR* m_pNormalKey;
    const TCHAR* m_pHoverKey;

    bool m_bHover;
//public:
//    void SetAtlas(const wchar_t* atlasKey, const AtlasRect& normal, const AtlasRect& hover);
//    //bool Hover() override;
//private:
//    const TCHAR* m_pAtlasKey;
//    AtlasRect m_rcNormal;
//    AtlasRect m_rcHover;
};
