// OpenBimWin32.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "OpenBimWin32.h"
#include <string>

#include "windows.h"
#include "Shlwapi.h"
#include "shellApi.h"
#pragma comment(lib, "Shlwapi.lib")

bool CreateProcessWithCommand(const wchar_t* application, const wchar_t* command, HANDLE* process)
{
    PROCESS_INFORMATION pi;
    STARTUPINFOW si;

    memset(&si, 0, sizeof(si));
    si.dwFlags = 0;
    si.cb = sizeof(si);
    si.hStdInput = NULL;
    si.hStdOutput = NULL;
    si.hStdError = NULL;

    wchar_t* command_dup = wcsdup(command);

    if (::CreateProcessW(application,
        command_dup,
        NULL,
        NULL,
        (si.dwFlags & STARTF_USESTDHANDLES) ? TRUE : FALSE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi))
    {
        ::CloseHandle(pi.hThread);
        if (process == NULL)
            ::CloseHandle(pi.hProcess);
        else
            *process = pi.hProcess;
        free(command_dup);
        return true;
    }

    free(command_dup);
    return false;
}

bool FindBimExe(std::wstring& bimPath)
{
    bimPath = L"";
    LPCTSTR cpp_data = L"SOFTWARE\\PKPM\\PKPM-BIMS";
    HKEY hKey = NULL;
    //@根表名称 @要打开的子表项;@固定值-0;@申请的权限;@返回句柄；
    if (ERROR_SUCCESS != RegOpenKeyExW(HKEY_LOCAL_MACHINE, cpp_data, 0, KEY_READ | KEY_WOW64_64KEY, &hKey))
    {
        return false;
    }
    DWORD KeyCnt = 0;
    DWORD KeyMaxLen = 0;
    DWORD NameCnt = 0;
    DWORD NameMaxLen = 0;
    DWORD MaxDateLen = 0;
    if (ERROR_SUCCESS == RegQueryInfoKey(hKey, NULL, NULL, 0, &KeyCnt, &KeyMaxLen, NULL, &NameCnt, &NameMaxLen, &MaxDateLen, NULL, NULL))
    {
        for (DWORD dwIndex = 0; dwIndex < NameCnt; dwIndex++)
        {
            DWORD DateSize = MaxDateLen + 1;
            DWORD NameSize = NameMaxLen + 1;
            DWORD Type = 0;
            wchar_t* szValueName = new wchar_t[NameSize];
            LPBYTE szValueDate = (LPBYTE)malloc(DateSize);
            RegEnumValue(hKey, dwIndex, szValueName, &NameSize, NULL, &Type, szValueDate, &DateSize);
            if (std::wstring(szValueName) == L"PATH" || std::wstring(szValueName) == L"Path")
            {
                bimPath = (wchar_t*)(szValueDate);
                bimPath += L"PBBim.exe";
                delete szValueName;
                if (!PathFileExists(bimPath.c_str()))
                {
                    bimPath.clear();
                    return false;
                }
                break;
            }
            delete szValueName;
        }
    }
    return !bimPath.empty();
}

//懒得改了
bool OpenBimExe(const std::wstring& bimPath)
{
    if (!bimPath.empty())
        return CreateProcessWithCommand(bimPath.c_str(), NULL, NULL);
    return false;
}

void DoOpenBim()
{
    std::wstring bimPath;
    if (FindBimExe(bimPath) && OpenBimExe(bimPath))
        ;
    else
    {
        ShellExecuteW(NULL, L"open", L"https://www.pkpm.cn/index.php?m=content&c=index&a=lists&catid=69", NULL, NULL, SW_SHOW);
    }
}

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OPENBIMWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    DoOpenBim();
    // 执行应用程序初始化:
    //if (!InitInstance (hInstance, nCmdShow))
    //{
    //    return FALSE;
    //}

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENBIMWIN32));

    MSG msg;

    // 主消息循环:
    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    //    {
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }
    //}

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OPENBIMWIN32));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OPENBIMWIN32);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
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
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
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

// “关于”框的消息处理程序。
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
