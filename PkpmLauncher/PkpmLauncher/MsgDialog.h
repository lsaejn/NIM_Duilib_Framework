#pragma once

#include <string>

namespace MsgBox
{
	void Warning(HWND wnd, const std::wstring& content, const std::wstring& title);
	void Show(const std::wstring& content, bool debugNeeded=false);
	void Show(const std::wstring& content, const std::wstring& title);

	void WarningViaID(HWND wnd, const std::wstring& contentID, const std::wstring& titleID);
	void ShowViaID(const std::wstring& contentID);
	void ShowViaID(const std::wstring& contentID, const std::wstring& titleID);
	void ShowViaIDWithSpecifiedTitle(const std::wstring& contentID, const std::wstring& title);
	void ShowViaIDWithSpecifiedCtn(const std::wstring& content, const std::wstring& titleID);
}
