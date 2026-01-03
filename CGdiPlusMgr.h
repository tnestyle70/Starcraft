#pragma once

// GDI+ 초기화 및 종료를 관리하는 클래스
class CGdiPlusMgr
{
private:
    CGdiPlusMgr();
    ~CGdiPlusMgr();

public:
    static CGdiPlusMgr* Get_Instance()
    {
        if (!m_pInstance)
            m_pInstance = new CGdiPlusMgr;

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
    static CGdiPlusMgr* m_pInstance;
    ULONG_PTR m_gdiplusToken;
};

