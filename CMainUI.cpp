#include "pch.h"
#include "CMainUI.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CTileMgr.h"
#include "CObjMgr.h"
#include "CBuilding.h"
#include "CScrollMgr.h"

CMainUI* CMainUI::m_pInstance = nullptr;

CMainUI::CMainUI()
    : m_dcPanel(nullptr)
    , m_bmpPanel(nullptr)
    , m_oldPanel(nullptr)
    , m_hFont(nullptr)
{
    ZeroMemory(&m_tProgressInfo, sizeof(ProgressbarInfo));
}

CMainUI::~CMainUI()
{
}

// 알파 비트맵 생성 함수
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
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
    //미니맵 초기화
    InitializeMinimap();
}

void CMainUI::Render(HDC hDC)
{
    RenderFrame(hDC);
    //RenderProgressbar(hDC);
    RenderBuildingInfo(hDC);
    RenderBuildingWire(hDC);
    RenderMinimap(hDC);
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

void CMainUI::SetProgressInfo(const ProgressbarInfo& info)
{
    m_tProgressInfo = info;
}

void CMainUI::RenderProgressbar(HDC hDC)
{
    //visible 체크
    if (!m_tProgressInfo.bIsVisible)
        return;
    if (!m_tProgressInfo.pUnitName)
        return;

    const int BAR_WIDTH = 108;
    const int BAR_HEIGHT = 9;

    const int PANEL_HEIGHT = 233;
    const int PANEL_TOP = WINCY - PANEL_HEIGHT;

    int barX = (WINCX - BAR_WIDTH) / 2;  // Centers it
    int barY = PANEL_TOP + 15;            // 15px from top of panel

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


    // 진행률 렌더링 - 계산 정보는 이미 받아옴
    HDC hRectDC = CBmpMgr::Get_Instance()->Find_Image(L"HP_RECT");

    if (hRectDC)
    {
        const int RECT_WIDTH = 4;   // HP_RECT 하나의 너비
        const int RECT_HEIGHT = 5;  // HP_RECT 높이
        const int TOTAL_RECTS = 27; // 108 / 6 = 18개

        int fillRects = (int)(TOTAL_RECTS * m_tProgressInfo.fProgress);

        // fillRects 개수만큼 타일 그리기
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
    RenderProgressText(hDC, barX, barY);
}

void CMainUI::RenderProgressText(HDC hDC, int barX, int barY)
{
    if (!m_tProgressInfo.bIsVisible)
        return;
    if (!m_tProgressInfo.pUnitName)
        return;

    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(255, 255, 255));

    //유닛 이름 
    RECT textRect;
    textRect.left = barX;
    textRect.top = barY - 18;
    textRect.right = barX + 108;
    textRect.bottom = barY - 2;

    DrawText(hDC, m_tProgressInfo.pUnitName, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 큐 정보
    if (m_tProgressInfo.iQueueCount > 1)
    {
        wchar_t queueText[32];
        swprintf_s(queueText, L"(+%d)", m_tProgressInfo.iQueueCount - 1);

        textRect.top = barY + 9 + 2;
        textRect.bottom = textRect.top + 20;

        SetTextColor(hDC, RGB(200, 200, 200));
        DrawText(hDC, queueText, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
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
    if (m_tBuildingUIInfo.IsProducing)
    {
        RenderCurrentProduction(hDC);
        RenderProductionQueue(hDC);
    }   
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

    SelectObject(hDC, hOldFont);
    SelectObject(hDC, hOldFont);
}

void CMainUI::RenderCurrentProduction(HDC hDC)
{
    if (!m_tBuildingUIInfo.pCurrentUnit)
        return;
    const int PANEL_HEIGHT = 233;
    const int PANEL_TOP = WINCY - PANEL_HEIGHT;
    //현재 생산 중인 유닛 텍스트
    int textY = PANEL_TOP + 65;
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(255, 255, 255));
    RECT textRect;
    textRect.left = WINCX / 2 - 100;
    textRect.top = textY;
    textRect.right = WINCX / 2 + 100;
    textRect.bottom = textY + 18;
    DrawText(hDC, m_tBuildingUIInfo.pCurrentUnit, -1, &textRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    // ProgressBar
    const int BAR_WIDTH = 108;
    const int BAR_HEIGHT = 9;
    int barX = (WINCX - BAR_WIDTH) / 2;
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
    HDC hIconDC = CBmpMgr::Get_Instance()->Find_Image(L"ICON_BUTTON_UI");
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
    // 디버깅 추가
    char buf[256];
    sprintf_s(buf, "마우스: (%d,%d), 미니맵영역: (%d,%d)~(%d,%d)\n",
        mousePos.x, mousePos.y,
        m_dstMinimap.left, m_dstMinimap.top,
        m_dstMinimap.right, m_dstMinimap.bottom);
    OutputDebugStringA(buf);

    if (!PtInRect(&m_dstMinimap, mousePos))
    {
        OutputDebugStringA("-> 미니맵 밖!\n");
        return;
    }

    OutputDebugStringA("-> 미니맵 안! 처리 시작\n");

    int localX = mousePos.x - m_dstMinimap.left;
    int localY = mousePos.y - m_dstMinimap.top;

    sprintf_s(buf, "local: (%d,%d)\n", localX, localY);
    OutputDebugStringA(buf);

    float worldX = localX * m_fMinimapScale;
    float worldY = localY * m_fMinimapScale;

    sprintf_s(buf, "world: (%.1f,%.1f)\n", worldX, worldY);
    OutputDebugStringA(buf);

    CScrollMgr::Get_Instance()->Set_ScrollX(worldX - WINCX * 0.5f);
    CScrollMgr::Get_Instance()->Set_ScrollY(worldY - WINCY * 0.5f);

    sprintf_s(buf, "스크롤 설정 완료: (%.1f,%.1f)\n",
        CScrollMgr::Get_Instance()->Get_ScrollX(),
        CScrollMgr::Get_Instance()->Get_ScrollY());
    OutputDebugStringA(buf);
    /*
    //미니맵 영역 클릭 확인
    if (!PtInRect(&m_dstMinimap, mousePos))
        return;
    //화면의 절대 좌표를 미니맵의 내부 좌표로 변환
    //미니맵 왼쪽 위 기준 0, 0 좌표계
    int localX = mousePos.x - m_dstMinimap.left;
    int localY = mousePos.y - m_dstMinimap.top;
    //미니맵 좌표를 월드 좌표로 변환
    //미니맵 좌표 * SCALE = 월드 좌표
    float worldX = localX * m_fMinimapScale;
    float worldY = localY * m_fMinimapScale;
    //카메라를 미니맵 -> 월드로 바꾼 좌표로 이동
    CScrollMgr::Get_Instance()->Set_ScrollX((worldX - WINCX * 0.5f));
    CScrollMgr::Get_Instance()->Set_ScrollY((worldY - WINCY * 0.5f));
    */
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
    switch (m_tBuildingUIInfo.eType)
    {
    case eBuildingType::COMMAND_CENTER:
        buildingRow = 15;
        break;
    case eBuildingType::BARRACKS:
        buildingRow = 18;
        break;
    case eBuildingType::FACTORY:
        buildingRow = 23;
        break;
    case eBuildingType::STARPORT:
        buildingRow = 24;
        break;
    case eBuildingType::SUPPLY_DEPOT:
        buildingRow = 16;
        break;
    default:
        return;  // 알 수 없는 타입은 렌더링하지 않음
    }

    int healthCol = 0;  

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

void CMainUI::Release()
{
    if (m_hFont)
    {
        DeleteObject(m_hFont);
        m_hFont = nullptr;
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
}