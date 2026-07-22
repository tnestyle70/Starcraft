#include "pch.h"
#include "CMainUI.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CTileMgr.h"
#include "CObjMgr.h"
#include "CBuilding.h"
#include "CScrollMgr.h"
#include "CSelectionMgr.h"
#include "CResourceMgr.h"
#include "CFogMgr.h"
#include "CTimeMgr.h"
#include "CButton.h"
#include "CAbstractFactory.h"
#include "CPortraitMgr.h"
#include "CGameDataMgr.h"

CMainUI* CMainUI::m_pInstance = nullptr;

CMainUI::CMainUI() : m_dcPanel(nullptr), m_bmpPanel(nullptr), m_oldPanel(nullptr)
    ,m_hFont(nullptr), m_dcMinimapFog(nullptr), m_bmpMinimapFog(nullptr), m_oldMinimapFog(nullptr)
    ,m_dcFrame(nullptr), m_bmpFrame(nullptr), m_oldFrame(nullptr)
{
    ZeroMemory(&m_tProgressInfo, sizeof(ProgressbarInfo));
    ZeroMemory(&m_tBuildingUIInfo, sizeof(BuildingUIInfo));
    ZeroMemory(&m_tUnitUIInfo, sizeof(UnitUIInfo));
}

CMainUI::~CMainUI()
{
}

// 알파 비트맵 생성 함수 - 추후에 최적화
HBITMAP CMainUI::CreateAlphaBitmap(HDC hdc, HDC hSrcDC, int width, int height, COLORREF transparentColor)
{
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Top-down (음수로 설정)
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32비트 (RGBA)
    bmi.bmiHeader.biCompression = BI_RGB;

    BYTE* pBits = nullptr;
    HBITMAP hAlphaBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);

    if (!hAlphaBitmap || !pBits)
        return nullptr;

    // 원본 이미지의 픽셀 데이터를 읽어와서 알파 채널 적용
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            COLORREF pixel = GetPixel(hSrcDC, x, y);
            int index = (y * width + x) * 4;

            if (pixel == transparentColor) {
                // 투명 처리 (마젠타 색상)
                pBits[index] = 0;     // Blue
                pBits[index + 1] = 0; // Green
                pBits[index + 2] = 0; // Red
                pBits[index + 3] = 0; // Alpha = 0 (완전 투명)
            }
            else {
                // 불투명 처리
                pBits[index] = GetBValue(pixel);     // Blue
                pBits[index + 1] = GetGValue(pixel); // Green
                pBits[index + 2] = GetRValue(pixel); // Red
                pBits[index + 3] = 255;              // Alpha = 255 (완전 불투명)
            }
        }
    }

    return hAlphaBitmap;
}

void CMainUI::Initialize()
{
    // 1. 영역 설정
    m_srcPanel = { 0, 367, 800, 600 };
    int panelW = m_srcPanel.right - m_srcPanel.left;
    int panelH = m_srcPanel.bottom - m_srcPanel.top;

    m_dstPanel.left = 0;
    m_dstPanel.top = WINCY - panelH;
    m_dstPanel.right = panelW;
    m_dstPanel.bottom = WINCY;

    m_rcMainUIArea = m_dstPanel;

    HDC hUIDC = CBmpMgr::Get_Instance()->Find_Image(L"MainUI");
    if (!hUIDC) return;

    // 2. 임시 DC 생성 - 원본 이미지에서 UI 영역만 추출
    HDC hTempDC = CreateCompatibleDC(hUIDC);
    HBITMAP hTempBmp = CreateCompatibleBitmap(hUIDC, panelW, panelH);
    HBITMAP hTempOld = (HBITMAP)SelectObject(hTempDC, hTempBmp);

    // 원본에서 UI 영역만 복사
    BitBlt(hTempDC, 0, 0, panelW, panelH, hUIDC, m_srcPanel.left, m_srcPanel.top, SRCCOPY);

    // 3. 알파 비트맵 생성 (마젠타 색상을 투명 처리)
    m_bmpPanel = CreateAlphaBitmap(hUIDC, hTempDC, panelW, panelH, RGB(255, 0, 255));

    // 임시 DC 해제
    SelectObject(hTempDC, hTempOld);
    DeleteObject(hTempBmp);
    DeleteDC(hTempDC);

    // 4. 알파 비트맵을 위한 DC 생성
    m_dcPanel = CreateCompatibleDC(hUIDC);
    m_oldPanel = (HBITMAP)SelectObject(m_dcPanel, m_bmpPanel);

    // 폰트 생성 (한 번만)
    m_hFont = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Jetbrain MONO");
    //자원용 폰트 생성
    m_hResourceFont = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Jetbrain MONO");

    //미니맵 초기화
    InitializeMinimap();
    //안개용 DC 생성
    HDC hDC = GetDC(g_hWnd);
    int minimapWidth = m_dstMinimap.right - m_dstMinimap.left; //160
    int minimapHeight = m_dstMinimap.bottom - m_dstMinimap.top; //160
    m_dcMinimapFog = CreateCompatibleDC(hDC);

    //DIBSection 32bpp 생성
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = minimapWidth;
    bmi.bmiHeader.biHeight = -minimapHeight; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    //CreateCompatibleBitmap -> CreateDIBSection으로 교체
    void* pBits = nullptr;
    m_bmpMinimapFog = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    m_pMinimapFogBits = (BYTE*)pBits;  //이게 있어야 UpdateMinimapFog에서 직접 쓸 수 있음
    m_oldMinimapFog = (HBITMAP)SelectObject(m_dcMinimapFog, m_bmpMinimapFog);

    //m_bmpMinimapFog = CreateCompatibleBitmap(hDC, minimapWidth, minimapHeight);
    //m_oldMinimapFog = (HBITMAP)SelectObject(m_dcMinimapFog, m_bmpMinimapFog);

    ReleaseDC(g_hWnd, hDC);
    m_fMinimapFogDelay = 0.f;

    //Menu 버튼
    CObj* pMenuButton = CAbstractFactory<CButton>::Create(556.f,500.f);
    pMenuButton->Initialize();
    CButton* ppMenuButton = dynamic_cast<CButton*>(pMenuButton);
    if (ppMenuButton)
    {
        ppMenuButton->Set_BmpFrameKey(L"MENU_BTN");
    }
    //CObjMgr말고 m_ButtonList로 CMainUI에서 직접 버튼 관리
    m_ButtonList.push_back(ppMenuButton);

    //WinText 버튼 생성
    CObj* pWinTextButton = CAbstractFactory<CButton>::Create(400.f, 250.f);
    pWinTextButton->Initialize();
    CButton* ppWinText = dynamic_cast<CButton*>(pWinTextButton);
    if (ppWinText)
    {
        ppWinText->Set_BmpFrameKey(L"WIN_TEXT");
    }
    m_pWinTextButton = ppWinText;
}

int CMainUI::Update()
{
    for (auto& pButton : m_ButtonList)
    {
        pButton->Update();
        pButton->Late_Update();
    }
    if (m_bEndGame)
    {
        m_pWinTextButton->Update();
        m_pWinTextButton->Late_Update();
    }
    return NOEVENT;
}

void CMainUI::Render(HDC hDC)
{
    UpdateMinimapFog(); //전장의 안개 업데이트 
    RenderFrame(hDC); //메인 프레임
    RenderBuildingInfo(hDC); //빌딩
    RenderBuildingWire(hDC);
    RenderUnitInfo(hDC); //단일 유닛
    RenderMultiUnitWires(hDC); //여러 유닛
    RenderShieldBar(hDC); //쉴드바
    RenderHealthBar(hDC); //체력바
    RenderMPBar(hDC); //MP바
    RenderMinimap(hDC); //미니맵
    RenderResource(hDC); //리소스
    //버튼 렌더링
    for (auto& pButton : m_ButtonList)
    {
        pButton->Render(hDC);
    }
    if (m_bEndGame)
    {
        m_pWinTextButton->Render(hDC);
    }
}

void CMainUI::RenderFrame(HDC hDC)
{
    if (!m_dcPanel) return;

    int w = m_srcPanel.right - m_srcPanel.left;
    int h = m_srcPanel.bottom - m_srcPanel.top;

    // AlphaBlend 사용 (TransparentBlt보다 훨씬 빠름!)
    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255; // 완전 불투명
    bf.AlphaFormat = AC_SRC_ALPHA; // 소스 알파 채널 사용

    AlphaBlend(hDC,
        m_dstPanel.left, m_dstPanel.top, w, h,
        m_dcPanel,
        0, 0, w, h,
        bf);
}

void CMainUI::SetBuildingUIInfo(const BuildingUIInfo& info)
{
    m_tBuildingUIInfo = info;
}

void CMainUI::RenderBuildingInfo(HDC hDC)
{
    if (!m_tBuildingUIInfo.IsVisible)
        return;
    RenderBuildingName(hDC);
    RenderBuildingPortrait(hDC);
    if (m_tBuildingUIInfo.IsProducing)
    {
        RenderCurrentProduction(hDC);
        RenderProductionQueue(hDC);
    }   
}

void CMainUI::RenderBuildingPortrait(HDC hDC)
{
    if (!m_tBuildingUIInfo.IsVisible)
        return;

    eBuildingType type = m_tBuildingUIInfo.eType;

    eRaceType raceType = m_tBuildingUIInfo.eRaceType;

    if (raceType == eRaceType::RACE_PROTOSS)
    {
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, eUnitType::NONE, eRaceType::RACE_PROTOSS);
        return;
    }

    switch (type)
    {
    case eBuildingType::HIVE:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, eUnitType::NONE, eRaceType::RACE_ZERG);
        return;
    case eBuildingType::HYDRALISK_DEN:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, eUnitType::NONE, eRaceType::RACE_ZERG);
        return;
    case eBuildingType::SPIRE:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, eUnitType::NONE, eRaceType::RACE_ZERG);
        return;
    case eBuildingType::SPAWNING_POOL:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, eUnitType::NONE, eRaceType::RACE_ZERG);
        return;
    case eBuildingType::ULTRALISK_DEN:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, eUnitType::NONE, eRaceType::RACE_ZERG);
        return;
    default:
        break;
    }

    CPortraitMgr::Get_Instance()->RenderPortraitBMP(hDC, eUnitType::NONE);
}

void CMainUI::RenderBuildingName(HDC hDC)
{
    if (!m_tBuildingUIInfo.pBuildingName)
        return;

    const int PANEL_HEIGHT = 100;
    const int PANEL_TOP = WINCY - PANEL_HEIGHT;
    //건물 이름
    int nameX = 420;
    int nameY = 490;
    //HP바
    int hpX = 250; 
    int hpY = 550;
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(255, 255, 255));
    HFONT hOldFont = (HFONT)SelectObject(hDC, m_hFont);

    RECT nameRect;
    nameRect.left = nameX - 100;
    nameRect.top = nameY;
    nameRect.right = nameX + 100;
    nameRect.bottom = nameY + 20;
    DrawText(hDC, m_tBuildingUIInfo.pBuildingName, -1, &nameRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    bool bHasShield = (m_tBuildingUIInfo.iMaxShield > 0);

    // 2. HP 정보 표시 Wire 이미지 아래에
    wchar_t hpText[64];
    swprintf_s(hpText, L"%d / %d", m_tBuildingUIInfo.iHP, m_tBuildingUIInfo.iMaxHP);
    SetTextColor(hDC, RGB(0, 255, 0));  // 초록색

    RECT hpRect;
    hpRect.left = hpX - 100;
    hpRect.top = hpY + 20;  //Wire 바로 밑에
    hpRect.right = hpX + 100;
    hpRect.bottom = hpY + 40;
    DrawText(hDC, hpText, -1, &hpRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    if (bHasShield)
    {
        // 3. Shield 정보 표시 HP 정보 위에 표시
        wchar_t shieldText[64];
        swprintf_s(shieldText, L"%d / %d", m_tBuildingUIInfo.iShield, m_tBuildingUIInfo.iMaxShield);
        SetTextColor(hDC, RGB(50, 206, 235));  // 파란색

        RECT shieldRect;
        shieldRect.left = hpX - 100;
        shieldRect.top = hpY - 10;  //HP 정보 바로 위에 표시
        shieldRect.right = hpX + 100;
        shieldRect.bottom = hpY + 40;
        DrawText(hDC, shieldText, -1, &shieldRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    SelectObject(hDC, hOldFont);
    SelectObject(hDC, hOldFont);
}

void CMainUI::RenderCurrentProduction(HDC hDC)
{
    if (!m_tBuildingUIInfo.pCurrentUnit)
        return;
    //현재 생산 중인 유닛 텍스트
    int textY = 510;
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(255, 255, 255));
    RECT textRect;
    textRect.left = WINCX / 2 - 100 + 20;
    textRect.top = textY;
    textRect.right = WINCX / 2 + 100 + 20;
    textRect.bottom = textY + 18;
    DrawText(hDC, m_tBuildingUIInfo.pCurrentUnit, -1, &textRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    // ProgressBar
    const int BAR_WIDTH = 108;
    const int BAR_HEIGHT = 9;
    int barX = (WINCX - BAR_WIDTH) / 2 + 20;
    int barY = textY + 20;

    // ProgressBar 배경
    HDC hEmptyBarDC = CBmpMgr::Get_Instance()->Find_Image(L"PROGRESS_EMPTY");
    if (hEmptyBarDC)
    {
        GdiTransparentBlt(hDC,
            barX, barY,
            BAR_WIDTH, BAR_HEIGHT,
            hEmptyBarDC,
            0, 0,
            BAR_WIDTH, BAR_HEIGHT,
            RGB(0, 255, 0));
    }

    // 진행도
    HDC hRectDC = CBmpMgr::Get_Instance()->Find_Image(L"HP_RECT");
    if (hRectDC)
    {
        const int RECT_WIDTH = 4;
        const int RECT_HEIGHT = 5;
        const int TOTAL_RECTS = 27;

        int fillRects = (int)(TOTAL_RECTS * m_tBuildingUIInfo.fProgress);

        for (int i = 0; i < fillRects; ++i)
        {
            int rectX = barX + (i * RECT_WIDTH);

            GdiTransparentBlt(hDC,
                rectX, barY,
                RECT_WIDTH, RECT_HEIGHT,
                hRectDC,
                0, 0,
                RECT_WIDTH, RECT_HEIGHT,
                RGB(0, 255, 0));
        }
    }
}

void CMainUI::RenderProductionQueue(HDC hDC)
{
    if (!m_tBuildingUIInfo.IsVisible)
        return;
    //대기열 위치
    int queueX = 300;
    int queueY = 500;
    const int ICON_SIZE = 44;
    const int ICON_GAP = 4;
    const int MAX_SLOTS = 5;
    const int NUMBER_SIZE = 48;
    const int NUMBER_GAP = 2;
    HDC hETCDC = CBmpMgr::Get_Instance()->Find_Image(L"ETC");
    if (!hETCDC)
        return;

    //5개의 빈 블롯 그리기
    for (int i = 0; i < MAX_SLOTS; ++i)
    {
        int x, y;
        int srcX, srcY;
        //숫자
        if (i == 0)
        {
            x = queueX;
            y = queueY;
            srcX = 0;
            srcY = 0;
        }
        else
        {
            x = queueX + (i - 1) * NUMBER_SIZE;
            y = queueY + 48;
            srcX = (i - 1) * NUMBER_SIZE;
            srcY = 0 + 48;
        }
        // 슬롯 배경 그리기
        GdiTransparentBlt(hDC,
            x, y, //화면에서 그릴 위치(목적지 x, y)
            NUMBER_SIZE, NUMBER_SIZE, //화면에 그릴 크기
            hETCDC,
            srcX, srcY, //원본 이미지에서 복사할 시작 위치
            NUMBER_SIZE, NUMBER_SIZE, //월본 이미지에서 복사할 크기
            RGB(0, 255, 255));
    }
    
    //종족별 아이콘 렌더링
    eRaceType type = m_tBuildingUIInfo.eRaceType;

    if (type == eRaceType::RACE_TERRAN)
    {
        HDC hIconDC = CBmpMgr::Get_Instance()->Find_Image(L"ICON_BUTTON_UI");
        if (hIconDC && !m_tBuildingUIInfo.queue.empty())
        {
            //대기열 아이콘 그리기
            for (size_t i = 0; i < m_tBuildingUIInfo.queue.size(); ++i)
            {
                int x, y;
                //숫자
                if (i == 0)
                {
                    x = queueX;
                    y = queueY;
                }
                else
                {
                    x = queueX + (i - 1) * NUMBER_SIZE;
                    y = queueY + 48;
                }
                //int x = queueX + (i * (ICON_SIZE + ICON_GAP));
                //int y = queueY;
                //아이콘 렌더링
                int iconIndex = m_tBuildingUIInfo.queue[i].iIconKey;
                if (iconIndex < 0)
                    continue;

                int srcX = 0;
                int srcY = iconIndex * ICON_SIZE;

                //작게 축소해서 그리기
                GdiTransparentBlt(hDC, x, y,
                    ICON_SIZE, ICON_SIZE,
                    hIconDC,
                    0, srcY,
                    ICON_SIZE, ICON_SIZE,
                    RGB(0, 255, 0));
            }
        }
    }
    else if (type == eRaceType::RACE_PROTOSS)
    {
        if (!m_tBuildingUIInfo.queue.empty())
        {
            //대기열 아이콘 그리기
            for (size_t i = 0; i < m_tBuildingUIInfo.queue.size(); ++i)
            {
                int x, y;
                //숫자
                if (i == 0)
                {
                    x = queueX;
                    y = queueY;
                }
                else
                {
                    x = queueX + (i - 1) * NUMBER_SIZE;
                    y = queueY + 48;
                }
                CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(m_tBuildingUIInfo.queue[i].wsIconName);
                if (!pPng)
                    continue;
                pPng->Render_Alpha(hDC, x, y, ICON_SIZE, ICON_SIZE, false, false);
            }
        }
    }
}

void CMainUI::RenderBuildingWire(HDC hDC)
{
    if (!m_tBuildingUIInfo.IsVisible)
        return;
    HDC hWireDC = CBmpMgr::Get_Instance()->Find_Image(L"WIRE");
    if (!hWireDC)
        return;
    //건물 타입에 따른 다른 오프셋 적용하기 
    const int WIRE_CELL_SIZE = 64;
    const int WIRE_COLS = 6;
    //건물 타입에 따른 이미지 인덱스
    int buildingRow = -1;

    TCHAR szKey[256];
    CMyPng* pPng = nullptr;

    switch (m_tBuildingUIInfo.eType)
    {
    case eBuildingType::COMMAND_CENTER:
        buildingRow = 15;
        break;
    case eBuildingType::SUPPLY_DEPOT:
        buildingRow = 16;
        break;
    case eBuildingType::REFINERY:
        buildingRow = 17;
        break;
    case eBuildingType::BARRACKS:
        buildingRow = 18;
        break;
    case eBuildingType::ENGINEERING_BAY:
        buildingRow = 19;
        break;
    case eBuildingType::TURRET:
        buildingRow = 20;
        break;
    case eBuildingType::ACADEMY:
        buildingRow = 21;
        break;
    case eBuildingType::BUNKER:
        buildingRow = 22;
        break;
    case eBuildingType::FACTORY:
        buildingRow = 23;
        break;
    case eBuildingType::STARPORT:
        buildingRow = 24;
        break;
    case eBuildingType::SCIENCE_FACILITY:
        buildingRow = 25;
        break;
    case eBuildingType::ARMORY:
        buildingRow = 26;
        break;
    case eBuildingType::COMBAT_STATION:
        buildingRow = 27;
        break;
    case eBuildingType::NUCLEAR_SILO:
        buildingRow = 28;
        break;
    case eBuildingType::SCIENCE_SECRET:
        buildingRow = 29;
        break;
    case eBuildingType::SCIENCE_PHYSICS:
        buildingRow = 30;
        break;
    case eBuildingType::STARPORT_ADDON:
        buildingRow = 31;
        break;
    case eBuildingType::FACTORY_ADDON:
        buildingRow = 32;
        break;
        //저그 빌딩 와이어
    case eBuildingType::HATCHERY:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Hatchery_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                210, WINCY - 130 + 30,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::LAIR:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Lair_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                210, WINCY - 130 + 30,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::HIVE:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Hive_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                210, WINCY - 130 + 30,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::HYDRALISK_DEN:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"HydraliskDen_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                210, WINCY - 130 + 30,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::SPIRE:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Spire_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                210, WINCY - 130 + 30,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::SPAWNING_POOL:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"SpawningPool_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                210, WINCY - 130 + 30,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::ULTRALISK_DEN:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"UltraliskDen_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                210, WINCY - 130 + 30,
                iWidth, iHeight, false);
        }
        return;
        //프로토스
    case eBuildingType::NEXUS:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Nexus_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::PYLON:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Pylon_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::GATEWAY:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Gateway_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::CYBERNETICS_CORE:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Core_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::SHIELD_BATTERY:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"ShieldBattery_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::ROBOTICS_FACILITY:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Robotics_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::STARGATE:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Stargate_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::CITADEL_OF_ADUN:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Adun_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::ROBOTICS_SUPPORT_BAY:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"RoboticsSupportBay_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::FLEET_BEACON:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"FleetBeacon_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::TEMPLAR_ARCHIVES:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"TemplarArchive_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::OBSERVATORY:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Observatory_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    case eBuildingType::ARBITER_TRIBUNAL:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"ArbitorTribunal_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false);
        }
        return;
    default:
        return;  // 알 수 없는 타입은 렌더링하지 않음
    }

    int healthCol = GetWireColumnByHealth(m_tBuildingUIInfo.iHP,
        m_tBuildingUIInfo.iMaxHP);

    int srcX = healthCol * WIRE_CELL_SIZE;
    int srcY = buildingRow * WIRE_CELL_SIZE;

    const int PANEL_HEIGHT = 130;
    const int PANEL_TOP = WINCY - PANEL_HEIGHT;

    int destX = 210;
    int destY = PANEL_TOP + 30;

    GdiTransparentBlt(hDC,
        destX, destY,
        WIRE_CELL_SIZE, WIRE_CELL_SIZE,
        hWireDC,
        srcX, srcY,
        WIRE_CELL_SIZE, WIRE_CELL_SIZE,
        RGB(0, 0, 0));
}

void CMainUI::SetMultiBuildingUIInfo(const MultiBuildingUIInfo& info)
{
    m_tMultiBuildingUIInfo = info;
}

void CMainUI::RenderMultiBuildingWires(HDC hDC)
{
    /*
    //8 * 2 그리드로 멀티 유닛 와이어프레임 렌더링
    if (!m_tMultiUnitUIInfo.IsVisible)
        return;
    HDC hFrameDC = CBmpMgr::Get_Instance()->Find_Image(L"TRAIN_FRAME");
    if (!hFrameDC)
        return;
    HDC hWireDC = CBmpMgr::Get_Instance()->Find_Image(L"SMALL_WIRE");
    if (!hWireDC)
        return;
    //프레임 사이즈 
    const int FRAME_SIZE = 42;
    const int FRAME_COLS = 6;

    //와이어 사이즈 
    const int WIRE_SIZE = 32;
    const int WIRE_COLS = 6;

    const int WIRE_OFFSET = (FRAME_SIZE - WIRE_SIZE) * 0.5; //중앙 정렬 오프셋

    //UI 위치
    const int BASE_X = 220;  // 시작 X 좌표
    const int BASE_Y = 495;  // 시작 Y 좌표
    const int GAP_X = 2;     // 와이어 간 가로 간격
    const int GAP_Y = 10;     // 와이어 간 세로 간격
    const int COLS = 6;      // 한 줄에 8개
    const int ROWS = 2;      // 2줄

    // 각 유닛 타입별 BigWire.bmp 내 시작 행 인덱스
    // 예시 (실제 비트맵 구조에 맞게 조정 필요)
    const int UNIT_TYPE_ROW_MAP[11] = {
        -1, // NONE
        0,   // SCV (0, 1, 2행)
        1,   // MARINE (3, 4, 5행)
        2,   // MEDIC (6, 7, 8행)
        3,   //GHOST
        4,   //FIREBAT
        5,   // VULTURE (9, 10, 11행)
        6,  // TANK (12, 13, 14행)
        7,  //SIEGETANK
        8,  //GOLIATH
        11  //BATTLECRUISER
    };

    //최대 16개 유닛 카운트
    int displayCount = min(12, m_tMultiUnitUIInfo.iUnitCount);
    for (int i = 0; i < displayCount; ++i)
    {
        const MultiUnitWireInfo& info = m_tMultiUnitUIInfo.units[i];
        //그리드 위치 계산
        int col = i % COLS;
        int row = i / COLS;
        //프레임 그려질 위치
        int frameDestX = BASE_X + col * (FRAME_SIZE + GAP_X);
        int frameDestY = BASE_Y + row * (FRAME_SIZE + GAP_X);

        //유닛 타입에 따른 기본 행 인덱스(bmp wire에 맞춰서 수정)
        int typeIndex = static_cast<int>(info.eType);
        if (typeIndex < 0 || typeIndex >= 11)
            typeIndex = 0;
        int baseRow = UNIT_TYPE_ROW_MAP[typeIndex];
        //체력 상태에 따른 행 오프셋
        int healthOffset = GetWireHealthState(info.iHP, info.iMaxHP);
        int wireRow = baseRow + healthOffset;
        //각 유닛은 6열 중 특정 열 사용
        int wireCol = 0;

        //프레임
        GdiTransparentBlt(hDC,
            frameDestX, frameDestY,
            FRAME_SIZE, FRAME_SIZE,  // 42x42로 그림!
            hFrameDC,
            0, 0,  // 프레임 비트맵은 보통 단일 이미지
            FRAME_SIZE, FRAME_SIZE,
            RGB(255, 0, 255));

        //외이어 그려질 위치
        int wireDestX = frameDestX + WIRE_OFFSET;
        int wireDestY = frameDestY + WIRE_OFFSET;
        //와이어
        int wireSrcX = wireCol * WIRE_SIZE;
        int wireSrcY = wireRow * WIRE_SIZE;

        //와이어
        GdiTransparentBlt(hDC,
            wireDestX, wireDestY,
            WIRE_SIZE, WIRE_SIZE,
            hWireDC,
            wireSrcX, wireSrcY,
            WIRE_SIZE, WIRE_SIZE,
            RGB(0, 0, 0));
    }
    */
}


void CMainUI::SetUnitUIInfo(const UnitUIInfo& info)
{
    m_tUnitUIInfo = info;
}

void CMainUI::RenderUnitInfo(HDC hDC)
{
    if (!m_tUnitUIInfo.IsVisible)
        return;
    RenderUnitName(hDC);
    RenderUnitStats(hDC);
    RenderUnitWire(hDC);
    RenderUnitPortrait(hDC);
}

void CMainUI::RenderUnitPortrait(HDC hDC)
{
    if (!m_tUnitUIInfo.IsVisible)
        return;

    eUnitType type = m_tUnitUIInfo.eType;

    eRaceType raceType = m_tUnitUIInfo.eRaceType;

    if (raceType == eRaceType::RACE_PROTOSS)
    {
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, eUnitType::NONE, eRaceType::RACE_PROTOSS);
        return;
    }

    switch (type)
    {
    case eUnitType::NONE:
        break;
    case eUnitType::SCV:
        break;
    case eUnitType::MARINE:
        break;
    case eUnitType::MEDIC:
        break;
    case eUnitType::FIREBAT:
        CPortraitMgr::Get_Instance()->RenderPortraitBMPRow(hDC, m_tUnitUIInfo.eType);
        return;
    case eUnitType::GHOST:
        break;
    case eUnitType::VULTURE:
        CPortraitMgr::Get_Instance()->RenderPortraitBMPRow(hDC, m_tUnitUIInfo.eType);
        return;
    case eUnitType::TANK:
        break;
    case eUnitType::SIEGE_TANK:
        break;
    case eUnitType::GOLIATH:
        CPortraitMgr::Get_Instance()->RenderPortraitBMPRow(hDC, m_tUnitUIInfo.eType);
        return;
    case eUnitType::BATTLECRUISER:
        break;
    case eUnitType::DRONE:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, m_tUnitUIInfo.eType, eRaceType::RACE_ZERG);
        return;
    case eUnitType::ZERGLING:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, m_tUnitUIInfo.eType, eRaceType::RACE_ZERG);
        return;
    case eUnitType::HYDRALISK:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, m_tUnitUIInfo.eType, eRaceType::RACE_ZERG);
        return;
    case eUnitType::ULTRALISK:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, m_tUnitUIInfo.eType, eRaceType::RACE_ZERG);
        return;
    case eUnitType::MUTALISK:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, m_tUnitUIInfo.eType, eRaceType::RACE_ZERG);
        return;
    case eUnitType::OVERLOAD:
        CPortraitMgr::Get_Instance()->RenderPortraitPNG(hDC, m_tUnitUIInfo.eType, eRaceType::RACE_ZERG);
        return;
    case eUnitType::ZEALOT:
        break;
    default:
        break;
    }

    CPortraitMgr::Get_Instance()->RenderPortraitBMP(hDC, m_tUnitUIInfo.eType);
}

void CMainUI::RenderUnitName(HDC hDC)
{
    if (!m_tUnitUIInfo.pUnitName)
        return;
    const int PANEL_HEIGHT = 100;
    const int PANEL_TOP = WINCY - PANEL_HEIGHT;
    //유닛 이름
    int nameX = 420;
    int nameY = 490;
    //HP바
    int hpX = 250;
    int hpY = 550;

    //유닛 이름 표시
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(255, 255, 255));
    HFONT hOldFont = (HFONT)SelectObject(hDC, m_hFont);
    RECT nameRect;
    nameRect.left = nameX - 100;
    nameRect.top = nameY;
    nameRect.right = nameX + 100;
    nameRect.bottom = nameY + 20;
    DrawText(hDC, m_tUnitUIInfo.pUnitName, -1, &nameRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    bool bHasMP = (m_tUnitUIInfo.iMaxMP > 0);
    bool bHasShield = (m_tUnitUIInfo.iMaxShield > 0);

    // 2. HP 정보 표시 Wire 이미지 아래에
    wchar_t hpText[64];
    swprintf_s(hpText, L"%d / %d", m_tUnitUIInfo.iHP, m_tUnitUIInfo.iMaxHP);
    SetTextColor(hDC, RGB(0, 255, 0));  // 초록색

    RECT hpRect;
    hpRect.left = hpX - 100;
    hpRect.top = hpY + 20;  //Wire 바로 밑에
    hpRect.right = hpX + 100;
    hpRect.bottom = hpY + 40;
    DrawText(hDC, hpText, -1, &hpRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    //3. MP 정보 표시
    if(bHasMP)
    {
        wchar_t mpText[64];
        swprintf_s(mpText, L"%d / %d", m_tUnitUIInfo.iMP, m_tUnitUIInfo.iMaxMP);
        SetTextColor(hDC, RGB(135, 206, 235));  // 파란색

        RECT mpRect;
        mpRect.left = hpX + 30;
        mpRect.top = hpY + 20;  //HealthBar 위에 표시
        mpRect.right = hpX + 100;
        mpRect.bottom = hpY + 40;
        DrawText(hDC, mpText, -1, &mpRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    //4. Shield 정보 표시
    if (bHasShield)
    {
        wchar_t shieldText[64];
        swprintf_s(shieldText, L"%d / %d", m_tUnitUIInfo.iShield, m_tUnitUIInfo.iMaxShield);
        SetTextColor(hDC, RGB(50, 206, 235));  // 파란색

        RECT shieldRect;
        shieldRect.left = hpX - 100;
        shieldRect.top = hpY - 10;  //HealthBar 위에 표시
        shieldRect.right = hpX + 100;
        shieldRect.bottom = hpY + 40;
        DrawText(hDC, shieldText, -1, &shieldRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    SelectObject(hDC, hOldFont);
    SelectObject(hDC, hOldFont);
}

void CMainUI::RenderUnitStats(HDC hDC) //유닛의 방어구, 공격 상태 아이콘 렌더링
{
    if (!m_tUnitUIInfo.IsVisible)
        return;
    // 아이콘 위치
    const int ICON_SIZE = 77;
    const int ICON_START_X = 360;
    const int ICON_START_Y = 540;  // 와이어프레임 + 이름 아래
    const int ICON_SPACING = 10;
    const int TEXT_OFFSET_X = ICON_SIZE * 0.5 - 3;

    if (m_tUnitUIInfo.eRaceType == eRaceType::RACE_PROTOSS ||
        m_tUnitUIInfo.eRaceType == eRaceType::RACE_ZERG)
        return;

    //Armor 아이콘 렌더링
    HDC hArmorDC = CBmpMgr::Get_Instance()->Find_Image(L"ARMOR_ICON");
    if (!hArmorDC)
        return;
    GdiTransparentBlt(hDC,
        ICON_START_X, ICON_START_Y + 1,
        ICON_SIZE * 0.7, ICON_SIZE * 0.7,
        hArmorDC,
        0, 0,
        ICON_SIZE, ICON_SIZE,
        RGB(0, 0, 0));
    // Armor 수치 텍스트
    WCHAR szArmor[16];
    swprintf_s(szArmor, L"%d", CObjMgr::Get_Instance()->GetArmorUpgrade());
    SetTextColor(hDC, RGB(255, 255, 255));
    SetBkMode(hDC, TRANSPARENT);
    TextOut(hDC, ICON_START_X + TEXT_OFFSET_X, 
        ICON_START_Y + TEXT_OFFSET_X + 1
        ,szArmor, lstrlen(szArmor));

    //Attack Icon
    HDC hAttackDC = nullptr;
    switch (m_tUnitUIInfo.eType)
    {
    case eUnitType::SCV:
        hAttackDC = CBmpMgr::Get_Instance()->Find_Image(L"SCV_ATTACK_ICON");
        break;
    case eUnitType::MARINE:
        hAttackDC = CBmpMgr::Get_Instance()->Find_Image(L"MARINE_ATTACK_ICON");
        break;
    case eUnitType::GHOST:  
        hAttackDC = CBmpMgr::Get_Instance()->Find_Image(L"GHOST_ATTACK_ICON");
        break;
    case eUnitType::FIREBAT:
        hAttackDC = CBmpMgr::Get_Instance()->Find_Image(L"FIREBAT_ATTACK_ICON");
        break;
    case eUnitType::VULTURE:
        hAttackDC = CBmpMgr::Get_Instance()->Find_Image(L"VULTURE_ATTACK_ICON");
        break;
    case eUnitType::TANK:
        hAttackDC = CBmpMgr::Get_Instance()->Find_Image(L"TANK_ATTACK_ICON");
        break;
    case eUnitType::SIEGE_TANK:
        hAttackDC = CBmpMgr::Get_Instance()->Find_Image(L"SIEGE_ATTACK_ICON");
        break;
    case eUnitType::GOLIATH:
        hAttackDC = CBmpMgr::Get_Instance()->Find_Image(L"VULTURE_ATTACK_ICON");
        break;
    case eUnitType::BATTLECRUISER:
        hAttackDC = CBmpMgr::Get_Instance()->Find_Image(L"BATTLECRUISER_ATTACK_ICON");
        break;
    default:
        return;  // 알 수 없는 타입은 렌더링하지 않음
    }
    // 2. Attack 아이콘 + 수치
    if (hAttackDC)
    {
        GdiTransparentBlt(hDC,
            ICON_START_X + 60, ICON_START_Y,
            ICON_SIZE * 0.7, ICON_SIZE * 0.7,
            hAttackDC,
            0, 0,
            ICON_SIZE, ICON_SIZE,
            RGB(0, 0, 0));
    }
    //Attack수치
    WCHAR szAttack[16];
    swprintf_s(szAttack, L"%d", CObjMgr::Get_Instance()->GetAttackUpgrade());
    TextOut(hDC, 
        ICON_START_X + TEXT_OFFSET_X + 60, 
        ICON_START_Y + TEXT_OFFSET_X + 1
        ,szArmor, lstrlen(szArmor));
}

void CMainUI::RenderUnitWire(HDC hDC)
{
    if (!m_tUnitUIInfo.IsVisible)
        return;
    HDC hWireDC = CBmpMgr::Get_Instance()->Find_Image(L"WIRE");
    if (!hWireDC)
        return;
    //유닛 타입에 따른 다른 오프셋 적용하기 
    const int WIRE_CELL_SIZE = 64;
    const int WIRE_COLS = 6;
    //건물 타입에 따른 이미지 인덱스
    int unitRow = -1;

    TCHAR szKey[256];
    CMyPng* pPng = nullptr;

    switch (m_tUnitUIInfo.eType)
    {
    case eUnitType::SCV:
        unitRow = 0;
        break;
    case eUnitType::MARINE:
        unitRow = 1;
        break;
    case eUnitType::MEDIC:
        unitRow = 2;
        break;
    case eUnitType::GHOST:
        unitRow = 3;
        break;
    case eUnitType::FIREBAT:
        unitRow = 4;
        break;
    case eUnitType::VULTURE:
        unitRow = 5;
        break;
    case eUnitType::TANK:
        unitRow = 6;
        break;
    case eUnitType::SIEGE_TANK:
        unitRow = 7;
        break;
    case eUnitType::GOLIATH:
        unitRow = 8;
        break;
    case eUnitType::BATTLECRUISER:
        unitRow = 11;
        break;
    case eUnitType::LAVA:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Lava_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                224, WINCY - 130 + 40,
                iWidth * 0.7, iHeight * 0.7, false);
        }
        return;
    case eUnitType::DRONE:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Drone_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                224, WINCY - 130 + 35,
                iWidth, iHeight, false);
        }
        return;
    case eUnitType::ZERGLING:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Zergling_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                224, WINCY - 130 + 35,
                iWidth, iHeight, false);
        }
        return;
    case eUnitType::HYDRALISK:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Hydralisk_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                224, WINCY - 130 + 35,
                iWidth, iHeight, false);
        }
        return;
    case eUnitType::ULTRALISK:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Ultralisk_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                224, WINCY - 130 + 35,
                iWidth, iHeight, false);
        }
        return;
    case eUnitType::MUTALISK:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Mutalisk_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                224, WINCY - 130 + 35,
                iWidth, iHeight, false);
        }
        return;
    case eUnitType::OVERLOAD:
        //바로 PNG 렌더링
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Overload_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                224, WINCY - 130 + 35,
                iWidth, iHeight, false);
        }
        return;
        //프로토스
    case eUnitType::PROBE:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Probe_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::ZEALOT:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Zealot_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::DRAGON:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Dragoon_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::HIGH_TEMPLAR:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"HighTemplar_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::DARK_TEMPLAR:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"DarkTemplar_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::SHUTTLE:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Shuttle_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::DARK_ARCHON:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"DarkArchon_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::CARRIER:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Carrier_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::CORSAIR:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Corsair_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::ARBITER:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Arbiter_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::REAVOR:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Reaver_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    case eUnitType::SCOUT:
        pPng = CBmpMgr::Get_Instance()->Find_Png(L"Scout_BigWire");
        if (pPng)
        {
            int iWidth = pPng->Get_Width();
            int iHeight = pPng->Get_Height();

            pPng->Render_Alpha(hDC,
                220, WINCY - 130 + 20,
                iWidth, iHeight, false, false);
        }
        return;
    default:
        return;  // 알 수 없는 타입은 렌더링하지 않음
    }

    int healthCol = GetWireColumnByHealth(m_tUnitUIInfo.iHP,
        m_tUnitUIInfo.iMaxHP);

    int srcX = healthCol * WIRE_CELL_SIZE;
    int srcY = unitRow * WIRE_CELL_SIZE;

    const int PANEL_HEIGHT = 130;
    const int PANEL_TOP = WINCY - PANEL_HEIGHT;

    int destX = 210;
    int destY = PANEL_TOP + 30;

    GdiTransparentBlt(hDC,
        destX, destY,
        WIRE_CELL_SIZE, WIRE_CELL_SIZE,
        hWireDC,
        srcX, srcY,
        WIRE_CELL_SIZE, WIRE_CELL_SIZE,
        RGB(0, 0, 0));
}

int CMainUI::GetWireColumnByHealth(int iHP, int iMaxHP)
{
    if (iMaxHP <= 0) return 0;

    float ratio = (float)iHP / (float)iMaxHP;

    if (ratio >= 0.84f)
    {
        return 0;
    }
    else if (ratio >= 0.68f)
    {
        return 1;
    }
    else if (ratio >= 0.54f)
    {
        return 2;
    }
    else if (ratio >= 0.38f)
    {
        return 3;
    }
    else if (ratio >= 0.22f)
    {
        return 4;
    }
    else
    {
        return 5;
    }

    return 0;
}

void CMainUI::SetMultiUnitUIInfo(const MultiUnitUIInfo& info)
{
    m_tMultiUnitUIInfo = info;
}

int CMainUI::GetWireHealthState(int hp, int maxHP)
{
    //체력 상태에 따른 행 인덱스의 변화
    if (maxHP <= 0) return 0;

    float ratio = (float)hp / (float)maxHP;
    if (ratio > 0.66f)
        return 0; //초록
    else if (ratio > 0.33f)
        return 1; //노랑
    else
        return 2; //빨강
}

void CMainUI::RenderMultiUnitWires(HDC hDC)
{
    //8 * 2 그리드로 멀티 유닛 와이어프레임 렌더링
    if (!m_tMultiUnitUIInfo.IsVisible)
        return;

    HDC hFrameDC = CBmpMgr::Get_Instance()->Find_Image(L"TRAIN_FRAME");
    if (!hFrameDC)
        return;

    //프레임 사이즈 
    const int FRAME_SIZE = 42;
    const int FRAME_COLS = 6;

    //와이어 사이즈 
    const int WIRE_SIZE = 32;
    const int WIRE_COLS = 6;

    const int WIRE_OFFSET = (FRAME_SIZE - WIRE_SIZE) * 0.5; //중앙 정렬 오프셋

    //UI 위치 - 셔틀, 벙커 여부에 따른 시작 위치 조정

    int BASE_X, BASE_Y;
    if (m_tMultiUnitUIInfo.IsLoadUnit)
    {
        BASE_X = 320;
        BASE_Y = 525;
    }
    else
    {
        BASE_X = 220;
        BASE_Y = 495;
    }
    //const int BASE_X = 220;  // 시작 X 좌표
    //const int BASE_Y = 495;  // 시작 Y 좌표
    const int GAP_X = 2;     // 와이어 간 가로 간격
    const int GAP_Y = 10;     // 와이어 간 세로 간격
    const int COLS = 6;      // 한 줄에 8개
    const int ROWS = 2;      // 2줄

    //테란
    HDC hWireDC = CBmpMgr::Get_Instance()->Find_Image(L"SMALL_WIRE");
    if (!hWireDC)
        return;
    // 각 유닛 타입별 BigWire.bmp 내 시작 행 인덱스
    // 예시 (실제 비트맵 구조에 맞게 조정 필요)
    const int UNIT_TYPE_ROW_MAP[11] = {
        -1, // NONE
        0,   // SCV (0, 1, 2행)
        1,   // MARINE (3, 4, 5행)
        2,   // MEDIC (6, 7, 8행)
        3,   //GHOST
        4,   //FIREBAT
        5,   // VULTURE (9, 10, 11행)
        6,  // TANK (12, 13, 14행)
        7,  //SIEGETANK
        8,  //GOLIATH
        11  //BATTLECRUISER
    };

   
    unordered_map<eUnitType, const TCHAR*> unitPngMap =
    {
        //프로토스
        {eUnitType::PROBE, L"Probe_SmallWire"},
        {eUnitType::ZEALOT, L"Zealot_SmallWire"},
        {eUnitType::SHUTTLE, L"Shuttle_SmallWire"},
        {eUnitType::ARCHON, L"Archon_SmallWire"},
        {eUnitType::DARK_ARCHON, L"DarkArchon_SmallWire"},
        {eUnitType::DARK_TEMPLAR, L"DarkTemplar_SmallWire"},
        {eUnitType::DRAGON, L"Dragoon_SmallWire"},
        {eUnitType::ARBITER, L"Arbiter_SmallWire"},
        {eUnitType::CARRIER, L"Carrier_SmallWire"},
        //{eUnitType::INTERCEPTOR, L"Interceptor_SmallWire"},
        {eUnitType::CORSAIR, L"Corsair_SmallWire"},
        {eUnitType::HIGH_TEMPLAR, L"HighTemplar_SmallWire"},
        {eUnitType::REAVOR, L"Reaver_SmallWire"},
        {eUnitType::SCOUT, L"Scout_SmallWire"},
        {eUnitType::OBSERVER, L"Observer_SmallWire"},
        //저그
        {eUnitType::DRONE, L"Drone_SmallWire"},
        {eUnitType::LAVA, L"Lava_SmallWire"},
        {eUnitType::ZERGLING, L"Zergling_SmallWire"},
        {eUnitType::OVERLOAD, L"Overload_SmallWire"},
        {eUnitType::HYDRALISK, L"Hydralisk_SmallWire"},
        {eUnitType::MUTALISK, L"Mutalisk_SmallWire"},
        {eUnitType::ULTRALISK, L"Ultralisk_SmallWire"}
    };

    //최대 16개 유닛 카운트
    int displayCount = min(12, m_tMultiUnitUIInfo.iUnitCount);
    for (int i = 0; i < displayCount; ++i)
    {
        const MultiUnitWireInfo& info = m_tMultiUnitUIInfo.units[i];
        //그리드 위치 계산
        int col = i % COLS;
        int row = i / COLS;
        //프레임 그려질 위치
        int frameDestX = BASE_X + col * (FRAME_SIZE + GAP_X);
        int frameDestY = BASE_Y + row * (FRAME_SIZE + GAP_X);

        //유닛 타입에 따른 기본 행 인덱스(bmp wire에 맞춰서 수정)
        int typeIndex = static_cast<int>(info.eType);
        if (typeIndex < 0 || typeIndex >= 11)
            typeIndex = 0;
        int baseRow = UNIT_TYPE_ROW_MAP[typeIndex];
        //체력 상태에 따른 행 오프셋
        int healthOffset = GetWireHealthState(info.iHP, info.iMaxHP);
        int wireRow = baseRow + healthOffset;
        //각 유닛은 6열 중 특정 열 사용
        int wireCol = 0;

        //프레임
        GdiTransparentBlt(hDC,
            frameDestX, frameDestY,
            FRAME_SIZE, FRAME_SIZE,  // 42x42로 그림!
            hFrameDC,
            0, 0,  // 프레임 비트맵은 보통 단일 이미지
            FRAME_SIZE, FRAME_SIZE,
            RGB(255, 0, 255));

        //외이어 그려질 위치
        int wireDestX = frameDestX + WIRE_OFFSET;
        int wireDestY = frameDestY + WIRE_OFFSET;
        //와이어
        int wireSrcX = wireCol * WIRE_SIZE;
        int wireSrcY = wireRow * WIRE_SIZE;

        eRaceType type = m_tMultiUnitUIInfo.units[i].eRaceType;

        if (type == eRaceType::RACE_TERRAN)
        {
            //와이어
            GdiTransparentBlt(hDC,
                wireDestX, wireDestY,
                WIRE_SIZE, WIRE_SIZE,
                hWireDC,
                wireSrcX, wireSrcY,
                WIRE_SIZE, WIRE_SIZE,
                RGB(0, 0, 0));
        }
        else if (type == eRaceType::RACE_PROTOSS || type == eRaceType::RACE_ZERG)
        {
            auto it = unitPngMap.find(m_tMultiUnitUIInfo.units[i].eType);
            if (it == unitPngMap.end())
                continue;
            CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png(it->second);
            if (pPng)
            {
                int iWidth = pPng->Get_Width();
                int iHeight = pPng->Get_Height();

                pPng->Render_Alpha(hDC,
                    wireDestX, wireDestY,
                    iWidth, iHeight, false, false);
            }
        }
    }
}

void CMainUI::RenderShieldBar(HDC hDC)
{

}

void CMainUI::RenderHealthBar(HDC hDC)
{
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    //선택되지 않은 경우 체력바 표시 X
    if (selected.empty())
        return;

    int iHP, iMaxHP;
    int iShield, iMaxShield;
    
    for (CObj* pObj : selected)
    {
        if (!pObj || pObj->IsDead() || !pObj->IsSelectable()) 
            return;
        //유닛, 건물 판단
        CUnit* pUnit = dynamic_cast<CUnit*>(pObj);
        CBuilding* pBuilding = dynamic_cast<CBuilding*>(pObj);

        if (!pUnit && !pBuilding)
            continue;

        if (pUnit)
        {
            iHP = pUnit->Get_HP();
            iMaxHP = pUnit->Get_MaxHP();
            iShield = pUnit->Get_Shield();
            iMaxShield = pUnit->Get_MaxShield();
        }
        else if (pBuilding)
        {
            //건설 중이거나 Ghost 모드일 경우 표시 X
            if (pBuilding->IsGhost()) 
                continue;
            iHP = pBuilding->Get_HP();
            iMaxHP = pBuilding->Get_MaxHP();
            iShield = pBuilding->Get_Shield();
            iMaxShield = pBuilding->Get_MaxShield();
        }
        else 
            continue;

        //체력바 정보
        const int RECT_WIDTH = 4;
        const int RECT_HEIGHT = 5;

        if (iMaxHP <= 0 || iHP < 0) 
            continue;

        int maxTile;

        if (iMaxHP >= 1200)
        {
            maxTile = 40;
        }
        else if (iMaxHP <= 1000 && iMaxHP > 500)
        {
            maxTile = 25;
        }
        else if (iMaxHP <= 500 && iMaxHP >= 300)
        {
            maxTile = 20;
        }
        else if (iMaxHP < 300 && iMaxHP >= 100)
        {
            maxTile = 10;
        }
        else
            maxTile = 8;
        //월드 좌표 가져오기
        Vec2 worldPos = pObj->Get_Pos();
        INFO objInfo = pObj->Get_Info();
        //스크롤 정보
        int scrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
        int scrollY = CScrollMgr::Get_Instance()->Get_ScrollY();
        //월드 좌표 -> 스크린 좌표 변환
        int screenX = (int)(worldPos.fX - scrollX);
        int screenY = (int)(worldPos.fY - scrollY);
        //체력바의 전체 길이 계산
        int barWidth = maxTile * RECT_WIDTH;

        //체력바 중앙을 유닛의 중앙에 맞춤
        const int HP_BAR_X = screenX - (barWidth * 0.5);
        const int HP_BAR_Y = screenY + (int)(objInfo.fCY * 0.5f) + 3; //3픽셀 아래

        //현재 HP 비율 계산
        float fHPRatio = (float)iHP / (float)iMaxHP;
        if (fHPRatio < 0.f) fHPRatio = 0.f;
        if (fHPRatio > 1.f) fHPRatio = 1.f;

        int fillRect = (int)(maxTile * fHPRatio);

        //흰 체력바 렌더링
        HDC hEmptyRectDC = CBmpMgr::Get_Instance()->Find_Image(L"UI_RECT");
        if (hEmptyRectDC)
        {
            for (int i = 0; i < maxTile; ++i)
            {
                int iRectX = HP_BAR_X + (i * RECT_WIDTH);

                BitBlt(hDC, iRectX, HP_BAR_Y,
                    RECT_WIDTH, RECT_HEIGHT,
                    hEmptyRectDC,
                    0, 0, SRCCOPY);
            }
        }
        //HP Ratio에 따른 체력색 사용
        const TCHAR* pHPImageKey = L"HP_RECT";
        if (fHPRatio <= 0.3f)
        {
            pHPImageKey = L"HP_RECT";
        }
        else if (fHPRatio <= 0.6f)
        {
            pHPImageKey = L"HP_RECT";  // 노란색 (60% 이하)
        }
        //HP 타일 렌더링
        HDC hHPDC = CBmpMgr::Get_Instance()->Find_Image(pHPImageKey);
        if (hHPDC)
        {
            for (int i = 0; i < fillRect; ++i)
            {
                int iRectX = HP_BAR_X + (i * RECT_WIDTH);
                BitBlt(hDC, iRectX, HP_BAR_Y,
                    RECT_WIDTH, RECT_HEIGHT,
                    hHPDC,
                    0, 0, SRCCOPY);
            }
        }

        //Shield 바 렌더링
        if (iMaxShield == 0)
            continue;
        
        //Shield바 체력바 위쪽에 맞추기
        const int SHIELD_BAR_X = HP_BAR_X;
        const int SHIELD_BAR_Y = HP_BAR_Y - 6; //6픽셀 위로 배치

        //현재 Shield 비율 계산
        float fShieldRatio = (float)iShield / (float)iMaxShield;

        if (fShieldRatio < 0.f) fShieldRatio = 0.f;
        if (fShieldRatio > 1.f) fShieldRatio = 1.f;

        int fillShieldRect = (int)(maxTile * fShieldRatio);
        //빈 shield바
        if (hEmptyRectDC)
        {
            for (int i = 0; i < maxTile; ++i)
            {
                int iRectX = SHIELD_BAR_X + (i * RECT_WIDTH); // 수정
                BitBlt(hDC, iRectX, SHIELD_BAR_Y,
                    RECT_WIDTH, RECT_HEIGHT,
                    hEmptyRectDC, 0, 0, SRCCOPY);
            }
        }
        //Shield 타일 렌더링
        CMyPng* pShieldRect = CBmpMgr::Get_Instance()->Find_Png(L"Shield_Rect");
        if (pShieldRect)
        {
            for (int i = 0; i < fillShieldRect; ++i)
            {
                int iRectX = SHIELD_BAR_X + (i * RECT_WIDTH);
                int iWidth = pShieldRect->Get_Width();
                int iHeight = pShieldRect->Get_Height();
                pShieldRect->Render_Alpha(hDC, iRectX, SHIELD_BAR_Y,
                    iWidth, iHeight, false, false);
            }
        }
    }
}

void CMainUI::RenderMPBar(HDC hDC)
{
    auto& selected = CSelectionMgr::Get_Instance()->GetSelected();
    //선택되지 않은 경우 체력바 표시 X
    if (selected.empty())
        return;

    int iMP, iMaxMP;

    for (CObj* pObj : selected)
    {
        if (!pObj || pObj->IsDead() || !pObj->IsSelectable())
            return;
        //유닛 판단
        CUnit* pUnit = dynamic_cast<CUnit*>(pObj);

        if (!pUnit)
            continue;

        if (pUnit)
        {
            iMP = pUnit->Get_MP();
            iMaxMP = pUnit->Get_MaxMP();
        }
        else
            continue;

        //체력바 정보
        const int RECT_WIDTH = 4;
        const int RECT_HEIGHT = 5;

        if (iMaxMP <= 0 || iMP < 0)
            continue;

        int maxTile;
        if (iMaxMP >= 500)
        {
            maxTile = 20;
        }
        else if (iMaxMP < 500 && iMaxMP >= 300)
        {
            maxTile = 20;
        }
        else
        {
            maxTile = 10;
        }
        //월드 좌표 가져오기
        Vec2 worldPos = pObj->Get_Pos();
        INFO objInfo = pObj->Get_Info();
        //스크롤 정보
        int scrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
        int scrollY = CScrollMgr::Get_Instance()->Get_ScrollY();
        //월드 좌표 -> 스크린 좌표 변환
        int srceenX = (int)(worldPos.fX - scrollX);
        int srceenY = (int)(worldPos.fY - scrollY);
        //체력바의 전체 길이 계산
        int barWidth = maxTile * RECT_WIDTH;
        //체력바 중앙을 유닛의 중앙에 맞춤
        const int MP_BAR_X = srceenX - (barWidth * 0.5);
        const int MP_BAR_Y = srceenY + (int)(objInfo.fCY * 0.5f) + 8; //6픽셀 아래

        //현재 HP 비율 계산
        float fMPRatio = (float)iMP / (float)iMaxMP;
        if (fMPRatio < 0.f) fMPRatio = 0.f;
        if (fMPRatio > 1.f) fMPRatio = 1.f;

        int fillRect = (int)(maxTile * fMPRatio);

        //흰 체력바 렌더링
        HDC hEmptyRectDC = CBmpMgr::Get_Instance()->Find_Image(L"UI_RECT");
        if (hEmptyRectDC)
        {
            for (int i = 0; i < maxTile; ++i)
            {
                int iRectX = MP_BAR_X + (i * RECT_WIDTH);

                BitBlt(hDC, iRectX, MP_BAR_Y,
                    RECT_WIDTH, RECT_HEIGHT,
                    hEmptyRectDC,
                    0, 0, SRCCOPY);
            }
        }
        const TCHAR* pMPImageKey = L"MP_RECT";
        //MP 타일 렌더링
        HDC hMPDC = CBmpMgr::Get_Instance()->Find_Image(pMPImageKey);
        if (hMPDC)
        {
            for (int i = 0; i < fillRect; ++i)
            {
                int iRectX = MP_BAR_X + (i * RECT_WIDTH);

                //CBmpMgr::Get_Instance()->Render_Alpha_Simple(
                //    pMPImageKey, hDC ,iRectX, MP_BAR_Y,
                //    RECT_WIDTH, RECT_HEIGHT);

                //BitBlt(hDC, iRectX, MP_BAR_Y,
                //    RECT_WIDTH, RECT_HEIGHT,
                //    hMPDC,
                //    0, 0, SRCCOPY);

                TransparentBlt(hDC,
                    iRectX, MP_BAR_Y,           // 목적지 위치
                    RECT_WIDTH, RECT_HEIGHT,    // 크기
                    hMPDC,                      // 소스 DC
                    0, 0,                       // 소스 시작 위치
                    RECT_WIDTH, RECT_HEIGHT,    // 소스 크기
                    RGB(0, 0, 0));          // 투명 처리할 색상 (마젠타)
            }
        }
        
    }
}

void CMainUI::InitializeMinimap()
{
    HDC hMinimapDC = CBmpMgr::Get_Instance()->Find_Image(L"MINIMAP");
    if (!hMinimapDC)
        return;
    //미니맵 크기 설정
    int minimapWidth = 160;
    int minimapHeight = 160;
    //화면 왼쪽 하단에 위치
    m_dstMinimap.left = 10;
    m_dstMinimap.top = 440;
    m_dstMinimap.right = 10 + minimapWidth;
    m_dstMinimap.bottom = m_dstMinimap.top + minimapHeight;
    //미니맵 비트맵 생성
    m_dcMinimap = CreateCompatibleDC(hMinimapDC);
    m_bmpMinimap = CreateCompatibleBitmap(hMinimapDC, minimapWidth, minimapHeight);
    m_oldMinimap = (HBITMAP)SelectObject(m_dcMinimap, m_bmpMinimap);
    //원본 미니맵 복사
    BitBlt(m_dcMinimap, 0, 0, minimapWidth, minimapHeight,
        hMinimapDC, 0, 0, SRCCOPY);
    //스케일 계산 - 전체맵 크기 4096 * 4096을 미니맵 크기로 계산
    m_fMinimapScale = 25.6f;
    // 7. 카메라 프레임 비트맵 생성 (하얀 사각형)
    m_dcFrame = CreateCompatibleDC(hMinimapDC);
    m_bmpFrame = CreateCompatibleBitmap(hMinimapDC, 4, 4);
    m_oldFrame = (HBITMAP)SelectObject(m_dcFrame, m_bmpFrame);

    // 프레임을 흰색으로 채움
    RECT rc = { 0, 0, 4, 4 };
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(m_dcFrame, &rc, whiteBrush);
    DeleteObject(whiteBrush);
}

void CMainUI::RenderMinimap(HDC hDC)
{
    if (!m_dcMinimap) return;
    int width = m_dstMinimap.right - m_dstMinimap.left;
    int height = m_dstMinimap.bottom - m_dstMinimap.top + 30;
    //원본 미니맵
    BitBlt(hDC, m_dstMinimap.left, m_dstMinimap.top,
        width, height,
        m_dcMinimap, 
        0, 0, SRCCOPY);
    //유닛 건물 초록색 표시
    RenderMinimapUnit(hDC);

    //안개 오버레이
    if (m_dcMinimapFog)
    {
        int minimapWidth = m_dstMinimap.right - m_dstMinimap.left; //160
        int minimapHeight = m_dstMinimap.bottom - m_dstMinimap.top; //160

        BLENDFUNCTION bf{};
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 255;
        bf.AlphaFormat = AC_SRC_ALPHA; //소스 알파 사용

        AlphaBlend(hDC,
            m_dstMinimap.left,
            m_dstMinimap.top,
            minimapWidth,
            minimapHeight,
            m_dcMinimapFog,
            0, 0,
            minimapWidth,
            minimapHeight,
            bf);
    }

    ////안개 오버레이
    //if (m_dcMinimapFog)
    //{
    //    int minimapWidth = m_dstMinimap.right - m_dstMinimap.left; //160
    //    int minimapHeight = m_dstMinimap.bottom - m_dstMinimap.top; //160
    //    GdiTransparentBlt(hDC,
    //        m_dstMinimap.left,
    //        m_dstMinimap.top,
    //        minimapWidth,
    //        minimapHeight,
    //        m_dcMinimapFog,
    //        0, 0,
    //        minimapWidth,
    //        minimapHeight,
    //        RGB(255, 0, 255));  // 마젠타를 투명 키로
    //    /*
    //    BLENDFUNCTION bf;
    //    bf.BlendOp = AC_SRC_OVER;
    //    bf.BlendFlags = 0;
    //    bf.SourceConstantAlpha = 200;  // 투명도 (0~255)
    //    bf.AlphaFormat = 0;

    //    AlphaBlend(hDC,
    //        m_dstMinimap.left,
    //        m_dstMinimap.top,
    //        minimapWidth,
    //        minimapHeight,
    //        m_dcMinimapFog,
    //        0, 0,
    //        minimapWidth,
    //        minimapHeight,
    //        bf);
    //        */
    //}

    //카메라 프레임
    RenderMinimapFrame(hDC);
}

void CMainUI::RenderMinimapUnit(HDC hDC)
{
    CObjMgr* objMgr = CObjMgr::Get_Instance();
    //건물 그리기
    auto& buildings = objMgr->Get_ObjList(OBJ_BUILDING);
    for (auto* pBuilding : buildings)
    {
        CBuilding* building = dynamic_cast<CBuilding*>(pBuilding);
        if (!building || building->IsGhost())
            continue;
        Vec2 worldPos = building->Get_Pos();
        //월드 좌표 -> 미니맵 좌표로 변환
        int minimapX = m_dstMinimap.left + (worldPos.fX / m_fMinimapScale);
        int minimapY = m_dstMinimap.top + (worldPos.fY / m_fMinimapScale);
        // 2x2 초록색 점
        HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
        RECT dotRect = { minimapX - 2, minimapY - 2, minimapX + 2, minimapY + 2 };
        FillRect(hDC, &dotRect, greenBrush);
        DeleteObject(greenBrush);
    }
    //유닛 그리기
    auto& units = objMgr->Get_ObjList(OBJ_UNIT);
    for (auto* obj : units)
    {
        CUnit* unit = dynamic_cast<CUnit*>(obj);
        if (!unit) continue;
        Vec2 worldPos = unit->Get_Pos();
        //월드 좌표 -> 미니맵 좌표로 변환
        int minimapX = m_dstMinimap.left + (worldPos.fX / m_fMinimapScale);
        int minimapY = m_dstMinimap.top + (worldPos.fY / m_fMinimapScale);
        // 2x2 초록색 점
        HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
        RECT dotRect = { minimapX - 1, minimapY - 1, minimapX + 1, minimapY + 1 };
        FillRect(hDC, &dotRect, greenBrush);
        DeleteObject(greenBrush);
        //1 X 1 초록색 점
        //SetPixel(hDC, minimapX, minimapY, RGB(0, 255, 0));
    }
}

void CMainUI::RenderMinimapFrame(HDC hDC)
{
    CScrollMgr* scroll = CScrollMgr::Get_Instance();
    //현재 카메라 위치
    float scrollX = scroll->Get_ScrollX();
    float scrollY = scroll->Get_ScrollY();
    //카메라가 보는 영역의 중심
    float centerX = scrollX + WINCX * 0.5f;
    float centerY = scrollY + WINCY * 0.5f;
    //월드 좌표 / 25.6f 로 미니맵 좌표로 변환
    int minimapX = m_dstMinimap.left + (int)(centerX / m_fMinimapScale);
    int minimapY = m_dstMinimap.top + (int)(centerY / m_fMinimapScale);
    //카메라 영역을 미니맵 스케일로 축소
    int camWidth = (int)WINCX / m_fMinimapScale;
    int camHeight = (int)WINCY / m_fMinimapScale;
    //프레임 위치
    int frameX = minimapX - camWidth / 2; 
    int frameY = minimapY - camHeight / 2;
    // 흰색 펜으로 사각형 테두리만 그리기 (매우 빠름!)
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));

    Rectangle(hDC, frameX, frameY, frameX + camWidth, frameY + camHeight);

    SelectObject(hDC, hOldPen);
    SelectObject(hDC, hOldBrush);
    DeleteObject(hPen);
}

void CMainUI::HandleMinimapClick(POINT mousePos)
{
    //미니맵 영역 클릭 확인
    if (!PtInRect(&m_dstMinimap, mousePos))
        return;

    //화면의 절대 좌표를 미니맵의 내부 좌표로 변환
    //미니맵 왼쪽 위 기준 0, 0 좌표계
    int localX = mousePos.x - m_dstMinimap.left;
    int localY = mousePos.y - m_dstMinimap.top;
    //미니맵 좌표를 월드 좌표로 변환
    //미니맵 좌표 * SCALE = 월드 좌표
    float worldCenterX = localX * m_fMinimapScale;
    float worldCenterY = localY * m_fMinimapScale;

    float targetScrollX = worldCenterX - WINCX * 0.5f;
    float targetScrollY = worldCenterY - WINCY * 0.5f;

    //카메라를 미니맵 -> 월드로 바꾼 좌표로 이동
    CScrollMgr::Get_Instance()->Set_ScrollX((targetScrollX));
    CScrollMgr::Get_Instance()->Set_ScrollY((targetScrollY));
}

void CMainUI::UpdateMinimapFog()
{
    if (!m_dcMinimapFog || !m_pMinimapFogBits)
        return;

    float dt = CTimeMgr::Get_Instance()->GetDT();
    m_fMinimapFogDelay -= dt;
    if (m_fMinimapFogDelay <= 0)
        m_fMinimapFogDelay = MINIMAP_FOG_DELAY;
    else
        return;

    int w = m_dstMinimap.right - m_dstMinimap.left;   //160
    int h = m_dstMinimap.bottom - m_dstMinimap.top;   //160

    DWORD* dst = (DWORD*)m_pMinimapFogBits;

    for (int py = 0; py < h; ++py)
    {
        for (int px = 0; px < w; ++px)
        {
            float worldX = px * m_fMinimapScale;
            float worldY = py * m_fMinimapScale;

            int row = (int)(worldY / TILECY);
            int col = (int)(worldX / TILECX);

            BYTE a = 255; // 기본 UNKNOWN

            if (row >= 0 && row < TILEY && col >= 0 && col < TILEX)
            {
                eFogState state = CFogMgr::Get_Instance()->GetFogState(row, col);
                if (state == eFogState::VISIBLE)      a = 0;
                else if (state == eFogState::EXPLORED)a = m_byMinimapExploredAlpha;
                else                                   a = 255;
            }

            // 검정색 오버레이: RGB=0, Alpha만 세팅
            dst[py * w + px] = (DWORD(a) << 24); // 0xAA000000
        }
    }
}

void CMainUI::RenderResource(HDC hDC)
{
    ResourceState state = CResourceMgr::Get_Instance()->GetState();
    //비트맵 이미지 가져오기
    HDC hETCDC = CBmpMgr::Get_Instance()->Find_Image(L"ETC");
    if (!hETCDC)
        return;
    //위치, 크기 설정
    const int ICON_SIZE = 14;
    const int ICON_OFFSET_X = 80;
    const int TEXT_OFFSET_X = 20;
    const int BASE_X = 500;
    const int BASE_Y = 10;
    // 4. 폰트 및 색상 설정
    HFONT hOldFont = (HFONT)SelectObject(hDC, m_hResourceFont);
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 255, 0));  
    // 5. 미네랄 표시
    GdiTransparentBlt(hDC, BASE_X, BASE_Y, ICON_SIZE, ICON_SIZE,
        hETCDC, 0, 100, ICON_SIZE, ICON_SIZE, RGB(0, 255, 0));
    WCHAR mineralText[32];
    swprintf_s(mineralText, L"%d", state.mineral);
    TextOutW(hDC, BASE_X + TEXT_OFFSET_X, BASE_Y, mineralText, wcslen(mineralText));

    // 6. 가스 표시 (ETC.bmp의 16, 0 위치)
    GdiTransparentBlt(hDC, BASE_X + ICON_OFFSET_X, BASE_Y, ICON_SIZE, ICON_SIZE,
        hETCDC, 15, 100, ICON_SIZE, ICON_SIZE, RGB(0, 255, 0));
    WCHAR gasText[32];
    swprintf_s(gasText, L"%d", state.gas);
    TextOutW(hDC, BASE_X + ICON_OFFSET_X + TEXT_OFFSET_X, BASE_Y, gasText, wcslen(gasText));

    //// 7. 인구수 표시 (ETC.bmp의 32, 0 위치)
    GdiTransparentBlt(hDC, BASE_X + ICON_OFFSET_X * 2, BASE_Y, ICON_SIZE, ICON_SIZE,
        hETCDC, 30, 100, ICON_SIZE, ICON_SIZE, RGB(0, 255, 0));
    WCHAR supplyText[32];
    swprintf_s(supplyText, L"%d/%d", state.supplyUsed, state.supplyCap);
    TextOutW(hDC, BASE_X + (ICON_OFFSET_X + TEXT_OFFSET_X) * 2, 
        BASE_Y, supplyText, wcslen(supplyText));

    // 8. 폰트 복원
    SelectObject(hDC, hOldFont);
}

bool CMainUI::IsInUIArea(POINT pt)
{
    return PtInRect(&m_dstMinimap, pt);
}

bool CMainUI::IsInCommandSlot(POINT pt)
{
    return false;
}

void CMainUI::Release()
{
    Safe_Delete(m_pWinTextButton);

    if (m_hFont)
    {
        DeleteObject(m_hFont);
        m_hFont = nullptr;
    }
    if (m_hResourceFont)
    {
        DeleteObject(m_hResourceFont);
        m_hResourceFont = nullptr;
    }
    if (m_dcPanel)
    {
        SelectObject(m_dcPanel, m_oldPanel);
        DeleteObject(m_bmpPanel); // 알파 비트맵 해제
        DeleteDC(m_dcPanel);
        m_dcPanel = nullptr;
        m_bmpPanel = nullptr;
        m_oldPanel = nullptr;
    }
    //미니맵 정리
    if (m_dcMinimap)
    {
        SelectObject(m_dcMinimap, m_oldMinimap);
        DeleteObject(m_bmpMinimap);
        DeleteDC(m_dcMinimap);
    }

    if (m_dcFrame)
    {
        SelectObject(m_dcFrame, m_oldFrame);
        DeleteObject(m_bmpFrame);
        DeleteDC(m_dcFrame);
    }
    //안개 정리
    if (m_dcMinimapFog)
    {
        SelectObject(m_dcMinimapFog, m_oldMinimapFog);
        DeleteObject(m_bmpMinimapFog);
        DeleteDC(m_dcMinimapFog);
        m_dcMinimapFog = nullptr;
    }
}