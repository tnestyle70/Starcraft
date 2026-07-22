#include "pch.h"
#include "CMainGame.h"
#include "CSceneMgr.h"
#include "CTimeMgr.h"
#include "CBmpMgr.h"
#include "CInputMgr.h"
#include "CSoundMgr.h"
#include "CResourceLoader.h"

CMainGame::CMainGame()
	: m_dwTime(GetTickCount()), m_iFPS(0)
{
	ZeroMemory(m_szFPS, sizeof(m_szFPS));
}

CMainGame::~CMainGame()
{
	Release();
}

void CMainGame::Initialize()
{
	m_hDC = GetDC(g_hWnd);

	CSceneMgr::Get_Instance()->Scene_Change(CSceneMgr::SC_PRELOAD);
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Image/MyFolder/MenuBack.bmp", L"Back");
	CTimeMgr::Get_Instance()->Initialize();
	CInputMgr::Get_Instance()->Initialize();
	CSoundMgr::Get_Instance()->Initialize();

	//CResourceLoader::Get_Instance()->LoadMenuResource();
	//CResourceLoader::Get_Instance()->LoadMenuRaceResource();
	//CResourceLoader::Get_Instance()->LoadProtossResource();
	//CResourceLoader::Get_Instance()->LoadTerranResource();
	//CResourceLoader::Get_Instance()->LoadZergResource();
}

void CMainGame::Update()
{
	CSceneMgr::Get_Instance()->Update();

	CInputMgr::Get_Instance()->Update();
}

void CMainGame::Late_Update()
{
	CSceneMgr::Get_Instance()->Late_Update();
}

void CMainGame::Render()
{
	++m_iFPS;

	if (m_dwTime + 1000 < GetTickCount())
	{
		swprintf_s(m_szFPS, L"FPS : %d", m_iFPS);
		SetWindowText(g_hWnd, m_szFPS);

		m_iFPS = 0;
		m_dwTime = GetTickCount();
	}

	if (CSceneMgr::SC_LOGO == CSceneMgr::Get_Instance()->Get_SceneID())
	{
		CSceneMgr::Get_Instance()->Render(m_hDC);
		return;
	}

	HDC hBackDC = CBmpMgr::Get_Instance()->Find_Image(L"Back");

	CSceneMgr::Get_Instance()->Render(hBackDC);

	BitBlt(m_hDC,
		0, 0, WINCX, WINCY,
		hBackDC, 0, 0,
		SRCCOPY);
}

void CMainGame::Release()
{
	CSceneMgr::Destroy_Instance();
	CInputMgr::Destroy_Instance();
	CBmpMgr::Destroy_Instance();

	ReleaseDC(g_hWnd, m_hDC);
}
