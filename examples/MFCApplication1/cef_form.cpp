#include "pch.h"
#include "cef_form.h"
#include "CDirSelectThread.h"
#include "Console.h"
#include "string_util.h"
#include "HttpUtil.h"
#include "FileSystem.h"

#include <sys/stat.h>
#include <stdlib.h>
#include <tchar.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <filesystem>
#include <regex>
#include <string_view>


#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/memorystream.h"

#include "nlohmann/json.hpp"


using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::Writer;
using namespace rapidjson;
using namespace Alime::HttpUtility;

//CString cfg_key_str;
//CString get_cfg_path_reg_key()
//{
//	return cfg_key_str;
//}
//details
namespace //which will write to configFile
{
	//exe为起点
	const std::wstring RelativePathForHtmlRes = L"resources\\HtmlRes\\index.html";
	const char* toRead[] = { "navbarIndex", "parentIndex", "childrenIndex","projectIndex" };
	const char* defaultAdvertise = "{data:[ \
{\"key\":\"官方网站: www.pkpm.cn\", \"value\":\"www.pkpm.cn\"}, "
"{\"key\":\"官方论坛: www.pkpm.cn/bbs\",\"value\":\"www.pkpm.cn/bbs\"}"
"]}";

	std::wstring FullPathOfPkpmIni()
	{
		static auto path = nbase::win32::GetCurrentModuleDirectory() + L"cfg/pkpm.ini";
		return path;
	}
	std::string FullPathOfPkpmIniA()
	{
		static auto path = nbase::UnicodeToAnsi(nbase::win32::GetCurrentModuleDirectory() + L"cfg/pkpm.ini");
		return path;
	}

	std::string DictToJson(const std::vector<std::pair<std::string, std::string>>& dict)
	{
		rapidjson::Document doc;
		rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

		Value root(kObjectType);
		for (auto i = 0; i != dict.size(); ++i)
		{
			rapidjson::Value strObject(rapidjson::kStringType);
			strObject.SetString(dict[i].first.c_str(), allocator);
			rapidjson::Value strObjectValue(rapidjson::kStringType);
			strObjectValue.SetString(dict[i].second.c_str(), allocator);
			root.AddMember(strObject, strObjectValue, allocator);
		}
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		root.Accept(writer);
		std::string result = s.GetString();
		//MessageBox(NULL, result.c_str(), "test", 1);
		return s.GetString();
	}
}


const std::wstring CefForm::kClassName = L"PKPM V5.1.1启动界面";
//我们的广告原页面是https://update.pkpm.cn/PKPM2010/Info/index.html
//而且是永远不会变的

CefForm::CefForm()
	:maxPrjNum_(GetPrivateProfileInt(L"WorkPath", L"MaxPathName", 6, FullPathOfPkpmIni().c_str())),
	prjPaths_(maxPrjNum_),
	isWebPageAvailable_(false),
	cef_control_(NULL),
	indexHeightLighted_(-1)
{
	webDataReader_.Init();
	ReadWorkPathFromFile("CFG/pkpm.ini");
	InitAdvertisement();
	size_t numofPrj=CorrectWorkPath();
	if (numofPrj > 0)
		indexHeightLighted_ = 0;
	//一个控制台，以命令方式模拟网页点击事件
	//将被写入log模块
	//Alime::Console::CreateConsole();
	//Alime::Console::SetTitle(L"Alime");
	//std::thread t([this]() {
	//	while (!stop)//没有办法优雅地中止这个线程,因windows似乎不能向stdin直接write?
	//	{
	//		auto str = Alime::Console::ReadLine();
	//		if (str.find(L"SetCaption") != std::wstring::npos)
	//		{
	//			auto result = str.substr(10);
	//			SetCaption(u8"PKPM结构设计软件 10版 V5.1.1    " + nbase::UTF16ToUTF8(result));
	//		}
	//		else if (str.find(L"Clear") != std::wstring::npos)
	//		{
	//			Alime::Console::Clear();
	//		}
	//		else if (str.find(L"XmlFile") != std::wstring::npos)
	//		{
	//			//readSpecific
	//			auto result = str.substr(8);
	//			auto str = webDataReader_.readSpecific(nbase::UnicodeToAnsi(result));
	//			Alime::Console::Write(nbase::UTF8ToUTF16(str));
	//		}
	//		else {
	//			Alime::Console::SetColor(1, 0, 0, 0);
	//			Alime::Console::WriteLine(L"没有这个接口");
	//			Alime::Console::SetColor(0, 0, 0, 1);
	//		}
	//	}
	//	});
	//t.detach();
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
	cef_control_= dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control"));
	cef_control_dev_= dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control_dev"));
	btn_dev_tool_= dynamic_cast<ui::Button*>(FindControl(L"btn_dev_tool"));
	edit_url_= dynamic_cast<ui::RichEdit*>(FindControl(L"edit_url"));
	label_= dynamic_cast<ui::Label*>(FindControl(L"projectName"));

	// 设置输入框样式
	//保留这段代码是因为前端需要一个刷新机制
	if (edit_url_)
	{
		edit_url_->SetSelAllOnFocus(true);
		edit_url_->AttachReturn(nbase::Bind(&CefForm::OnNavigate, this, std::placeholders::_1));
	}

	// 监听页面加载完毕通知
	cef_control_->AttachLoadStart(nbase::Bind(&CefForm::RegisterCppFuncs, this));
	cef_control_->AttachLoadEnd(nbase::Bind(&CefForm::OnLoadEnd, this, std::placeholders::_1));
	cef_control_->AttachDevTools(cef_control_dev_);

	wchar_t buffer[128] = { 0 };
	auto nRead = GetPrivateProfileStringW(L"Html", L"PathOfCefHtml", 
		RelativePathForHtmlRes.c_str(), buffer, ArraySize(buffer), FullPathOfPkpmIni().c_str());
	auto cefHtmlPath= nbase::win32::GetCurrentModuleDirectory()+ buffer;
	cef_control_->LoadURL(cefHtmlPath);
	if (!nim_comp::CefManager::GetInstance()->IsEnableOffsetRender())
		cef_control_dev_->SetVisible(false);

	auto hwnd = GetHWND();
	CallBack f = [this]() {
		prjPaths_.moveToFront(indexHeightLighted_);
		SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
		cef_control_->CallJSFunction(L"flush",
			nbase::UTF8ToUTF16("{\"uselessMsg\":\"test\"}"),
			ToWeakCallback([this](const std::string& chosenData) {
				}
		));
	};
	this->shortCutHandler_.SetCallBacks(hwnd, f);
	shortCutHandler_.Init();

	//我不太确定是不是需要自己增加拖放支持。
	LONG style = ::GetWindowLong(this->m_hWnd, GWL_EXSTYLE);//获取原窗体的样式
	auto hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	style |= WS_EX_ACCEPTFILES;//更改样式
	hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	::SetWindowLongW(hwnd, GWL_EXSTYLE, style);//重新设置窗体样式

	SetWindowTextA(GetHWND(), "PkpmV5.1.1");
	defaultCaption_ =label_->GetText();
	//SetCaption(u8"PKPM结构设计软件 10版 V5.1.1");

	//旧代码
	SetCfgPmEnv();//增加pm环境
	appDll_.InitPkpmAppFuncPtr();
	appDll_.Invoke_InitPkpmApp();

	//换肤按钮的响应
	ui::Button* settings = dynamic_cast<ui::Button*>(FindControl(L"settings"));
	settings->AttachClick([this](ui::EventArgs* args) {
		RECT rect = args->pSender->GetPos();
		ui::CPoint point;
		point.x = rect.left- 130 ;
		point.y = rect.top + 10;
		ClientToScreen(m_hWnd, &point);

		nim_comp::CMenuWnd* pMenu = new nim_comp::CMenuWnd(NULL);
		ui::STRINGorID xml(L"settings_menu.xml");
		pMenu->Init(xml, _T("xml"), point);
		ui::ListBox* pVbox = dynamic_cast<ui::ListBox*>(pMenu->FindControl(L"themeWindow"));
		pVbox->AttachSelect([this](ui::EventArgs* args) {
			auto themeType=args->pSender->GetName();
			int current = args->wParam;
			//我们只关心用户本次选择
			assert(args->lParam==-1);
			PostMessage(WM_THEME_SELECTED, current, -1);
			return true;
			});
		return true;
		});
}


LRESULT CefForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	nim_comp::CefManager::GetInstance()->PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}


bool CefForm::OnClicked(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	/*
		如果你坚持要在本项目使用mfc对话框
		你有几个选择:
		1.像下面这样在线程里启动模态/非模态对话框。我保留了下面这个例子。
		2.将对话框放到动态库的函数里。
	*/
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
		//				MessageBox(NULL,L"...",L"ananann",1);
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
	else if (name == L"closebtn")
	{
		cef_control_->CallJSFunction(L"currentChosenData", 
			nbase::UTF8ToUTF16("{\"uselessMsg\":\"test\"}"),
			ToWeakCallback([this](const std::string& chosenData) {
				//nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(chosenData), 3000, GetHWND());
				//这必然是个合法json
				OnSetDefaultMenuSelection(chosenData);
				}
		));
	}
	return true;
}


LRESULT CefForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_LBUTTONDOWN)
	{
		OutputDebugString(L"Receive WM_LBUTTONDOWN");
		//PostMessage(WM_TEST, 0, 0);
	}
	if (uMsg == WM_TEST)
	{
		OutputDebugString(L"Receive WM_TEST");
		//PostMessage(WM_TEST, 0, 0);
	}
	if (uMsg == WM_LBUTTONDBLCLK)
	{
		OutputDebugString(L"Receive WM_NCLBUTTONDBLCLK");
		//{
		//	cef_control_->CallJSFunction(L"showJsMessage", nbase::UTF8ToUTF16("{\"msg\":\"hello!!!\"}"),
		//		ToWeakCallback([this](const std::string& json_result) {
		//			nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(json_result), 3000, GetHWND());
		//		}
		//	));
		//}
	}
	if (uMsg == WM_DROPFILES)
	{
		OutputDebugString(L"Receive WM_DROPFILES");
		HDROP hDropInfo = (HDROP)wParam;
		UINT count;
		TCHAR filePath[MAX_PATH] = { 0 };
		count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
		if (count != 1)
		{
			AfxMessageBox(L"仅支持拖拽单个目录", MB_SYSTEMMODAL);
		}
		else
		{
			DragQueryFile(hDropInfo, 0, filePath, sizeof(filePath));
			if (PathFileExists(filePath) && PathIsDirectory(filePath))
			{
				std::string pathString = nbase::UnicodeToAnsi(filePath);
				if (pathString.back() != '\\')
					pathString += '\\';
				AddWorkPaths(pathString, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
				cef_control_->CallJSFunction(L"flush",
					nbase::UTF8ToUTF16("{\"uselessMsg\":\"test\"}"),
					ToWeakCallback([this](const std::string& chosenData) {
						}
				));
			}
			else
				AfxMessageBox(L"仅支持拖拽目录", MB_OK | MB_SYSTEMMODAL);
		}
		DragFinish(hDropInfo);
	}
	if (uMsg == WM_KEYDOWN)
	{
		if (116 == wParam)
		{
			cef_control_->Refresh();
		}
		//出现过一个问题，就是网页无法获得焦点
		//我不得已，自己处理了ctrlv和delete事件
		//直到我意识到可以在WM_SIZE里加入SetFocus。
		//然而这段代码页需还会有用。我会保留到发盘为止。
		if(0)if (0x80 == (0x80 & GetKeyState(VK_CONTROL)))
		{
			if ('V' == wParam || 'v' == wParam)
			{
				std::string filePath;
				if (OpenClipboard(GetHWND()))
				{
					if (IsClipboardFormatAvailable(CF_TEXT))
					{
						HANDLE hclip = GetClipboardData(CF_TEXT);
						char* pBuf = static_cast<char*>(GlobalLock(hclip));
						LocalUnlock(hclip);
						USES_CONVERSION;
						filePath = std::move(std::string(pBuf));
					}
					CloseClipboard();
				}
				if (!filePath.empty())
				{
					if (PathFileExistsA(filePath.c_str()) && PathIsDirectoryA(filePath.c_str()))
					{
						if (filePath.back() != '\\')
							filePath += "\\";
						if (AddWorkPaths(filePath, nbase::UnicodeToAnsi(FullPathOfPkpmIni())))
						{
							cef_control_->CallJSFunction(L"flush",
								nbase::UTF8ToUTF16("{\"uselessMsg\":\"test\"}"),
								ToWeakCallback([this](const std::string& chosenData) {
									}
							));
							return TRUE;
						}
					}
					else
					{
						//LOG_ERROR<<"invalid path";
					}
				}
				return TRUE;
			}
		}
		if(0)if (wParam == VK_DELETE)
		{
			cef_control_->CallJSFunction(L"currentChosenData",
				nbase::UTF8ToUTF16("{\"uselessMsg\":\"test\"}"),
				ToWeakCallback([this](const std::string& chosenData) {
					std::string str(nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(chosenData)));
					nlohmann::json jsonstr = nlohmann::json::parse(str);
					int index = jsonstr["projectIndex"];
					{
						static unsigned long long  lastTick = 0;
						auto now = GetTickCount();
						auto interval = now - lastTick;
						lastTick = now;
						if (interval < 500)
							return;
						try {
							if (prjPaths_.size() > 0)
							{
								int prjSelected = index;
								prjPaths_.deleteAt(prjSelected);
								SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
								cef_control_->CallJSFunction(L"flush",
									nbase::UTF8ToUTF16("{\"uselessMsg\":\"test\"}"),
									ToWeakCallback([this](const std::string& chosenData) {
										}
								));
							}
						}
						catch (...) {
							//FATAL<<
							std::string debugStr = R"({ "Call ONNEWPROJECT": "Fail." })";
						}
					}
					}
			));
			
		}
	}
	if (uMsg == WM_SIZE)
	{
		if(cef_control_)
			cef_control_->SetFocus();
	}
	if (uMsg == WM_THEME_SELECTED)
	{
		OutputDebugString(std::to_wstring(wParam).c_str());
		assert(lParam == -1);
		SwicthThemeTo(wParam);
	}
	return ui::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

#include "SkinSwitcher.h"
void CefForm::SwicthThemeTo(int index)
{
	//我是打心底里痛恨设计模式
	//云信的tab可以把消息写到xml里
	//可是我们这里是个menuitem来执行皮肤切换
	nim_comp::Box* vistual_caption = dynamic_cast<nim_comp::Box*>(FindControl(L"vistual_caption"));
	auto sw=SkinSwitcherFatctory::Get(index);
	sw->Switch(vistual_caption, label_);
	SaveThemeIndex(index);
}

void CefForm::SaveThemeIndex(int index)
{

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
	if(FindControl(L"btn_back"))
		FindControl(L"btn_back")->SetEnabled(cef_control_->CanGoBack());
	if(FindControl(L"btn_forward"))
		FindControl(L"btn_forward")->SetEnabled(cef_control_->CanGoForward());
}


void CefForm::RegisterCppFuncs()
{
	//debug
	//不要继续使用。为了节省空间，我删除了资源。
	cef_control_->RegisterCppFunc(L"ShowMessageBox", 
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(params), 3000, GetHWND());
			callback(false, R"({ "message": "Success0." })");
		})
	);

	cef_control_->RegisterCppFunc(L"ONRCLICKPRJ",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			//nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(params), 3000, GetHWND());
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			std::string filePath = document["projectName"].GetString();
			OnRightClickProject(nbase::UTF8ToUTF16(filePath));
			callback(true, R"({ "message": "Success0." })");
			})
	);

	cef_control_->RegisterCppFunc(L"CONFIGFILES",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			std::string filePath=document["filePath"].GetString();
			auto re = webDataReader_.ReadSpecific(filePath);
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

	cef_control_->RegisterCppFunc(L"ONSETCAPTION",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			const char* captionName = document["caption"].GetString();
			//projectIndex
			int project= document["projectIndex"].GetInt();
			assert(nbase::AnsiToUtf8(prjPaths_[project]) == captionName);
			//....
			SetHeightLightIndex(project);
			//fix me,写入配置文件
			std::string captionU8 = nbase::UTF16ToUTF8(defaultCaption_);
			if(captionU8.empty())
				captionU8 = u8"PKPM结构设计软件 10版 V5.1.1   ";
			captionU8 += captionName;
			SetCaption(captionU8);
#ifdef DEBUG
			std::string debugStr = R"({ "SetCaption": "Success." })";
			callback(true, debugStr);
#endif // DEBUG
			return;
			}
		)
	);

	cef_control_->RegisterCppFunc(L"SHORTCUT",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			std::string shortcutName = document["shortcutName"].GetString();
#ifdef DEBUG
			std::string debugStr = R"({ "SetCaption": "Success." })";
			callback(true, debugStr);
#endif // DEBUG
			shortcutName = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(shortcutName)).c_str();
			OnShortCut(shortcutName.c_str());
			return;
			}
		)
	);

	cef_control_->RegisterCppFunc(L"OPENDOCUMENT",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			std::string path = document["fullPath"].GetString();
			OnOpenDocument(nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(path)));
			std::string debugStr = R"({ "SetCaption": "Success." })";
			callback(true, debugStr);
			return;
			}
		)
	);

	cef_control_->RegisterCppFunc(L"DATAFORMAT",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			nlohmann::json json = nlohmann::json::parse(params);
			std::string modName=json["modName"];
			std::string exeName = json["exeName"];
				
			std::string ansiMod=nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(modName));
			std::string ansiExe = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(exeName));
			int index = json["prjIndex"];
			std::string workDir = prjPaths_[index];
			DataFormatTransfer(ansiMod, ansiExe, workDir);
			//deprecated 
			//由于可以从网页拿索引。回调被废弃
			/*
			cef_control_->CallJSFunction(L"currentChosenData",
				nbase::UTF8ToUTF16("{\"uselessMsg\":\"test\"}"),
				ToWeakCallback([this, ansiMod, ansiExe](const std::string& chosenData) {
					std::string str(nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(chosenData)));
					nlohmann::json jsonstr= nlohmann::json::parse(str);
					int index = jsonstr["projectIndex"];
					std::string wordDir = prjPaths_[index];
					DataFormatTransfer(ansiMod, ansiExe, wordDir);
					}
			));
			*/
			std::string debugStr = R"({ "Pass me the projectName or index": "Success." })";
			callback(true, debugStr);
			}
		)
	);

	cef_control_->RegisterCppFunc(L"ONNEWPROJECT",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			auto strAnsi = OnNewProject();
			if (strAnsi.empty())
			{
				std::string debugStr = R"({ "call ONNEWPROJECT": "Failed." })";
				callback(false, debugStr);
			}
			else
			{
				if(strAnsi.back()!='\\')
					strAnsi = strAnsi + "\\";
				if (!AddWorkPaths(strAnsi, nbase::UnicodeToAnsi((nbase::win32::GetCurrentModuleDirectory() + L"cfg\\pkpm.ini"))))
				{
					std::string debugStr = R"({ "call ONNEWPROJECT": "Failed." })";
					callback(false, debugStr);
				}
				nlohmann::json json;
				json["pathSelected"] = nbase::AnsiToUtf8(strAnsi);
				auto str = json.dump();
				std::string debugStr = R"({ "call ONNEWPROJECT": "successed." })";
				callback(true, str);
				return;
			}
			}
		)
	);

	//OnGetDefaultMenuSelection
	cef_control_->RegisterCppFunc(L"ONGETDEfAULTMENUSELECTION",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			auto selection =OnGetDefaultMenuSelection();
			callback(true, nbase::AnsiToUtf8(selection));
			})
	);

	//双击工程
	cef_control_->RegisterCppFunc(L"DBCLICKPROJECT",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			nlohmann::json json = nlohmann::json::parse(params);
			//旧代码，实际上只需要3个参数, 为了不给前端造成疑惑，就不改了
			std::string prjPath_ansi = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json["pathSelected"]));
			std::string pathOfCore = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json["pathOfCore"]));
			std::string coreWithPara = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json["coreWithPara"]));
			std::string secMenu = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json["secMenu"]));
			std::string trdMenu = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json["trdMenu"]));
			std::vector<std::string> vec;
			vec.push_back(prjPath_ansi); 
			vec.push_back(pathOfCore);
			vec.push_back(coreWithPara);
			vec.push_back(secMenu);
			vec.push_back(trdMenu);
			//cef_control_->HideToolTip();
			//你可以像shortCutHandler.cpp那样处理参数，避免写出现在这样的代码
			OnDbClickProject(vec);

			std::string debugStr = R"({ "call ONNEWPROJECT": "Success." })";
			callback(true, nbase::AnsiToUtf8(debugStr));
			})
	);

	//
	cef_control_->RegisterCppFunc(L"ONDROPFILES",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			HDROP hDropInfo=NULL;
			UINT count;
			CHAR filePath[260] = { 0 };
			count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
			if (count != 1)
			{
				::AfxMessageBox(L"仅支持拖拽单个目录", MB_OK | MB_SYSTEMMODAL);
				return;
			}
			else
			{
				DragQueryFileA(hDropInfo, 0, filePath, sizeof(filePath));
				if (PathFileExistsA(filePath) && PathIsDirectoryA(filePath))
				{
					filePath[strlen(filePath)] = '\\';
					if (AddWorkPaths(filePath, nbase::UnicodeToAnsi(FullPathOfPkpmIni())))
					{
						//fix me
						//我也不知道说什么了，这个前端的同事很多工作都是我代劳的
						//她压根不检查返回值，无论有没有新建成功，她都重读一次工程列表
					}
				}
				else
				{
					::AfxMessageBox(L"仅支持拖拽目录", MB_OK | MB_SYSTEMMODAL);
					return;
				}
			}
			DragFinish(hDropInfo);
			})
	);

	//ctrl + v
	cef_control_->RegisterCppFunc(L"ONCTRLV",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			std::string filePath;
			if (OpenClipboard(GetHWND()))
			{
				if (IsClipboardFormatAvailable(CF_TEXT))
				{
					HANDLE hclip = GetClipboardData(CF_TEXT);
					char* pBuf = static_cast<char*>(GlobalLock(hclip));
					LocalUnlock(hclip);
					USES_CONVERSION;
					filePath = std::move(std::string(pBuf));
				}
				CloseClipboard();
			}
			if (!filePath.empty())
			{
				if (PathFileExistsA(filePath.c_str()) && PathIsDirectoryA(filePath.c_str()))
				{
					if(filePath.back()!='\\')
						filePath += "\\";
					if (AddWorkPaths(filePath, nbase::UnicodeToAnsi(FullPathOfPkpmIni())))
					{
						std::string debugStr = R"({ "call ONNEWPROJECT": "Success." })";
						callback(true, nbase::AnsiToUtf8(debugStr));
						return;
					}

				}
				else
				{
					//可以崩溃了
					//LOG_ERROR<<"invalid path";
					std::string debugStr = R"({ "call ONNEWPROJECT": "FAILED." })";
					callback(false, nbase::AnsiToUtf8(debugStr));
				}
			}
			std::string debugStr = R"({ "call ONNEWPROJECT": "FAILED." })";
			callback(false, nbase::AnsiToUtf8(debugStr));
			})
		);

	cef_control_->RegisterCppFunc(L"ONDELETE",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			static unsigned long long  lastTick = 0;
			auto now = GetTickCount();
			auto interval = now - lastTick;
			lastTick = now;
			if (interval < 500)	
				return;
			//旧代码，deleteAt(-1)原定是给js处理的
			try {
				if (prjPaths_.size() > 0)
				{
					//获取工作的索引
					nlohmann::json json = nlohmann::json::parse(params);
					//std::string indexStr(json["index"]);
					int prjSelected = json["index"];
					//没有工程为什么不传-1?
					prjPaths_.deleteAt(prjSelected);
					SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
					//fix me
					//SaveMenuSelection(false);
					/*Fix me
					Refresh不能在别的线程调用，而mfc又有自己的事件循环，
					所以，只能在documentComplete里做判断。
					我这里先对付过去了。
					你应该判断上一次keyup和这次keydown的间隔
					*/
					std::string debugStr = R"({ "Call ONNEWPROJECT": "Success." })";
					callback(true, nbase::AnsiToUtf8(debugStr));
				}
			}
			catch (...) {
				//FATAL<<
				std::string debugStr = R"({ "Call ONNEWPROJECT": "Fail." })";
				callback(false, nbase::AnsiToUtf8(debugStr));
			}
		})
	);

	cef_control_->RegisterCppFunc(L"NEWVERSION",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			bool newReleased = TellMeNewVersionExistOrNot();
			if(newReleased)
				callback(true, R"({ "NewVersion": "true" })");
			else
				callback(false, R"({ "NewVersion": "false" })");
			})
	);

	cef_control_->RegisterCppFunc(L"ADVERTISES",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			auto advertisement = TellMeAdvertisement();
			auto u8str = nbase::AnsiToUtf8(advertisement);
			callback(true, u8str);
			})
	);

	cef_control_->RegisterCppFunc(L"OPENURL",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			nlohmann::json json = nlohmann::json::parse(params);
			std::wstring url = nbase::UTF8ToUTF16(json["adUrl"]);
			::ShellExecute(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWDEFAULT);
			std::string debugStr = R"({ "OPENURL": "Always Success." })";
			callback(true, debugStr);
			})
	);

}

//fileName一般就是CFG/PKPM.ini了
/*
这是个旧函数，每次都读配置文件，之所以没有
删除是因为，它曾经帮了大忙0.0
(比方说，它意外地处理了网页刷新造成的内存/文件不匹配，用户使用程序过程中手动改配置文件)，
我保留它大概是希望获得额外的心理安慰。
正确的做法是检查一次配置文件，读到内存，然后维护内存即可。
*/
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
	for (int i = 0; i < maxPrjNum_; ++i)
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
	//fixe me rapidjson 似乎自己编码了u8
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

std::string CefForm::OnGetDefaultMenuSelection()
{
	std::vector<std::pair<std::string, std::string>> dict;
	const int n = sizeof(toRead) / sizeof(*toRead);
	char indexRet[32] = { 0 };
	for (size_t i = 0; i != n; ++i)
	{
		memset(indexRet, 0, sizeof(indexRet));
		GetPrivateProfileStringA("Html", toRead[i], "error", indexRet, sizeof(indexRet), nbase::UnicodeToAnsi(FullPathOfPkpmIni()).c_str());
		if (i == n - 1 && prjPaths_.size() == 0)
		{
			dict.push_back(std::make_pair(toRead[i], "-1"));
			break;
		}
		dict.push_back(std::make_pair(toRead[i], indexRet));	
	}
	auto json_str = DictToJson(dict);
	return json_str;
}

void CefForm::OnShortCut(const char* cutName)
{
	shortCutHandler_.CallFunc(cutName);
}

//我们只在回调中使用u8字符串
std::string CefForm::OnNewProject()
{
	typedef void (*func)(const char*, char*);
	auto hdll = LoadLibrary(L"PKPM2010V511.dll");
	func ptr = (func)GetProcAddress(hdll, "OpenDlgSelectDir");
	if (!ptr)
	{
		//LOG_FATAL<<
		return {};
	}	
	char result[MAX_PATH] = { 0 };
	std::string defaultPath = prjPaths_[indexHeightLighted_];
	ptr(defaultPath.c_str(), result);
	FreeLibrary(hdll);
	return result;
	///////////////////////////////////////////////////
	//以下为废弃代码，没有selectDlg时，可使用
	TCHAR  folderPath[MAX_PATH] = { 0 };
	std::wstring path;
	BROWSEINFO  sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择新的工程路径");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX
		| BIF_DONTGOBELOWDOMAIN | BIF_USENEWUI;
	sInfo.lpfn = NULL;

	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		if (::SHGetPathFromIDList(lpidlBrowse, folderPath))
		{
			path = folderPath;
			::CoTaskMemFree(lpidlBrowse);
			std::string dirName(nbase::UnicodeToAnsi(path));
			dirName += "\\";
			std::string path_of_pkpm_dot_ini = 
				nbase::UnicodeToAnsi(FullPathOfPkpmIni());//这个真的没有办法
			AddWorkPaths(dirName, path_of_pkpm_dot_ini);
			return nbase::UnicodeToAnsi(path);
		}
	}	
	return {};
}

void CefForm::OnRightClickProject(const std::wstring& prjName)
{
	ShellExecute(NULL, _T("open"), _T("explorer.exe"), prjName.c_str(), NULL, SW_SHOWNORMAL);
}

//有问题,我应该让前端调用时就把工程路径和索引一块过来
void CefForm::DataFormatTransfer(const std::string& module, const std::string& app, const std::string& workdir)
{
	if (!prjPaths_.size())
	{
		::AfxMessageBox(L"请先选择工作目录", MB_OK | MB_SYSTEMMODAL);
		return;
	}
	{
		ShowWindow(SW_HIDE);
		char oldWorkPath[256] = { 0 };
		GetCurrentDirectoryA(sizeof(oldWorkPath), oldWorkPath);
		BOOL ret = SetCurrentDirectoryA(workdir.c_str());
		if (!ret)
		{
			MessageBox(NULL, L"工作目录错误或者没有权限", L"PKPMV51", 1);
			return;
		}
		run_cmd(module.c_str(), app.c_str(), "");
		//m_WorkPath = prjPaths_[prjSelected].c_str();
		//run_cmd(CString(vec.front().c_str()), CString((*++iter).c_str()), "");
		SetCurrentDirectoryA(oldWorkPath);
		for (int i = 0; i != prjPaths_.size(); ++i)
		{
			if (prjPaths_[i] == workdir)
			{
				prjPaths_.moveToFront(i);
				SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
				break;
			}
		}
		//fix me 
		//这里需要网页重新读一下工程列表
		//SaveMenuSelection();
		//this->m_pBrowserApp->Refresh();
		//ShowWindow(SW_SHOW);
		ShowWindow();
		
	}
	//try
	//{
	//	cef_control_->CallJSFunction(L"MenuSelectionOnHtml", L"", 
	//		ToWeakCallback([this, module_app_name](const std::string& json_result){
	//			//nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(json_result), 3000, GetHWND());
	//			std::string str_Ret = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(module_app_name));
	//			auto vec = nbase::StringTokenize(str_Ret.c_str(), ",");
	//			auto iter = vec.begin();
	//			ShowWindow(SW_HIDE);
	//			char oldWorkPath[256] = { 0 };
	//			GetCurrentDirectoryA(sizeof(oldWorkPath), oldWorkPath);
	//			int prjSelected = -1;
	//			BOOL ret = SetCurrentDirectoryA(prjPaths_[prjSelected].c_str());
	//			if (!ret)
	//			{
	//				MessageBoxA(NULL, "工作目录错误或者没有权限", "PKPMV51", 1);
	//			}
	//			//run_cmd(CString(vec.front().c_str()), CString((*++iter).c_str()), "");
	//			SetCurrentDirectoryA(oldWorkPath);
	//			//在这里通知网页更新工程
	//			//SaveMenuSelection();
	//			ShowWindow(SW_SHOW);
	//		}
	//	));
	//}
	//catch (CException & e)
	//{
	//	std::abort();
	//}
}

void CefForm::OnDbClickProject(const std::vector<std::string>& args)
{
	if(args.size()!=5)
		::AfxMessageBox(L"严重错误", MB_OK | MB_SYSTEMMODAL);
	if (!prjPaths_.size())
	{//不应该再出现这个错误
		::AfxMessageBox(L"工作目录不存在", MB_OK | MB_SYSTEMMODAL);
		return;
	}
	std::string path(args[0]);
	{
		ShowWindow(SW_HIDE);
		char oldWorkPath[256] = { 0 };
		GetCurrentDirectoryA(sizeof(oldWorkPath), oldWorkPath);
		auto ret = SetCurrentDirectoryA(path.c_str());
		if (!ret)
		{
			::AfxMessageBox(L"工作目录错误或者没有权限", MB_OK | MB_SYSTEMMODAL);
			//return;
		}
		run_cmd(args[3].c_str(), args[4].c_str(),"");
		SetCurrentDirectoryA(oldWorkPath);
		for (int i = 0; i != prjPaths_.size(); ++i)
		{
			if (prjPaths_[i] == path)
			{
				prjPaths_.moveToFront(i);
				SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
				break;
			}
		}
		ShowWindow();
	}
}

void CefForm::OnListMenu(const std::vector<std::string>& args)
{
	OnDbClickProject(args);
}

void CefForm::OnOpenDocument(const std::string& filename)
{
	std::string FullPath(nbase::UnicodeToAnsi(nbase::win32::GetCurrentModuleDirectory()) + "Ribbon\\");
	FullPath += filename;

	SHELLEXECUTEINFO ShExecInfo;
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	auto wstr = nbase::AnsiToUnicode(FullPath);
	ShExecInfo.lpFile = wstr.c_str();
	ShExecInfo.lpParameters = _T("");
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
}


void CefForm::SaveWorkPaths(collection_utility::BoundedQueue<std::string>& prjPaths, const std::string& filename)
{
	bool hasAdministratorsRights = true;
	for (int i = 0; i < maxPrjNum_; ++i)
	{
		std::string workPathId = std::string("WorkPath") + std::to_string(i);
		bool ret = false;
		if (static_cast<size_t>(i) < prjPaths.size())
		{		
			ret=WritePrivateProfileStringA("WorkPath", workPathId.c_str(), prjPaths[i].c_str(), filename.c_str());
		}			
		else//de a bug 
		{
			ret=WritePrivateProfileStringA("WorkPath", workPathId.c_str(), NULL, filename.c_str());
		}
		if (!ret)
			hasAdministratorsRights = false;
	}
	if(!hasAdministratorsRights)
		AfxMessageBox(L"无法保存工程信息，建议使用管理员权限打开本软件", MB_OK | MB_SYSTEMMODAL);
}



std::string CefForm::PathChecker(const std::string& newProj, bool &legal)
{
	legal = true;
	std::filesystem::path fullpath(newProj);
	auto realPath = std::filesystem::canonical(fullpath);
	std::string realPathOfNewProj = nbase::UnicodeToAnsi(realPath);
	if (realPathOfNewProj.back() != '\\')
		realPathOfNewProj += '\\';
	if (realPathOfNewProj.length() == 3)//盘符
		legal = false;
	return realPathOfNewProj;
}



bool CefForm::AddWorkPaths(const std::string& newProj, const std::string& filename)
{
	if (newProj.empty())
		return false;
	bool isLegalPath;
	auto realPathOfNewProj=PathChecker(newProj, isLegalPath);
	if (!isLegalPath)
		return false;
	auto iter = std::find(prjPaths_.begin(), prjPaths_.end(), realPathOfNewProj);
	if (iter == prjPaths_.end())
	{
		prjPaths_.put(realPathOfNewProj);
		SaveWorkPaths(prjPaths_, filename);
		return true;
	}
	else
	{
		auto index = std::distance(prjPaths_.begin(), iter);
		//MessageBox(_T("文件夹已存在"));
		prjPaths_.moveToFront(index);
		SaveWorkPaths(prjPaths_, filename);
		return true;
	}
	return false;
}

void CefForm::OnSetDefaultMenuSelection(const std::string& json_str)
{
	std::string str(nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json_str)));
	Document d;
	d.Parse(str.c_str());
	size_t i = 0;
	for (; i != ArraySize(toRead) - 1; ++i)
	{
		Value& s = d[toRead[i]];
		auto indexSelection = s.GetInt();
		WritePrivateProfileStringA("Html", toRead[i], std::to_string(indexSelection).c_str(), nbase::UnicodeToAnsi(FullPathOfPkpmIni()).c_str());
	}
	//数据结构是前端定的，所以，我其实没有选择。
	Value& s = d[toRead[i]];
	int indexSelection = s.GetInt();
	if (!prjPaths_.size() || !indexSelection)
		return;
	prjPaths_.moveToFront(indexSelection);
	SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
}

void CefForm::run_cmd(const CStringA& moduleName, const CStringA& appName1_, const CStringA& appName2)
{
	CStringA appName1(appName1_);
	ASSERT(!moduleName.IsEmpty());
	ASSERT(!appName1.IsEmpty());
	//需要提醒他们改配置文件。
	if (moduleName == "数据转换")
	{
		if (appName1 == "SP3D/PDS")
			appName1 = "PDS";
		else if (appName1 == "Revit")
		{
			appName1 = "导出Revit";
		}
	}
	char str[128] = { 0 };
	if (appName2.IsEmpty())
	{
		sprintf(str, "%s|%s", moduleName.GetString(), appName1.GetString());
	}
	else
	{
		sprintf(str, "%s|%s_%s", moduleName.GetString(), appName1.GetString(), appName2.GetString());
	}
	//CWnd* mainWnd = ::AfxGetMainWnd();
	//ASSERT(mainWnd);
	//ASSERT(SetCurrentDirectory(m_WorkPath));
	appDll_.Invoke_RunCommand(str);
}


bool CefForm::SetCfgPmEnv()
{
	const int LENGTH_OF_ENV = 1024 * 8;
	//哔了狗了，unique_ptr知道吗?
	TCHAR* szOriEnvPath = new  TCHAR[LENGTH_OF_ENV];//所以这片内存去哪了....?
	DWORD dwRet = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, LENGTH_OF_ENV);
	if (!dwRet)
	{
		AfxMessageBox(L"Error! Can not find Path", MB_OK | MB_SYSTEMMODAL);
		return false;
	}
	else if (LENGTH_OF_ENV < dwRet)//需要重新分配内存
	{
		delete[] szOriEnvPath;
		szOriEnvPath = new  TCHAR[dwRet + 1];
		DWORD dwRetNewAlloc;
		dwRetNewAlloc = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, dwRet);
		ASSERT(dwRetNewAlloc <= dwRet);
	}

	CString strCFG = (nbase::win32::GetCurrentModuleDirectory() + L"CFG").c_str(); //svr::GetRegCFGPath();

	TCHAR path[256];
	GetModuleFileName(NULL, path, 256);
	PathRemoveFileSpec(path);
	CString strPM = path;
	strPM += "\\Ribbon\\PM";

	CString strPath;
	strPath = strCFG + L";";
	strPath += strPM + L";";
	strPath += szOriEnvPath;
	strPath.Trim();
	int iv = SetEnvironmentVariable(_T("PATH"), strPath);
	if (iv == 0)
	{
		AfxMessageBox(L"无法设置PATH路径", MB_OK | MB_SYSTEMMODAL);
		delete[] szOriEnvPath;
		return false;
	}
	else
	{
		delete[] szOriEnvPath;
		return true;
	}
}


void CefForm::InitAdvertisement()
{
	//基础设施不够，要承受this失效的风险。但用户推出程序，问题应该不大。
	std::thread func(std::bind(&CefForm::AdvertisementThreadFunc, this));
	func.detach();
}

#include <chrono>
//由于前端搞不定，所以逼着我开线程查询广告
//这样，前端读取所有广告前，需要加锁
void CefForm::AdvertisementThreadFunc()
{
	assert(isWebPageAvailable_ == false);
	//std::this_thread::sleep_for(std::chrono::seconds(20));
	bool AdPageCanAccess = false;
	//网页不是我在维护，他们可能犯任何错误
	//比如,json写错，编码不对
	try
	{
		AdPageCanAccess = VersionPage();
	}
	catch (const std::exception& e)
	{
		UNREFERENCED_PARAMETER(e);
		//LOG_ERROR<<e.what();
#ifdef DEBUG
		AfxMessageBox(L"广告内容格式有误，或者编码不正确", MB_OK | MB_SYSTEMMODAL);
#endif // DEBUG
		AdPageCanAccess = false;
	}
	
	if (!AdPageCanAccess)//获取网页失败
	{
		//可以不锁,只是提醒你
		std::lock_guard<std::mutex> guarder(lock_);
		isWebPageAvailable_ = false;
		return;
	}
	else
	{
		std::lock_guard<std::mutex> guarder(lock_);
		isWebPageAvailable_ = true;
	}
}

bool CefForm::VersionPage()
{
	HttpRequest req;
	//111.230.143.87是我自己的云服务器。
	//广告页面http://111.230.143.87/index.html
	wchar_t buffer[128] = { 0 };
	const std::wstring backupServer = L"update.pkpm.cn";
	const std::wstring backupQuery = L"/PKPM2010/Info/index.html";
	GetPrivateProfileStringW(L"Html", L"server", backupServer.c_str(), buffer, sizeof(buffer) - 1, FullPathOfPkpmIni().c_str());
	req.server = buffer;
	memset(buffer, 0, sizeof(buffer));
	if (req.server == backupServer)
		req.query = backupQuery;
	else
	{
		GetPrivateProfileStringW(L"Html", L"query", L"/PKPM2010/Info/index.html", buffer, sizeof(buffer) - 1, FullPathOfPkpmIni().c_str());
		req.query = buffer;
	}
	req.acceptTypes.push_back(L"text/html");
	req.acceptTypes.push_back(L"application/xhtml+xml");
	req.method = L"Get";
	req.port = 80;
	req.secure = false;
	HttpResponse res;
	HttpQuery(req, res);
	if (200 != res.statusCode)
	{
#ifdef _DEBUG
		OutputDebugStringW(L"没有联网功能");
#endif // _DEBUG
		return false;
	}
	////////////else///////////////////
	auto result = res.GetBodyUtf8();
	std::wstring re = result.c_str();
	auto astring = nbase::UnicodeToAnsi(re);
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::regex reg("(<(body)>)([\\s\\S]*)(</\\2>)");
		//vs2010下，tr版本的正则表达式无法匹配"空白符"和字符串的组合
		//字面量中不允许出现空格
		if (_MSC_VER <= 1600)
			astring.erase(std::remove_if(astring.begin(), astring.end(), [](char c) {
			return c == '\n' || c == '\r' || c == ' ';}), 
				astring.end());
		auto re = std::regex_search(astring, reg);
		std::smatch match;
		if (std::regex_search(astring, match, reg))
		{
			pageInfo_ = match[3];
			return true;
		}
	}
	return false;
}

bool CefForm::TellMeNewVersionExistOrNot()
{
	bool isWebPageAvailable = false;
	{
		std::lock_guard<std::mutex> guarder(lock_);
		isWebPageAvailable = isWebPageAvailable_;
	}
	if (!isWebPageAvailable)
		return false;
	else
	{
		Document document;
		document.Parse(pageInfo_.c_str());
		assert(document.HasMember("UpdateUrl"));//旧代码，觉得没用就删除吧
		assert(document.HasMember("Advertise"));
		auto& arr = document["Advertise"]["NationWide"];
		assert(arr.IsArray());
#ifdef ALIME_DEBUG
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			std::string keyName = "Advertisement";
			std::string adver(arr[i][keyName.c_str()].GetString());
		}
#endif // _DEBUG
		int MainVersionOnServer = std::stoi(document["Version"]["MainVersion"].GetString());
		int ViceVersionOnServer = std::stoi(document["Version"]["ViceVersion"].GetString());
		int SubVersionOnServer = std::stoi(document["Version"]["SubVersion"].GetString());
		int mv = -1, vv = -1, sv = 0;
		std::wstring VersionPath = nbase::win32::GetCurrentModuleDirectory() + L"CFG\\";
		auto vec = FindVersionFiles(nbase::UnicodeToAnsi(VersionPath).c_str(), "V", "ini");
		if (!vec.empty())
		{
			getLatestVersion(vec, mv, vv, sv);
			//偷懒
			if (MainVersionOnServer != mv || ViceVersionOnServer != vv || SubVersionOnServer != sv)
				return true;
		}
	}
	return false;
}

std::vector<std::string> CefForm::FindVersionFiles(
	const char* path,
	const char* prefix,
	const char* suffix)
{
	std::string toFind(path);
	toFind += "\\*.";//时间再 一次很赶
	toFind += suffix;
	std::vector<std::string> result;
	long handle;		//用于查找的句柄
	_finddata_t fileinfo;
	handle = _findfirst(toFind.c_str(), &fileinfo);
	if (handle == -1)
		return result;
	do
	{
		if (string_utility::startWith(fileinfo.name, prefix)/* && endWith(fileinfo.name, ".ini")*/)
		{
			std::string filename(fileinfo.name);
			filename = filename.substr(1, filename.size() - 5);
			result.push_back(filename);
		}
	} while (!_findnext(handle, &fileinfo));
	_findclose(handle);
	return result;
}

void CefForm::getLatestVersion(std::vector<std::string>& result,
	int& major_version, int& minor_version, int& sub_version)
{
	if (result.empty())
		throw std::exception("vector empty");
	std::sort(result.begin(), result.end(),
		[](const std::string& s1, const std::string& s2)->bool {
			auto num_in_s1 = string_utility::string_split(s1, ".");
			auto num_in_s2 = string_utility::string_split(s2, ".");
			int size_s1 = num_in_s1.size();
			int size_s2 = num_in_s2.size();
			int n = size_s1 < size_s2 ? size_s1 : size_s2;
			for (int i = 0; i != n; ++i)
			{
				auto num1 = atoi(num_in_s1[i].c_str());
				auto num2 = atoi(num_in_s2[i].c_str());
				if (num1 != num2)
					return num1 - num2 < 0 ? true : false;
			}
			return size_s1 < size_s2 ? true : false;
		});
	auto ret = string_utility::string_split(result.back(), ".");
	if (ret.size() < 3)
	{
		sub_version = 0;
	}
	else
	{
		sub_version = std::stoi(ret[2]);
	}
	major_version = std::stoi(ret[0]);
	minor_version = std::stoi(ret[1]);
}

std::string CefForm::TellMeAdvertisement()
{
	bool isAdvertisementAvailable = false;
	{
		//bool是原子的，但我不清楚直接判断会不会被编译器优化。
		//atomic_bool和mutex择其一即可
		std::lock_guard<std::mutex> guarder(lock_);
		isAdvertisementAvailable = isWebPageAvailable_;
	}
	if (!isAdvertisementAvailable)
		return defaultAdvertise;
	else
	{
		if (pageInfo_.empty())
			return defaultAdvertise;
		else
		{
			std::vector<std::pair<std::string, std::string>> data;
			Document document;
			document.Parse(pageInfo_.c_str());
			auto& arr = document["Advertise"]["NationWide"];
			assert(arr.IsArray());
			for (size_t i = 0; i < arr.Size(); ++i)
			{
				std::string adver_content(arr[i]["Advertisement"].GetString());
				std::string adver_url(arr[i]["Url"].GetString());
				data.push_back(std::make_pair(adver_content, adver_url));
			}
			//////////begin/////////////////////
			Document doc;
			rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

			Value array(kArrayType);//< 创建一个数组对象
			for (int i = 0; i != data.size(); ++i)
			{
				rapidjson::Value obj(rapidjson::kObjectType);
				Value content(kStringType);
				content.SetString(data[i].first.c_str(), allocator);
				obj.AddMember("key", content, allocator);
				Value url(kStringType);
				url.SetString(data[i].second.c_str(), allocator);
				obj.AddMember("value", url, allocator);
				array.PushBack(obj, allocator);
			}

			Value root(kObjectType);
			root.AddMember("data", array, allocator);
			rapidjson::StringBuffer s;
			rapidjson::Writer<rapidjson::StringBuffer> writer(s);
			root.Accept(writer);
			std::string toSend = s.GetString();//for debug
			return s.GetString();
		}
	}
	return defaultAdvertise;
}

/*
程序不可能每次新建工程都检查全部的路径，
我们只需要保证打开文件时，拿到的路径是有效的，
并且每次新建的新工程路径是有效的。这样就可以了。
FileSystem里可能有bug,0.0，但也没人帮测
*/
size_t CefForm::CorrectWorkPath()
{
	std::vector<std::wstring> vec;
	wchar_t prjPathStr[256] = { 0 };
	for (int i = 0; i < maxPrjNum_; ++i)
	{
		auto workPathId = L"WorkPath" + std::to_wstring(i);
		memset(prjPathStr, 0, ArraySize(prjPathStr)*sizeof(wchar_t));
		GetPrivateProfileString(L"WorkPath", workPathId.c_str(), L"error", prjPathStr, ArraySize(prjPathStr), FullPathOfPkpmIni().c_str());
		if (std::wstring_view(prjPathStr) != L"error")
		{
			//fix me, 去掉这个判断
			if (PathFileExists(prjPathStr) && PathIsDirectory(prjPathStr))
			{
				Alime::FileSystem::Folder folder(prjPathStr);
				if (!folder.Exists())	continue;
				std::wstring destPath;
				if (!Alime::FileSystem::PathNameCaseSensitive(folder, destPath))  continue;
				if (destPath.back() != L'\\')
					destPath += L'\\';
				if (std::find(vec.cbegin(), vec.cend(), destPath) == vec.cend())
					vec.push_back(destPath);
			}
		}
	}
	for (int i = 0; i < 2*maxPrjNum_; ++i)
	{
		std::wstring workPathId = L"WorkPath"+std::to_wstring(i);
		if (static_cast<size_t>(i) < vec.size())
			WritePrivateProfileString(L"WorkPath", workPathId.c_str(), vec[i].c_str(), FullPathOfPkpmIni().c_str());
		else//又人可能从12宫格升上来，我们保留前6个有效工程，剩下的删除
			WritePrivateProfileString(L"WorkPath", workPathId.c_str(), NULL, FullPathOfPkpmIni().c_str());
	}
	return vec.size();
}

void CefForm::SetHeightLightIndex(int i)
{
	indexHeightLighted_ = i;
}