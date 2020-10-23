#pragma once

#include "Alime/NonCopyable.h"
#include "windows.h"

/*
出现很难重现的现象——启动界面显示不出来的情况，
可能是消息无法入队？
增加托盘可以避免一些尴尬。
*/
class AppTray: noncopyable
{
public:
	NOTIFYICONDATA notifyIcon_;

	void Init(HWND hwnd, WORD ID, UINT msg)
	{
		notifyIcon_.cbSize = sizeof(NOTIFYICONDATA);
		notifyIcon_.hIcon = (HICON)::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(ID), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR | LR_SHARED);
		notifyIcon_.hWnd = hwnd;

		lstrcpy(notifyIcon_.szTip, L"PKPM启动程序");
		notifyIcon_.uCallbackMessage = msg;
		notifyIcon_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		Shell_NotifyIcon(NIM_ADD, &notifyIcon_);
	}

	~AppTray()
	{
		Shell_NotifyIcon(NIM_DELETE, &notifyIcon_);
		//MessageBox(NULL, L"deleted", L"test", 1);
	}


};
