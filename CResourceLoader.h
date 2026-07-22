#pragma once
#include "Define.h"

class CResourceLoader
{
private:
	CResourceLoader() {};
	~CResourceLoader() {};
public:
	static CResourceLoader* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CResourceLoader;
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
public:
	//메뉴 리소스
	void LoadMenuResource();
	void LoadMenuRaceResource();
	//인게임 리소스
	void LoadProtossResource();
	void LoadZergResource();
	void LoadTerranResource();
private:
	static CResourceLoader* m_pInstance;
};