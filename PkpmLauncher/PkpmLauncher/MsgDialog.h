#pragma once

#include <string>

namespace MsgBox
{
	void Warning(HWND wnd, const std::wstring& content, const std::wstring& title);
	void WarningViaID(HWND wnd, const std::wstring& id, const std::wstring& title);
	void Show(const std::wstring& content, bool debugNeeded=false);
	void ShowViaID(const std::wstring& id);
}
