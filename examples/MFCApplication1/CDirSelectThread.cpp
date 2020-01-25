#include "pch.h"
#include "CDirSelectThread.h"
#include "D:\\works\\cppTest\\cppTest\\MFCLibrary1\\MyDialog.h"
#include "D:\\works\\cppTest\cppTest\MFCLibrary1\resource.h"
#pragma comment(lib ,"D:\\works\\NimDuilibFramework\\bin\\MFCLibrary1.lib")

IMPLEMENT_DYNCREATE(CDirSelectThread, CWinThread)


CDirSelectThread::CDirSelectThread()
{
}


CDirSelectThread::~CDirSelectThread()
{
}


BEGIN_MESSAGE_MAP(CDirSelectThread, CWinThread)
    ON_THREAD_MESSAGE(WM_TEST, OnTest)
END_MESSAGE_MAP()


BOOL CDirSelectThread::InitInstance()
{
    return TRUE;
}

int CDirSelectThread::Run()
{
    MessageBox(NULL, L"fuck", L"a", 1);
    auto hDll = LoadLibrary(_T("D:\\works\\NimDuilibFramework\\bin\\MFCLibrary1.dll"));// 加载DLL库文件，DLL名称和路径用自己的
    typedef void (*pExport)(void);
    pExport f= (pExport)GetProcAddress(hDll, "fuck");
    f();
    fuck();
    for (int i = 0; i != 10000; ++i)
        OutputDebugString(L"fuckmom\n");
    Notify();
    return 1;
}


int CDirSelectThread::ExitInstance()
{
    return CWinThread::ExitInstance();
}


void CDirSelectThread::OnTest(WPARAM wParam, LPARAM lParam)
{
    ::AfxMessageBox(L"test");
}

void CDirSelectThread::SetQuitEvent(HANDLE event)
{
    event_ = event;
}

void CDirSelectThread::Notify()
{
    SetEvent(event_);
}
