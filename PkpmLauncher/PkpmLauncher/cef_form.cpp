#include "pch.h"
#include "cef_form.h"
#include "string_util.h"
#include "MsgDialog.h"
#include "SkinSwitcher.h"
#include "ConfigFileManager.h"

#include "Alime/ProcessInfo.h"
#include "Alime/HttpUtil.h"
#include "Alime/FileSystem.h"
#include "Alime/Console.h"

#include <filesystem>
#include <regex>

#include "RapidjsonForward.h"
#include "SpdlogForward.h"
#include "nlohmann/json.hpp"
using namespace Alime::HttpUtility;
using namespace application_utily;

const std::wstring CefForm::kClassName =
  ConfigManager::GetInstance().GetCefFormClassName();

const char* toRead[] = { "navbarIndex", "parentIndex", "childrenIndex","projectIndex" };



CefForm::CefForm()
	:maxPrjNum_(GetPrivateProfileInt(L"WorkPath", L"MaxPathName",
		6, FullPathOfPkpmIni().c_str())),
	prjPaths_(maxPrjNum_),
	isWebPageAvailable_(false),
	cef_control_(NULL),
	indexHeightLighted_(-1),
	latch_(1)
{
	webDataReader_.Init();
	size_t numofPrj=CorrectWorkPath();
	if (numofPrj > 0)
		indexHeightLighted_ = 0;
	ReadWorkPathFromFile("CFG/pkpm.ini");
	InitSpdLog();
	InitAdvertisement();
}

CefForm::~CefForm()
{
	spdlog::drop_all();
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
	SetWindowText(GetHWND(), ConfigManager::GetInstance().GetCefFormWindowText().c_str());
	InitUiVariable();
	auto cefHtmlPath= nbase::win32::GetCurrentModuleDirectory()+ ConfigManager::GetInstance().GetRelativePathForHtmlRes();
	cef_control_->LoadURL(cefHtmlPath);
	EnableAcceptFiles();
	SetCfgPmEnv();
	appDll_.InitPkpmAppFuncPtr();
	appDll_.Invoke_InitPkpmApp();
	AttachFunctionToShortCut();
	AttachClickCallbackToSkinButton();
	SwicthThemeTo(ConfigManager::GetInstance().GetInterfaceStyleNo());
	ModifyScaleForCaption();
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
		//写一个loop函数，执行下面的逻辑
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
	}
	else if (name == L"btn_wnd_max")
	{
		return false;
	}
	else if (name == L"closebtn")
	{
		//bug出现在Winxp的机器上，
		std::thread t([this]() {
			cef_control_->CallJSFunction(L"currentChosenData",
				nbase::UTF8ToUTF16("{\"uselessMsg\":\"test\"}"),
				ToWeakCallback([this](const std::string& chosenData) {
					OnSetDefaultMenuSelection(chosenData);
					}
			));
		});
		t.detach();
		//丑陋0.0, 但是简单暴力
		Sleep(200);
	}
	return true;
}

LRESULT CefForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_LBUTTONDOWN)
	{
		OutputDebugString(L"Receive WM_LBUTTONDOWN");
	}
	else if (uMsg == WM_DROPFILES)
	{
		OutputDebugString(L"Receive WM_DROPFILES");
		HDROP hDropInfo = (HDROP)wParam;
		UINT count;
		TCHAR filePath[MAX_PATH] = { 0 };
		count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
		if (count != 1)
		{
			MsgBox::Warning(GetHWND(), L"仅支持拖拽单个目录", L"意外的错误");
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
				MsgBox::Warning(GetHWND(), L"仅支持拖拽目录", L"错误");
		}
		DragFinish(hDropInfo);
	}
	else if (uMsg == WM_KEYDOWN)
	{
		if (0x80 == (0x80 & GetKeyState(VK_CONTROL)))
		{
			if ('D' == wParam || 'd' == wParam)
			{
				ConsoleForDebug();
			}
		}
		//提供F5给前端
		if (116 == wParam)
		{
#ifdef DEBUG
			cef_control_->Refresh();
#endif // DEBUG		
		}
	}
	else if (uMsg == WM_SIZE)
	{
		if (m_pRoot)
		{
			FindControl(L"maxbtn")->SetVisible(!::IsZoomed(m_hWnd));
			FindControl(L"restorebtn")->SetVisible(::IsZoomed(m_hWnd));
		}
		if(cef_control_)
			cef_control_->SetFocus();
	}
	else if (uMsg == WM_THEME_SELECTED)
	{
		OutputDebugString(std::to_wstring(wParam).c_str());
		assert(lParam == -1);
		auto ret=SwicthThemeTo(wParam);
		if(ret)
			SaveThemeIndex(wParam);
	}
	else if (uMsg == WM_SHOWMAINWINDOW)
	{
		auto appPath = nbase::win32::GetCurrentModuleDirectory();
		SetCurrentDirectory(appPath.c_str());
		ShowWindow();
	}
	else if (uMsg == WM_MOUSEWHEEL)
	{
		OutputDebugString(L"MOUSEWHEEL");
	}
	else if (uMsg == WM_SETADVERTISEINJS)
	{
		{
			auto advString = TellMeAdvertisement();
			cef_control_->CallJSFunction(L"SetAdvertise",
				nbase::UTF8ToUTF16(advString),
				ToWeakCallback([this](const std::string& chosenData) {
					}
			));
		}
	}
	else if(uMsg == WM_QUERYENDSESSION ||
		uMsg == WM_ENDSESSION)
	{
		{
			auto pid = ProcessInspector::GetCurrentPid();
			auto pidOfChildren = ProcessInspector::GetPidsOfChildProcess(pid);
			for (auto elem : pidOfChildren)
			{
				ProcessInspector::KillProcess(elem);
			}
		}
	}
	return ui::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

bool CefForm::SwicthThemeTo(int index)
{
	nim_comp::Box* vistual_caption = dynamic_cast<nim_comp::Box*>(FindControl(L"vistual_caption"));
	auto sw=SkinSwitcherFatctory::Get(index);
	if (!sw)
		return false;
	sw->Switch(vistual_caption, label_);
	return true;
}

void CefForm::SaveThemeIndex(int index)
{
	auto path=application_utily::FullPathOfPkpmIni();
	ConfigManager::GetInstance().SetInterfaceStyleNo(index);
	WritePrivateProfileStringW(L"InterfaceStyle", L"index",
		std::to_wstring(index).c_str(), path.c_str());
}

void CefForm::OnLoadEnd(int httpStatusCode)
{
	DisplayAuthorizationCodeDate();
}

void CefForm::ModifyScaleForCef()
{
	if (ConfigManager::GetInstance().IsAdaptDpiOn())
		return;
	else if (ConfigManager::GetInstance().IsModifyWindowOn())
	{
		UINT dpi=ui::DpiManager::GetMainMonitorDPI();
		auto scale=MulDiv(dpi, 100, 96);	
		///see www.magpcss.org/ceforum/viewtopic.php?f=6&t=11491
		cef_control_->SetZoomLevel(scale / 100.0f);
		if (cef_control_dev_)
			cef_control_dev_->SetZoomLevel(scale / 100.0f);
	}
}

void CefForm::ModifyScaleForCaption()
{
	if (ConfigManager::GetInstance().IsAdaptDpiOn())
		return;
	else if (ConfigManager::GetInstance().IsModifyWindowOn())
	{
		//fix me, 未完成
		DefaultDpiAdaptor helper;
		AcceptDpiAdaptor(&helper);
		return;
		UINT dpi = ui::DpiManager::GetMainMonitorDPI();
		auto scale = MulDiv(dpi, 100, 96);
		double rate = scale / 100.0;

		RECT rc;
		GetWindowRect(GetHWND(), &rc);
		int width = static_cast<int>((rc.right - rc.left) * rate);
		int height = static_cast<int>((rc.bottom - rc.top) * rate);
		SetWindowPos(GetHWND(), HWND_TOP, 0, 0, width,
			height, SWP_NOMOVE);
		auto captionHeight=vistual_caption_->GetFixedHeight();
		auto captionWidth = vistual_caption_->GetFixedWidth();
		vistual_caption_->SetAttribute(L"height", std::to_wstring(captionHeight * rate));
		rc=GetCaptionRect();
		SetCaptionRect(
			ui::UiRect(0, 0,
				static_cast<int>(captionWidth *rate),
				static_cast<int>(captionHeight *rate)));
	}
}

void CefForm::OnLoadStart()
{
	//早期的demo存在网页调用c++函数时函数还没注册的情况
	RegisterCppFuncs();
	ModifyScaleForCef();
}

void CefForm::RegisterCppFuncs()
{
	//调试窗口，需要加上Toast资源文件夹，以便使用这个对话框。
	cef_control_->RegisterCppFunc(L"ShowMessageBox", 
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(params), 3000, GetHWND());
			callback(false, R"({ "message": "Success0." })");
		})
	);

	cef_control_->RegisterCppFunc(L"ONRCLICKPRJ",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
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
			WebDataVisitorInU8 visitor(filePath);
			webDataReader_.Accept(&visitor);
			auto re = visitor.GetValue();
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
			int Index = document["projectIndex"].GetInt();
			assert(nbase::AnsiToUtf8(prjPaths_[Index]) == captionName);
			SetHeightLightIndex(Index);
			SetCaptionWithProjectName(captionName);
#ifdef DEBUG
			std::string debugStr = R"({ "SetCaption": "Success." })";
			if(callback)
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
			if (!prjPaths_.IsIndexLegal(index))
			{
				if (prjPaths_.empty())
				{
					MsgBox::Warning(GetHWND(), L"没有选择工程", L"错误");
				}
				std::string debugStr = R"({ "invalid index": "failed to call c++ function." })";
				callback(true, debugStr);
				return;
			}
			std::string workDir = prjPaths_[index];
			DataFormatTransfer(ansiMod, ansiExe, workDir);

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
				strAnsi = nbase::StringTrim(strAnsi);
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

	cef_control_->RegisterCppFunc(L"ONGETDEfAULTMENUSELECTION",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			auto selection =OnGetDefaultMenuSelection();
			callback(true, nbase::AnsiToUtf8(selection));
			})
	);

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
			//cef_control_->HideToolTip(); 系统tooltip被弃用
			//你可以像shortCutHandler.cpp里的模板那样处理参数，避免写出现vec
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
				MsgBox::Warning(GetHWND(), L"仅支持拖拽单个目录", L"错误");
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
						std::string debugStr = R"({ "call ONNEWPROJECT": "Success." })";
						callback(true, nbase::AnsiToUtf8(debugStr));
					}
					else
					{
						std::string debugStr = R"({ "call ONNEWPROJECT": "fail." })";
						callback(false, nbase::AnsiToUtf8(debugStr));
					}
				}
				else
				{
					MsgBox::Warning(GetHWND(), L"仅支持拖拽目录", L"错误");
					return;
				}
			}
			DragFinish(hDropInfo);
			})
	);

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
					filePath = std::move(std::string(pBuf));
				}
				CloseClipboard();
			}
			if (!filePath.empty())
			{
				if (PathFileExistsA(filePath.c_str()) && PathIsDirectoryA(filePath.c_str()))
				{
					filePath=nbase::StringTrim(filePath);
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
					//LOG_ERROR<<"invalid path";
					std::string debugStr = R"({ "call ONNEWPROJECT": "FAILED." })";
					callback(false, nbase::AnsiToUtf8(debugStr));
				}
			}
			std::string debugStr = R"({ "call ONNEWPROJECT": "FAILED. empty Path" })";
			callback(false, nbase::AnsiToUtf8(debugStr));
			})
		);

	cef_control_->RegisterCppFunc(L"ONDELETE",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			static unsigned long long  lastTick = 0;
			auto now = GetTickCount();
			auto interval = now - lastTick;
			lastTick = now;
			if (interval < 400)	
				return;
			try {
				if (prjPaths_.size() > 0)
				{
					nlohmann::json json = nlohmann::json::parse(params);
					int prjSelected = json["index"];
					if (prjPaths_.IsIndexLegal(prjSelected))
					{
						//LOG_ERROR << "";
					}
					prjPaths_.deleteAt(prjSelected);
					if (prjPaths_.empty())
					{
						SetHeightLightIndex(-1);
						SetCaptionWithProjectName("");
					}
					SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
					std::string debugStr = R"({ "Call ONNEWPROJECT": "Success." })";
					callback(true, nbase::AnsiToUtf8(debugStr));
				}
			}
			catch (...) {
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
			//auto u8str = nbase::AnsiToUtf8(advertisement);
			callback(true, advertisement);
			})
	);

	cef_control_->RegisterCppFunc(L"OPENURL",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			static unsigned long long  lastTick = 0;
			auto now = GetTickCount();
			auto interval = now - lastTick;
			lastTick = now;
			if (interval < 200)
			{
				MsgBox::Warning(GetHWND(), L"你的操作过于频繁", L"警告");
				return;
			}			
			nlohmann::json json = nlohmann::json::parse(params);
			try {
				std::wstring url = nbase::UTF8ToUTF16(json["adUrl"]);
				::ShellExecute(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWDEFAULT);
				std::string debugStr = R"({ "OPENURL": "Always Success." })";
				callback(true, debugStr);
			}
			catch (...)
			{
				MsgBox::Warning(GetHWND(), L"无效的参数", L"无法打开网页");
			}
			})
	);

	//标题栏暂时只支持修改背景色和文字颜色
	//这个设置被我定义在resources\themes\default\cef\caption_style.xml中
	//网页可以调用本接口切换风格
	cef_control_->RegisterCppFunc(L"SetInterfaceStyle",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			nlohmann::json json = nlohmann::json::parse(params);
			int styleNo = json["index"];
			bool ret=this->SwicthThemeTo(styleNo);
			if(ret)
				this->SaveThemeIndex(styleNo);
			})
	);

}


/*
这是个旧函数，每次都读配置文件，之所以没有
删除是因为，它曾经帮了大忙0.0
(比方说，它意外地处理了网页刷新造成的内存/文件不匹配，用户使用程序过程中手动改配置文件)，
我保留它大概是希望获得额外的心理安慰。
*/
//fileName并不受我控制
std::string CefForm::ReadWorkPathFromFile(const std::string& filename)
{
	prjPaths_.clear();
	const std::string fullpath = nbase::UnicodeToAnsi(nbase::win32::GetCurrentModuleDirectory()) + filename.c_str();

	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	rapidjson::Value array(rapidjson::kArrayType);//< 创建一个数组对象
	std::vector<std::string> vec;
	char prjPathStr[1024] = { 0 };
	for (int i = 0; i < maxPrjNum_; ++i)
	{
		auto workPathId = "WorkPath" + std::to_string((ULONGLONG)i);
		memset(prjPathStr, 0, ArraySize(prjPathStr));
		auto nRead = GetPrivateProfileStringA("WorkPath", workPathId.c_str(), "error", prjPathStr, ArraySize(prjPathStr), fullpath.c_str());
		if (!strcmp("error", prjPathStr))
		{
			//prjPathStr=="error"表明用户手动修改了配置文件!
			continue;
		}
		else if (prjPathStr[nRead - 1] != '\\')
		{
			continue;
		}
		std::string timeStamp;
		auto ret = GetPrjInfo(prjPathStr, timeStamp);
		rapidjson::Value obj(rapidjson::kObjectType);
		if (ret)
		{
			vec.emplace_back(prjPathStr);
			rapidjson::Value key(rapidjson::kStringType);
			key.SetString(workPathId.c_str(), allocator);
			rapidjson::Value value(rapidjson::kStringType);
			value.SetString(prjPathStr, allocator);
			obj.AddMember("WorkPath", value, allocator);

			value.SetString(timeStamp.c_str(), allocator);
			obj.AddMember("Date", value, allocator);

			std::string bmpPath = prjPathStr;
			bmpPath += "BuildUp.bmp";
			if (IsSnapShotExist(bmpPath))
			{
				bmpPath = "file://" + bmpPath;//网页需要增加这个前缀
				bmpPath = FileEncode(bmpPath);
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
	{
		return false;
	}
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
	else
	{
		//winxp 下 stat系列函数都会出现bug。我先保留上面的分支
		//search " _stat not working on Windows XP"
		WIN32_FILE_ATTRIBUTE_DATA fileData;
		GetFileAttributesExA(pathStr.c_str(), GetFileExInfoStandard, &fileData);
		SYSTEMTIME systemTime;
		bool res = FileTimeToSystemTime(&fileData.ftCreationTime, &systemTime);
		if (res)
		{
			char buf[128] = { 0 };
			sprintf(buf, "%4d年%02d月%02d日 %02d:%02d",
				systemTime.wYear, systemTime.wMonth, systemTime.wDay,
				systemTime.wHour, systemTime.wMinute);
			timestamp=buf;
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

void CefForm::SetCaptionWithProjectName(const std::string& prjName)
{
	std::wstring captionWithPrefix;
	if (defaultCaption_.empty())
		captionWithPrefix = L"PKPM结构设计软件 10版 V5.1.3   ";
	else
		captionWithPrefix = defaultCaption_;
	if (prjName.empty())
	{
		label_->SetText(defaultCaption_);
		return;
	}
		
	auto prjNameU16 = nbase::UTF8ToUTF16(prjName);	
	captionWithPrefix += Alime::FileSystem::GetAbbreviatedPath(prjNameU16);
	label_->SetText(captionWithPrefix);
}

std::string CefForm::OnGetDefaultMenuSelection()
{
	std::vector<std::pair<std::string, std::string>> dict;
	const int n = sizeof(toRead) / sizeof(*toRead);
	char indexRet[32] = { 0 };
	for (size_t i = 0; i != n; ++i)
	{
		memset(indexRet, 0, sizeof(indexRet));
		GetPrivateProfileStringA("Html", toRead[i], "error", indexRet, sizeof(indexRet),
			nbase::UnicodeToAnsi(FullPathOfPkpmIni()).c_str());
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

std::string CefForm::OnNewProject()
{
	int useDevelopVersion = 0;
	useDevelopVersion = ConfigManager::GetInstance().IsSystemFolderDialogOn();
	if (!useDevelopVersion)
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
		std::string defaultPath;
		if (-1==indexHeightLighted_ || !prjPaths_.size())
		{
			OutputDebugString(L"dangerous operation");
		}
		else
			defaultPath = prjPaths_[indexHeightLighted_];
		ptr(defaultPath.c_str(), result);
		FreeLibrary(hdll);
		return result;
	}
	else
	{
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
	}
	return {};
}

void CefForm::OnRightClickProject(const std::wstring& prjName)
{
	if (PathFileExists(prjName.c_str()) && PathIsDirectory(prjName.c_str()))
	{
		application_utily::OpenDocument(prjName);
	}
	else
		MsgBox::Warning(GetHWND(), L"工程目录无法打开，可能已经被删除" ,L"路径错误");
}

//有问题,我应该让前端调用时就把工程路径和索引一块过来
void CefForm::DataFormatTransfer(const std::string& module, const std::string& app, const std::string& workdir)
{
	if (!prjPaths_.size())
	{
		MsgBox::Warning(GetHWND(), L"请先选择工作目录", L"错误");
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
		ShowWindow();	
	}
}

void CefForm::OnDbClickProject(const std::vector<std::string>& args)
{
	if (args.size() != 5)
	{
		MsgBox::Warning(GetHWND(), L"打开工程的参数不正确", L"严重错误");
	}
	if (!prjPaths_.size())
	{
		MsgBox::Warning(GetHWND(), L"工作目录不存在", L"严重错误");
		return;
	}
	std::string path(args[0]);
	{
		ShowWindow(false);
		auto ret = SetCurrentDirectoryA(path.c_str());
		if (!ret)
		{
			MsgBox::Warning(GetHWND(), L"工作目录错误或者没有权限", L"权限错误");
		}
		for (int i = 0; i != prjPaths_.size(); ++i)
		{
			if (prjPaths_[i] == path)
			{
				prjPaths_.moveToFront(i);
				SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
				break;
			}
		}
		std::thread t([=]() {
			SetCurrentDirectoryA(path.c_str());
			run_cmd(args[3].c_str(), args[4].c_str(), "");
			::PostMessage(m_hWnd, WM_SHOWMAINWINDOW, 0, 0);
			});
		t.detach();
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
	auto wstr = nbase::AnsiToUnicode(FullPath);
	application_utily::OpenDocument(wstr);
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
		else
		{
			ret=WritePrivateProfileStringA("WorkPath", workPathId.c_str(), NULL, filename.c_str());
		}
		if (!ret)
			hasAdministratorsRights = false;
	}
	if (!hasAdministratorsRights)
	{
		MsgBox::Warning(GetHWND(), L"无法保存工程信息，建议使用管理员权限打开本软件", L"权限错误");
	}
		
}

std::string CefForm::PathChecker(const std::string& newProj, bool &legal)
{
	std::string realPathOfNewProj;
	legal = true;
	Alime::FileSystem::Folder folder(nbase::AnsiToUnicode(newProj));
	std::wstring realPathw;
	if (!folder.Exists() || 
		!Alime::FileSystem::PathNameCaseSensitive(folder, realPathw))
	{
		legal = false;
		return {};
	}
	realPathOfNewProj = nbase::UnicodeToAnsi(realPathw);
	if (realPathOfNewProj.back() != '\\')
		realPathOfNewProj += '\\';
	if (realPathOfNewProj.length() == 3&& realPathOfNewProj[1]==':')//盘符
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
	}
	else
	{
		auto index = std::distance(prjPaths_.begin(), iter);
		prjPaths_.moveToFront(index);
	}
	SaveWorkPaths(prjPaths_, filename);
	return true;
}

void CefForm::OnSetDefaultMenuSelection(const std::string& json_str)
{
	std::string str(nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json_str)));
	rapidjson::Document d;
	d.Parse(str.c_str());
	size_t i = 0;
	for (; i != ArraySize(toRead) - 1; ++i)
	{
		rapidjson::Value& s = d[toRead[i]];
		auto indexSelection = s.GetInt();
		WritePrivateProfileStringA("Html", toRead[i], std::to_string(indexSelection).c_str(), nbase::UnicodeToAnsi(FullPathOfPkpmIni()).c_str());
	}
	//数据结构是前端定的，所以我其实没有选择, 工程索引对我来说没有什么意义，但我还是要管收\发\存。
	rapidjson::Value& s = d[toRead[i]];
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
	char str[128] = { 0 };
	if (appName2.IsEmpty())
	{
		sprintf(str, "%s|%s", moduleName.GetString(), appName1.GetString());
	}
	else
	{
		sprintf(str, "%s|%s_%s", moduleName.GetString(), appName1.GetString(), appName2.GetString());
	}
	appDll_.Invoke_RunCommand(str);
}

//旧代码，vc程序员的素质堪忧，我大约有60的时候是用来擦屎
bool CefForm::SetCfgPmEnv()
{
	const int LENGTH_OF_ENV = 1024 * 8;
	TCHAR* szOriEnvPath = new  TCHAR[LENGTH_OF_ENV];//所以这片内存去哪了....?这个屎我不擦
	DWORD dwRet = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, LENGTH_OF_ENV);
	if (!dwRet)
	{
		::AfxMessageBox(L"Error! Can not find Path", MB_OK | MB_SYSTEMMODAL);
		return false;
	}
	else if (LENGTH_OF_ENV < dwRet)
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
	//fix me, not safe
	std::thread func(std::bind(&CefForm::AdvertisementThreadFunc, this));
	func.detach();
}

void CefForm::AdvertisementThreadFunc()
{
	assert(isWebPageAvailable_ == false);
	bool AdPageCanAccess = false;
	//网页不是我在维护
	try
	{
		AdPageCanAccess = GetVersionPage();
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

//我们的广告原页面是https://update.pkpm.cn/PKPM2010/Info/index.html
bool CefForm::GetVersionPage()
{
	HttpRequest req;
	req.server = ConfigManager::GetInstance().GetAdvertisementServer();
	req.query = ConfigManager::GetInstance().GetAdvertisementQuery();
	req.acceptTypes.push_back(L"text/html");
	req.acceptTypes.push_back(L"application/xhtml+xml");
	req.method = L"Get";
	req.port = 80;
	req.secure = false;
	HttpResponse res;
	HttpQuery(req, res);
	if (200 != res.statusCode)
	{
		return false;
	}
	////////////else///////////////////
	auto result = res.GetBodyUtf8();
	std::wstring re = result.c_str();
	auto astring = nbase::UnicodeToAnsi(re);
	{
		std::regex reg("(<(body)>)([\\s\\S]*)(</\\2>)");
		//vs2010下，tr版本的正则表达式无法匹配"空白符"和字符串的组合
		if (_MSC_VER <= 1600)
			astring.erase(std::remove_if(astring.begin(), astring.end(), [](char c) {
			return c == '\n' || c == '\r' || c == ' ';}), 
				astring.end());
		auto re = std::regex_search(astring, reg);
		std::smatch match;
		if (std::regex_search(astring, match, reg))
		{
			pageInfo_ = match[3];
			//考虑用户内网网页重定向的问题,简单处理即可
			if (pageInfo_.find("UpdateUrl") == std::string::npos ||
				pageInfo_.find("Version") == std::string::npos)
				return false;
			return true;
		}
	}
	return false;
}

bool CefForm::TellMeNewVersionExistOrNot()
{
	bool isWebPageAvailable = false;
	{
		isWebPageAvailable = isWebPageAvailable_;
	}
	if (!isWebPageAvailable)
		return false;
	else
	{
		rapidjson::Document document;
		document.Parse(pageInfo_.c_str());
		assert(document.HasMember("UpdateUrl"));
		assert(document.HasMember("Advertise"));
		auto& arr = document["Advertise"]["NationWide"];
		assert(arr.IsArray());
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
	toFind += "\\*.";
	toFind += suffix;
	std::vector<std::string> result;
	long handle;
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
		isAdvertisementAvailable = isWebPageAvailable_;
	}
	if (!isAdvertisementAvailable)
		return ConfigManager::GetInstance().GetDefaultAdvertise();
	else
	{
		if (pageInfo_.empty())
			return ConfigManager::GetInstance().GetDefaultAdvertise();
		else
		{
			std::vector<std::pair<std::string, std::string>> data;
			rapidjson::Document document;
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
			rapidjson::Document doc;
			rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

			rapidjson::Value array(rapidjson::kArrayType);//< 创建一个数组对象
			for (int i = 0; i != data.size(); ++i)
			{
				rapidjson::Value obj(rapidjson::kObjectType);
				rapidjson::Value content(rapidjson::kStringType);
				content.SetString(data[i].first.c_str(), allocator);
				obj.AddMember("key", content, allocator);
				rapidjson::Value url(rapidjson::kStringType);
				url.SetString(data[i].second.c_str(), allocator);
				obj.AddMember("value", url, allocator);
				array.PushBack(obj, allocator);
			}
			rapidjson::Value root(rapidjson::kObjectType);
			root.AddMember("data", array, allocator);
			rapidjson::StringBuffer s;
			rapidjson::Writer<rapidjson::StringBuffer> writer(s);
			root.Accept(writer);
			std::string toSend = s.GetString();//for debug
			return nbase::AnsiToUtf8(s.GetString());
		}
	}
	return ConfigManager::GetInstance().GetDefaultAdvertise();
}

//大致修一下路径
size_t CefForm::CorrectWorkPath()
{
	std::vector<std::wstring> vec;
	wchar_t prjPathStr[1024] = { 0 };
	for (int i = 0; i < maxPrjNum_; ++i)
	{
		auto workPathId = L"WorkPath" + std::to_wstring(i);
		memset(prjPathStr, 0, ArraySize(prjPathStr)*sizeof(wchar_t));
		GetPrivateProfileString(L"WorkPath", workPathId.c_str(), L"error",
			prjPathStr, ArraySize(prjPathStr), FullPathOfPkpmIni().c_str());
		if (std::wstring_view(prjPathStr) != L"error")
		{
			//fix me, 去掉这个判断
			if (PathFileExists(prjPathStr) && PathIsDirectory(prjPathStr))
			{
				Alime::FileSystem::Folder folder(prjPathStr);
				if (!folder.Exists())
					continue;
				std::wstring destPath;
				if (!Alime::FileSystem::PathNameCaseSensitive(folder, destPath))  continue;
				if (destPath.back() != L'\\')
					destPath += L'\\';
				if (std::find(vec.cbegin(), vec.cend(), destPath) == vec.cend())
					vec.push_back(destPath);
			}
		}
	}
	//我们检查更多的工程，保留前6个有效工程，强迫用户使用新版本
	for (int i = 0; i < 2*maxPrjNum_; ++i)
	{
		std::wstring workPathId = L"WorkPath"+std::to_wstring(i);
		if (static_cast<size_t>(i) < vec.size())
			WritePrivateProfileString(L"WorkPath", workPathId.c_str(), vec[i].c_str(), FullPathOfPkpmIni().c_str());
		else
			WritePrivateProfileString(L"WorkPath", workPathId.c_str(), NULL, FullPathOfPkpmIni().c_str());
	}
	return vec.size();
}

void CefForm::SetHeightLightIndex(const int _i)
{
	indexHeightLighted_ = _i;
}

LRESULT CefForm::OnNcLButtonDbClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!::IsZoomed(GetHWND()))
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	return 0;
}

void CefForm::EnableAcceptFiles()
{
	//离屏渲染模式下，似乎需要手动开启拖拽文件支持
	LONG style = ::GetWindowLong(this->m_hWnd, GWL_EXSTYLE);
	auto hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	style |= WS_EX_ACCEPTFILES;
	hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	::SetWindowLongW(this->m_hWnd, GWL_EXSTYLE, style);
}

void CefForm::AttachClickCallbackToSkinButton()
{
	skinSettings_->AttachClick([this](ui::EventArgs* args) {
		RECT rect = args->pSender->GetPos();
		ui::CPoint point;
		point.x = rect.left - 130;
		point.y = rect.top + 10;
		ClientToScreen(m_hWnd, &point);

		nim_comp::CMenuWnd* pMenu = new nim_comp::CMenuWnd(NULL);
		ui::STRINGorID xml(L"settings_menu.xml");
		pMenu->Init(xml, _T("xml"), point);
		ui::ListBox* pVbox = dynamic_cast<ui::ListBox*>(pMenu->FindControl(L"themeWindow"));
		pVbox->AttachSelect([this](ui::EventArgs* args) {
			auto themeType = args->pSender->GetName();
			int current = args->wParam;
			assert(args->lParam == -1);
			PostMessage(WM_THEME_SELECTED, current, -1);
			return true;
			});
		return true;
		});
}

int CefForm::RemainingTimeOfUserLock(std::string* SerialNumber)
{
	//参数:@ 模块号, @子模块号 @返回的授权码
	//返回值: 剩余天数
	typedef int(_stdcall *FuncInWinAuthorize)(int&, int&, char*);
	auto pathOfWinAuthorize = nbase::win32::GetCurrentModuleDirectory()+L"Ribbon\\WinAuthorize.dll";
	auto handle = LoadLibrary(pathOfWinAuthorize.c_str());
	if (!handle)
		return -1;
	FuncInWinAuthorize dayLeftFunc = NULL;
	dayLeftFunc = (FuncInWinAuthorize)GetProcAddress(handle, "_Login_SubMod2@12");
	if (!dayLeftFunc)
	{
		FreeLibrary(handle);
		return -1;
	}		
	int ty = 100;
	int sub_ky = 0;
	char gSN[17] = { 0 };
	int dayLeft = 0;
	dayLeft=dayLeftFunc(ty, sub_ky, gSN);
	if(SerialNumber!=NULL)
		*SerialNumber = gSN;
	FreeLibrary(handle);
	return dayLeft;
}

void CefForm::InitUiVariable()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefForm::OnClicked, this, std::placeholders::_1));
	cef_control_ = dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control"));
	cef_control_dev_ = dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control_dev"));
	btn_dev_tool_ = dynamic_cast<ui::Button*>(FindControl(L"btn_dev_tool"));
	label_ = dynamic_cast<ui::Label*>(FindControl(L"projectName"));
	//标题从xml的控件里读，很合理...
	defaultCaption_ = label_->GetText();
	skinSettings_ = dynamic_cast<ui::Button*>(FindControl(L"settings"));
	vistual_caption_ = dynamic_cast<ui::HBox*>(FindControl(L"vistual_caption"));
	this_window_ = dynamic_cast<ui::Window*>(FindControl(L"main_wnd"));
	cef_control_->AttachLoadStart(nbase::Bind(&CefForm::OnLoadStart, this));
	cef_control_->AttachLoadEnd(nbase::Bind(&CefForm::OnLoadEnd, this, std::placeholders::_1));
	if (cef_control_dev_)
		cef_control_->AttachDevTools(cef_control_dev_);
	if (nim_comp::CefManager::GetInstance()->IsEnableOffsetRender())
	{
		if (cef_control_dev_)
		{
#ifdef DEBUG
			cef_control_dev_->SetVisible(true);
#else
			cef_control_dev_->SetVisible(false);
#endif
		}
	}
}

void CefForm::AttachFunctionToShortCut()
{
	CallBack f = [this]() {
		if (prjPaths_.IsIndexLegal(indexHeightLighted_))
			prjPaths_.moveToFront(indexHeightLighted_);
		if (!prjPaths_.empty())
			SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
		cef_control_->CallJSFunction(L"flush",
			nbase::UTF8ToUTF16("{\"uselessMsg\":\"test\"}"),
			ToWeakCallback([this](const std::string& chosenData) {
				}
		));
	};
	this->shortCutHandler_.SetCallBacks(GetHWND(), f);
	shortCutHandler_.Init();
}

void CefForm::ConsoleForDebug()
{
	//一个控制台，给王工调试接口
	Alime::Console::CreateConsole();
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	Alime::Console::SetWindowPosition(rc.right-20, rc.top+20);
	Alime::Console::SetWindowSize(400, rc.bottom - rc.top-28);
	Alime::Console::SetTitle(L"Alime");
	Alime::Console::SetColor(Alime::Console::CYAN);
	std::thread t([this]() {
		while (true)
		{
			auto str = Alime::Console::ReadLine();
			if (str.find(L"SetCaption") != std::wstring::npos)
			{
				auto result = str.substr(10);
				SetCaptionWithProjectName(u8"PKPM结构设计软件 10版 V5.1.1    " + nbase::UTF16ToUTF8(result));
			}
			else if (str.find(L"Clear") != std::wstring::npos)
			{
				Alime::Console::Clear();
			}
			else if (str.find(L"Lock") != std::wstring::npos)
			{
				std::string authorizationCode;
				auto daysLeft = RemainingTimeOfUserLock(&authorizationCode);
				if (daysLeft == -1)
				{
					Alime::Console::SetColor(Alime::Console::RED);
					Alime::Console::WriteLine(L"未知的错误");
					Alime::Console::SetColor(Alime::Console::CYAN);
				}					
				else
				Alime::Console::WriteLine(nbase::AnsiToUnicode(authorizationCode)+L"\n剩余天数:"+
					std::to_wstring(daysLeft));
			}
			else if (str.find(L"Quit") != std::wstring::npos)
			{
				break;
			}
			else 
			{
				Alime::Console::SetColor(1, 0, 0, 0);
				Alime::Console::WriteLine(L"没有这个接口");
				Alime::Console::SetColor(Alime::Console::CYAN);
			}
		}
		FreeConsole();
		});
	t.detach();
}

void CefForm::InitSpdLog()
{	
	auto logFolderW = nbase::win32::GetCurrentModuleDirectory() + L"resources\\Logs\\";
	auto logFileNameW = logFolderW + L"log.txt";
	std::string LogsFile = nbase::UnicodeToAnsi(logFileNameW);
	if (PathFileExists(logFileNameW.c_str()) && !PathIsDirectory(logFileNameW.c_str()))//多余，存在就不可能是文件夹
	{
		bool ret=DeleteFile(logFileNameW.c_str());
		if (!ret)
		{
			return;
		}
	}
	{
		Alime::FileSystem::Folder p(logFolderW);
		if (!p.Exists())
			p.Create(false);
	}
	try {
		auto file_logger = spdlog::basic_logger_mt("fileLogger", LogsFile.c_str());
		file_logger->set_pattern("[%Y%m%d %H:%M:%S.%f] [%t] [%l] [%v]");
		file_logger->flush_on(spdlog::level::trace);
		spdlog::set_default_logger(file_logger);
		spdlog::set_level(spdlog::level::trace);
#ifdef DEBUG
		spdlog::info("Support for floats {:03.2f}", 1.23456);
		spdlog::critical("Support for int: {1:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42, 33);
		spdlog::info("Support for floats {1: 03.2f} and {1:03.2f}", 1.23456, 1.33333);
		spdlog::info("Support for floats {0: 03.2f} and {1: 3.2f} {2}", 1.44456, 1.33333, "too");
		spdlog::info("Positional args are {1} {0}..", "too", "supported");	
#else
#endif // DEBUG	
	}
	catch(...){
		//can not open logFile
		return;
	}
}

void CefForm::AcceptDpiAdaptor(IAdaptor* acc)
{
	acc->AdaptCaption(this);
}

ui::Label* CefForm::GetCaptionLabel()
{
	return this->label_;
}

ui::HBox* CefForm::GetCaptionBox()
{
	return this->vistual_caption_;
}

nim_comp::CefControlBase* CefForm::GetCef()
{
	return this->cef_control_;
}

//fix me. 把判断如何显示字符串的逻辑放到js里显然是不合适的
//但又有一定灵活性。
//那么，现在的解决方案就是单机锁小于某个天数，必然显示
void CefForm::DisplayAuthorizationCodeDate()
{
	std::thread checkThread([this]() {
		std::string sn;
		int daysLeft = RemainingTimeOfUserLock(&sn);
		if (daysLeft >= 0 && daysLeft < ConfigManager::GetInstance().DaysLeftToNotify())
		{
			nlohmann::json json;
			json["dayLeft"] = daysLeft;
			auto toSend = nbase::UTF8ToUTF16(json.dump());
			cef_control_->CallJSFunction(L"showLicenseKey",
				toSend,
				ToWeakCallback([this](const std::string& chosenData) {
					}
			));
		}
		});
	checkThread.detach();
}