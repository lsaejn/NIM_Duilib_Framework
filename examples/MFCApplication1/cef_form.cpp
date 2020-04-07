#include "pch.h"
#include "io.h"
#include "cef_form.h"
#include "string_util.h"
#include "HttpUtil.h"
#include "FileSystem.h"
#include "MsgDialog.h"
#include "SkinSwitcher.h"
#include "ConfigFileManager.h"

#include <filesystem>
#include <regex>

#include "RapidjsonForward.h"
#include "nlohmann/json.hpp"

using namespace Alime::HttpUtility;

/*
��������Ϊ���������У����Բ��ܸ��������ļ���
Ӳ����ı���������֮�١�
pkpm�Ĳ������˵�ǵ��͵ķ���̲ġ�����+��������+�û�����+�û����̹���
ȫ������һ��ֱ�ӵ����˳�����װ�������̣������ļ�����ֻ��ͨ�����³���
*/
namespace
{
	const std::wstring RelativePathForHtmlRes = L"resources\\HtmlRes\\index.html";
	const char* toRead[] = { "navbarIndex", "parentIndex", "childrenIndex","projectIndex" };
	//����""��Ϊ�ָ���
	const char* defaultAdvertise = "{data:[ \
{\"key\":\"�ٷ���վ: www.pkpm.cn\", \"value\":\"www.pkpm.cn\"}, "
"{\"key\":\"�ٷ���̳: www.pkpm.cn/bbs\",\"value\":\"www.pkpm.cn/bbs\"}"
"]}";

	std::wstring FullPathOfPkpmIni()
	{
		static auto path = nbase::win32::GetCurrentModuleDirectory() + L"cfg/pkpm.ini";
		return path;
	}

	std::string FullPathOfPkpmIniA()
	{
		static auto path = nbase::UnicodeToAnsi(nbase::win32::GetCurrentModuleDirectory()
			+ L"cfg/pkpm.ini");
		return path;
	}

	std::string DictToJson(const std::vector<std::pair<std::string, std::string>>& dict)
	{
		rapidjson::Document doc;
		rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

		rapidjson::Value root(rapidjson::kObjectType);
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
		return s.GetString();
	}
}

const std::wstring CefForm::kClassName = L"PKPM V5.1.2";

CefForm::CefForm()
	:maxPrjNum_(GetPrivateProfileInt(L"WorkPath", L"MaxPathName",
		6, FullPathOfPkpmIni().c_str())),
	prjPaths_(maxPrjNum_),
	isWebPageAvailable_(false),
	cef_control_(NULL),
	indexHeightLighted_(-1)
{
	webDataReader_.Init();
	InitAdvertisement();
	size_t numofPrj=CorrectWorkPath();
	if (numofPrj > 0)
		indexHeightLighted_ = 0;
	ReadWorkPathFromFile("CFG/pkpm.ini");
	//һ������̨�������ʽģ����ҳ����¼�
	//����д��logģ��
	//Alime::Console::CreateConsole();
	//Alime::Console::SetTitle(L"Alime");
	//std::thread t([this]() {
	//	while (!stop)//û�а취���ŵ���ֹ����߳�,��windows�ƺ�������stdinֱ��write?
	//	{
	//		auto str = Alime::Console::ReadLine();
	//		if (str.find(L"SetCaption") != std::wstring::npos)
	//		{
	//			auto result = str.substr(10);
	//			SetCaption(u8"PKPM�ṹ������ 10�� V5.1.1    " + nbase::UTF16ToUTF8(result));
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
	//			Alime::Console::WriteLine(L"û������ӿ�");
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
	SetWindowTextA(GetHWND(), "PkpmV5.1.2");
	InitUiVariable();
	// �����������ʽ, ������δ�������Ϊǰ����Ҫһ��ˢ�»���
	auto cefHtmlPath= nbase::win32::GetCurrentModuleDirectory()+ RelativePathForHtmlRes;
	cef_control_->LoadURL(cefHtmlPath);

	EnableAcceptFiles();
	
	//�ɴ���
	SetCfgPmEnv();//����pm����
	appDll_.InitPkpmAppFuncPtr();
	appDll_.Invoke_InitPkpmApp();

	//������ť����Ӧ
	AttachClickCallbackToSkinButton();
	AttachFunctionToShortCut();
	//Dpi����
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
		�������Ҫ�ڱ���Ŀʹ��mfc�Ի���
		���м���ѡ��:
		1.�������������߳�������ģ̬/��ģ̬�Ի����ұ���������������ӡ�
		2.���Ի���ŵ���̬��ĺ����
		��������Ҫ��duilib�Ի�����Ϊ�Ӵ��ڷŵ�mfc�Ի����ڲ�, ����
		��������ʹ��mfc��duilib����ʹ�öԷ���mfc�ؼ���
	*/
	if (name == L"btn_dev_tool")
	{
		//��������һ��ģ̬/��ģ̬�Ի���,��...ģ̬�Ƚϼ�һЩ
		/**/
		//CWinThread* m_pThrd;
		////����
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
				//���Ȼ�Ǹ��Ϸ�json
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
			MsgBox::Warning(GetHWND(), L"��֧����ק����Ŀ¼", L"����Ĵ���");
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
				MsgBox::Warning(GetHWND(), L"��֧����קĿ¼", L"����");
		}
		DragFinish(hDropInfo);
	}
	else if (uMsg == WM_KEYDOWN)
	{
		if (116 == wParam)
		{
#ifdef DEBUG
			cef_control_->Refresh();
#endif // DEBUG		
		}
		//���ֹ�һ�����⣬������ҳ�޷���ý���
		//�Ҳ����Ѵ�����ctrlv��delete�¼�
		//ֱ������ʶ��������WM_SIZE�����SetFocus��
		//Ȼ����δ���Ҳ��Ҫ�������á��һᱣ��������Ϊֹ��
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
		SwicthThemeTo(wParam);
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
	return ui::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void CefForm::SwicthThemeTo(int index)
{
	//���ŵ�tab���԰���Ϣд��xml��
	//����������Ҫ�ÿ細�ڵ�menuitem��ִ��Ƥ���л�
	//�һ�û�ҵ���ô�������ӡ�
	nim_comp::Box* vistual_caption = dynamic_cast<nim_comp::Box*>(FindControl(L"vistual_caption"));
	auto sw=SkinSwitcherFatctory::Get(index);
	sw->Switch(vistual_caption, label_);
	SaveThemeIndex(index);
}

void CefForm::SaveThemeIndex(int index)
{
	//fix me
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

void CefForm::ModifyScaleForCef()
{
	if (ConfigManager::GetInstance().IsAdaptDpiOn())
		return;
	else if (ConfigManager::GetInstance().IsAutoModifyWindowOn())
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
	else if (ConfigManager::GetInstance().IsAutoModifyWindowOn())
	{
		UINT dpi = ui::DpiManager::GetMainMonitorDPI();
		auto scale = MulDiv(dpi, 100, 96);
		double rate = scale / 100.0;
		ui::UiRect captionRect=this_window_->GetCaptionRect();
		ui::UiRect adjustedCaptionRect = { 0,
			0,
			static_cast<int>(captionRect.GetWidth() * rate),
			static_cast<int>(captionRect.GetHeight() * rate) };
		this_window_->SetCaptionRect(adjustedCaptionRect);

		RECT rc;
		GetWindowRect(GetHWND(), &rc);
		int width = static_cast<int>((rc.right - rc.left) * rate);
		int height = static_cast<int>((rc.bottom - rc.top) * rate);
		SetWindowPos(GetHWND(), HWND_TOP, 0, 0, width,
			height, SWP_NOMOVE);
		auto captionHeight=vistual_caption_->GetFixedHeight();
		vistual_caption_->SetAttribute(L"height", std::to_wstring(captionHeight * rate));
		//label_->SetFont(L"system_18");
	}
}

void CefForm::OnLoadStart()
{
	RegisterCppFuncs();
	ModifyScaleForCef();
}

void CefForm::RegisterCppFuncs()
{
	//����Toast��Դ�ļ��У��Ա�ʹ������Ի���
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
			int Index = document["projectIndex"].GetInt();
			assert(nbase::AnsiToUtf8(prjPaths_[Index]) == captionName);
			SetHeightLightIndex(Index);
			SetCaption(captionName);
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
			if (!prjPaths_.IsIndexLegal(index))
			{
				if (prjPaths_.empty())
				{
					MsgBox::Warning(GetHWND(), L"û��ѡ�񹤳�", L"����");
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
			//�ɴ��룬ʵ����ֻ��Ҫ3������, Ϊ�˲���ǰ������ɻ󣬾Ͳ�����
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
			//�������shortCutHandler.cpp�����������������д�����������Ĵ���
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
				MsgBox::Warning(GetHWND(), L"��֧����ק����Ŀ¼", L"����");
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
					MsgBox::Warning(GetHWND(), L"��֧����קĿ¼", L"����");
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
						SetHeightLightIndex(-1);
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

	cef_control_->RegisterCppFunc(L"RRMAININGTIME",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {

			})
	);

}

//fileNameһ�����CFG/PKPM.ini��
/*
���Ǹ��ɺ�����ÿ�ζ��������ļ���֮����û��
ɾ������Ϊ�����������˴�æ0.0
(�ȷ�˵��������ش�������ҳˢ����ɵ��ڴ�/�ļ���ƥ�䣬�û�ʹ�ó���������ֶ��������ļ�)��
�ұ����������ϣ����ö��������ο��
��ȷ�������Ǽ��һ�������ļ��������ڴ棬Ȼ��ά���ڴ漴�ɡ�
*/
std::string CefForm::ReadWorkPathFromFile(const std::string& filename)
{
	prjPaths_.clear();
	const std::string fullpath = nbase::UnicodeToAnsi(nbase::win32::GetCurrentModuleDirectory()) + filename.c_str();

	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	rapidjson::Value array(rapidjson::kArrayType);//< ����һ���������
	std::vector<std::string> vec;
	char prjPathStr[1024] = { 0 };
	for (int i = 0; i < maxPrjNum_; ++i)
	{
		auto workPathId = "WorkPath" + std::to_string((ULONGLONG)i);
		memset(prjPathStr, 0, ArraySize(prjPathStr));
		auto nRead = GetPrivateProfileStringA("WorkPath", workPathId.c_str(), "error", prjPathStr, ArraySize(prjPathStr), fullpath.c_str());
		if (!strcmp("error", prjPathStr))
		{
			//prjPathStr=="error"�����û��ֶ��޸��������ļ�!
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
				bmpPath = "file://" + bmpPath;//��ҳ��Ҫ�������ǰ׺
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
	//fixe me, rapidjson �ƺ��Լ�������u8
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
			sprintf(buf, "%4d��%02d��%02d�� %02d:%02d",
				tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
				tm_time.tm_hour, tm_time.tm_min);
			timestamp.assign(buf);
			return true;
		}
		MessageBox(NULL, L"_S_IFDIR", L"_S_IFDIR", 1);
	}
	else
	{
		//winxp �� statϵ�к����������bug
		//search " _stat not working on Windows XP"
		WIN32_FILE_ATTRIBUTE_DATA fileData;
		GetFileAttributesExA(pathStr.c_str(), GetFileExInfoStandard, &fileData);
		SYSTEMTIME systemTime;
		bool res = FileTimeToSystemTime(&fileData.ftCreationTime, &systemTime);
		if (res)
		{
			char buf[128] = { 0 };
			sprintf(buf, "%4d��%02d��%02d�� %02d:%02d",
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

void CefForm::SetCaption(const std::string& prjName)
{
	std::wstring captionWithPrefix;
	if (defaultCaption_.empty())
		captionWithPrefix = L"PKPM�ṹ������ 10�� V5.1.2   ";
	else
		captionWithPrefix = defaultCaption_;
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

//����ֻ�ڻص���ʹ��u8�ַ���
std::string CefForm::OnNewProject()
{
	int useDevelopVersion = 0;
	useDevelopVersion=GetPrivateProfileIntA("Html", "DevelopVersion", 0,
		nbase::UnicodeToAnsi(FullPathOfPkpmIni()).c_str());
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
		sInfo.lpszTitle = _T("��ѡ���µĹ���·��");
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
					nbase::UnicodeToAnsi(FullPathOfPkpmIni());//������û�а취
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
		ShellExecute(NULL, _T("open"), _T("explorer.exe"), prjName.c_str(), NULL, SW_SHOWNORMAL);
	}
	else
		MsgBox::Warning(GetHWND(), L"����Ŀ¼�޷��򿪣������Ѿ���ɾ��" ,L"·������");
}

//������,��Ӧ����ǰ�˵���ʱ�Ͱѹ���·��������һ�����
void CefForm::DataFormatTransfer(const std::string& module, const std::string& app, const std::string& workdir)
{
	if (!prjPaths_.size())
	{
		MsgBox::Warning(GetHWND(), L"����ѡ����Ŀ¼", L"����");
		return;
	}
	{
		ShowWindow(SW_HIDE);
		char oldWorkPath[256] = { 0 };
		GetCurrentDirectoryA(sizeof(oldWorkPath), oldWorkPath);
		BOOL ret = SetCurrentDirectoryA(workdir.c_str());
		if (!ret)
		{
			MessageBox(NULL, L"����Ŀ¼�������û��Ȩ��", L"PKPMV51", 1);
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
		MsgBox::Warning(GetHWND(), L"�򿪹��̵Ĳ�������ȷ", L"���ش���");
	}
	if (!prjPaths_.size())
	{//��Ӧ���ٳ����������
		MsgBox::Warning(GetHWND(), L"����Ŀ¼������", L"���ش���");
		return;
	}
	std::string path(args[0]);
	{
		ShowWindow(SW_HIDE);
		auto ret = SetCurrentDirectoryA(path.c_str());
		if (!ret)
		{
			MsgBox::Warning(GetHWND(), L"����Ŀ¼�������û��Ȩ��", L"Ȩ�޴���");
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
	if (!hasAdministratorsRights)
	{
		MsgBox::Warning(GetHWND(), L"�޷����湤����Ϣ������ʹ�ù���ԱȨ�޴򿪱����", L"Ȩ�޴���");
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
	if (realPathOfNewProj.length() == 3&& realPathOfNewProj[1]==':')//�̷�
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
		prjPaths_.moveToFront(index);
		SaveWorkPaths(prjPaths_, filename);
		return true;
	}
	return false;
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
	//���ݽṹ��ǰ�˶��ģ����ԣ�����ʵû��ѡ��
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
	//��Ҫ�������Ǹ������ļ���
	if (moduleName == "����ת��")
	{
		if (appName1 == "SP3D/PDS")
			appName1 = "PDS";
		else if (appName1 == "Revit")
		{
			appName1 = "����Revit";
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
	appDll_.Invoke_RunCommand(str);
}

bool CefForm::SetCfgPmEnv()
{
	const int LENGTH_OF_ENV = 1024 * 8;
	//���˹��ˣ�unique_ptr֪����?
	TCHAR* szOriEnvPath = new  TCHAR[LENGTH_OF_ENV];//������Ƭ�ڴ�ȥ����....?
	DWORD dwRet = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, LENGTH_OF_ENV);
	if (!dwRet)
	{
		::AfxMessageBox(L"Error! Can not find Path", MB_OK | MB_SYSTEMMODAL);
		return false;
	}
	else if (LENGTH_OF_ENV < dwRet)//��Ҫ���·����ڴ�
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
		AfxMessageBox(L"�޷�����PATH·��", MB_OK | MB_SYSTEMMODAL);
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
	//������ʩ������Ҫ����thisʧЧ�ķ��ա������ⷢ�����˳�����ʱ������Ӧ�ò���
	std::thread func(std::bind(&CefForm::AdvertisementThreadFunc, this));
	func.detach();
}

//����ǰ�˸㲻�������Ա����ҿ��̲߳�ѯ���
//������ǰ�˶�ȡ���й��ǰ����Ҫ����
void CefForm::AdvertisementThreadFunc()
{
	assert(isWebPageAvailable_ == false);
	bool AdPageCanAccess = false;
	//��ҳ��������ά�������ǿ��ܷ��κδ���
	//����,jsonд�����벻��
	try
	{
		AdPageCanAccess = VersionPage();
	}
	catch (const std::exception& e)
	{
		UNREFERENCED_PARAMETER(e);
		//LOG_ERROR<<e.what();
#ifdef DEBUG
		AfxMessageBox(L"������ݸ�ʽ���󣬻��߱��벻��ȷ", MB_OK | MB_SYSTEMMODAL);
#endif // DEBUG
		AdPageCanAccess = false;
	}
	
	if (!AdPageCanAccess)//��ȡ��ҳʧ��
	{
		//���Բ���,ֻ��������
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

//���ǵĹ��ԭҳ����https://update.pkpm.cn/PKPM2010/Info/index.html
//���ҳ��http://111.230.143.87/index.html
bool CefForm::VersionPage()
{
	HttpRequest req;
	wchar_t buffer[128] = { 0 };
	const std::wstring backupServer = L"update.pkpm.cn";
	const std::wstring backupQuery = L"/PKPM2010/Info/index.html";
	GetPrivateProfileStringW(L"Html", L"server", backupServer.c_str(), 
		buffer, sizeof(buffer) - 1, FullPathOfPkpmIni().c_str());
	req.server = buffer;
	memset(buffer, 0, sizeof(buffer));
	if (req.server == backupServer)
		req.query = backupQuery;
	else
	{
		GetPrivateProfileStringW(L"Html", L"query", L"/PKPM2010/Info/index.html",
			buffer, sizeof(buffer) - 1, FullPathOfPkpmIni().c_str());
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
		OutputDebugStringW(L"û����������");
#endif // _DEBUG
		return false;
	}
	////////////else///////////////////
	auto result = res.GetBodyUtf8();
	std::wstring re = result.c_str();
	auto astring = nbase::UnicodeToAnsi(re);
	{
		std::regex reg("(<(body)>)([\\s\\S]*)(</\\2>)");
		//vs2010�£�tr�汾��������ʽ�޷�ƥ��"�հ׷�"���ַ��������
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
			//͵��
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
		//atomic_bool��mutex����һ����
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

			rapidjson::Value array(rapidjson::kArrayType);//< ����һ���������
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
			return s.GetString();
		}
	}
	return defaultAdvertise;
}

/*
������ÿ���½����̶����ȫ����·�����������ܿ��̼߳��ӡ�
��Ϊ�Ҳ�����д�ı��༭����
ֻ��Ҫ��֤���ļ�ʱ���õ���·������Ч�ģ�����ÿ��
�½����¹���·������Ч�ġ������Ϳ����ˡ��������御��
FileSystem�������bug, 0.0����Ҳû�˰�⣬�����˾��Ҫ�����񼶳���Ա��
û�����⣬û�ĵ���һ���������롣
*/
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
			//fix me, ȥ������ж�
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
	//���Ǽ�����Ĺ��̣�����ǰ6����Ч���̣�ǿ���û�ʹ���°汾
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
	//������Ⱦģʽ�£��ƺ���Ҫ�ֶ�������ק�ļ�֧��
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

int CefForm::remainingTimeOfUserLock(std::string* SerialNumber)
{
	//����:@ ģ���, @��ģ��� @���ص���Ȩ��
	//����ֵ: ʣ������
	typedef int(_stdcall *FuncInWinAuthorize)(int&, int&, char*);
	auto pathOfWinAuthorize = nbase::win32::GetCurrentModuleDirectory()+L"Ribbon\\WinAuthorize.dll";
	auto handle = LoadLibrary(pathOfWinAuthorize.c_str());
	FuncInWinAuthorize dayLeftFunc = (FuncInWinAuthorize)GetProcAddress(handle, "_Login_SubMod2@12");
	int ty = 100;
	int sub_ky = 0;
	char* gSN = new char[17];
	int dayLeft = dayLeftFunc(ty, sub_ky, gSN);
	*SerialNumber = gSN;
	delete[] gSN;
	return dayLeft;
}

void CefForm::InitUiVariable()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefForm::OnClicked, this, std::placeholders::_1));
	cef_control_ = dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control"));
	cef_control_dev_ = dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control_dev"));
	btn_dev_tool_ = dynamic_cast<ui::Button*>(FindControl(L"btn_dev_tool"));
	edit_url_ = dynamic_cast<ui::RichEdit*>(FindControl(L"edit_url"));
	label_ = dynamic_cast<ui::Label*>(FindControl(L"projectName"));
	//�����xml�Ŀؼ�������ܺ���...
	defaultCaption_ = label_->GetText();
	skinSettings_ = dynamic_cast<ui::Button*>(FindControl(L"settings"));
	vistual_caption_ = dynamic_cast<ui::HBox*>(FindControl(L"vistual_caption"));
	this_window_ = dynamic_cast<ui::Window*>(FindControl(L"main_wnd"));
	if (edit_url_)
	{
		edit_url_->SetSelAllOnFocus(true);
		edit_url_->AttachReturn(nbase::Bind(&CefForm::OnNavigate, this, std::placeholders::_1));
	}
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