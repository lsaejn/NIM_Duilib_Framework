#include "pch.h"
#include "circleDialog.h"

namespace nim_comp {

	using namespace ui;

	CircleBox* ShowCircleBox(HWND hwnd, MsgboxCallback cb,const std::wstring& title, bool title_is_id)
	{
		CircleBox* msgbox = new CircleBox;
		HWND hWnd = msgbox->Create(hwnd, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
		if (hWnd == NULL)
			return NULL;
		MutiLanSupport* multilan = MutiLanSupport::GetInstance();
		msgbox->SetTitle(title_is_id ? multilan->GetStringViaID(title) : title);
		msgbox->Show(hwnd, cb);
		return msgbox;
	}

	const LPCTSTR CircleBox::kClassName = L"CircleBox";

	CircleBox::CircleBox()
	{
	}

	CircleBox::~CircleBox()
	{
	}

	std::wstring CircleBox::GetSkinFolder()
	{
		return L"msgbox";
	}

	std::wstring CircleBox::GetSkinFile()
	{
		return L"circle.xml";
	}

	std::wstring CircleBox::GetWindowClassName() const
	{
		return kClassName;
	}

	std::wstring CircleBox::GetWindowId() const
	{
		return kClassName;
	}

	UINT CircleBox::GetClassStyle() const
	{
		return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
	}

	LRESULT CircleBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	void CircleBox::OnEsc(BOOL& bHandled)
	{
		bHandled = TRUE;
		EndMsgBox(MB_NO);
	}
	void CircleBox::Close(UINT nRet)
	{
		// 提示框关闭之前先Enable父窗口，防止父窗口隐到后面去。
		HWND hWndParent = GetWindowOwner(m_hWnd);
		if (hWndParent)
		{
			::EnableWindow(hWndParent, TRUE);
			::SetFocus(hWndParent);
		}

		__super::Close(nRet);
	}

	void CircleBox::InitWindow()
	{
		m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CircleBox::OnClicked, this, std::placeholders::_1));

		title_ = (Label*)FindControl(L"title");
		nbase::ThreadManager::PostDelayedTask(nbase::Bind(&CircleBox::EndMsgBox, this, MB_YES), nbase::TimeDelta::FromSeconds(3));
	}

	bool CircleBox::OnClicked(ui::EventArgs* msg)
	{
		std::wstring name = msg->pSender->GetName();
		if (name == L"closebtn")
		{
			EndMsgBox(MB_NO);
		}
		return true;
	}

	void CircleBox::SetTitle(const std::wstring& str)
	{
		title_->SetText(str);
	}

	void CircleBox::Show(HWND hwnd, MsgboxCallback cb)
	{
		msgbox_callback_ = cb;

		::EnableWindow(hwnd, FALSE);
		CenterWindow();
		ShowWindow(true);
	}

	void CircleBox::EndMsgBox(MsgBoxRet ret)
	{
		this->Close(0);

		if (msgbox_callback_)
		{
			nbase::ThreadManager::PostTask(0, nbase::Bind(msgbox_callback_, ret));
		}
	}

}
