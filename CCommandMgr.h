#pragma once
#include "CUnit.h"

class CCommandMgr
{
private:
    CCommandMgr();
    CCommandMgr(const CCommandMgr& rhs) = delete;
    CCommandMgr& operator=(CCommandMgr& rObj) = delete;
    ~CCommandMgr();
public:
	static CCommandMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CCommandMgr;
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
	void IssueMove(Vec2& vTargetMouse);
private:
	static CCommandMgr* m_pInstance;
};