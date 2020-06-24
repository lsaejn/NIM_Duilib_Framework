#include "pch.h"
#include "ShortCutHandler.h"
#include "templates.h"

const wchar_t* separator = L"\\";
const wchar_t* UpdateUrl = L"https://www.pkpm.cn/index.php?m=content&c=index&a=show&catid=70&id=112";
const wchar_t* urlAboutPkpm = L"https://www.pkpm.cn";

std::wstring GetCfgPath_Inner()
{
	static std::wstring cfgPath = nbase::win32::GetCurrentModuleDirectory() + L"CFG\\";
	if (!PathFileExists(cfgPath.c_str()))
	{
		AfxMessageBox(L"安装目录下没有CFG文件夹");
		std::abort();
	}
	return cfgPath;
}

class ShortCutHandlerImpl
{
public:
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
	CallBack DoBeforeCallFunc_;//调用前的函数,一般是隐藏主窗口
	CallBack DoAfterCallFunc_;//显示主窗口
	CallBack RefreshConfigFileFunc_;//给模型打包用的，它们需要读工程路径，而这个路径是从配置文件里去读。人才!
public:
	ShortCutHandlerImpl()
	{
		m_strNameOfIntegrity = L"PkpmIntegrityCheck.exe";
		m_strNameOfPManager = L"PMANAGER.exe";

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
			AfxMessageBox(L"无法找到程序或者您已经在运行程序");
	}

	void OnParameterSettings()
	{
		typedef void (*pExport)(void);
		pExport func = (pExport)GetProcAddress(dll_, "OpenPkpmInfoSheet");
		func();
	}

	//fix me, 参数可以去掉了
	void OnSwitchToNetVersion(HWND mainWnd)
	{
		if (!mainWnd)
		{
			AfxMessageBox(L"主窗口尚未初始化完成", MB_SYSTEMMODAL);
			std::abort();
		}
		std::wstring exePathName = GetCfgPath_Inner() + _T("PKPMAuthorize.exe");
		if (-1 == _taccess(exePathName.c_str(), 0))
		{
			AfxMessageBox((L"无法找到该程序 "+exePathName).c_str(), MB_SYSTEMMODAL);
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
		ShellExecute(NULL, _T("open"), UpdateUrl, NULL, NULL, SW_SHOW);
	}

	void OnBnClickedBtnFileMgr()
	{
		std::wstring path = nbase::win32::GetCurrentModuleDirectory() + L"TDGL\\" + m_strNameOfPManager.c_str();
		if (!PathFileExists(path.c_str()))
		{
			AfxMessageBox(L"无法找到模型打包程序 PMANAGER.exe");
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
	SHORTCUTFUNC(ShortCutHandlerImpl, "关于PKPM", OnAboutPkpm)
	SHORTCUTFUNC(ShortCutHandlerImpl, "改进说明", OnImprovement)
	SHORTCUTFUNC(ShortCutHandlerImpl, "模型打包", OnModelPacking)
	SHORTCUTFUNC(ShortCutHandlerImpl, "注册控件", OnRegiser)
	SHORTCUTFUNC(ShortCutHandlerImpl, "联系我们", OnContactUs)
	SHORTCUTFUNC(ShortCutHandlerImpl, "完整性检查", OnIntegrityCheck)
	SHORTCUTFUNC(ShortCutHandlerImpl, "参数设置", OnParameterSettings)
	SHORTCUTFUNC(ShortCutHandlerImpl, "锁码设置", OnSwitchToNetVersion)
	SHORTCUTFUNC(ShortCutHandlerImpl, "用户手册", OnUserManual)
	SHORTCUTFUNC(ShortCutHandlerImpl, "在线更新", OnUpdateOnline)
}

void ShortCutHandler::CallFunc(const std::string& cutName)
{
	if (Contains(cutName))
		(funcMaps_[cutName])();
	else
		throw std::invalid_argument("invalid event name");
}

bool ShortCutHandler::Contains(const std::string& cutName) const
{
	return funcMaps_.find(cutName) != funcMaps_.end();
}

//copy一份然后move
void ShortCutHandler::SetCallBacks(HWND wnd, CallBack _f)
{
	mainWnd_ = wnd;
	//then
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
