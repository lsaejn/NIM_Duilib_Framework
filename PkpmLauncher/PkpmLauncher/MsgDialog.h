#pragma once

#include <string>
#include "ConfigFileManager.h"

/*
有些错误发生在窗口创建之前,此时nimbox无法使用。
*/
namespace MsgBox
{
	//nimbox
	void Warning(HWND wnd, const std::wstring& content, const std::wstring& title);
	void WarningViaID(HWND wnd, const std::wstring& contentID, const std::wstring& titleID);

	//sysbox
	//debugNeeded 需要在配置文件里设置调试模式
	void Show(const std::wstring& content, bool debugNeeded=false);
	void Show(const std::wstring& content, const std::wstring& title, bool debugNeeded = false);
	void ShowViaID(const std::wstring& contentID, const std::wstring& titleID, bool debugNeeded = false);
	void ShowViaIDWithSpecifiedTitle(const std::wstring& contentID, const std::wstring& title, bool debugNeeded = false);
	void ShowViaIDWithSpecifiedCtn(const std::wstring& content, const std::wstring& titleID, bool debugNeeded = false);
}
