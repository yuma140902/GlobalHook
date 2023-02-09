#pragma comment(lib, "psapi.lib")
#include "pch.h"

#include <cstdio>
#include <tchar.h>

#include "dll.h"

// グローバル変数をプロセス間で共有する
#pragma comment(linker, "/section:.shareddata,rws")
#pragma data_seg(".shareddata")
HHOOK s_hKeyHook = 0;
// キーコードの送り先のウィンドウハンドル
HWND s_hWnd = 0;
#pragma data_seg()

HINSTANCE hInst;

EXPORT_API_ int SetHook(HWND hWnd)
{
    TCHAR procName[MAX_PATH];
    TCHAR msg[MAX_PATH + 128];
    HANDLE hProcess = GetCurrentProcess();
    GetModuleFileNameEx(hProcess, NULL, procName, MAX_PATH);
    DWORD pid = GetCurrentProcessId();
    _stprintf_s(msg, _T("%s (PID:%d)"), procName, pid);

    MessageBox(NULL, msg, _T("SetWindowsHookEx"), MB_OK);
    s_hKeyHook = SetWindowsHookEx(WH_KEYBOARD, KeyHookProc, hInst, 0);
    if (s_hKeyHook == NULL) {
        // failed
    } else {
        // succeeded
        s_hWnd = hWnd;
    }
    return 0;
}

EXPORT_API_ int ResetHook()
{
    TCHAR procName[MAX_PATH];
    TCHAR msg[MAX_PATH + 128];
    HANDLE hProcess = GetCurrentProcess();
    GetModuleFileNameEx(hProcess, NULL, procName, MAX_PATH);
    DWORD pid = GetCurrentProcessId();
    _stprintf_s(msg, _T("%s (PID:%d)"), procName, pid);

    MessageBox(NULL, msg, _T("UnhookWindowsHookEx"), MB_OK);
    if (UnhookWindowsHookEx(s_hKeyHook) != 0) {
        // succeeded
    } else {
        // failed
    }
    return 0;
}

#define LP_BUTTON_PRESSED(lp) (((lp)&0x80000000) == 0)
#define LP_BUTTON_IS_NORMAL(lp) (((lp)&0x20000000) == 0)

EXPORT_API_ LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wp, LPARAM lp)
{
    if (nCode < 0) {
        return CallNextHookEx(s_hKeyHook, nCode, wp, lp);
    }
    if (nCode == HC_ACTION) {
        TCHAR procName[MAX_PATH];
        TCHAR msg[MAX_PATH + 128];
        HANDLE hProcess = GetCurrentProcess();
        GetModuleFileNameEx(hProcess, NULL, procName, MAX_PATH);
        DWORD pid = GetCurrentProcessId();

        if (LP_BUTTON_PRESSED(lp)) {
            if (LP_BUTTON_IS_NORMAL(lp)) {
                if (wp != VK_RETURN) {
                    _stprintf_s(msg, _T("%c has pressed \r\n\"%s\" (PID:%d)"), (int)wp, procName, pid);
                    MessageBox(NULL, msg, _T("key pressed"), MB_OK);
                    PostMessage(s_hWnd, WM_KEYDOWN, wp, 0);
                }
            } else {
                MessageBox(NULL, _T("a system key has pressed"), _T("key pressed"), MB_OK);
                PostMessage(s_hWnd, WM_KEYDOWN, wp, 0);
            }
        }
    }

    return CallNextHookEx(s_hKeyHook, nCode, wp, lp);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        TCHAR procName[MAX_PATH];
        TCHAR msg[MAX_PATH + 128];
        HANDLE hProcess = GetCurrentProcess();
        GetModuleFileNameEx(hProcess, NULL, procName, MAX_PATH);
        DWORD pid = GetCurrentProcessId();
        _stprintf_s(msg, _T("Attached to \"%s\" (PID:%d)"), procName, pid);
        MessageBox(NULL, msg, _T("DllMain ATTACH"), MB_OK);
        hInst = hModule;
    } break;
    case DLL_PROCESS_DETACH: {
        TCHAR procName[MAX_PATH];
        TCHAR msg[MAX_PATH + 128];
        HANDLE hProcess = GetCurrentProcess();
        GetModuleFileNameEx(hProcess, NULL, procName, MAX_PATH);
        DWORD pid = GetCurrentProcessId();
        _stprintf_s(msg, _T("Detached from \"%s\" (PID:%d)"), procName, pid);
        MessageBox(NULL, msg, _T("DllMain DETACH"), MB_OK);
    } break;
    }
    return TRUE;
}
