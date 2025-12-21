#pragma once
class CMainUI;

class CUIMgr
{
private:
    CUIMgr();
    ~CUIMgr();

public:
    static CUIMgr* Get_Instance();
    static void Destroy_Instance();

    void Initialize();
    void Release();

   // void Update();
    void Render(HDC hDC);

private:
    static CUIMgr* m_pInstance;

    CMainUI* m_pMainUI = nullptr;
};