#pragma once
/********************************************************
Alime
Developer: lsaejn
Date:2020/04/17

功能被放入processIterator和windowIterator。
Fix me,等待发盘后删除此文件。
********************************************************/
#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <Psapi.h>

#include <string>
#include <vector>

#pragma comment (lib,"Psapi.lib")
///win32

struct ProcessInfo
{
    HWND hWnd;
    DWORD pid;
    DWORD tid;
    std::wstring className;
    std::wstring windowText;
    std::wstring exeName;
    bool isVisible;
    ProcessInfo()
        :hWnd(NULL),
        pid(0),
        tid(0),
        isVisible(false)
    {
    }
};


namespace ProcessInspector
{
    using constStrRef = const std::wstring&;
    using Pid = DWORD;
    ///获取当前进程PID
    Pid GetCurrentPid();
    ///获取当前进程HANDLE
    HANDLE GetCurrentHandle();
    ///类名
    std::vector<Pid> GetPidByClassNameOfWindow(constStrRef className);
    ///标题
    std::vector<Pid> GetPidByWindowTextOfWindow(constStrRef windowText);
    ///联合查找
    Pid GetPidOfWindow(constStrRef className, constStrRef windowText);
    ///获取进程的子进程ID
    std::vector<Pid> GetPidsOfChildProcess(Pid pid);
    ///根据进程id获得句柄
    HANDLE GetHandleFromPid(Pid pid);
    ///根据进程句柄获得pid
    Pid GetPidFromHandle(HANDLE handle);
    ///暴力结束一个进程
    void KillProcess(HANDLE handle);
    void KillProcess(Pid pid);
    ///根据pid获得窗口。
    std::vector<HWND> FindHwndByPid(const Pid pid);

    Pid GetPidFromHwnd(HWND hwnd);

    ProcessInfo GetProcessInfo(Pid processID);
    
    bool GetProcessName(Pid processID, wchar_t* buffer);
};

