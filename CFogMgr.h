#pragma once

enum class eFogState
{
	UNKNOWN, EXPLORED, VISIBLE
};

class CFogMgr
{
private:
	CFogMgr();
	~CFogMgr();
public:
    void Initialize();
    void Update();
    void Render(HDC hDC);
    void Release();
    //안개 상태 관리 
    eFogState GetFogState(int row, int col);
    void SetFogState(int row, int col, eFogState state);
    //시야 계산
    void RevealArea(int centerRow, int centerCol, int sightRange);
    void UpdateVision();
    void ResetVisibleToExplored();
    //안개 렌더링
    void RenderFog(HDC hDC);
    //유틸리티 
    bool IsVisible(int row, int col);
    bool IsExplored(int row, int col);
    bool IsUnknown(int row, int col);
private:
    bool InRange(int row, int col) const; 
private:
    float m_fFogUpdateRate;
    const float UPDATE_INTERVAL = 0.3f;
    //타일별 안개 상태
    vector<eFogState> m_vecFogState;
    //안개 렌더링용 비트맵
    HDC m_hFogDC;
    HBITMAP m_hFogBitmap;
    HBITMAP m_hOldBitmap;
    //안개 최적화용 변수
    bool m_bFogDirty;
    HBRUSH m_hBlackBrush;
    HBRUSH m_hGrayBrush;
    HPEN m_hNullPen;
    HDC m_hFogCacheDC;
    HBITMAP m_hFogCacheBitmap;
    HBITMAP m_hFogCacheOld;
private:
    //EXPLORED 알파용 소스 1X1 소스
    HDC m_hAlphaSrcDC = nullptr;
    HBITMAP m_hAlphaSrcBmp = nullptr;
    HBITMAP m_hAlphaSrcOld = nullptr;
    BYTE m_byExploredAlpha = 200; //원하는 투명도
public:
    void MakeDirty() { m_bFogDirty = true; }
public:
    static CFogMgr* Get_Instance()
    {
        if (!m_pInstance)
            m_pInstance = new CFogMgr;
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
private:
	static CFogMgr* m_pInstance;
};