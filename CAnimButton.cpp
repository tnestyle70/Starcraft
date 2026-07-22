#include "pch.h"
#include "CAnimButton.h"
#include "CBmpMgr.h"
#include "CMyPng.h"
#include "CSceneMgr.h"
#include "CInputMgr.h"
#include "CTimeMgr.h"
#include "CAnimMgr.h"
#include "CSoundMgr.h"
#include "CMenuRace.h"
#include "CGameDataMgr.h"

CAnimButton::CAnimButton()
{
}

CAnimButton::~CAnimButton()
{
    Release();
}

void CAnimButton::Initialize()
{
    m_eRender = RENDER_UI;
    m_tInfo.fCX = 200.f;
    m_tInfo.fCY = 300.f;
}

int CAnimButton::Update()
{
    __super::Update_Rect();
    return 0;
}

void CAnimButton::Late_Update()
{
    //기본 애니메이션 재생
    m_fNormalAnimTime -= CTimeMgr::Get_Instance()->GetDT();
    if (m_fNormalAnimTime < 0)
    {
        m_fNormalAnimTime = m_fFrameDelay;
        m_iNormalCurrentFrame++;
        if (m_iNormalCurrentFrame > m_iNormalFrameCount)
        {
            m_iNormalCurrentFrame = 0;
        }
    }
    POINT ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);
    // 호버 체크
    m_bHover = PtInRect(&m_tRect, ptMouse);

    if (m_bHover)
    {
        //애니메이션 업데이트
        m_fAnimTime -= CTimeMgr::Get_Instance()->GetDT();
        if (m_fAnimTime < 0)
        {
            m_fAnimTime = m_fFrameDelay;
            m_iCurrentFrame++;
            if (m_iCurrentFrame > m_iFrameCount)
            {
                m_iCurrentFrame = 0;
            }
        }
        //호버링 상태에서 LEFT_MOUSE 눌렀을 경우 Stage로 이동
        if (CInputMgr::Get_Instance()->KeyPress(LEFT_MOUSE))
        {
            if (!lstrcmp(L"Start", m_pButtonType))
            {
                CSoundMgr::Get_Instance()->PlaySound(L"Mouse/MouseDown.wav", SOUND_BUTTON, 0.4f);
                m_pMenuRace->SetStart(true);
            }
            else if (!lstrcmp(L"Terran", m_pButtonType))
            {
                CSoundMgr::Get_Instance()->PlaySound(L"Mouse/MouseDown.wav", SOUND_BUTTON, 0.4f);
                CGameDataMgr::Get_Instance()->Set_PlayerRace(eRaceType::RACE_TERRAN);
                m_pMenuRace->SetStart(true);
            }
            else if (!lstrcmp(L"Protoss", m_pButtonType))
            {
                CSoundMgr::Get_Instance()->PlaySound(L"Mouse/MouseDown.wav", SOUND_BUTTON, 0.4f);
                CGameDataMgr::Get_Instance()->Set_PlayerRace(eRaceType::RACE_PROTOSS);
                m_pMenuRace->SetStart(true);
            }
            else if (!lstrcmp(L"Zerg", m_pButtonType))
            {
                CSoundMgr::Get_Instance()->PlaySound(L"Mouse/MouseDown.wav", SOUND_BUTTON, 0.4f);
                CGameDataMgr::Get_Instance()->Set_PlayerRace(eRaceType::RACE_ZERG);
                m_pMenuRace->SetStart(true);
            }
            else if (!lstrcmp(L"Menu_Race", m_pButtonType))
            {
                CSoundMgr::Get_Instance()->PlaySound(L"Mouse/MouseDown.wav", SOUND_BUTTON, 0.4f);
                CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_MENU_RACE);
            }
            else if (!lstrcmp(L"Edit", m_pButtonType))
            {
                CSoundMgr::Get_Instance()->PlaySound(L"Mouse/MouseDown.wav", SOUND_BUTTON, 0.4f);
                CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_EDIT);
            }
            else if (!lstrcmp(L"Exit", m_pButtonType))
            {
                CSoundMgr::Get_Instance()->PlaySound(L"Mouse/MouseDown.wav", SOUND_BUTTON, 0.4f);
                DestroyWindow(g_hWnd);
            }
        }
    }
    else
    {
        m_iCurrentFrame = 0;
        m_fAnimTime = 0.f;
    }
}

void CAnimButton::Render(HDC hDC)
{
    //기본 애니메이션 재생
    TCHAR szNormalKey[256];
    wsprintf(szNormalKey, m_pNormalAnimKey, m_iNormalCurrentFrame);
    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szNormalKey);
    if (pPng)
    {
        pPng->Render_Alpha(hDC, m_tRect.left, m_tRect.top);
    }
    //호버링시 애니메이션 재생

    if (m_bHover)
    {
        TCHAR szHoverKey[256];
        wsprintf(szHoverKey, m_pAnimBaseKey, m_iCurrentFrame);
        pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(szHoverKey);
        if (pPng)
        {
            pPng->Render_Alpha(hDC, m_tRect.left, m_tRect.top);
        }
    }
}

void CAnimButton::Release()
{
}

void CAnimButton::Set_NormalImage(const TCHAR* pNormalKey)
{
    m_pNormalKey = pNormalKey;

    // Normal 이미지 크기로 버튼 크기 설정
    CMyPng* pPng = CBmpMgr::Get_Instance()->Find_Png_Anim(pNormalKey);
    if (pPng)
    {
        m_tInfo.fCX = (float)pPng->Get_Width();
        m_tInfo.fCY = (float)pPng->Get_Height();
    }
}

void CAnimButton::Set_NormalAnimation(const TCHAR* pBaseKey, int iFrameCount)
{
    m_pNormalAnimKey = pBaseKey;
    m_iNormalFrameCount = iFrameCount;
    m_iNormalCurrentFrame = 0;  
    m_fNormalAnimTime = 0.f;
}

void CAnimButton::Set_HoverAnimation(const TCHAR* pBaseKey, int iFrameCount)
{
    m_pAnimBaseKey = pBaseKey;
    m_iFrameCount = iFrameCount;
    m_iCurrentFrame = 0;  
    m_fAnimTime = 0.f;
}