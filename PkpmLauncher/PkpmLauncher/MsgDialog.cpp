#include "pch.h"
#include "duilib/Utils/MultiLangSupport.h"

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

		nim_comp::ShowMsgBox(wnd, NULL, content, false, title, false, L"确定", false);
	}

	void WarningViaID(HWND wnd, const std::wstring& id, const std::wstring& title)
	{
		std::wstring content = ui::MutiLanSupport::GetInstance()->GetStringViaID(id);
		nim_comp::ShowMsgBox(wnd, NULL, content, false, title, false, L"确定", false);
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




	void ShowViaID(const std::wstring& id)
	{
		std::wstring content=ui::MutiLanSupport::GetInstance()->GetStringViaID(id);
		AfxMessageBox(content.c_str(), MB_SYSTEMMODAL);
	}
}