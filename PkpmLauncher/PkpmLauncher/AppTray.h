#pragma once

#include "Alime/NonCopyable.h"
#include "templates.h"
#include "windows.h"

/*
�������̿��Ա���һЩ����--���������ں�̨��ʾ�������������
ȷ��ֻ������һ��
*/


class AppTray: noncopyable
{
public:
	static AppTray* g_tray;
	NOTIFYICONDATA notifyIcon_;

	void Init(HWND hwnd, WORD ID, UINT msg);

	//fix me, ��Ӧ�÷������ﴦ����Ҫһ��delegate
	static LONG WINAPI UnhandledExceptionForAppTray(PEXCEPTION_POINTERS pExInfo)
	{
		UNUSEDPARAMS(pExInfo);
		Shell_NotifyIcon(NIM_DELETE, &g_tray->notifyIcon_);
		AfxMessageBox(L"����������",MB_SYSTEMMODAL);
		return 1;
	}

	~AppTray()
	{
		Shell_NotifyIcon(NIM_DELETE, &notifyIcon_);
		//MessageBox(NULL, L"deleted", L"test", 1);
	}


};

