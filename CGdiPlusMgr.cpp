#include "pch.h"
#include "CGdiPlusMgr.h"

CGdiPlusMgr* CGdiPlusMgr::m_pInstance = nullptr;

CGdiPlusMgr::CGdiPlusMgr()
    : m_gdiplusToken(0)
{
    // GDI+ 초기화
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CGdiPlusMgr::~CGdiPlusMgr()
{
    // GDI+ 종료
    if (m_gdiplusToken)
    {
        GdiplusShutdown(m_gdiplusToken);
        m_gdiplusToken = 0;
    }
}
