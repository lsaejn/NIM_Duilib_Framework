#include "pch.h"
#include "ShortCutHandler.h"
#include "MFCApplication1Dlg.h"
#include "svr.h"
#include "pkpm2014svr.h"

const wchar_t* separator = L"\\";
const wchar_t* UpdateUrl = L"https://www.pkpm.cn/index.php?m=content&c=index&a=show&catid=70&id=112";
const wchar_t* urlAboutPkpm = L"https://www.pkpm.cn";


TCHAR* m_ini_file = _T("PKPM2010V51.ini");


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


CString get_cfg_path_reg_key()
{
	return GetCfgPath_Inner().c_str();
}


class ShortCutHandlerImpl
{
public:
	CString m_strNameOfIntegrity;
	CString m_strNameOfPManager;

	ShortCutHandlerImpl()
	{
		TCHAR filename_Integrity[128] = { 0 };
		GetPrivateProfileString(_T("CONFIG"), _T("INTEGRITYCHECK"), _T("PkpmIntegrityCheck.exe"),
			filename_Integrity, sizeof(filename_Integrity) - 1, m_ini_file);
		m_strNameOfIntegrity = filename_Integrity;
		m_strNameOfIntegrity.Trim();

		TCHAR filename_PManager[128];
		GetPrivateProfileString(_T("CONFIG"), _T("PMANAGER"), _T("PMANAGER.exe"), 
			filename_PManager, sizeof(filename_PManager), m_ini_file);
		m_strNameOfPManager = filename_PManager;
		m_strNameOfPManager.Trim();
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
		OnBnClickedBtnFileMgr();
	}

	void OnRegiser()
	{
		std::wstring path = nbase::win32::GetCurrentModuleDirectory();
		path += L"PKPM2010V511.dll";
		auto hDll = LoadLibrary(path.c_str());// 加载DLL库文件，DLL名称和路径用自己的
		typedef int (*pExport)(void);
		pExport func = (pExport)GetProcAddress(hDll, "RegCheck");
		auto ret=func();
		if (ret != IDOK)
			return;
		std::wstring cfgpa = nbase::win32::GetCurrentModuleDirectory()+L"CFG\\"+ _T("RegPKPMCtrl.exe");
		CString cmdline;
		cmdline.Format(L"/n,/select,%s", cfgpa.c_str());
		::ShellExecute(NULL, L"open", L"explorer.exe", cmdline, NULL, SW_SHOWNORMAL);
	}

	void OnContactUs()
	{
		/*ShellExecute(NULL, L"open", m_sOnlineAsk, NULL, NULL, SW_SHOWDEFAULT);*/
	}

	void OnIntegrityCheck()
	{
		CString regcmd = svr::GetRegCFGPath() + m_strNameOfIntegrity;
		CFile fi;
		if (fi.Open(regcmd, CFile::readOnly, NULL, NULL))
		{
			fi.Close();
			ShellExecute(NULL, _T("open"), regcmd, NULL, NULL, SW_NORMAL);
		}
		else
		{
			CString strHint;
			strHint.Format(L"无法找到程序或者您已经在运行程序 %s", regcmd);
			AfxMessageBox(strHint);
		}
	}

	void OnParameterSettings()
	{
		std::wstring path = nbase::win32::GetCurrentModuleDirectory();
		path += L"PKPM2010V511.dll";
		auto hDll = LoadLibrary(path.c_str());// 加载DLL库文件，DLL名称和路径用自己的
		typedef void (*pExport)(void);
		pExport func = (pExport)GetProcAddress(hDll, "OpenPkpmInfoSheet");
		func();
		//{
		//	CString strFullName;
		//	GetAppPathByCFGPATHMarker(get_cfg_path_reg_key(), strFullName);
		//	toolsvr::FixPathStr(strFullName);
		//	if (DRIVE_REMOTE == GetDriveType(strFullName))
		//	{
		//		CString strHint = "您即将更改服务器的配置文件，请您确认是否具有服务器文件的读写权限。";
		//		//strHint = g_stringMgr.LoadString(IDS_MAYNOT_CONFIG_PKPM_INI);
		//		if (IDCANCEL == AfxMessageBox(strHint, MB_OKCANCEL))
		//			return;
		//	}
		//	int nRet = IDOK;
		//	int nOpen = 0;
		//	do
		//	{
		//		int nlen;
		//		bool bHasPwe = false;
		//		if (fuc_IsProjectWiseVailed)
		//		{
		//			bHasPwe = fuc_IsProjectWiseVailed(NULL, nlen);
		//		}
		//		CPkpmInfoSheet dialog(_T("PKPM全局参数设置"), NULL, bHasPwe, nOpen);
		//		dialog.m_otherspage.SetPageType(2);//当前页面类型
		//		dialog.m_psh.dwFlags &= ~PSH_HASHELP;
		//		nRet = dialog.DoModal();
		//	} while (false);
		//}
	}

	void OnSwitchToNetVersion(HWND mainWnd)
	{
		CString cfgpath = GetCfgPath_Inner().c_str();

		CString exePathName = cfgpath + _T("PKPMAuthorize.exe");
		if (-1 == _taccess(exePathName, 0))
		{
			CString strHint;
			strHint += _T("File belowed is not existed.\n");
			strHint += exePathName;
			MessageBox(NULL,strHint, _T("lockconfig"),1);
			return;
		}

		TCHAR STRPATH[MAX_PATH];
		ZeroMemory(STRPATH, sizeof(TCHAR) * MAX_PATH);
		_tcscpy_s(STRPATH, MAX_PATH, exePathName.GetBuffer());
		STARTUPINFO info = { 0 };
		info.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION prinfo;

		CRect rc;
		if (!mainWnd)
		{
			//for debug不应该出现这种情况
			AfxMessageBox(L"主窗口尚未初始化完成");
			std::abort();
		}
		//fix me,隐藏/显示应该封装	
		GetWindowRect(mainWnd, rc);
		ShowWindow(mainWnd, SW_HIDE);
		CreateProcess(NULL, STRPATH, NULL, NULL, FALSE, 0, NULL, NULL, &info, &prinfo);
		WaitForSingleObject(prinfo.hProcess, -1);
		//ShowWindow(SW_SHOWNORMAL);
		MoveWindow(mainWnd,rc.left, rc.top, rc.Width(), rc.Height(), 0);
		::ShowWindow(mainWnd, SW_SHOW);
		//::SetWindowPos(this->m_hWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		CloseHandle(prinfo.hThread);
		CloseHandle(prinfo.hProcess);
	}

	void OnUserManual()
	{
		//CString helpPath = GetAppPath() + "Help\\UserGuider";
		//::ShellExecute(0, "open", "explorer.exe", helpPath, NULL, SW_SHOWNORMAL);
	}

	void OnUpdateOnline()
	{
		ShellExecute(NULL, _T("open"), UpdateUrl, NULL, NULL, SW_SHOW);
	}

	void OnBnClickedBtnFileMgr()
	{
		CArray<MODULE_PATH> modPaths;
		CString strPatTDGL;
		if (svr::getModulePath(modPaths))
		{
			for (int i = 0; i != modPaths.GetSize(); ++i)
			{
				if (!modPaths[i].m_name.CompareNoCase(L"TDGL"))
				{
					strPatTDGL = modPaths[i].m_path;
					toolsvr::FixPathStr(strPatTDGL);
					break;
				}
			}
		}

		if (strPatTDGL.IsEmpty())
		{
			CString strHint;
			strHint.Format(L"无法找到安装目录TDGL");
			MessageBox(NULL, strHint, L"错误提示", 1);
			return;
		}

		
		CString strCfgPa;
		cfgpathsvr::GetAppPathByCFGPATHMarker(get_cfg_path_reg_key(), strCfgPa);
		toolsvr::FixPathStr(strCfgPa);

		CString regcmd = strPatTDGL + m_strNameOfPManager;
		if (!PathFileExists(regcmd))
		{
			regcmd = (nbase::win32::GetCurrentModuleDirectory() + L"TDGL\\").c_str() + m_strNameOfPManager;
			CString cmdParm = L"-f ";
			cmdParm += (L"\"" + GetCfgPath_Inner() + L"pkpm.ini\"").c_str();
			ShellExecute(NULL, L"open", regcmd, cmdParm, NULL, SW_NORMAL);
			return;
		}

		CString cmdParm = L"-f ";
		cmdParm += L"\"" + strCfgPa + L"pkpm.ini\"";
		CFile fi;
		if (fi.Open(regcmd, CFile::readOnly, NULL, NULL))
		{
			fi.Close();
			HINSTANCE hi = ShellExecute(NULL, L"open", regcmd, cmdParm, NULL, SW_NORMAL);
		}
		else
		{
			CString strHint;
			strHint.Format(L"无法找到或启动程序或已打开 %s %s", regcmd, cmdParm);
			MessageBox(NULL, strHint, L"错误提示", 1);
		}
	}
};


//我删掉了了旧代码里的宏，以避免以后维护者的不适
//#define SHORTCUTFUNCS(className,JsName,InnerFuncs) \
//	funcMap_.insert(std::make_pair(##JsName,&className::InnerFuncs));

#define SHORTCUTFUNCS_(className,JsName,InnerFuncs) \
	funcMaps_.insert(std::make_pair(##JsName,std::bind(&className::InnerFuncs,this->impl_)));

#define SHORTCUTFUNCS_WITHWND(className,JsName,InnerFuncs) \
	funcMaps_.insert(std::make_pair(##JsName,std::bind(&className::InnerFuncs,this->impl_,this->mainWnd_)));

template<typename R, typename... Args, typename Class>
constexpr int getCountOfPara(R(Class::*)(Args...))
{
	return sizeof...(Args);
};

template <typename F, typename Tuple, std::size_t... Indices>
decltype(auto) forward_to_f(F f, Tuple&& args,
	std::index_sequence<Indices...>) {
	return std::bind(f, std::get<Indices>(std::move(args))...);
}

template <size_t count, typename F, typename ...Args>
std::function<void(void)> makeFunc(F&& f, Args... args)
{
	auto tuple = std::forward_as_tuple(args...);
	auto seq = std::make_index_sequence<count>{};
	return forward_to_f(f, tuple, seq);
}

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
	//SHORTCUTFUNCS_WITHWND(ShortCutHandlerImpl, "锁码设置", OnSwitchToNetVersion)
	//makeFunc<getCountOfPara(&ShortCutHandlerImpl::OnUserManual)+1>(&ShortCutHandlerImpl::OnUserManual, this->impl_, this->mainWnd_);
	//makeFunc<getCountOfPara(&ShortCutHandlerImpl::OnSwitchToNetVersion)+1>(&ShortCutHandlerImpl::OnSwitchToNetVersion, this->impl_, this->mainWnd_);
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

void ShortCutHandler::SetHwnd(HWND wnd)
{
	mainWnd_ = wnd;
}
