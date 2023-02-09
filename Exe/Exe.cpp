// Exe.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "pch.h"

#include "Exe.h"
#pragma comment(lib, "Dll.lib")
#include "dll.h"
#include "framework.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst; // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING]; // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING]; // メイン ウィンドウ クラス名

HWND g_hWnd;
const int g_ClientWidth = 260;
const int g_ClientHeight = 40;
HFONT g_hFont1;
HFONT g_hOldFont;
WPARAM g_wp;
LPARAM g_lp;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

void callback(WPARAM wp, LPARAM lp);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EXE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXE));

    SetHook(g_hWnd);
    // SetKeyPressCallBack(callback);

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    ResetHook();

    return (int)msg.wParam;
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_EXE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

    // hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    // CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    DWORD dwStyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME;
    RECT rc = { 0,
        0,
        g_ClientWidth,
        g_ClientHeight };
    AdjustWindowRectEx(&rc, dwStyle, FALSE, 0);
    int nWidth = rc.right - rc.left;
    int nHeight = rc.bottom - rc.top;
    g_hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        dwStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nWidth,
        nHeight,
        NULL, NULL,
        hInstance, NULL);

    if (!g_hWnd) {
        return FALSE;
    }

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    return TRUE;
}

void wm_paint_handler(HWND hWnd, LPCTSTR msg)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    // TODO: HDC を使用する描画コードをここに追加してください...
    g_hOldFont = static_cast<HFONT>(SelectObject(hdc, g_hFont1));
    RECT rc;
    GetClientRect(hWnd, &rc);
    HPEN hpen = (HPEN)GetStockObject(WHITE_PEN);
    SelectObject(hdc, hpen);
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    DrawText(hdc, msg, _tcslen(msg), &rc, DT_LEFT | DT_WORDBREAK);
    EndPaint(hWnd, &ps);
}

void callback(WPARAM w, LPARAM l)
{
    g_wp = w;
    g_lp = l;
    RECT rc = { 0 };
    GetClientRect(g_hWnd, &rc);
    InvalidateRect(g_hWnd, &rc, FALSE);
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message) {
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        // 選択されたメニューの解析:
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    } break;
    case WM_PAINT: {
        TCHAR msg[256] = { 0 };
        _stprintf_s(msg, _T("(´・ω・｀)"));
        wm_paint_handler(hWnd, msg);
    } break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

HFONT MyCreateFont(int nHeight, DWORD dwCharSet, LPCTSTR lpName)
{
    return CreateFont(
        nHeight, 0, 0, 0,
        FW_DONTCARE,
        FALSE, FALSE, FALSE,
        dwCharSet,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        lpName);
}
