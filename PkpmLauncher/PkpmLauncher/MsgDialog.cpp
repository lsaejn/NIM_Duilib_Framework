#include "pch.h"
#include "MsgDialog.h"

namespace MsgBox
{
	void SysWarning(HWND wnd, const std::wstring& content, const std::wstring& title)
	{
		MessageBox(wnd, content.c_str(), title.c_str(), MB_SYSTEMMODAL);
	}

	void Warning(HWND wnd, const std::wstring& content, const std::wstring& title)
	{
#ifdef USE_SYS_MESSAGEBOX
		SysWarning(wnd, content, title);
#endif // !USE_SYS_MESSAGEBOX

		nim_comp::ShowMsgBox(wnd, NULL, content, false, title, false, L"È·¶¨", false);
	}

	void Show(const std::wstring& content, bool debugNeeded)
	{
		if(debugNeeded == false)
			AfxMessageBox(content.c_str(), MB_SYSTEMMODAL);
		else
		{
#ifdef DEBUG
			AfxMessageBox(content.c_str(), MB_SYSTEMMODAL);
#endif // DEBUG
		}
	}
}