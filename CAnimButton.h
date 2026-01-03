#pragma once
#include "CObj.h"

class CAnimButton : public CObj
{
public:
    CAnimButton();
    virtual ~CAnimButton();

public:
    void Initialize() override;
    int Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
public:
    // 기본 이미지 설정
    void Set_NormalImage(const TCHAR* pNormalKey);
    void Set_NormalAnimation(const TCHAR* pBaseKey, int iNormalFrameCount);
    void Set_HoverAnimation(const TCHAR* pBaseKey, int iFrameCount);
    void Set_ButtonType(const TCHAR* pType) { m_pButtonType = pType; }
private:
    // Normal 이미지
    const TCHAR* m_pNormalKey = nullptr;
    // Normal 애니메이션
    const TCHAR* m_pNormalAnimKey = nullptr;
    int m_iNormalFrameCount = 0;
    int m_iNormalCurrentFrame = 0;
    float m_fNormalAnimTime = 0;
    // Hover 애니메이션
    const TCHAR* m_pAnimBaseKey = nullptr;  // "Btn_Single_Anim_"
    int m_iFrameCount = 0;            // 35
    int m_iCurrentFrame = 0;          // 0~34
    float m_fAnimTime = 0;            // 애니메이션 타이머
    float m_fFrameDelay = 0.05f;          // 프레임당 시간 (0.03초 = 30fps)
    // 버튼 타입
    const TCHAR* m_pButtonType = nullptr;   // "Start", "Edit", "Exit
    bool m_bHover = false;
};

