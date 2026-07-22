#pragma once
#include "Define.h"

class CCameraMgr
{
private:
	CCameraMgr() {};
	~CCameraMgr() {};
public:
	void Initialize();
	void Update();
public:
	static CCameraMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CCameraMgr;
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
	static CCameraMgr* m_pInstance;
};