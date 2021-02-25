#include "pch.h"
#include "AppTray.h"
AppTray* AppTray::g_tray = nullptr;

void AppTray::Init(HWND hwnd, WORD ID, UINT msg)
{
	notifyIcon_.cbSize = sizeof(NOTIFYICONDATA);
	notifyIcon_.hIcon = (HICON)::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(ID), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR | LR_SHARED);
	notifyIcon_.hWnd = hwnd;

	lstrcpy(notifyIcon_.szTip, L"PKPMÆô¶¯³ÌÐò");
	notifyIcon_.uCallbackMessage = msg;
	notifyIcon_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &notifyIcon_);
	//assert(g_tray==nullptr);
	g_tray = this;
	SetUnhandledExceptionFilter(UnhandledExceptionForAppTray);
}