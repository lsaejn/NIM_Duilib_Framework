#pragma once

const int WM_TEST = WM_USER + 1;
/*
最初的方案。因为最初的工程里有大量mfc对话框。而我们没有mfc循环。
在线程里开对话框，可以省去封装动态库的工作量。
*/

class CDirSelectThread
	:public CWinThread
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

