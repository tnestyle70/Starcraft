#include "pch.h"
#include "CUIMgr.h"
#include "CBmpMgr.h"
#include "CMainUI.h"   // 너가 만든 CMainUI

CUIMgr* CUIMgr::m_pInstance = nullptr;

CUIMgr::CUIMgr() {}
CUIMgr::~CUIMgr() { Release(); }

CUIMgr* CUIMgr::Get_Instance()
{
    if (!m_pInstance) m_pInstance = new CUIMgr;
    return m_pInstance;
}

void CUIMgr::Destroy_Instance()
{
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

void CUIMgr::Initialize()
{
    // MainUI.bmp는 한 번만 로드
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/UI/MainUI.bmp", L"MainUI");

    m_pMainUI = new CMainUI;
    m_pMainUI->Initialize();
}

void CUIMgr::Release()
{
    if (m_pMainUI)
    {
        m_pMainUI->Release();
        delete m_pMainUI;
        m_pMainUI = nullptr;
    }
}
/*
void CUIMgr::Update()
{
    if (m_pMainUI) m_pMainUI->Update();
}
*/
void CUIMgr::Render(HDC hDC)
{
    if (m_pMainUI) m_pMainUI->Render(hDC);
}