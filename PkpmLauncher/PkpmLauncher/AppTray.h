#pragma once

#include "Alime/NonCopyable.h"
#include "templates.h"
#include "windows.h"

/*
增加托盘可以避免一些尴尬--启动界面在后台显示不出来的情况。
确保只被包含一次
*/


class AppTray: noncopyable
{
public:
	static AppTray* g_tray;
	NOTIFYICONDATA notifyIcon_;

	void Init(HWND hwnd, WORD ID, UINT msg);

	//fix me, 不应该放在这里处理，需要一个delegate
	static LONG WINAPI UnhandledExceptionForAppTray(PEXCEPTION_POINTERS pExInfo)
	{
		UNUSEDPARAMS(pExInfo);
		Shell_NotifyIcon(NIM_DELETE, &g_tray->notifyIcon_);
		AfxMessageBox(L"程序发生错误",MB_SYSTEMMODAL);
		return 1;
	}

	~AppTray()
	{
		Shell_NotifyIcon(NIM_DELETE, &notifyIcon_);
		//MessageBox(NULL, L"deleted", L"test", 1);
	}


};

