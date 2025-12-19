#include "pch.h"
#include "CTimeMgr.h"

CTimeMgr* CTimeMgr::m_pInstance = nullptr;

CTimeMgr::CTimeMgr()
{
}

CTimeMgr::~CTimeMgr()
{
}

void CTimeMgr::Initialize()
{
	QueryPerformanceFrequency(&m_liFreq);
	QueryPerformanceCounter(&m_liPrev);
	m_dDT = 0;
	m_dDTClamp = 0.5;
}

void CTimeMgr::BeginFrame()
{
	LARGE_INTEGER liNow;
	QueryPerformanceCounter(&liNow);

	const LONGLONG llTicks = liNow.QuadPart - m_liPrev.QuadPart;
	m_liPrev = liNow;

	double dDT = (double)llTicks / (double)m_liFreq.QuadPart;
	
	if (m_dDT > m_dDTClamp)
	{
		m_dDT = m_dDTClamp;
	}

	m_dDT = dDT;
}
