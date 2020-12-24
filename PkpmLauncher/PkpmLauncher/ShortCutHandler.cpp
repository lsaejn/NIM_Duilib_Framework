#include "pch.h"
#include "ShortCutHandler.h"
#include "templates.h"
#include "utility.h"
#include "Alime/FileSystem.h"
#include "ConfigFileManager.h"
#include "SpdlogForward.h"


const wchar_t* separator = L"\\";
const wchar_t* UpdateUrl = L"https://www.pkpm.cn/index.php?m=content&c=index&a=show&catid=70&id=112";
const wchar_t* urlAboutPkpm = L"https://www.pkpm.cn";

std::wstring GetCfgPath_Inner()
{
	static std::wstring cfgPath = nbase::win32::GetCurrentModuleDirectory() + L"CFG\\";
	if (!PathFileExists(cfgPath.c_str()))
	{
		MsgBox::ShowViaID(L"ERROR_TIP_FIND_CFG_FOLDER_ERROR", L"TITLE_FOLDER_ERROR");
		std::abort();
	}
	return cfgPath;
}

class ShortCutHandlerImpl
{
public:
	friend class ShortCutHandler;
	class DllScopeGuarder
	{
	public:
		void SetDll(HMODULE dll)
		{
			dll_ = dll;
		}
		~DllScopeGuarder()
		{
			if(dll_)
				FreeLibrary(dll_);
		}
		HMODULE dll_;
	};
private:
	std::wstring m_strNameOfIntegrity;
	std::wstring m_strNameOfPManager;
	HMODULE dll_;
	DllScopeGuarder guard_;
	CallBack DoBeforeCallFunc_;//����ǰ�ĺ���,һ��������������
	CallBack DoAfterCallFunc_;//��ʾ������
	CallBack RefreshConfigFileFunc_;//��ģ�ʹ���õģ�������Ҫ������·���������·���Ǵ������ļ���ȥ�����˲�!
	BooleanCallBack NewVersionChecker_;//��ѯ�Ƿ�����°汾
	HWND mainWnd_;
public:
	ShortCutHandlerImpl()
	{
		m_strNameOfIntegrity = L"PkpmIntegrityCheck.exe";
		m_strNameOfPManager = L"PMANAGER.exe";

		std::wstring path = nbase::win32::GetCurrentModuleDirectory();
		path += L"PKPM2010V511.dll";
		dll_ = LoadLibrary(path.c_str());// ����DLL���ļ���DLL���ƺ�·�����Լ���
		if (!dll_)
		{
			OutputDebugString(L"��̬�����ʧ��");
			return;
		}
		guard_.SetDll(dll_);
	}

	~ShortCutHandlerImpl()
	{
		//delete
	}

	void SetNewVersionChecker(BooleanCallBack f)
	{
		NewVersionChecker_ = std::move(f);
	}

	void SetFreshFunc(CallBack _f)
	{
		RefreshConfigFileFunc_ = std::move(_f);
	}

	void SetBeforeFunc(CallBack _f)
	{
		DoBeforeCallFunc_ = std::move(_f);
	}

	void SetAfterFunc(CallBack _f)
	{
		DoAfterCallFunc_ = std::move(_f);
	}

	void OnAboutPkpm()
	{
		::ShellExecute(NULL, L"open", urlAboutPkpm, NULL, NULL, SW_SHOWDEFAULT);
	}

	void OnImprovement()
	{
		auto updateFilePath = nbase::win32::GetCurrentModuleDirectory() + L"Help\\UpdateGuider";
		::ShellExecute(NULL, L"open", L"explorer.exe", updateFilePath.c_str(), NULL, SW_SHOWNORMAL);
	}

	void OnModelPacking()
	{
		if(RefreshConfigFileFunc_)
			RefreshConfigFileFunc_();
		OnBnClickedBtnFileMgr();
	}

	void OnRegiser()
	{
		typedef int (*pExport)(void);
		pExport func = (pExport)GetProcAddress(dll_, "RegCheck");
		auto ret=func();
		if (ret != IDOK)
			return;
		std::wstring cfgpa = nbase::win32::GetCurrentModuleDirectory()+L"CFG\\"+ _T("RegPKPMCtrl.exe");
		std::wstring cmdline = L"/n,/select," + cfgpa;
		::ShellExecute(NULL, L"open", L"explorer.exe", cmdline.c_str(), NULL, SW_SHOWNORMAL);
	}

	void OnContactUs()
	{
		/*ShellExecute(NULL, L"open", m_sOnlineAsk, NULL, NULL, SW_SHOWDEFAULT);*/
	}

	void OnIntegrityCheck()
	{
		std::wstring regcmd = GetCfgPath_Inner() + m_strNameOfIntegrity;
		CFile fi;
		if (fi.Open(regcmd.c_str(), CFile::readOnly, NULL, NULL))
		{
			fi.Close();
			ShellExecute(NULL, _T("open"), regcmd.c_str(), NULL, NULL, SW_NORMAL);
		}
		else
			MsgBox::ShowViaID(L"ERROR_TIP_FIND_APP_ERROR", L"TITLE_ERROR");
	}

	void OnParameterSettings()
	{
		typedef void (*pExport)(void);
		pExport func = (pExport)GetProcAddress(dll_, "OpenPkpmInfoSheet");
		func();
	}

	void OnSwitchToNetVersion(HWND mainWnd)
	{
		if (!mainWnd)
		{
			AfxMessageBox(L"Fatal error, can not find main window", MB_SYSTEMMODAL);
			std::abort();
		}
		std::wstring exePathName = GetCfgPath_Inner() + _T("PKPMAuthorize.exe");
		if (-1 == _taccess(exePathName.c_str(), 0))
		{
			MsgBox::ShowViaIDWithSpecifiedCtn(exePathName, L"TITLE_FIND_FILE_ERROR");
			return;
		}
		TCHAR STRPATH[MAX_PATH];
		ZeroMemory(STRPATH, sizeof(TCHAR) * MAX_PATH);
		_tcscpy_s(STRPATH, MAX_PATH, exePathName.c_str());
		STARTUPINFO info = { 0 };
		info.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION prinfo;
		DoBeforeCallFunc_();
		CreateProcess(NULL, STRPATH, NULL, NULL, FALSE, 0, NULL, NULL, &info, &prinfo);
		WaitForSingleObject(prinfo.hProcess, INFINITE);
		DoAfterCallFunc_();	
		CloseHandle(prinfo.hThread);
		CloseHandle(prinfo.hProcess);
	}

	void OnUserManual()
	{
		//::ShellExecute(0, "open", "explorer.exe", helpPath, NULL, SW_SHOWNORMAL);
	}

	void OnUpdateOnline()
	{
		if (true || NewVersionChecker_())//һ����ȷ������ĸĶ�
		{
			using Alime::FileSystem::FilePath;
			using Alime::FileSystem::File;
			auto exePath=ConfigManager::GetInstance().GetInstallerPath();
			FilePath path{ application_utily::GetExeFolderPath() + exePath };
			if (path.IsFile())
			{
				File file(path);
				if (file.Exists())
				{
					auto param = L" "+std::to_wstring((int)mainWnd_);
					bool ret=application_utily::CreateProcessWithCommand(file.GetFilePath().GetFullPath().c_str(), param.c_str(), NULL);
					if(!ret)
						spdlog::critical("fail to open installer.exe.");
					return;
				}
				else
				{
					spdlog::debug("fail to find installer.exe");
					ShellExecute(NULL, _T("open"), UpdateUrl, NULL, NULL, SW_SHOW);
				}
			}
		}
	}

	//2020/07/13 ����ͼģ��ʦ
	void OnOpenModelViewerMaster()
	{
		application_utily::OnOpenModelViewerMaster();
	}

	void OnBnClickedBtnFileMgr()
	{	
		std::wstring path = nbase::win32::GetCurrentModuleDirectory() + L"TDGL\\" + m_strNameOfPManager.c_str();
		if (!PathFileExists(path.c_str()))
		{
			MsgBox::ShowViaIDWithSpecifiedCtn(path, L"TITLE_FIND_FILE_ERROR");
			return;
		}
		std::wstring cmdParm = L"-f ";
		cmdParm += (L"\"" + GetCfgPath_Inner() + L"pkpm.ini\"").c_str();
		ShellExecute(NULL, L"open", path.c_str(), cmdParm.c_str(), NULL, SW_NORMAL);
		return;
	}
};

#define SHORTCUTFUNC(className,JsName,InnerFuncs) \
	funcMaps_.insert(std::make_pair(##JsName,makeFunc<getCountOfPara(&className::InnerFuncs)+1>(&className::InnerFuncs,this->impl_,this->mainWnd_)));


ShortCutHandler::ShortCutHandler()
	:impl_(new ShortCutHandlerImpl)
{
}

ShortCutHandler::~ShortCutHandler()
{
	delete impl_;
}

void ShortCutHandler::Init()
{
	//���ģ�����˵��ҳ��ԱӦ��ѧ���Լ����ݲ���, �������������������ļ�����
	SHORTCUTFUNC(ShortCutHandlerImpl, "����PKPM", OnAboutPkpm)
	SHORTCUTFUNC(ShortCutHandlerImpl, "����", OnAboutPkpm)
	SHORTCUTFUNC(ShortCutHandlerImpl, "�Ľ�˵��", OnImprovement)
	SHORTCUTFUNC(ShortCutHandlerImpl, "ģ�ʹ��", OnModelPacking)
	SHORTCUTFUNC(ShortCutHandlerImpl, "ע��ؼ�", OnRegiser)
	SHORTCUTFUNC(ShortCutHandlerImpl, "��ϵ����", OnContactUs)
	SHORTCUTFUNC(ShortCutHandlerImpl, "�����Լ��", OnIntegrityCheck)
	SHORTCUTFUNC(ShortCutHandlerImpl, "��������", OnParameterSettings)
	SHORTCUTFUNC(ShortCutHandlerImpl, "��������", OnSwitchToNetVersion)
	SHORTCUTFUNC(ShortCutHandlerImpl, "�û��ֲ�", OnUserManual)
	SHORTCUTFUNC(ShortCutHandlerImpl, "���߸���", OnUpdateOnline)
	SHORTCUTFUNC(ShortCutHandlerImpl, "ͼģ��ʦ", OnOpenModelViewerMaster)
}

void ShortCutHandler::CallFunc(const std::string& cutName)
{
	if (Contains(cutName))
		(funcMaps_[cutName])();
	else
	{
		MsgBox::WarningViaID(mainWnd_, L"ERROR_TIP_UNEXCEPTED_MENU", L"TITLE_ERROR");
	}
}

bool ShortCutHandler::Contains(const std::string& cutName) const
{
	return funcMaps_.find(cutName) != funcMaps_.end();
}

void ShortCutHandler::SetNewVersionChecker(BooleanCallBack f)
{
	impl_->SetNewVersionChecker(f);
}

//copyһ��Ȼ��move
void ShortCutHandler::SetCallBacks(HWND wnd, CallBack _f)
{
	mainWnd_ = wnd;
	//then
	impl_->mainWnd_ = this->mainWnd_;
	if (mainWnd_)
	{
		impl_->SetBeforeFunc([this]() {
			ShowWindow(this->mainWnd_, SW_HIDE);
			});
		impl_->SetAfterFunc([this]() {
			ShowWindow(this->mainWnd_, SW_SHOW);
			});
		impl_->SetFreshFunc(std::move(_f));
	}
}
