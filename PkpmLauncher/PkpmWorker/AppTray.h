#pragma once

#include "Alime/NonCopyable.h"
#include "templates.h"
#include "windows.h"
#include "shellapi.h"

/// <summary>
/// �������̿��Ա���һЩ����--���������ں�̨��ʾ�������������
/// ȷ��ֻ������һ��
/// </summary>


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
		//fix me, �ַ�����Ҫ���ʻ�
		MessageBox(NULL,L"����������", L"����", MB_SYSTEMMODAL);
		return 1;
	}

	~AppTray()
	{
		Shell_NotifyIcon(NIM_DELETE, &notifyIcon_);
		//MessageBox(NULL, L"deleted", L"test", 1);
	}
};

