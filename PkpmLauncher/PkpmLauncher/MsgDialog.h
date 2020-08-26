#pragma once

#include <string>

/*
有些错误发生在窗口创建之前,此时nimbox无法使用。
*/
namespace MsgBox
{
	//nimbox
	void Warning(HWND wnd, const std::wstring& content, const std::wstring& title);
	void WarningViaID(HWND wnd, const std::wstring& contentID, const std::wstring& titleID);

	//sysbox
	void Show(const std::wstring& content, bool debugNeeded=false);
	void Show(const std::wstring& content, const std::wstring& title);
	void ShowViaID(const std::wstring& contentID);
	void ShowViaID(const std::wstring& contentID, const std::wstring& titleID);
	void ShowViaIDWithSpecifiedTitle(const std::wstring& contentID, const std::wstring& title);
	void ShowViaIDWithSpecifiedCtn(const std::wstring& content, const std::wstring& titleID);
}
