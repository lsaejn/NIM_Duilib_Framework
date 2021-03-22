#include "pch.h"
#include "shlwapi.h"
#include "shellapi.h"
#include "ShortCutHandler.h"
#include "templates.h"
#include "utility.h"
#include "Alime/FileSystem.h"
#include "ConfigFileManager.h"
#include "SpdlogForward.h"

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
	HMODULE dll_;
	DllScopeGuarder guard_;
	CallBack DoBeforeCallFunc_;//调用前的函数,一般是隐藏主窗口
	CallBack DoAfterCallFunc_;//显示主窗口
	CallBack RefreshConfigFileFunc_;//给模型打包用的，它们需要读工程路径，而这个路径是从配置文件里去读。人才!
	BooleanCallBack NewVersionChecker_;//查询是否存在新版本
	HWND mainWnd_;
public:
	ShortCutHandlerImpl()
	{
		std::wstring path = nbase::win32::GetCurrentModuleDirectory();
		path += L"PKPM2010V511.dll";
		dll_ = LoadLibrary(path.c_str());// 加载DLL库文件，DLL名称和路径用自己的
		if (!dll_)
		{
			OutputDebugString(L"动态库加载失败");
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
		//std::wstring cfgpa = nbase::win32::GetCurrentModuleDirectory()+L"CFG\\"+ _T("RegPKPMCtrl.exe");
		std::wstring regControl = ConfigManager::GetInstance().GetProgramPath(L"regControl");
		std::wstring cmdline = L"/n,/select," + regControl;
		::ShellExecute(NULL, L"open", L"explorer.exe", cmdline.c_str(), NULL, SW_SHOWNORMAL);
	}

	void CallFuncByName(const std::wstring& name)
	{
		auto path=ConfigManager::GetInstance().GetProgramPath(name);
		if (path.empty() || !PathFileExists(path.c_str()))
		{
			MsgBox::ShowViaID(L"ERROR_TIP_FIND_APP_ERROR", L"TITLE_ERROR");
		}
		else
		{
			::ShellExecute(NULL, _T("open"), path.c_str(), NULL, NULL, SW_NORMAL);
		}
	}

	//void OnIntegrityCheck()
	//{
	//	const std::wstring &regcmd = m_strNameOfIntegrity;
	//	if (PathFileExists(regcmd.c_str()))
	//	{
	//		::ShellExecute(NULL, _T("open"), regcmd.c_str(), NULL, NULL, SW_NORMAL);
	//	}
	//	else
	//		MsgBox::ShowViaID(L"ERROR_TIP_FIND_APP_ERROR", L"TITLE_ERROR");
	//}

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
			MessageBox(NULL, L"Fatal error, can not find main window", L"error", MB_SYSTEMMODAL);
			std::abort();
		}
		std::wstring exePathName = ConfigManager::GetInstance().GetProgramPath(L"authorize");
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

	void OnUpdateOnline()
	{
		if (true || NewVersionChecker_())//一个不确定需求的改动
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
					ShellExecute(NULL, _T("open"), 
						ConfigManager::GetInstance().GetStructDownLoadWeb().c_str(),
						NULL, NULL, SW_SHOW);
				}
			}
		}
	}

	void OnOpenModelViewerMaster()
	{
		application_utily::OnOpenModelViewerMaster();
	}

	void OnBnClickedBtnFileMgr()
	{
		const std::wstring& path = ConfigManager::GetInstance().GetProgramPath(L"模型打包");
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


#define SHORTCUTFUNC(className, JsName, InnerFuncs) \
	funcMaps_.insert(std::make_pair(##JsName,makeFunc<getCountOfPara(&className::InnerFuncs)+1>(&className::InnerFuncs,this->impl_,this->mainWnd_)));
#define SHORTCUTFUNC1(className, JsName, InnerFuncs) \
	funcMaps_.insert(std::make_pair(JsName, std::bind(&className::InnerFuncs, this->impl_, JsName)));


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
	//不改，按理说网页人员应该学会自己传递参数, 而不是让我利用配置文件反射
	SHORTCUTFUNC1(ShortCutHandlerImpl, L"完整性检查", CallFuncByName)
	SHORTCUTFUNC1(ShortCutHandlerImpl, L"注册控件", CallFuncByName)
	SHORTCUTFUNC1(ShortCutHandlerImpl, L"锁码验证", CallFuncByName)

	SHORTCUTFUNC(ShortCutHandlerImpl, L"模型打包", OnModelPacking)
	SHORTCUTFUNC(ShortCutHandlerImpl, L"关于PKPM", OnAboutPkpm)
	SHORTCUTFUNC(ShortCutHandlerImpl, L"关于", OnAboutPkpm)
	SHORTCUTFUNC(ShortCutHandlerImpl, L"改进说明", OnImprovement)
	//SHORTCUTFUNC(ShortCutHandlerImpl, L"联系我们", OnContactUs)
	SHORTCUTFUNC(ShortCutHandlerImpl, L"参数设置", OnParameterSettings)
	//SHORTCUTFUNC(ShortCutHandlerImpl, L"用户手册", OnUserManual)
	SHORTCUTFUNC(ShortCutHandlerImpl, L"在线更新", OnUpdateOnline)
	SHORTCUTFUNC(ShortCutHandlerImpl, L"图模大师", OnOpenModelViewerMaster)
	//SHORTCUTFUNC(ShortCutHandlerImpl, "自动测试", OnOpenAutoTest)
		
}

void ShortCutHandler::CallFunc(const std::wstring& cutName)
{
	if (Contains(cutName))
		(funcMaps_[cutName])();
	else
	{
		MsgBox::WarningViaID(mainWnd_, L"ERROR_TIP_UNEXCEPTED_MENU", L"TITLE_ERROR");
		//尽力而为的模式。为了让网页和配置文件发挥最大左右
		this->impl_->CallFuncByName(cutName.c_str());
	}
}

bool ShortCutHandler::Contains(const std::wstring& cutName) const
{
	return funcMaps_.find(cutName) != funcMaps_.end();
}

void ShortCutHandler::SetNewVersionChecker(BooleanCallBack f)
{
	impl_->SetNewVersionChecker(f);
}

//copy一份然后move
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
