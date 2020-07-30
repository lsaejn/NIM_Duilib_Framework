#pragma once
/********************************************************
Alime
Developer: lsaejn
Date:2020/04/17

���ܱ�����processIterator��windowIterator��
Fix me,�ȴ����̺�ɾ�����ļ���
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
    ///��ȡ��ǰ����PID
    Pid GetCurrentPid();
    ///��ȡ��ǰ����HANDLE
    HANDLE GetCurrentHandle();
    ///����
    std::vector<Pid> GetPidByClassNameOfWindow(constStrRef className);
    ///����
    std::vector<Pid> GetPidByWindowTextOfWindow(constStrRef windowText);
    ///���ϲ���
    Pid GetPidOfWindow(constStrRef className, constStrRef windowText);
    ///��ȡ���̵��ӽ���ID
    std::vector<Pid> GetPidsOfChildProcess(Pid pid);
    ///���ݽ���id��þ��
    HANDLE GetHandleFromPid(Pid pid);
    ///���ݽ��̾�����pid
    Pid GetPidFromHandle(HANDLE handle);
    ///��������һ������
    void KillProcess(HANDLE handle);
    void KillProcess(Pid pid);
    ///����pid��ô��ڡ�
    std::vector<HWND> FindHwndByPid(const Pid pid);

    Pid GetPidFromHwnd(HWND hwnd);

    ProcessInfo GetProcessInfo(Pid processID);
    
    bool GetProcessName(Pid processID, wchar_t* buffer);
};

