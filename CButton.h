#pragma once
#include "CObj.h"

struct AtlasRect { int iX, iY, iWidth, iHeight; };

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
    void SetAtlas(const wchar_t* atlasKey, const AtlasRect& normal, const AtlasRect& hover);
    //bool Hover() override;
private:
    const TCHAR* m_pAtlasKey;
    AtlasRect m_rcNormal;
    AtlasRect m_rcHover;
    bool m_bHover;
};
