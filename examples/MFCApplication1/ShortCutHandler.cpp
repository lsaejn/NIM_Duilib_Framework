#include "pch.h"
#include "ShortCutHandler.h"
#include "DlgRegRunAsHint.h"
#include "MFCApplication1Dlg.h"
#include "svr.h"
#include "pkpm2014svr.h"

const wchar_t* separator = L"\\";
const wchar_t* UpdateUrl = L"https://www.pkpm.cn/index.php?m=content&c=index&a=show&catid=70&id=112";
const wchar_t* urlAboutPkpm = L"https://www.pkpm.cn";
const wchar_t serverIp[128] = L"111.230.143.87";
const char* defaultAdvertise = "{data:[ \
{\"key\":\"�ٷ���վ: www.pkpm.cn\", \"value\":\"www.pkpm.cn\"}, "
"{\"key\":\"�ٷ���̳: www.pkpm.cn/bbs\",\"value\":\"www.pkpm.cn/bbs\"}"
"]}";

extern HWND mainWnd;
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
		auto hDll = LoadLibrary(path.c_str());// ����DLL���ļ���DLL���ƺ�·�����Լ���
		typedef void (*pExport)(void);
		pExport func = (pExport)GetProcAddress(hDll, "RegCheck");
		func();
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
			strHint.Format(L"�޷��ҵ�����������Ѿ������г��� %s", regcmd);
			AfxMessageBox(strHint);
		}
	}

	void OnParameterSettings()
	{
		//{
		//	CString strFullName;
		//	GetAppPathByCFGPATHMarker(get_cfg_path_reg_key(), strFullName);
		//	toolsvr::FixPathStr(strFullName);
		//	if (DRIVE_REMOTE == GetDriveType(strFullName))
		//	{
		//		CString strHint = "���������ķ������������ļ�������ȷ���Ƿ���з������ļ��Ķ�дȨ�ޡ�";
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
		//		CPkpmInfoSheet dialog(_T("PKPMȫ�ֲ�������"), NULL, bHasPwe, nOpen);
		//		dialog.m_otherspage.SetPageType(2);//��ǰҳ������
		//		dialog.m_psh.dwFlags &= ~PSH_HASHELP;
		//		nRet = dialog.DoModal();
		//	} while (false);
		//}
	}

	void OnSwitchToNetVersion()
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
			strHint.Format(L"�޷��ҵ���װĿ¼TDGL");
			MessageBox(NULL, strHint, L"������ʾ", 1);
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
			strHint.Format(L"�޷��ҵ�������������Ѵ� %s %s", regcmd, cmdParm);
			MessageBox(NULL, strHint, L"������ʾ", 1);
		}
	}
};


//��ɾ�����˾ɴ�����ĺ꣬�Ա����Ժ�ά���ߵĲ���
#define SHORTCUTFUNCS(className,JsName,InnerFuncs) \
	funcMap_.insert(std::make_pair(##JsName,&className::InnerFuncs));


ShortCutHandler::ShortCutHandler()
	:impl_(new ShortCutHandlerImpl)
{
	Init();
}

ShortCutHandler::~ShortCutHandler()
{
	delete impl_;
}

void ShortCutHandler::Init()
{
	SHORTCUTFUNCS(ShortCutHandlerImpl, "����PKPM", OnAboutPkpm)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "�Ľ�˵��", OnImprovement)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "ģ�ʹ��", OnModelPacking)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "ע��ؼ�", OnRegiser)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "��ϵ����", OnContactUs)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "�����Լ��", OnIntegrityCheck)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "��������", OnParameterSettings)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "��������", OnSwitchToNetVersion)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "�û��ֲ�", OnUserManual)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "���߸���", OnUpdateOnline)
}

void ShortCutHandler::CallFunc(const std::string& cutName)
{
	if (Contains(cutName))
		(impl_->*funcMap_[cutName])();
	else
		throw std::invalid_argument("invalid event name");
}

bool ShortCutHandler::Contains(const std::string& cutName) const
{
	return funcMap_.find(cutName) != funcMap_.end();
}
