#include "pch.h"

#include<cmath>

#include <filesystem>
#include <regex>
#include <chrono>
#include <memory>

#include "RapidjsonForward.h"
#include "SpdlogForward.h"
#include "nlohmann/json.hpp"
#include "Alime/HttpUtil.h"
#include "Alime/FileSystem.h"
#include "Alime/Console.h"
#include "Alime/IteratorHelper.h"
#include "Alime/ScopeGuard.h"

#include "cef_form.h"
#include "string_util.h"
#include "SkinSwitcher.h"
#include "ConfigFileManager.h"
#include "VersionCmpStrategy.h"
#include "FileDialog.h"
#include "Macros.h"
#include "DuiDialog/circleDialog.h"

using namespace Alime::HttpUtility;
using namespace application_utily;

const char* toRead[] = { "navbarIndex", "parentIndex", "childrenIndex","projectIndex" };
const std::wstring CefForm::kClassName= ConfigManager::GetInstance().GetCefFormClassName();

CefForm::CefForm()
	:maxPrjNum_(GetPrivateProfileInt(L"WorkPath", L"MaxPathName", 8, FullPathOfPkpmIni().c_str())),
	prjPaths_(maxPrjNum_),
	cef_control_(NULL),
	indexHeightLighted_(-1),
	pool_("A girl lost her name, Arya")
{
	webDataReader_.Init();
	size_t numofPrj=CorrectWorkPath();
	indexHeightLighted_ = numofPrj > 0 ? 0 : -1;
	InitSpdLog();
	AppendThreadTask();
}

void CefForm::AppendThreadTask()
{
	pool_.SetMaxQueueSize(4);
	pool_.Start(2);
	pool_.Run(std::bind(&WebPageDownLoader::Run, &webPageData_, std::function<void()>()));
	pool_.Run(std::bind(&WebArticleReader::Run, &webArticleReader_, [this]() {webArticleReader_.Init();}));
}

CefForm::~CefForm()
{
	spdlog::drop_all();
}

std::wstring CefForm::GetSkinFolder()
{
	return ConfigManager::GetInstance().GetSkinFolderPath();
}

std::wstring CefForm::GetSkinFile()
{
	return DpiAdaptorFactory::GetAdaptor()->SelectSkinFile();
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
	cef_control_->LoadURL(nbase::win32::GetCurrentModuleDirectory() + ConfigManager::GetInstance().GetRelativePathForHtmlRes());
	EnableAcceptFiles();
	SetCfgPmEnv();
	appDll_.InitPkpmAppFuncPtr();
	appDll_.Invoke_InitPkpmApp();
	AttachFunctionToShortCut();
	AttachClickCallbackToSkinButton();
	SwicthThemeTo(ConfigManager::GetInstance().GetGuiStyleInfo().first);
}

LRESULT CefForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	nim_comp::CefManager::GetInstance()->PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

bool CefForm::OnClicked(ui::EventArgs* msg)
{
	if (msg->pSender->GetName() == L"closebtn")
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
		Sleep(200);//丑陋0.0, 但是简单暴力
	}
	return true;
}

//Fix me, windowImpBase里有大部分的处理函数，窗口级的处理最好放到函数里
LRESULT CefForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DROPFILES)
	{
		HDROP hDropInfo = (HDROP)wParam;
		UINT count;
		wchar_t filePath[MAX_PATH] = { 0 };
		count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
		if (count != 1)
		{
			MsgBox::WarningViaID(GetHWND(), L"ERROR_TIP_DRAG_SINGLE_FOLDER", L"TITLE_ERROR");
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
					ToWeakCallback([this](const std::string& /*chosenData*/) {
						}
				));
			}
			else
				MsgBox::WarningViaID(GetHWND(), L"ERROR_TIP_DRAG_FOLDER_ONLY", L"TITLE_ERROR");
		}
		DragFinish(hDropInfo);
	}
	else if (uMsg == WM_KEYDOWN)
	{
		if (VK_F5 == wParam && ConfigManager::GetInstance().IsWebPageRefreshOn())
			cef_control_->Refresh();
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
		assert(lParam == -1);
		auto ret=SwicthThemeTo(wParam);
		if(ret)
			SaveThemeIndex(wParam, L"defaultStyle");
	}
	else if (uMsg == WM_SHOWMAINWINDOW)
	{
		auto appPath = nbase::win32::GetCurrentModuleDirectory();
		SetCurrentDirectory(appPath.c_str());
		spdlog::critical("Receive WM_SHOWMAINWINDOW");
		ShowWindow();
	}
	else if (uMsg == WM_SHOWAUTHORIZE)
	{	
		nlohmann::json json;
		auto ptr = (AuthorizationCodeDate*)wParam;
		if (-1 != ptr->data_.daysLeft_)
		{
			json["daysLeft"] = ptr->data_.daysLeft_;
			auto toSend = nbase::UTF8ToUTF16(json.dump());
			cef_control_->CallJSFunction(L"showLicenseKey", toSend,
				ToWeakCallback([this](const std::string& /*dummyFunction*/) {
					}
			));
		}
		delete ptr;
	}
	else if(uMsg == WM_QUERYENDSESSION ||uMsg == WM_ENDSESSION)
	{
		auto pid = ProcessHelper::GetCurrentPid();
		auto pidOfChildren = ProcessIterator::GetPidsOfChildProcess(pid);
		for (auto elem : pidOfChildren)
		{
			ProcessHelper::KillProcess(elem);
		}
	}
	else if (uMsg == WM_TRAY)
	{
		if (lParam == WM_LBUTTONDOWN)
		{
			ShowWindow(true);
		}
		else if (lParam == WM_RBUTTONDOWN)
		{
			if (0)
			{
				ui::CPoint point;
				GetCursorPos(&point);
				point.x = point.x + 10;
				point.y = point.y - 100;

				pMenu_ = new nim_comp::CMenuWnd(NULL);
				ui::STRINGorID xml(L"settings_menu.xml");
				pMenu_->Init(xml, _T("xml"), point);
				pMenu_->Show();
				ui::ListBox* pVbox = dynamic_cast<ui::ListBox*>(pMenu_->FindControl(L"themeWindow"));
				if (!pVbox)
					return false;
				pVbox->AttachSelect([this](ui::EventArgs* args) {
					auto themeType = args->pSender->GetName();
					PostMessage(WM_THEME_SELECTED, args->wParam, args->lParam);//lParam should always -1
					return true;
					}
				);
			}
		}
	}
	return ui::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

bool CefForm::SwicthThemeTo(int index)
{
	if (index < 0)
	{
		spdlog::critical("bad theme index: {0:d}", index);
		return false;
	}
	if(1)//fix me。皮肤管理这块主动权应该放在我这，或者网页提供换肤接口
	{
		auto wrapedCap=SkinFatctory().GetWrappedCaption(this, index);
		if (!wrapedCap)
			return false;
		wrapedCap->ReDraw();
		return true;
	}
	auto sw = SkinSwitcherFatctory::Get(index);
	if (!sw)
		return false;
	sw->Switch(vistual_caption_, label_);
	return true;
}

void CefForm::SaveThemeIndex(int index, const std::wstring& name)
{
	ConfigManager::GetInstance().SetGuiStyleInfo({ index,name });
}

void CefForm::OnLoadEnd(int /*httpStatusCode*/)
{
	auto lockDatePtr = new AuthorizationCodeDate();
	pool_.Run(std::bind(&AuthorizationCodeDate::Run, lockDatePtr, [this, lockDatePtr]() {
		auto ret=PostMessageW(WM_SHOWAUTHORIZE, WPARAM(lockDatePtr), 0);
		if (!ret)
			delete lockDatePtr;
		}));
}

//早期的demo出现 网页调用c++函数时函数还没注册
void CefForm::OnLoadStart()
{
	RegisterCppFuncs();
	//fix me
	AcceptDpiAdaptor(DpiAdaptorFactory::GetAdaptor().get());
}

//不要试图以json来标识函数
void CefForm::RegisterCppFuncs()
{
	cef_control_->RegisterCppFunc(L"ONRCLICKPRJ",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			std::string filePath = document["projectName"].GetString();
			OnRightClickProject(nbase::UTF8ToUTF16(filePath));
			callback(true, R"({ "message": "ONRCLICKPRJ called successfully"})");
			})
	);

	cef_control_->RegisterCppFunc(L"CONFIGFILES",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			std::string filePath=document["filePath"].GetString();
			//fix me， 极度不合理的需求，导致硬编码函数要改名了
			if (string_utility::endWith(filePath.c_str(), "Dummy.xml"))
			{
				std::string lastQuery = webDataReader_.LastQuery();
				if (!lastQuery.empty())
					OpenBimExe();			
			}
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
			std::string debugStr = R"({ "message": "Function SetCaption called successfully." })";
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
			std::string debugStr = R"({ "message": "Function SHORTCUT called successfully." })";
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
			std::string debugStr = R"({ "message": "OPENDOCUMENT always called successfully ." })";
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
					MsgBox::WarningViaID(GetHWND(), L"ERROR_TIP_NO_WORKFOLDER", L"TITLE_ERROR");
				}
				std::string debugStr = R"({ "message": "invalid index." })";
				callback(false, debugStr);
				return;
			}
			std::string workDir = prjPaths_[index];
			DataFormatTransfer(ansiMod, ansiExe, workDir);

			std::string debugStr = R"({ "message": "Pass me the projectName or index." })";
			callback(true, debugStr);
			}
		)
	);

	cef_control_->RegisterCppFunc(L"ONNEWPROJECT",
		ToWeakCallback([this](const std::string& /*params*/, nim_comp::ReportResultFunction callback) {
			auto strAnsi = OnNewProject();
			if (strAnsi.empty())
			{
				std::string debugStr = R"({ "message": "call ONNEWPROJECT Failed." })";
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
				std::string debugStr = R"({ "message": "call ONNEWPROJECT successed." })";
				callback(true, str);
				return;
			}
			}
		)
	);

	//fix me, ONGETDEfAULTMENUSELECTION 里有个小写f....
	cef_control_->RegisterCppFunc(L"ONGETDEfAULTMENUSELECTION",
		ToWeakCallback([this](const std::string& /*params*/, nim_comp::ReportResultFunction callback) {
			auto selection =OnGetDefaultMenuSelection();
			callback(true, nbase::AnsiToUtf8(selection));
			})
	);

	cef_control_->RegisterCppFunc(L"DBCLICKPROJECT",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {		
			//旧代码，实际上只需要3个参数, 为了不给前端造成疑惑，就不改了
			try {
				nlohmann::json json = nlohmann::json::parse(params);
				std::string prjPath_ansi = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json["pathSelected"]));
				std::string pathOfCore = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json["pathOfCore"]));//废弃
				std::string coreWithPara = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json["coreWithPara"]));//废弃
				std::string secMenu = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json["secMenu"]));
				std::string trdMenu = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(json["trdMenu"]));
				if (secMenu == "BIM软件" || secMenu == "BIM软件系列" || secMenu=="装配式")
				{
					OpenBimExe();
					return;
				}
				std::vector<std::string> vec{prjPath_ansi, pathOfCore, coreWithPara, secMenu, trdMenu};
				OnDbClickProject(vec);
			}
			catch (...) {
				MsgBox::WarningViaID(GetHWND(), L"ERROR_TIP_NO_PROJECT", L"TITLE_ERROR");
			}
			std::string debugStr = R"({ "message": "call ONNEWPROJECT Success." })";
			callback(true, nbase::AnsiToUtf8(debugStr));
			})
	);

	cef_control_->RegisterCppFunc(L"ONCTRLV",
		ToWeakCallback([this](const std::string& /*params*/, nim_comp::ReportResultFunction callback) {
			std::string filePath;
			GetClipBoardInfo(GetHWND(), filePath);
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
			catch (...)
			{
				std::string debugStr = R"({ "Call ONNEWPROJECT": "Fail." })";
				callback(false, nbase::AnsiToUtf8(debugStr));
			}
		})
	);

	cef_control_->RegisterCppFunc(L"NEWVERSION",
		ToWeakCallback([this](const std::string& /*params*/, nim_comp::ReportResultFunction callback) {
			bool newReleased = TellMeNewVersionExistOrNot();
			if(newReleased)
				callback(true, R"({ "NewVersion": "true" })");
			else
				callback(false, R"({ "NewVersion": "false" })");
			})
	);

	cef_control_->RegisterCppFunc(L"ADVERTISES",
		ToWeakCallback([this](const std::string& /*params*/, nim_comp::ReportResultFunction callback) {
			auto advertisement = TellMeAdvertisement();
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

	cef_control_->RegisterCppFunc(L"GETWEBARTICLES",
		ToWeakCallback([this](const std::string& /*params*/, nim_comp::ReportResultFunction callback) {
			if (webArticleReader_.is_good())
			{
				auto content = webArticleReader_.GetWebArticles();
				callback(true, content);
			}
			else
			{
				auto ptr = IArticleReader::GetArticleReader(IArticleReader::ArticleType::NATIVE_WEB);
				ptr->Init();
				if (ptr->is_good())
				{
					std::string content;
					content = ptr->RawString();
					callback(true, content);
				}
				else
				{
					//几乎不可能出错。希望前端检查返回值
					callback(false, R"({ "message": "GETNATIVEARTICLES failed, this should not happen" })");
				}
			}
			})
	);

	cef_control_->RegisterCppFunc(L"GETNATIVEARTICLES",
		ToWeakCallback([this](const std::string& /*params*/, nim_comp::ReportResultFunction callback) {
			if (webArticleReader_.is_good())
			{
				auto content = webArticleReader_.GetNativeArticles();
				callback(true, content);
				return;
			}
			else
			{
				std::string content;
				auto ptr = IArticleReader::GetArticleReader(IArticleReader::ArticleType::NATIVE);
				ptr->Init();
				if (ptr->is_good())
				{
					content = ptr->RawString();
					callback(true, content);
				}
				else
				{
					MsgBox::Warning(GetHWND(), L"读取内容出错", L"GETNATIVEARTICLES");
					callback(false, R"({ "message": "GETNATIVEARTICLES failed, this should not happen" })");
				}	
			}
			})
	);

	cef_control_->RegisterCppFunc(L"SETSTYLE",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			nlohmann::json json = nlohmann::json::parse(params);
			int styleIndex = json["styleIndex"];
			std::wstring styleName= nbase::UTF8ToUTF16(json["styleName"]);
			bool ret=this->SwicthThemeTo(styleIndex);
			if(ret)
				this->SaveThemeIndex(styleIndex, styleName);
			})
	);

	cef_control_->RegisterCppFunc(L"GETSTYLE",
		ToWeakCallback([this](const std::string& /*params*/, nim_comp::ReportResultFunction callback) {
			nlohmann::json json;
			auto info=ConfigManager::GetInstance().GetGuiStyleInfo();
			json["styleIndex"] = info.first;
			json["styleName"] = nbase::UTF16ToUTF8(info.second);
			callback(true, json.dump());
			})
	);
}

/*
出于内部有些2B有改配置文件的习惯。每次我都读配置文件。
*/
std::string CefForm::ReadWorkPathFromFile(const std::string& filename)
{
	prjPaths_.clear();
	const std::string fullpath = nbase::UnicodeToAnsi(nbase::win32::GetCurrentModuleDirectory()) + filename.c_str();

	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	rapidjson::Value array(rapidjson::kArrayType);
	std::vector<std::string> vec;
	char buffer[1024] = { 0 };
	for (int i = 0; i < maxPrjNum_; ++i)
	{
		auto workPathId = "WorkPath" + std::to_string(i);
		memset(buffer, 0, ArraySize(buffer));
		auto nRead = GetPrivateProfileStringA("WorkPath", workPathId.c_str(), "error", buffer, ArraySize(buffer), fullpath.c_str());
		if (!strcmp("error", buffer) || buffer[nRead - 1] != '\\')
		{	
			continue;//prjPathStr=="error"表明用户手动修改了配置文件!
		}
		std::string timeStamp;
		auto ret = GetPrjInfo(buffer, timeStamp);
		rapidjson::Value obj(rapidjson::kObjectType);
		if (ret)
		{
			vec.emplace_back(buffer);
			rapidjson::Value key(rapidjson::kStringType);
			key.SetString(workPathId.c_str(), allocator);
			rapidjson::Value value(rapidjson::kStringType);
			value.SetString(buffer, allocator);
			obj.AddMember("WorkPath", value, allocator);

			value.SetString(timeStamp.c_str(), allocator);
			obj.AddMember("Date", value, allocator);

			std::string bmpPath = buffer;
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
	const char* /*surfix*/)
{
	auto index = pathStr.find_last_not_of("/\\");
	std::string path = pathStr.substr(0, index + 1);
	if (ENOENT == _access(path.c_str(), 0))
	{
		return false;
	}
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
			FullPathOfPkpmIniA().c_str());
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
	std::string defaultPath;
	if (-1 == indexHeightLighted_ || !prjPaths_.size())
	{
		OutputDebugString(L"dangerous operation");
	}
	else
		defaultPath = prjPaths_[indexHeightLighted_];
	auto type = FolderDialogFactory::GetFdType(ConfigManager::GetInstance().GetFolderDialogType());
	auto dialog = FolderDialogFactory::GetFolderDialog(type);
	auto folder=dialog->GetExistingDirectory(nullptr, defaultPath.c_str(), true);
	return folder;
}

void CefForm::OnRightClickProject(const std::wstring& prjName)
{
	if (PathFileExists(prjName.c_str()) && PathIsDirectory(prjName.c_str()))
		application_utily::OpenDocument(prjName);
	else
		MsgBox::WarningViaID(GetHWND(), L"ERROR_TIP_PROJECT_DELETED", L"TITLE_PROJECT_ERROR");
}

//有问题,我应该让前端调用时就把工程路径和索引一块过来
void CefForm::DataFormatTransfer(const std::string& module, const std::string& app, const std::string& workdir)
{
	if (!prjPaths_.size())
	{
		MsgBox::WarningViaID(GetHWND(), L"ERROR_TIP", L"TITLE_ERROR");
		return;
	}
	{
		ShowWindow(SW_HIDE);
		char oldWorkPath[256] = { 0 };
		GetCurrentDirectoryA(sizeof(oldWorkPath), oldWorkPath);
		BOOL ret = SetCurrentDirectoryA(workdir.c_str());
		if (!ret)
		{
			MsgBox::WarningViaID(GetHWND(), L"ERROR_TIP_NO_DIR_ACCESS_OR_ELSE", L"TITLE_ERROR");
			return;
		}
		run_cmd(module, app, "");
		SetCurrentDirectoryA(oldWorkPath);
		for (size_t i = 0; i != prjPaths_.size(); ++i)
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

//fix me, pkpmmain退出时可能导致了线程异常,程序没有办法发送WM_SHOWMAINWINDOW
//使用了RAII来确保消息发送。并且在以常理捕获里也发送了消息
//测试通过后，可以去掉异常里的消息发送。而记录这个异常是有意义的。
void CefForm::OnDbClickProject(const std::vector<std::string>& args)
{
	if (args.size() != 5 || !prjPaths_.size())
	{
		MsgBox::WarningViaID(GetHWND(), L"ERROR_TIP_PARAMETER_ERROR", L"TITLE_ERROR");
		return;
	}
	std::string path(args[0]);
	{
		ShowWindow(false);
		auto ret = SetCurrentDirectoryA(path.c_str());
		if (!ret)
		{
			MsgBox::WarningViaID(GetHWND(), L"ERROR_TIP_NO_DIR_ACCESS_OR_ELSE", L"TITLE_ACCESS_ERROR");
			return;
		}		
		for (size_t i = 0; i != prjPaths_.size(); ++i)
		{
			if (prjPaths_[i] == path)
			{
				prjPaths_.moveToFront(i);
				SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
				break;
			}
		}
		std::thread t([=]() {
			ALIME_SCOPE_EXIT{
				::SendMessage(m_hWnd, WM_SHOWMAINWINDOW, 0, 0);
			};
			SetCurrentDirectoryA(path.c_str());
			catch_exception([=]() {run_cmd(args[3], args[4], ""); }, []() {spdlog::critical("检测到PKPMMAIN.exe异常"); });
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
			ret=WritePrivateProfileStringA("WorkPath", workPathId.c_str(), prjPaths[i].c_str(), filename.c_str());
		else
			ret=WritePrivateProfileStringA("WorkPath", workPathId.c_str(), NULL, filename.c_str());
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
	bool isLegalPath=false;
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

void CefForm::run_cmd(const std::string& moduleName, const std::string& appName1, const std::string& appName2)
{
	std::string cmdStr;
	cmdStr = moduleName + "|" + appName1;
	if (!appName2.empty())
		cmdStr += "_" + appName2;
	appDll_.Invoke_RunCommand(cmdStr.data());
}

bool CefForm::SetCfgPmEnv()
{
	DWORD bufferSize = GetEnvironmentVariable(L"path", nullptr, 0);
	std::wstring originalPathEnv;
	if (bufferSize)
	{
		originalPathEnv.resize(bufferSize);
		GetEnvironmentVariable(L"path", &originalPathEnv[0], bufferSize);
		std::wstring cfgPath = nbase::win32::GetCurrentModuleDirectory() + L"CFG";
		std::wstring pmModulePath = nbase::win32::GetCurrentModuleDirectory() + L"Ribbon\\PM";
		if (!nbase::FilePathIsExist(cfgPath, true) || !nbase::FilePathIsExist(pmModulePath, true))
		{
			MsgBox::ShowViaID(L"ERROR_TIP_SET_PM_ENV", L"TITLE_ERROR");
			return false;
		}
		std::wstring new_envirom(cfgPath + L";" + pmModulePath);
		new_envirom.append(L";").append(originalPathEnv.c_str());
		SetEnvironmentVariable(L"path", new_envirom.c_str());
		return true;
	}
	return false;
}

bool CefForm::TellMeNewVersionExistOrNot()
{
	if (!webPageData_.data_.lock()->isWebPageCooked_)
		return false;
	else
	{
		rapidjson::Document document;
		try
		{
			document.Parse(webPageData_.data_.lock()->pageInfo_.c_str());
			if (!document.HasMember("UpdateUrl") ||!document.HasMember("Advertise") ||
				!document["Advertise"]["NationWide"].IsArray())
				return false;
		}
		catch (...)
		{
			return false;
		}	
		std::wstring VersionPath = nbase::win32::GetCurrentModuleDirectory() + L"CFG\\";
		auto vec = FindSpecificFiles::FindFiles(nbase::UnicodeToAnsi(VersionPath).c_str(), "V", "ini");
		if (!vec.empty())
		{
			AscendingOrder stradegy;//fix me
			std::sort(vec.begin(), vec.end(), stradegy);
			const auto& LatestVersionOnLocal = vec.back();
			if (document.HasMember("VersionString"))//fix me
			{
				std::string versionString = document["VersionString"].GetString();
				return stradegy(LatestVersionOnLocal, versionString);
			}
		}
	}
	return false;
}

std::string CefForm::TellMeAdvertisement()
{
	if (!webPageData_.data_.lock()->isWebPageCooked_)
		return ConfigManager::GetInstance().GetDefaultAdvertise();
	else
	{
		//fix me，不需要复制一份
		auto pageInfo = webPageData_.data_.lock()->pageInfo_;
		if (!pageInfo.empty())
			return WebPageDownLoader::ParseWebPage(pageInfo);
	}
	return ConfigManager::GetInstance().GetDefaultAdvertise();
}

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
			Alime::FileSystem::Folder folder(prjPathStr);
			if (!folder.Exists())	
				continue;
			std::wstring destPath;
			if (!Alime::FileSystem::PathNameCaseSensitive(folder, destPath))
				continue;
			if (destPath.back() != L'\\')
				destPath += L'\\';
			if (std::find(vec.cbegin(), vec.cend(), destPath) == vec.cend())
				vec.push_back(destPath);
		}
	}
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

LRESULT CefForm::OnNcLButtonDbClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!::IsZoomed(GetHWND()))
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	else
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	return 0;
}

void CefForm::EnableAcceptFiles()
{
	LONG style = ::GetWindowLong(this->m_hWnd, GWL_EXSTYLE);
	auto hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	style |= WS_EX_ACCEPTFILES;
	hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	::SetWindowLongW(this->m_hWnd, GWL_EXSTYLE, style);
}

void CefForm::AttachClickCallbackToSkinButton()
{
	if (!skinSettings_)
		return;
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
		if (!pVbox) return false;
		pVbox->AttachSelect([this](ui::EventArgs* args) {
			auto themeType = args->pSender->GetName();
			PostMessage(WM_THEME_SELECTED, args->wParam, args->lParam);//lParam should always -1
			return true;
			});
		return true;
		});
}

void CefForm::InitUiVariable()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefForm::OnClicked, this, std::placeholders::_1));
	cef_control_ = dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control"));
	cef_control_dev_ = dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control_dev"));
	if (cef_control_dev_)
		cef_control_->AttachDevTools(cef_control_dev_);
	label_ = dynamic_cast<ui::Label*>(FindControl(L"projectName"));
	defaultCaption_ = label_->GetText().empty()? L"PKPM结构软件  ": label_->GetText();
	skinSettings_ = dynamic_cast<ui::Button*>(FindControl(L"settings"));
	vistual_caption_ = dynamic_cast<ui::HBox*>(FindControl(L"vistual_caption"));
	this_window_ = dynamic_cast<ui::Window*>(FindControl(L"main_wnd"));
	cef_control_->AttachLoadStart(nbase::Bind(&CefForm::OnLoadStart, this));
	cef_control_->AttachLoadEnd(nbase::Bind(&CefForm::OnLoadEnd, this, std::placeholders::_1));
	//tray_.Init(GetHWND(), 128, WM_TRAY);
}

void CefForm::AttachFunctionToShortCut()
{
	shortCutHandler_.SetCallBacks(GetHWND(), [this]() {
		if (prjPaths_.IsIndexLegal(indexHeightLighted_))
			prjPaths_.moveToFront(indexHeightLighted_);
		if (!prjPaths_.empty())
			SaveWorkPaths(prjPaths_, FullPathOfPkpmIniA());
		cef_control_->CallJSFunction(L"flush",L"{\"uselessMsg\":\"dummy\"}",
			ToWeakCallback([this](const std::string& /*chosenData*/) {
				}
		));});
	shortCutHandler_.Init();
}

void CefForm::InitSpdLog()
{	
	//fix me, create file here may raise some exception
	auto logFolderW = nbase::win32::GetCurrentModuleDirectory() + L"resources\\Logs\\";
	auto logFileNameW = logFolderW + L"log.txt";
	std::string LogsFile = nbase::UnicodeToAnsi(logFileNameW);
	//多余，存在就不可能是文件夹
	if (PathFileExists(logFileNameW.c_str()) && !PathIsDirectory(logFileNameW.c_str()))
	{
		if (nbase::GetFileSize(logFileNameW.c_str()) > 50 * 1024 * 1024)
		{
			bool ret = DeleteFile(logFileNameW.c_str());
			if (!ret)
				return;
		}
	}
	Alime::FileSystem::Folder p(logFolderW);
	if (!p.Exists())
		p.Create(false);
	try {
		auto file_logger = spdlog::basic_logger_mt("fileLogger", LogsFile.c_str());
		file_logger->set_pattern("[%Y%m%d %H:%M:%S.%f] [%t] [%l] [%v]");
		file_logger->flush_on(spdlog::level::trace);
		spdlog::set_default_logger(file_logger);
		spdlog::set_level(spdlog::level::trace);
	}
	catch(...){
		//can not open logFile
		return;
	}
}

void CefForm::AcceptDpiAdaptor(IAdaptor* visitor)
{
	visitor->AdaptCaption(this);
	visitor->AdaptCefWindow(this);
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

void CefForm::OpenBimExe()
{
	nim_comp::CircleBox* bx=nim_comp::ShowCircleBox(GetHWND(), 
		NULL, ui::MutiLanSupport::GetInstance()->GetStringViaID(L"TITLE_OPEN_BIM"));
	std::wstring bimPath;
	if (application_utily::FindBimExe(bimPath))
		application_utily::OpenBimExe(bimPath);
	else
	{
		if (!bx)
		{
			ShellExecute(NULL, _T("open"), ConfigManager::GetInstance().GetBimDownLoadWeb().c_str(), NULL, NULL, SW_SHOW);
		}
		else
		{
			nbase::ThreadManager::PostTask([bx]() {
				bx->SetTitle(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"TITLE_OPEN_BIM_WEB"));
				nbase::ThreadManager::PostDelayedTask([]() {
					ShellExecute(NULL, _T("open"), ConfigManager::GetInstance().GetBimDownLoadWeb().c_str(), NULL, NULL, SW_SHOW);
					}, nbase::TimeDelta::FromSeconds(1));
				});
		}
	}
}