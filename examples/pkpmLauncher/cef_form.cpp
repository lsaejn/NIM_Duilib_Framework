#include "stdafx.h"
#include "cef_form.h"

const std::wstring CefForm::kClassName = L"CEF_Control_Example";

CefForm::CefForm()
{
}

CefForm::~CefForm()
{
}

std::wstring CefForm::GetSkinFolder()
{
	return L"cef";
}

std::wstring CefForm::GetSkinFile()
{
	return L"cef.xml";
}

std::wstring CefForm::GetWindowClassName() const
{
	return kClassName;
}

ui::Control* CefForm::CreateControl(const std::wstring& pstrClass)
{
	// 扫描 XML 发现有名称为 CefControl 的节点，则创建一个 ui::CefControl 控件
	if (pstrClass == L"CefControl")
	{
		if (nim_comp::CefManager::GetInstance()->IsEnableOffsetRender())
			return new nim_comp::CefControl;
		else
			return new nim_comp::CefNativeControl;
	}

	return NULL;
}
#include "windows.h"
void CefForm::InitWindow()
{
	// 监听鼠标单击事件
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefForm::OnClicked, this, std::placeholders::_1));
	//m_pRoot->Setwi
	//m_pRoot->AttachBubbledEvent(ui::kEventMouseDoubleClick, nbase::Bind(&CefForm::OnDbClicked, this, std::placeholders::_1));
	//m_pRoot->AttachAllEvents(nbase::Bind(&CefForm::OnDbClicked, this, std::placeholders::_1));
	// 从 XML 中查找指定控件
	cef_control_		= dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control"));
	cef_control_dev_	= dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control_dev"));
	btn_dev_tool_		= dynamic_cast<ui::Button*>(FindControl(L"btn_dev_tool"));
	edit_url_			= dynamic_cast<ui::RichEdit*>(FindControl(L"edit_url"));

	// 设置输入框样式
	edit_url_->SetSelAllOnFocus(true);
	edit_url_->AttachReturn(nbase::Bind(&CefForm::OnNavigate, this, std::placeholders::_1));

	// 监听页面加载完毕通知
	cef_control_->AttachLoadEnd(nbase::Bind(&CefForm::OnLoadEnd, this, std::placeholders::_1));

	// 打开开发者工具
	cef_control_->AttachDevTools(cef_control_dev_);

	// 加载皮肤目录下的 html 文件
	//cef_control_->LoadURL(nbase::win32::GetCurrentModuleDirectory() + L"resources\\themes\\default\\cef\\cef.html");
	cef_control_->RegisterCppFunc(L"ShowMessageBox", ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
		nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(params), 3000, GetHWND());
		callback(false, R"({ "message": "Success." })");
		}));
	cef_control_->LoadURL(L"C:\\Users\\lsaejn\\Desktop\\HtmlDialog\\HtmlRes\\index.html");

	if (!nim_comp::CefManager::GetInstance()->IsEnableOffsetRender())
		cef_control_dev_->SetVisible(false);

	auto hwnd = GetHWND();
	LONG style = ::GetWindowLong(this->m_hWnd, GWL_EXSTYLE);//获取原窗体的样式
	auto hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	style |= WS_EX_ACCEPTFILES;//更改样式
	hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	::SetWindowLongW(hwnd, GWL_EXSTYLE, style);//重新设置窗体样式
}

LRESULT CefForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	nim_comp::CefManager::GetInstance()->PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

bool CefForm::OnClicked(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();

	if (name == L"btn_dev_tool")
	{
		if (cef_control_->IsAttachedDevTools())
		{
			cef_control_->DettachDevTools();
		}
		else
		{
			cef_control_->AttachDevTools(cef_control_dev_);
		}

		if (nim_comp::CefManager::GetInstance()->IsEnableOffsetRender())
		{
			cef_control_dev_->SetVisible(cef_control_->IsAttachedDevTools());
		}
	}
	else if (name == L"btn_back")
	{
		cef_control_->GoBack();
	}
	else if (name == L"btn_forward")
	{
		cef_control_->GoForward();
	}
	else if (name == L"btn_navigate")
	{
		OnNavigate(nullptr);
	}
	else if (name == L"btn_refresh")
	{
		cef_control_->Refresh();
	}
	else if (name == L"btn_wnd_max")
	{
		return false;
	}
	return true;
}

LRESULT CefForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_LBUTTONDBLCLK)
	{
		OutputDebugString(L"fuck");
	}
	if (uMsg == WM_LBUTTONDBLCLK)
	{
		OutputDebugString(L"WM_NCLBUTTONDBLCLK");
		OutputDebugString(L"this output means ");
		{
			cef_control_->CallJSFunction(L"showJsMessage", nbase::UTF8ToUTF16("{\"msg\":\"fuck\"}"),
				ToWeakCallback([this](const std::string& json_result) {
					nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(json_result), 3000, GetHWND());
				}
			));
		}
	}
	if (uMsg == WM_DROPFILES)
	{
		OutputDebugString(L"fuck WM_DROPFILES");
	}
	if (uMsg == WM_KEYDOWN)
	{
		if ('V' == wParam || 'v' == wParam)
		{
			if (0x80 == (0x80 & GetKeyState(VK_CONTROL)))
			{
				OutputDebugString(L"fuck WM_KEYDOWN");			OutputDebugString(L"fuck WM_KEYDOWN");
			}
		}
	}
	if (uMsg == WM_LBUTTONDOWN)
	{
		OutputDebugString(L"fuck");
	}
	return ui::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

bool CefForm::OnDbClicked(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (msg->Type == ui::kEventMouseDoubleClick)
	{
		int x = 3;
		x++;
	}
	return 1;
}

bool CefForm::OnNavigate(ui::EventArgs* msg)
{
	if (!edit_url_->GetText().empty())
	{
		cef_control_->LoadURL(edit_url_->GetText());
		cef_control_->SetFocus();
	}
	return true;
}

void CefForm::OnLoadEnd(int httpStatusCode)
{
	FindControl(L"btn_back")->SetEnabled(cef_control_->CanGoBack());
	FindControl(L"btn_forward")->SetEnabled(cef_control_->CanGoForward());

	// 注册一个方法提供前端调用
	cef_control_->RegisterCppFunc(L"ShowMessageBox", ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
		nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(params), 3000, GetHWND());
		callback(false, R"({ "message": "Success." })");
	}));
}
