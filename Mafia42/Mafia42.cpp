#include "pch.h"
#include "framework.h"
#include "Mafia42.h"
#include "MessageManager.h"
#include "ThreadManager.h"
#include "ClientIocpManager.h"
#include "Game.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    Game game;
    game.Init(g_hWnd);

    MSG msg = {};
    uint64 prevTick = 0;


    // 기본 메시지 루프입니다:
    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            uint64 now = ::GetTickCount64();

            game.Update();
            game.Render();

            prevTick = now;
            
        }
    }

    GThreadManager->Join();

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAFIA42));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = L"Mafia42";
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.


   
   RECT windowRect = { 0,0,WINSIZEX,WINSIZEY};
   ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false); // 메뉴바를 창 크기 안에 넣지 않겠습니다.

   HWND hWnd = CreateWindowW(L"Mafia42", L"Mafia42", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, WINSIZEX, WINSIZEY, nullptr, nullptr, hInstance, nullptr);

   g_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
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
        GET_SINGLE(ClientIocpManager)->End();
        break;

        // TODO : Client의 WorkerThread에서 Chat패킷을 받으면 PostMessage로 인해 Wndproc을 호출하게 됩니다. 여기서 Message 처리 진행
    case WM_USER_CHAT_RECV:
    {
        // 해당 case가 호출되는 시점은 모든 싱글톤 Manager들이 Init이 된 후이므로 Instance를 사용해도 보장받을 수 있는 시점입니다.
        const char* tempBuffer = (const char*)lParam;
        GET_SINGLE(MessageManager)->PushMessage(tempBuffer);

        // 그리고 채팅창까지 한 칸 올려야됩니다.
        GET_SINGLE(MessageManager)->Update();

        delete[] tempBuffer;
        tempBuffer = nullptr;
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}