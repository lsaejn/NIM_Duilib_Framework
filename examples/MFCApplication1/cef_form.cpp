#include "pch.h"
#include "cef_form.h"
#include "CDirSelectThread.h"

#include <sys/stat.h>
#include <stdlib.h>
#include <tchar.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include "xml2json.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/memorystream.h"
using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::Writer;
using namespace rapidjson;
//#include "CppFuncRegister.h"

namespace //which will write to configFile
{
	//exe为起点
	const std::wstring RelativePathForHtmlRes = L"Ribbon\\HtmlRes\\index.html";
}


const std::wstring CefForm::kClassName = L"CEF_Control_Example";

CefForm::CefForm()
	:maxPrjNum(6),prjPaths_(maxPrjNum)
{
	webDataReader_.init();
	ReadWorkPathFromFile("CFG/pkpm.ini");
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

void CefForm::InitWindow()
{
	SetIcon(128);
	// 监听鼠标单击事件
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefForm::OnClicked, this, std::placeholders::_1));
	//m_pRoot->AttachBubbledEvent(ui::kEventMouseDoubleClick, nbase::Bind(&CefForm::OnDbClicked, this, std::placeholders::_1));
	//m_pRoot->AttachAllEvents(nbase::Bind(&CefForm::OnDbClicked, this, std::placeholders::_1));
	// 从 XML 中查找指定控件
	cef_control_		= dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control"));
	cef_control_dev_	= dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control_dev"));
	btn_dev_tool_		= dynamic_cast<ui::Button*>(FindControl(L"btn_dev_tool"));
	edit_url_			= dynamic_cast<ui::RichEdit*>(FindControl(L"edit_url"));
	label_= dynamic_cast<ui::Label*>(FindControl(L"projectName"));

	// 设置输入框样式
	edit_url_->SetSelAllOnFocus(true);
	edit_url_->AttachReturn(nbase::Bind(&CefForm::OnNavigate, this, std::placeholders::_1));

	// 监听页面加载完毕通知
	cef_control_->AttachLoadStart(nbase::Bind(&CefForm::RegisterCppFuncs, this));
	cef_control_->AttachLoadEnd(nbase::Bind(&CefForm::OnLoadEnd, this, std::placeholders::_1));

	cef_control_->AttachDevTools(cef_control_dev_);

	auto path= nbase::win32::GetCurrentModuleDirectory()+ RelativePathForHtmlRes;
	//cef_control_->LoadURL(L"file:///D:/works/NimDuilibFramework/bin/Ribbon/HtmlRes/index.html");
	cef_control_->LoadURL(path);
	if (!nim_comp::CefManager::GetInstance()->IsEnableOffsetRender())
		cef_control_dev_->SetVisible(false);

	auto hwnd = GetHWND();
	LONG style = ::GetWindowLong(this->m_hWnd, GWL_EXSTYLE);//获取原窗体的样式
	auto hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	style |= WS_EX_ACCEPTFILES;//更改样式
	hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	::SetWindowLongW(hwnd, GWL_EXSTYLE, style);//重新设置窗体样式
	SetSizeBox({ 5,5,5,5 });

	SetWindowTextA(GetHWND(), "PkpmV5.1.1");
	SetCaption("hello fucker");
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
		//我们启动一个模态/非模态对话框,嗯...模态比较简单一些
		/**/
		//CWinThread* m_pThrd;
		////启动
		//m_pThrd = AfxBeginThread(RUNTIME_CLASS(CDirSelectThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED,0);
		//CDirSelectThread* workThread = dynamic_cast<CDirSelectThread*>(m_pThrd);
		//HANDLE event = CreateEvent(NULL, TRUE, FALSE, L"Quit");
		//workThread->SetQuitEvent(event);
		//workThread->ResumeThread();
		//HANDLE hp = m_pThrd->m_hThread;
		//if (hp)
		//{
		//	while (WAIT_OBJECT_0 != MsgWaitForMultipleObjects(1, &event, FALSE, 0, QS_ALLINPUT))
		//	{
		//		MSG msg;
		//		if (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		//		{
		//			if (msg.message == WM_LBUTTONDOWN)
		//			{
		//				MessageBox(NULL,L"fuck",L"ananann",1);
		//			}
		//			else
		//			{
		//				continue;
		//			}
		//			/*
		//				::TranslateMessage(&msg);
		//				::DispatchMessage(&msg);
		//			*/
		//		}
		//	}
		//	
		//	CloseHandle(event);
		//}
		//
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
				//OutputDebugString(L"fuck WM_KEYDOWN");
				//OutputDebugString(L"fuck WM_KEYDOWN");
			}
		}
	}
	if (uMsg == WM_LBUTTONDOWN)
	{
		OutputDebugString(L"fuck");
	}
	return ui::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

//这个什么时候调用?
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
}


void CefForm::RegisterCppFuncs()
{
	cef_control_->RegisterCppFunc(L"ShowMessageBox", 
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(params), 3000, GetHWND());
			callback(false, R"({ "message": "Success0." })");
		})
	);

	cef_control_->RegisterCppFunc(L"CONFIGFILES",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			std::string filePath=document["filePath"].GetString();
			auto re = webDataReader_.readSpecific(filePath);
			callback(true, re);
			return;
			}
		)
	);

	cef_control_->RegisterCppFunc(L"READWORKPATHS",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			std::string pkpmIniPath = document["pkpmIniPath"].GetString();
			auto re=ReadWorkPathFromFile(pkpmIniPath);
			callback(true, re);
			return;
			}
		)
	);

	cef_control_->RegisterCppFunc(L"OnSetCaption",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			const char* captionName = document["Caption"].GetString();
			SetCaption(captionName);
#ifdef DEBUG
			std::string debugStr = R"({ "SetCaption": "Success." })";
			callback(true, debugStr);
#endif // DEBUG
			return;
			}
		)
	);



}

//fileName一般就是CFG/PKPM.ini了
std::string CefForm::ReadWorkPathFromFile(const std::string& filename)
{
	prjPaths_.clear();
	const std::string fullpath = nbase::UnicodeToAnsi(nbase::win32::GetCurrentModuleDirectory()) + filename.c_str();

	//拼字符串给网页
	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	Value array(kArrayType);//< 创建一个数组对象
	std::vector<std::string> vec;
	char prjPathStr[256] = { 0 };
	for (int i = 0; i < maxPrjNum; ++i)
	{
		auto workPathId = "WorkPath" + std::to_string((ULONGLONG)i);
		memset(prjPathStr, 0, 256);
		auto nRead = GetPrivateProfileStringA("WorkPath", workPathId.c_str(), "error", prjPathStr, 256, fullpath.c_str());
		if (!strcmp("error", prjPathStr))
		{
			//prjPathStr=="error"表明用户手动修改了配置文件!
			continue;//or continue?
		}
		else if (prjPathStr[nRead - 1] != '\\')
		{
			//用户改了配置文件，但又没改对
			continue;
		}
		std::string timeStamp;
		auto ret = GetPrjInfo(prjPathStr, timeStamp);
		rapidjson::Value obj(rapidjson::kObjectType);//每一个数组里面是一个json格式
		if (ret)
		{
			vec.emplace_back(prjPathStr);
			Value key(kStringType);
			key.SetString(workPathId.c_str(), allocator);
			Value value(kStringType);
			value.SetString(prjPathStr, allocator);
			obj.AddMember("WorkPath", value, allocator);

			value.SetString(timeStamp.c_str(), allocator);
			obj.AddMember("Date", value, allocator);

			std::string bmpPath = prjPathStr;
			bmpPath += "BuildUp.bmp";
			if (IsSnapShotExist(bmpPath))
			{
				bmpPath = "file://" + bmpPath;//网页需要增加这个前缀
				value.SetString(bmpPath.c_str(), allocator);
				obj.AddMember("ImgPath", value, allocator);
			}
			else
			{
				value.SetString(bmpPath.c_str(), allocator);
				obj.AddMember("ImgPath", "error", allocator);
			}
			array.PushBack(obj, allocator);
		}
	}
	prjPaths_.put(vec.rbegin(), vec.rend());
	doc.AddMember("Data", array, allocator);
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	doc.Accept(writer);
	std::string result= s.GetString();
	return nbase::AnsiToUtf8(result);
}


bool CefForm::GetPrjInfo(const std::string& pathStr, std::string& timestamp,
	const char* surfix)
{
	auto index = pathStr.find_last_not_of("/\\");
	std::string path = pathStr.substr(0, index + 1);
	if (ENOENT == _access(path.c_str(), 0))
		return false;
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) == 0)
	{
		if ((_S_IFMT & statbuf.st_mode) == _S_IFDIR)
		{
			auto seconds = statbuf.st_mtime;
			auto tm_time = *localtime(&seconds);
			char buf[64];
			sprintf(buf, "%4d年%02d月%02d日 %02d:%02d",
				tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
				tm_time.tm_hour, tm_time.tm_min);
			timestamp.assign(buf);
			return true;
		}
	}
	return false;
}

bool CefForm::IsSnapShotExist(const std::string& path)
{
	ASSERT(!PathIsDirectoryA(path.c_str()));
	return static_cast<bool>(PathFileExistsA(path.c_str()));
}

void CefForm::SetCaption(const std::string& name)
{
	label_->SetText(nbase::UTF8ToUTF16(name));
}
