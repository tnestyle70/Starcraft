#pragma once
#include "CMenu.h"
#include "CStage.h"

class CSceneMgr
{
public:
	enum SCENEID {SC_LOGO, SC_MENU, SC_EDIT, SC_STAGE, SC_END};
private:
	CSceneMgr();
	CSceneMgr(const CSceneMgr& rhs) = delete;
	CSceneMgr& operator=(CSceneMgr& rObj) = delete;
	~CSceneMgr();

public:
	void Scene_Change(SCENEID eID);
	int	 Update();
	void Late_Update();
	void Render(HDC hDC);
	void Release();

public:
	static CSceneMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CSceneMgr;
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
	static CSceneMgr* m_pInstance;

	SCENEID		m_eCurScene;
	SCENEID		m_ePreScene;

	CScene* m_pScene;
};