#pragma once

const int WM_TEST = WM_USER + 1;
/*
����ķ�������Ϊ����Ĺ������д���mfc�Ի��򡣶�����û��mfcѭ����
���߳��￪�Ի��򣬿���ʡȥ��װ��̬��Ĺ�������
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

