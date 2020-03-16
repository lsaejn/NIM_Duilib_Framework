#include "pch.h"
#include "CDirSelectThread.h"

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
    //// ����DLL���ļ���DLL���ƺ�·�������Լ���
    auto hDll = LoadLibrary(_T("D:\\works\\NimDuilibFramework\\bin\\MFCLibrary1.dll"));
    typedef void (*pExport)(void);
    pExport f= (pExport)GetProcAddress(hDll, "foo");
    f();
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
