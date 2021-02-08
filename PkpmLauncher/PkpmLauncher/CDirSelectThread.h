#pragma once

const int WM_TEST = WM_USER + 1;

/// <summary>
/// MFC对话框线程。在Git记录里有教你如何在线程里使用MFC对话框。
/// 从资料来看，比我当初自己摸索着为PM写的要合理
/// </summary>

class CDirSelectThread : public CWinThread
{
    DECLARE_DYNCREATE(CDirSelectThread)
public:
    virtual BOOL InitInstance();
    virtual int Run();
    virtual int  ExitInstance();
    void SetQuitEvent(HANDLE);
    void Notify();
protected:
    CDirSelectThread();
    virtual ~CDirSelectThread();
protected:
    afx_msg void OnTest(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
private:
    HANDLE event_;
};

