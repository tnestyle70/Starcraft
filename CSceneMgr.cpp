#include "pch.h"
#include "CSceneMgr.h"
#include "CObjMgr.h"
#include "CPreLoad.h"

CSceneMgr* CSceneMgr::m_pInstance = nullptr;

CSceneMgr::CSceneMgr() : m_eCurScene(SC_PRELOAD), m_ePreScene(SC_END)
, m_pScene(nullptr), m_bSceneChanged(false)
{
}

CSceneMgr::~CSceneMgr()
{
	Release();
}

void CSceneMgr::Scene_Change(SCENEID eID)
{
	m_eNextScene = eID;
	m_bSceneChanged = true;
}

int CSceneMgr::Update()
{
	if (m_bSceneChanged)
	{
		m_bSceneChanged = false;
		m_eCurScene = m_eNextScene;
		if (m_ePreScene != m_eCurScene)
		{
			if (m_pScene)
			{
				m_pScene->Release();
			}

			switch (m_eCurScene)
			{
			case SC_PRELOAD:
				m_pScene = new CPreLoad;
				break;
			case SC_LOGO:
				m_pScene = new CLogo;
				break;
			case SC_MENU:
				m_pScene = new CMenu;
				break;
			case SC_MENU_RACE:
				m_pScene = new CMenuRace;
				break;
			case SC_STAGE:
				m_pScene = new CStage;
				break;
			case SC_EDIT:
				m_pScene = new CEdit;
				break;
			}
			m_pScene->Initialize();
			m_ePreScene = m_eCurScene;
		}
	}

	if (m_pScene)
		m_pScene->Update();

	return 0;
}

void CSceneMgr::Late_Update()
{
	if (m_pScene)
		m_pScene->Late_Update();
}

void CSceneMgr::Render(HDC hDC)
{
	if (m_pScene)
		m_pScene->Render(hDC);
}

void CSceneMgr::Release()
{
	m_pScene->Release();
	Safe_Delete(m_pScene);
}
