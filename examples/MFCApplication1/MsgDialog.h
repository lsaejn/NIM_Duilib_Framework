#pragma once

#include <string>
namespace MsgBox
{
	void Waring(HWND wnd, const std::wstring& content, const std::wstring& title);
	void Show(const std::wstring& content);
}
