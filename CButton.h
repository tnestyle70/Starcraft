#pragma once
#include "CObj.h"

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
};
