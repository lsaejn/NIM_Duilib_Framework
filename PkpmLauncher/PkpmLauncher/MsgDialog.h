#pragma once

#include <string>
#include "ConfigFileManager.h"

/*
��Щ�������ڴ��ڴ���֮ǰ,��ʱnimbox�޷�ʹ�á�
*/
namespace MsgBox
{
	//nimbox
	void Warning(HWND wnd, const std::wstring& content, const std::wstring& title);
	void WarningViaID(HWND wnd, const std::wstring& contentID, const std::wstring& titleID);

	//sysbox
	//debugNeeded ��Ҫ�������ļ������õ���ģʽ
	void Show(const std::wstring& content, bool debugNeeded=false);
	void Show(const std::wstring& content, const std::wstring& title, bool debugNeeded = false);
	void ShowViaID(const std::wstring& contentID, const std::wstring& titleID, bool debugNeeded = false);
	void ShowViaIDWithSpecifiedTitle(const std::wstring& contentID, const std::wstring& title, bool debugNeeded = false);
	void ShowViaIDWithSpecifiedCtn(const std::wstring& content, const std::wstring& titleID, bool debugNeeded = false);
}
