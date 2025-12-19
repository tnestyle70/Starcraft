#pragma once
#include "Define.h"
#include <Windows.h>
#include <algorithm>

class CTimeMgr
{
private:
	CTimeMgr();
	CTimeMgr(const CTimeMgr& rhs) = delete; //복사생성금지
	CTimeMgr& operator=(CTimeMgr& rObj) = delete; //대입 연산으로 인한 복사 금지
	~CTimeMgr();
public:
	void Initialize();
public:
	void BeginFrame();
	double GetDT() const { return m_dDT; }
public:
	static CTimeMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CTimeMgr;
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
	static CTimeMgr* m_pInstance;
	LARGE_INTEGER  m_liFreq{};
	LARGE_INTEGER  m_liPrev{};
	double m_dDT;
	double m_dDTClamp;
};