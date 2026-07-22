#include "pch.h"
#include "framework.h"
#include "Starcraft.h"
#include "CMainGame.h"
#include "CTimeMgr.h"
#include <WindowsX.h>
#include "CGdiPlusMgr.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HWND g_hWnd;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//디버깅
static void ShowLastErrorBox(const wchar_t* where)
{
    DWORD err = GetLastError();

    wchar_t* msg = nullptr;
    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, err, 0, (LPWSTR)&msg, 0, nullptr);

    wchar_t buf[1024];
    wsprintfW(buf, L"%s failed.\nGetLastError = %lu\n%s", where, err, msg ? msg : L"(no message)");
    MessageBoxW(nullptr, buf, L"Win32 Error", MB_OK | MB_ICONERROR);

    if (msg) LocalFree(msg);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_STARCRAFT, szWindowClass, MAX_LOADSTRING);
    //MyRegisterClass(hInstance);

    // 리소스(STRINGTABLE) 꼬였을 때 임시 우회: 하드코딩
    lstrcpyW(szTitle, L"Starcraft");
    lstrcpyW(szWindowClass, L"StarcraftWindowClass");

    ATOM atom = MyRegisterClass(hInstance);
    if (atom == 0) { ShowLastErrorBox(L"RegisterClassExW"); return 0; }

    if (!InitInstance(hInstance, nCmdShow))
    {
        // InitInstance 내부에서 CreateWindowW 실패 원인을 띄우게 만들 예정
        return 0;
    }

    //// 애플리케이션 초기화를 수행합니다:
    //if (!InitInstance (hInstance, nCmdShow))
    //{
    //    return FALSE;
    //}

    HACCEL hAccelTable = nullptr; // 임시: 가속키 리소스 안 쓰기
    //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_STARCRAFT));

    //Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    //ULONG_PTR gdiplusToken;
    //Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    CGdiPlusMgr::Get_Instance();

    CMainGame game;
    game.Initialize();

    MSG msg;
    msg.message = WM_NULL;

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            CTimeMgr::Get_Instance()->BeginFrame();
            game.Update();
            game.Late_Update();
            game.Render();
        }
    }
    game.Release();

    CGdiPlusMgr::Destroy_Instance();

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STARCRAFT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT rc = { 0, 0, WINCX, WINCY };

   // 화면 크기 가져오기
   int screenWidth = GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = GetSystemMetrics(SM_CYSCREEN);

   // 중앙 좌표 계산
   int x = (screenWidth - WINCX) / 2;
   int y = (screenHeight - WINCY) / 2;

   // 중앙에 배치
   HWND hWnd = CreateWindowW(
       szWindowClass,
       szTitle,
       WS_POPUP,
       x, y,  // ← CW_USEDEFAULT 대신 계산된 좌표
       rc.right - rc.left,
       rc.bottom - rc.top,
       nullptr, nullptr, hInstance, nullptr
   );

   //FPS 나오는 모드
   //AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

   //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, 
   //   rc.right - rc.left, 
   //   rc.bottom - rc.top, 
   //   nullptr, nullptr, hInstance, nullptr);

   //if (!hWnd)
   //{
   //    ShowLastErrorBox(L"CreateWindowW");
   //    return FALSE;
   //}

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_RBUTTONDOWN:
    {
        //POINT ptClient{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        //float fScrX = CScrollMgr::Get_Instance()->Get_ScrollX();
        //float fScrY = CScrollMgr::Get_Instance()->Get_ScrollY();

        //Vec2 vWorld{ ptClient.x + fScrX, ptClient.y + fScrY };
        //CInputMgr::Get_Instance()->SetWorldMouse(vWorld);
        //CCommandMgr::Get_Instance()->IssueMove(vWorld);
        return 0;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
