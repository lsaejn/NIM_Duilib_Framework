#include "pch.h"
#include "ShortCutHandler.h"
#include "svr.h"

const wchar_t* separator = L"\\";
const wchar_t* UpdateUrl = L"https://www.pkpm.cn/index.php?m=content&c=index&a=show&catid=70&id=112";
const wchar_t* urlAboutPkpm = L"https://www.pkpm.cn";
const wchar_t serverIp[128] = L"111.230.143.87";
const char* defaultAdvertise = "{data:[ \
{\"key\":\"�ٷ���վ: www.pkpm.cn\", \"value\":\"www.pkpm.cn\"}, "
"{\"key\":\"�ٷ���̳: www.pkpm.cn/bbs\",\"value\":\"www.pkpm.cn/bbs\"}"
"]}";

CString cfg_key_str;

CString get_cfg_path_reg_key()
{
	return cfg_key_str;
}


class ShortCutHandlerImpl
{
public:
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
		//CDlgRegRunAsHint dlg;
		//dlg.DoModal();
		//CString cfgpa = svr::GetRegCFGPath() + _T("RegPKPMCtrl.exe");
		//CString cmdline;
		//cmdline.Format(L"/n,/select,%s", cfgpa);
		//::ShellExecute(NULL, L"open", L"explorer.exe", cmdline, NULL, SW_SHOWNORMAL);
	}

	void OnContactUs()
	{
		/*ShellExecute(NULL, L"open", m_sOnlineAsk, NULL, NULL, SW_SHOWDEFAULT);*/
	}

	void OnIntegrityCheck()
	{
		//����һ������������������ÿ�ζ�Ҫ������
//ճ��������Ϊ�Ҵ����cfgĿ¼
		//CString regcmd = svr::GetRegCFGPath() + m_strNameOfIntegrity;
		//CFile fi;
		//if (fi.Open(regcmd, CFile::readOnly, NULL, NULL))
		//{
		//	fi.Close();
		//	HINSTANCE hi = ShellExecute(NULL, "open", regcmd, NULL, NULL, SW_NORMAL);
		//	WaitForSingleObject(hi, INFINITE);
		//}
		//else
		//{
		//	CString strHint;
		//	strHint.Format("�޷��ҵ����������� %s", regcmd);
		//	MessageBox(strHint, "������ʾ");
		//}
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
		/*OnBnClickedBtnSetLock();*/
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
		//CArray<MODULE_PATH> modPaths;
		//CString strPatTDGL;
		//if (svr::getModulePath(modPaths))
		//{
		//	for (int i = 0; i != modPaths.GetSize(); ++i)
		//	{
		//		if (!modPaths[i].m_name.CompareNoCase("TDGL"))
		//		{
		//			strPatTDGL = modPaths[i].m_path;
		//			toolsvr::FixPathStr(strPatTDGL);
		//			break;
		//		}
		//	}
		//}

		//if (strPatTDGL.IsEmpty())
		//{
		//	CString strHint;
		//	strHint.Format("�޷��ҵ���װĿ¼TDGL");
		//	MessageBox(strHint, "������ʾ");
		//	return;
		//}

		//CString strCfgPa;
		//GetAppPathByCFGPATHMarker(get_cfg_path_reg_key(), strCfgPa);
		//toolsvr::FixPathStr(strCfgPa);

		//CString regcmd = strPatTDGL + m_strNameOfPManager;
		//CString cmdParm = "-f ";
		//cmdParm += "\"" + strCfgPa + "pkpm.ini\"";
		//CFile fi;
		//if (fi.Open(regcmd, CFile::readOnly, NULL, NULL))
		//{
		//	fi.Close();
		//	HINSTANCE hi = ShellExecute(NULL, "open", regcmd, cmdParm, NULL, SW_NORMAL);
		//	WaitForSingleObject(hi, INFINITE);//wtf
		//}
		//else
		//{
		//	CString strHint;
		//	strHint.Format("�޷��ҵ����������� %s %s", regcmd, cmdParm);
		//	MessageBox(strHint, "������ʾ");
		//}
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
