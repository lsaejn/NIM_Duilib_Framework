#include "pch.h"
#include "cef_form.h"
#include "CDirSelectThread.h"
#include "Console.h"
#include "string_util.h"

#include <sys/stat.h>
#include <stdlib.h>
#include <tchar.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

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
//#include "CppFuncRegister.h"

HWND mainWnd=NULL;
//CString cfg_key_str;
//CString get_cfg_path_reg_key()
//{
//	return cfg_key_str;
//}
//details
namespace //which will write to configFile
{
	//exeΪ���
	const std::wstring RelativePathForHtmlRes = L"Ribbon\\HtmlRes\\index.html";
	const char* toRead[] = { "navbarIndex", "parentIndex", "childrenIndex","projectIndex" };

	std::wstring FullPathOfPkpmIni()
	{
		auto re = nbase::win32::GetCurrentModuleDirectory() + L"cfg/pkpm.ini";
		return re;
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


const std::wstring CefForm::kClassName = L"PKPM V5.1.1��������";

CefForm::CefForm()
	:maxPrjNum_(6),
	prjPaths_(maxPrjNum_)
{
	webDataReader_.init();
	shortCutHandler_.Init();
	ReadWorkPathFromFile("CFG/pkpm.ini");
	//����д��logģ��
	//Alime::Console::CreateConsole();
	//Alime::Console::SetTitle(L"Alime");
	//std::thread t([this]() {
	//	while (1)
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
	// ɨ�� XML ����������Ϊ CefControl �Ľڵ㣬�򴴽�һ�� ui::CefControl �ؼ�
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
	// ������굥���¼�
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefForm::OnClicked, this, std::placeholders::_1));
	//m_pRoot->AttachBubbledEvent(ui::kEventMouseDoubleClick, nbase::Bind(&CefForm::OnDbClicked, this, std::placeholders::_1));
	//m_pRoot->AttachAllEvents(nbase::Bind(&CefForm::OnDbClicked, this, std::placeholders::_1));
	// �� XML �в���ָ���ؼ�
	cef_control_= dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control"));
	cef_control_dev_= dynamic_cast<nim_comp::CefControlBase*>(FindControl(L"cef_control_dev"));
	btn_dev_tool_= dynamic_cast<ui::Button*>(FindControl(L"btn_dev_tool"));
	edit_url_= dynamic_cast<ui::RichEdit*>(FindControl(L"edit_url"));
	label_= dynamic_cast<ui::Label*>(FindControl(L"projectName"));

	// �����������ʽ
	edit_url_->SetSelAllOnFocus(true);
	edit_url_->AttachReturn(nbase::Bind(&CefForm::OnNavigate, this, std::placeholders::_1));
	//edit_url_->

	// ����ҳ��������֪ͨ
	cef_control_->AttachLoadStart(nbase::Bind(&CefForm::RegisterCppFuncs, this));
	cef_control_->AttachLoadEnd(nbase::Bind(&CefForm::OnLoadEnd, this, std::placeholders::_1));
	cef_control_->AttachDevTools(cef_control_dev_);

	auto path= nbase::win32::GetCurrentModuleDirectory()+ RelativePathForHtmlRes;
	cef_control_->LoadURL(path);
	if (!nim_comp::CefManager::GetInstance()->IsEnableOffsetRender())
		cef_control_dev_->SetVisible(false);

	auto hwnd = GetHWND();
	mainWnd = hwnd;
	LONG style = ::GetWindowLong(this->m_hWnd, GWL_EXSTYLE);//��ȡԭ�������ʽ
	auto hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	style |= WS_EX_ACCEPTFILES;//������ʽ
	hasAcc = (style & WS_EX_ACCEPTFILES) == WS_EX_ACCEPTFILES;
	::SetWindowLongW(hwnd, GWL_EXSTYLE, style);//�������ô�����ʽ
	SetSizeBox({ 5,5,5,5 });

	SetWindowTextA(GetHWND(), "PkpmV5.1.1");
	SetCaption(u8"PKPM�ṹ������ 10�� V5.1.1    ���ýӿ�OnSetCaption�޸��������");
	SetCfgPmEnv();
	appDll_.InitPkpmAppFuncPtr();
	appDll_.Invoke_InitPkpmApp();
	ui::Button* settings = dynamic_cast<ui::Button*>(FindControl(L"settings"));
	settings->AttachClick([this](ui::EventArgs* args) {
		RECT rect = args->pSender->GetPos();
		ui::CPoint point;
		point.x = rect.left - 175;
		point.y = rect.top + 10;
		ClientToScreen(m_hWnd, &point);

		nim_comp::CMenuWnd* pMenu = new nim_comp::CMenuWnd(NULL);
		ui::STRINGorID xml(L"settings_menu.xml");
		pMenu->Init(xml, _T("xml"), point);
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
			cef_control_->CallJSFunction(L"showJsMessage", nbase::UTF8ToUTF16("{\"msg\":\"hello!!!\"}"),
				ToWeakCallback([this](const std::string& json_result) {
					nim_comp::Toast::ShowToast(nbase::UTF8ToUTF16(json_result), 3000, GetHWND());
				}
			));

			int x = 3;
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

//���ʲôʱ�����?
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

	cef_control_->RegisterCppFunc(L"ONSETCAPTION",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			rapidjson::StringStream input(params.c_str());
			rapidjson::Document document;
			document.ParseStream(input);
			const char* captionName = document["caption"].GetString();
			//projectIndex
			int project= document["projectIndex"].GetInt();
			assert(nbase::AnsiToUtf8(prjPaths_[project]) == captionName);
			std::string captionU8 = u8"PKPM�ṹ������ 10�� V5.1.1   ";
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
			DataFormatTransfer(ansiMod, ansiExe);
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
				std::string debugStr = R"({ "call ONNEWPROJECT": "Failed." })";
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

	//˫������
	cef_control_->RegisterCppFunc(L"DBCLICKPROJECT",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			nlohmann::json json = nlohmann::json::parse(params);
			//ʵ����ֻ��Ҫ3������, Ϊ�˲���ǰ������ɻ󣬾Ͳ�����
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
			OnDbClickProject(vec);
			//prjPath, BSTR pathOfCore, BSTR coreWithPara, BSTR secMenu, BSTR trdMenu
			//params
			std::string debugStr = R"({ "call ONNEWPROJECT": "Success." })";
			callback(true, nbase::AnsiToUtf8(debugStr));
			})
	);

	//
	cef_control_->RegisterCppFunc(L"ONDROPFILES",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			HDROP hDropInfo;
			UINT count;
			CHAR filePath[260] = { 0 };
			count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
			if (count != 1)
			{
				::AfxMessageBox(L"��֧����ק����Ŀ¼");
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
						//SaveMenuSelection(false);
						//fix me
						//this->m_pBrowserApp->Refresh();
					}
				}
				else
				{
					::AfxMessageBox(L"��֧����קĿ¼");
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
						//fix me
						//SaveMenuSelection(false);
						//this->m_pBrowserApp->Refresh();
						std::string debugStr = R"({ "call ONNEWPROJECT": "Success." })";
						callback(true, nbase::AnsiToUtf8(debugStr));
						return;
					}

				}
				else
				{
					//���Ա�����
					//LOG_ERROR<<"invalid path";
				}
			}
			std::string debugStr = R"({ "call ONNEWPROJECT": "FAILED." })";
			callback(false, nbase::AnsiToUtf8(debugStr));
			})
		);

	cef_control_->RegisterCppFunc(L"ONDELETE",
		ToWeakCallback([this](const std::string& params, nim_comp::ReportResultFunction callback) {
			static unsigned long  lastTick = 0;
			auto now = GetTickCount();
			auto interval = now - lastTick;
			lastTick = now;
			if (interval < 500)	
				return S_OK;
			try {
				if (prjPaths_.size() > 0)
				{
					//��ȡ����������
					nlohmann::json json = nlohmann::json::parse(params);
					//std::string indexStr(json["index"]);
					int prjSelected = json["index"];
					//û�й���Ϊʲô����-1?
					prjPaths_.deleteAt(prjSelected);
					SaveWorkPaths(prjPaths_, nbase::UnicodeToAnsi(FullPathOfPkpmIni()));
					//fix me
					//SaveMenuSelection(false);
					/*Fix me
					Refresh�����ڱ���̵߳��ã���mfc�����Լ����¼�ѭ����
					���ԣ�ֻ����documentComplete�����жϡ�
					�������ȶԸ���ȥ�ˡ�
					��Ӧ���ж���һ��keyup�����keydown�ļ��
					*/
					return S_OK;
				}
			}
			catch (...) {
				//FATAL<<
			}


			})
	);


}

//fileNameһ�����CFG/PKPM.ini��
/*
�������д�������⣬����Ҳû�취��...
*/
std::string CefForm::ReadWorkPathFromFile(const std::string& filename)
{
	prjPaths_.clear();
	const std::string fullpath = nbase::UnicodeToAnsi(nbase::win32::GetCurrentModuleDirectory()) + filename.c_str();

	//ƴ�ַ�������ҳ
	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	Value array(kArrayType);//< ����һ���������
	std::vector<std::string> vec;
	char prjPathStr[256] = { 0 };
	for (int i = 0; i < maxPrjNum_; ++i)
	{
		auto workPathId = "WorkPath" + std::to_string((ULONGLONG)i);
		memset(prjPathStr, 0, 256);
		auto nRead = GetPrivateProfileStringA("WorkPath", workPathId.c_str(), "error", prjPathStr, 256, fullpath.c_str());
		if (!strcmp("error", prjPathStr))
		{
			//prjPathStr=="error"�����û��ֶ��޸��������ļ�!
			continue;//or continue?
		}
		else if (prjPathStr[nRead - 1] != '\\')
		{
			//�û����������ļ�������û�Ķ�
			continue;
		}
		std::string timeStamp;
		auto ret = GetPrjInfo(prjPathStr, timeStamp);
		rapidjson::Value obj(rapidjson::kObjectType);//ÿһ������������һ��json��ʽ
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
	//fixe me rapidjson �ƺ��Լ�������u8
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
			sprintf(buf, "%4d��%02d��%02d�� %02d:%02d",
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
	const int n = sizeof(toRead) / sizeof(*toRead);//��Ҫƭ�Լ��ˣ��������û�п��ܹ��ʻ�
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

//����ֻ�ڻص���ʹ��u8�ַ���
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
	ptr("", result);
	FreeLibrary(hdll);
	return result;
	///////////////////////////////////////////////////
	//����Ϊ�������룬û��selectDlgʱ����ʹ��
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
	return {};
}

void CefForm::OnRightClickProject(const std::wstring& prjName)
{
	ShellExecute(NULL, _T("open"), _T("explorer.exe"), prjName.c_str(), NULL, SW_SHOWNORMAL);
}

//д�������⣬��Ӧ����ǰ�˵���ʱ�Ͱѹ���·��
void CefForm::DataFormatTransfer(const std::string& module, const std::string& app)
{
	if (!prjPaths_.size())
	{
		::AfxMessageBox(L"����ѡ����Ŀ¼");
		return;
	}
	{
		ShowWindow(SW_HIDE);
		char oldWorkPath[256] = { 0 };
		GetCurrentDirectoryA(sizeof(oldWorkPath), oldWorkPath);
		BOOL ret = SetCurrentDirectoryA(oldWorkPath);
		if (!ret)
		{
			MessageBox(NULL, L"����Ŀ¼�������û��Ȩ��", L"PKPMV51", 1);
			//return;
		}
		run_cmd(module.c_str(), app.c_str(), "");
		//m_WorkPath = prjPaths_[prjSelected].c_str();
		//run_cmd(CString(vec.front().c_str()), CString((*++iter).c_str()), "");
		SetCurrentDirectoryA(oldWorkPath);
		//fix me 
		//������Ҫ��ҳ���¶�һ�¹����б�
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
	//				MessageBoxA(NULL, "����Ŀ¼�������û��Ȩ��", "PKPMV51", 1);
	//			}
	//			//run_cmd(CString(vec.front().c_str()), CString((*++iter).c_str()), "");
	//			SetCurrentDirectoryA(oldWorkPath);
	//			//������֪ͨ��ҳ���¹���
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
		::AfxMessageBox(L"���ش���");
	if (!prjPaths_.size())
	{//��Ӧ���ٳ����������
		::AfxMessageBox(L"����Ŀ¼������");
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
			::AfxMessageBox(L"����Ŀ¼�������û��Ȩ��");
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
	//�ӵ���
	if (!prjPaths_.size())
	{
		AfxMessageBox(L"û��ѡ����Ŀ¼");
		return;
	}
	//auto prjSelected = std::stoi(MenuSelectionOnHtml().back().second);
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
	for (int i = 0; i < maxPrjNum_; ++i)
	{
		std::string workPathId = std::string("WorkPath") + std::to_string(i);
		if (static_cast<size_t>(i) < prjPaths.size())
			WritePrivateProfileStringA("WorkPath", workPathId.c_str(), prjPaths[i].c_str(), filename.c_str());
		else//de a bug 
			WritePrivateProfileStringA("WorkPath", workPathId.c_str(), NULL, filename.c_str());
	}
}

bool CefForm::AddWorkPaths(const std::string& newProj, const std::string& filename)
{
	if (newProj.empty())
		return false;
	auto iter = std::find(prjPaths_.begin(), prjPaths_.end(), newProj);
	if (iter == prjPaths_.end())
	{
		prjPaths_.put(newProj);
		SaveWorkPaths(prjPaths_, filename);
		return true;
	}
	else
	{
		auto index = std::distance(prjPaths_.begin(), iter);
		//MessageBox(_T("�ļ����Ѵ���"));
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
		auto indexSelection = s.GetString();
		WritePrivateProfileStringA("Html", toRead[i], indexSelection, nbase::UnicodeToAnsi(FullPathOfPkpmIni()).c_str());
	}
	//���ݽṹ��ǰ�˶��ģ����ԣ���ʵû��ѡ��
	Value& s = d[toRead[i]];
	int indexSelection = atoi(s.GetString());
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

	if (moduleName == "����ת��")
	{
		if (appName1 == "SP3D/PDS")
			appName1 = "PDS";
		else if (appName1 == "Revit")
		{
			appName1 = "����Revit";
		}
	}
	char str[400];
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
	try
	{
		//appDll_.
		//appDll_.
		//appDll_.fuc_RunCommand(str);
	}
	catch (...)
	{
	}

	//�ָ�����Ŀ¼
	//ASSERT(SetCurrentDirectory(m_WorkPath));//?
}


bool CefForm::SetCfgPmEnv()
{
	const int LENGTH_OF_ENV = 1024 * 4;
	TCHAR* szOriEnvPath = new  TCHAR[LENGTH_OF_ENV];//������Ƭ�ڴ�ȥ����....?
	DWORD dwRet = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, LENGTH_OF_ENV);
	if (!dwRet)
	{
		AfxMessageBox(L"Error! Can not find Path");
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
	int	iv = SetEnvironmentVariable(_T("PATH"), strPath);
	if (iv == 0)
	{
		AfxMessageBox(L"�޷�����PATH·��");
		return false;
	}
	else
	{
		return true;
	}
	return false;
}