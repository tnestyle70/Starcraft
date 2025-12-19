#include "pch.h"
#include "CSceneMgr.h"

CSceneMgr* CSceneMgr::m_pInstance = nullptr;

CSceneMgr::CSceneMgr() : m_eCurScene(SC_LOGO), m_ePreScene(SC_END)
, m_pScene(nullptr)
{
}

CSceneMgr::~CSceneMgr()
{
	Release();
}

void CSceneMgr::Scene_Change(SCENEID eID)
{
	m_eCurScene = eID;

	if (m_ePreScene != m_eCurScene)
	{
		if (m_pScene)
		{
			m_pScene->Release();
			Safe_Delete(m_pScene);
		}

		switch (m_eCurScene)
		{
		case SC_MENU:
			m_pScene = new CMenu;
			break;
		case SC_STAGE:
			m_pScene = new CStage;
			break;
		}
		m_pScene->Initialize();
		m_ePreScene = m_eCurScene;
	}
}

int CSceneMgr::Update()
{
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
	Safe_Delete(m_pScene);
}
