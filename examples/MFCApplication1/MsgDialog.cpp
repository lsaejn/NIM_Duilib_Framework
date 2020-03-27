#include "pch.h"
#include "MsgDialog.h"

namespace MsgBox
{
	void SysWarning(HWND wnd, const std::wstring& content, const std::wstring& title)
	{
		MessageBox(wnd, content.c_str(), title.c_str(), MB_SYSTEMMODAL);
	}

	void Waring(HWND wnd, const std::wstring& content, const std::wstring& title)
	{
#ifdef USE_SYS_MESSAGEBOX
		SysWarning(wnd, content, title);
#endif // !USE_SYS_MESSAGEBOX

		nim_comp::ShowMsgBox(wnd, NULL, content, false, title, false, L"È·¶¨", false);
	}

	void Show(const std::wstring& content)
	{
		AfxMessageBox(content.c_str(), MB_SYSTEMMODAL);
	}
}