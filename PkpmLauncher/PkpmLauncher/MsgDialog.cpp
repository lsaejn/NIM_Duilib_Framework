#include "pch.h"
#include "duilib/Utils/MultiLangSupport.h"

namespace MsgBox
{
	void Warning(HWND wnd, const std::wstring& content, const std::wstring& title)
	{
#ifdef USE_SYS_MESSAGEBOX
		SysWarning(wnd, content, title);
#endif // !USE_SYS_MESSAGEBOX

		nim_comp::ShowMsgBox(wnd, NULL, content, false, title, false, L"OK", false);
	}

	void WarningViaID(HWND wnd, const std::wstring& id_content, const std::wstring& id_title)
	{
		std::wstring content = ui::MutiLanSupport::GetInstance()->GetStringViaID(id_content);
		std::wstring title = ui::MutiLanSupport::GetInstance()->GetStringViaID(id_title); 
		std::wstring confirm = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_OK");
		nim_comp::ShowMsgBox(wnd, NULL, content, false, title, false, confirm, false);
	}

	void Show(const std::wstring& content, bool debugNeeded)
	{
		if(debugNeeded == false)
			AfxMessageBox(content.c_str(), MB_SYSTEMMODAL);
		else
		{
			if(ConfigManager::GetInstance().IsShowMessageBoxOn())
				AfxMessageBox(content.c_str(), MB_SYSTEMMODAL);
		}
	}

	void SysWarning(HWND wnd, const std::wstring& content, const std::wstring& title, bool debugNeeded=false)
	{
		if (debugNeeded == false)
			MessageBox(wnd, content.c_str(), title.c_str(), MB_SYSTEMMODAL);
		else
		{
			if(ConfigManager::GetInstance().IsShowMessageBoxOn())
				MessageBox(wnd, content.c_str(), title.c_str(), MB_SYSTEMMODAL);
		}
	}

	void Show(const std::wstring& content, const std::wstring& title, bool debugNeeded)
	{
		SysWarning(NULL, content, title, debugNeeded);
	}

	void ShowViaID(const std::wstring& contentID, const std::wstring& titleID, bool debugNeeded)
	{
		std::wstring content = ui::MutiLanSupport::GetInstance()->GetStringViaID(contentID);
		std::wstring title = ui::MutiLanSupport::GetInstance()->GetStringViaID(titleID);
		SysWarning(NULL, content, title, debugNeeded);
	}

	void ShowViaIDWithSpecifiedTitle(const std::wstring& contentID, const std::wstring& title, bool debugNeeded)
	{
		std::wstring content = ui::MutiLanSupport::GetInstance()->GetStringViaID(contentID);
		SysWarning(NULL, content, title, debugNeeded);
	}

	void ShowViaIDWithSpecifiedCtn(const std::wstring& content, const std::wstring& titleID, bool debugNeeded)
	{
		std::wstring title = ui::MutiLanSupport::GetInstance()->GetStringViaID(titleID);
		SysWarning(NULL, content, title, debugNeeded);
	}
}