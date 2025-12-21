#include "pch.h"
#include "CMainUI.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"

CMainUI::CMainUI() {}
CMainUI::~CMainUI() {}

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

    // 2. 패널 DC만 유지 (미리 그려놓기)
    m_dcPanel = CreateCompatibleDC(hUIDC);
    m_bmpPanel = CreateCompatibleBitmap(hUIDC, panelW, panelH);
    m_oldPanel = SelectObject(m_dcPanel, m_bmpPanel);

    // 원본에서 UI 영역만 미리 복사해둠
    BitBlt(m_dcPanel, 0, 0, panelW, panelH, hUIDC, m_srcPanel.left, m_srcPanel.top, SRCCOPY);

    // 폰트 생성 (한 번만)
    m_hFont = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
}

void CMainUI::Render(HDC hDC)
{
    RenderFrame(hDC);
}

void CMainUI::RenderFrame(HDC hDC)
{
    if (!m_dcPanel) return;

    int w = m_srcPanel.right - m_srcPanel.left;
    int h = m_srcPanel.bottom - m_srcPanel.top;

    // 매 프레임 마스크 연산(AND/OR) 대신 TransparentBlt 사용
    // RGB(255, 0, 255) -> 핑크색을 투명하게 처리하여 hDC에 바로 복사
    GdiTransparentBlt(hDC,
        m_dstPanel.left, m_dstPanel.top, w, h,
        m_dcPanel,
        0, 0, w, h,
        RGB(255, 0, 255));
}

void CMainUI::Release()
{
    // 기존 마스크 관련 해제 코드들 삭제하고 m_dcPanel만 깔끔하게 해제
    if (m_hFont) { DeleteObject(m_hFont); m_hFont = nullptr; }
    if (m_dcPanel)
    {
        SelectObject(m_dcPanel, m_oldPanel);
        DeleteObject(m_bmpPanel);
        DeleteDC(m_dcPanel);
        m_dcPanel = nullptr;
    }
}