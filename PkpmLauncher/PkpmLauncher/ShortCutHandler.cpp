#include "pch.h"
#include "ShortCutHandler.h"
#include "svr.h"
#include "pkpm2014svr.h"
#include "templates.h"

const wchar_t* separator = L"\\";
const wchar_t* UpdateUrl = L"https://www.pkpm.cn/index.php?m=content&c=index&a=show&catid=70&id=112";
const wchar_t* urlAboutPkpm = L"https://www.pkpm.cn";


TCHAR* m_ini_file = _T("PKPM2010V51.ini");


std::wstring GetCfgPath_Inner()
{
	static std::wstring cfgPath = nbase::win32::GetCurrentModuleDirectory() + L"CFG\\";
	if (!PathFileExists(cfgPath.c_str()))
	{
		AfxMessageBox(L"��װĿ¼��û��CFG�ļ���");
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
	CString m_strNameOfIntegrity;
	CString m_strNameOfPManager;
	HMODULE dll_;
	DllScopeGuarder guard_;
	CallBack DoBeforeCallFunc_;//����ǰ�ĺ���,һ��������������
	CallBack DoAfterCallFunc_;//��ʾ������
	CallBack RefreshConfigFileFunc_;//��ģ�ʹ���õģ�������Ҫ������·���������·���Ǵ������ļ���ȥ�����˲�!
public:
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
			AfxMessageBox(L"�޷��ҵ�����������Ѿ������г���");
	}

	void OnParameterSettings()
	{
		typedef void (*pExport)(void);
		pExport func = (pExport)GetProcAddress(dll_, "OpenPkpmInfoSheet");
		func();
	}

	//fix me, ��������ȥ����
	void OnSwitchToNetVersion(HWND mainWnd)
	{
		if (!mainWnd)
		{
			//��Ӧ�ó����������
			AfxMessageBox(L"��������δ��ʼ�����", MB_SYSTEMMODAL);
			std::abort();
		}

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
		DoBeforeCallFunc_();
		CreateProcess(NULL, STRPATH, NULL, NULL, FALSE, 0, NULL, NULL, &info, &prinfo);
		WaitForSingleObject(prinfo.hProcess, INFINITE);
		DoAfterCallFunc_();	
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
			strHint.Format(L"�޷��ҵ���װĿ¼TDGL");
			MessageBox(NULL, strHint, L"������ʾ", 1);
			return;
		}

		
		CString strCfgPa;
		cfgpathsvr::GetAppPathByCFGPATHMarker(get_cfg_path_reg_key(), strCfgPa);
		toolsvr::FixPathStr(strCfgPa);
		if (strCfgPa.IsEmpty())
			strCfgPa = svr::GetRegCFGPath();

		CString regcmd = strPatTDGL + m_strNameOfPManager;
		if (!PathFileExists(regcmd))
		{
			regcmd = (nbase::win32::GetCurrentModuleDirectory() + L"TDGL\\").c_str() + m_strNameOfPManager;
			CString cmdParm = L"-f ";
			cmdParm += (L"\"" + GetCfgPath_Inner() + L"pkpm.ini\"").c_str();
			ShellExecute(NULL, L"open", regcmd, cmdParm, NULL, SW_NORMAL);
			return;
		}

		strCfgPa = (nbase::win32::GetCurrentModuleDirectory() + L"CFG\\").c_str();
		CString cmdParm = L"-f ";
		cmdParm += L"\"" + strCfgPa + L"pkpm.ini\"";
		CFile fi;
		if (fi.Open(regcmd, CFile::readOnly, NULL, NULL))
		{
			fi.Close();
			ShellExecute(NULL, L"open", regcmd, cmdParm, NULL, SW_NORMAL);
		}
		else
			AfxMessageBox(L"�޷��ҵ�����������Ѿ������г���");
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
	SHORTCUTFUNC(ShortCutHandlerImpl, "����PKPM", OnAboutPkpm)
	SHORTCUTFUNC(ShortCutHandlerImpl, "�Ľ�˵��", OnImprovement)
	SHORTCUTFUNC(ShortCutHandlerImpl, "ģ�ʹ��", OnModelPacking)
	SHORTCUTFUNC(ShortCutHandlerImpl, "ע��ؼ�", OnRegiser)
	SHORTCUTFUNC(ShortCutHandlerImpl, "��ϵ����", OnContactUs)
	SHORTCUTFUNC(ShortCutHandlerImpl, "�����Լ��", OnIntegrityCheck)
	SHORTCUTFUNC(ShortCutHandlerImpl, "��������", OnParameterSettings)
	SHORTCUTFUNC(ShortCutHandlerImpl, "��������", OnSwitchToNetVersion)
	SHORTCUTFUNC(ShortCutHandlerImpl, "�û��ֲ�", OnUserManual)
	SHORTCUTFUNC(ShortCutHandlerImpl, "���߸���", OnUpdateOnline)
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

//copyһ��Ȼ��move
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
