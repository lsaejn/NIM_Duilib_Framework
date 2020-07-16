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

	void WarningViaID(HWND wnd, const std::wstring& id_content, const std::wstring& id_title)
	{
		std::wstring content = ui::MutiLanSupport::GetInstance()->GetStringViaID(id_content);
		std::wstring title = ui::MutiLanSupport::GetInstance()->GetStringViaID(id_title);
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

	void ShowViaID(const std::wstring& contentID, const std::wstring& titleID)
	{
		std::wstring content = ui::MutiLanSupport::GetInstance()->GetStringViaID(contentID);
		std::wstring title = ui::MutiLanSupport::GetInstance()->GetStringViaID(titleID);
		SysWarning(NULL, content, title);
	}
}