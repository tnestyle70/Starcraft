#pragma once
#include "Define.h"

class CCursorMgr
{
private:
	CCursorMgr();
	~CCursorMgr();
public:
    void Initialize();
    void Update();
    void Render(HDC hDC);
    void Release();
    
    void SetCursorType(eCursorType _eType) { m_eCursorType = _eType; }
    eCursorType GetCursorType() const { return m_eCursorType; }
    void SetClickEffect();    // 클릭 시 원형 확대 이펙트
    
private:
    void UpdateCursorType();  // 마우스 아래 오브젝트 체크
    void UpdateAnimation();   // 회전, 확대/축소 애니메이션
    
private:
    static CCursorMgr* m_pInstance;
    
    eCursorType m_eCursorType;
    Vec2 m_vMousePos;
    
    // 애니메이션
    int m_iFrameIndex;        // 현재 애니메이션 프레임
    DWORD m_dwFrameTime;      // 마지막 프레임 전환 시간
    DWORD m_dwFrameDelay;     // 프레임 전환 간격 (ms)
    
    float m_fRotation;        // 초록색 커서 회전 각도
    float m_fScale;           // 크기(클릭 이펙트)
    
    bool m_bClickEffect;      // 클릭 이펙트 재생 중
    DWORD m_dwClickStart;     // 클릭 이펙트 시작 시간
    int m_iEffectFrame; //현재 이펙트 프레임(0~6)
    
    // 스프라이트 정보
    static const int SPRITE_SIZE = 50;    // 원본 스프라이트 크기
    static const int RENDER_SIZE = 50;    // 실제 렌더링 크기
    static const int GRID_WIDTH = 16;     // 가로 스프라이트 개수
    static const int DEFAULT_MAX_FRAMES = 5; // 기본 커서 애니메이션 프레임 수
    static const int HOVER_MAX_FRAMES = 14; //호버링 최대 애니메이션 프레임
    static const int CLICK_EFFECT_FRAMES = 5; //select frame
    static const int EFFECT_FRAME_DELAY = 30; //클릭 이펙트 프레임 딜레이
    
public:
    static CCursorMgr* Get_Instance()
    {
        if (!m_pInstance)
            m_pInstance = new CCursorMgr;
        return m_pInstance;
    }
    static void Destroy_Instance()
    {
        if (m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = nullptr;
        }
    }
};