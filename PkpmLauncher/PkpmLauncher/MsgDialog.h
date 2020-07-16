#pragma once

#include <string>

namespace MsgBox
{
	void Warning(HWND wnd, const std::wstring& content, const std::wstring& title);
	void WarningViaID(HWND wnd, const std::wstring& contentID, const std::wstring& titleID);
	void Show(const std::wstring& content, bool debugNeeded=false);
	void ShowViaID(const std::wstring& ID);
	void ShowViaID(const std::wstring& contentID, const std::wstring& titleID);
}
