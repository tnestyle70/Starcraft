#pragma once
#include "Define.h"
#include "CObj.h"
#include "CommandSlot.h"

struct ProgressbarInfo
{
    float fProgress;
    //건물 정보
    const TCHAR* pUnitName;
    int iQueueCount;
    bool bIsVisible;
};

struct BuildingUIInfo
{
    int iHP;
    int iMaxHP;
    const TCHAR* pBuildingName;
    eBuildingType eType;
    //생산정보
    bool IsProducing;
    //현재 생산 중인 유닛 정보
    const TCHAR* pCurrentUnit;
    float fProgress;
    //생산 대기열(최대 5개)
    struct QueueItem
    {
        eCommandID command;
        int iIconKey;
    };
    vector<QueueItem> queue; //생산 대기 유닛
    bool IsVisible;
    BuildingUIInfo()
        : pBuildingName(nullptr)
        , eType(eBuildingType::COMMAND_CENTER) // 또는 적절한 기본값
        , IsProducing(false)
        , pCurrentUnit(nullptr)
        , fProgress(0.f)
        , IsVisible(false)
        , iHP(0)
        , iMaxHP(0)
    {
        queue.clear(); // 명시적으로 비우기 (사실 필요없긴 함)
    }
};

class CMainUI
{
private:
    CMainUI();
    ~CMainUI();

public:
    void Initialize();
    void Render(HDC hDC);
    void Release();

public:
    void RenderFrame(HDC hDC);
    HBITMAP CreateAlphaBitmap(HDC hdc, HDC hSrcDC, int width, int height, COLORREF transparentColor);
    //Progressbar
    void SetProgressInfo(const ProgressbarInfo& info);
    void RenderProgressbar(HDC hDC);
    void RenderProgressText(HDC hDC, int barX, int barY);
    //Building 
    void SetBuildingUIInfo(const BuildingUIInfo& info);
    void RenderBuildingInfo(HDC hDC);
    void RenderBuildingName(HDC hDC);
    void RenderProductionQueue(HDC hDC);
    void RenderCurrentProduction(HDC hDC);
    //Minimap
    void InitializeMinimap();
    void RenderMinimap(HDC hDC);
    void RenderMinimapUnit(HDC hDC);
    void RenderMinimapFrame(HDC hDC);
    void HandleMinimapClick(POINT mousePos);
    //Wire 이미지
    void RenderBuildingWire(HDC hDC);
private:
    ProgressbarInfo m_tProgressInfo;
    BuildingUIInfo m_tBuildingUIInfo;

    RECT m_srcPanel;    // 원본 이미지에서의 영역
    RECT m_dstPanel;    // 화면에 그릴 영역

    HDC m_dcPanel;      // 알파 비트맵용 DC
    HBITMAP m_bmpPanel; // 알파 채널이 적용된 비트맵
    HBITMAP m_oldPanel; // 이전 비트맵 저장용

    HFONT m_hFont;      // 폰트
private:
    //미니맵 관련 정보
    RECT m_srcMinimap;
    RECT m_dstMinimap;
    float m_fMinimapScale; //월드 좌표 -> 미니맵 좌표 변환 비율
    HDC m_dcMinimap;
    HBITMAP m_bmpMinimap;
    HBITMAP m_oldMinimap;
    //카메라 프레임용
    HDC m_dcFrame;
    HBITMAP m_bmpFrame;
    HBITMAP m_oldFrame;
public:
    static CMainUI* Get_Instance()
    {
        if (nullptr == m_pInstance)
        {
            m_pInstance = new CMainUI;
        }

        return m_pInstance;
    }
    static void	Destroy_Instance()
    {
        if (m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = nullptr;
        }
    }
private:
    static CMainUI* m_pInstance;
};