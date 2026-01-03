#pragma once
#include <map>
#include <string>
#include <vector>

class CMyPng;

class CAnimMgr
{
private:
	CAnimMgr();
	~CAnimMgr();
private:
	std::map<wstring, CMyPng*> m_mapAnimFrames;
public:
	static CAnimMgr* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CAnimMgr;
		}
		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	static CAnimMgr* m_pInstance;
};