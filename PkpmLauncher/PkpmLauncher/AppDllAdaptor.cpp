#include "pch.h"
#include "AppDllAdaptor.h"
#include <tchar.h>
#include <io.h>


AppDllAdaptor::AppDllAdaptor()
{

}

AppDllAdaptor::~AppDllAdaptor()
{
	if (hd)
		FreeLibrary(hd);
}

bool AppDllAdaptor::InitPkpmAppFuncPtr()
{
	TCHAR dll_name[40];
	auto debugStr = GetPkpmXXXXiniPathName();
	GetPrivateProfileString(_T("CONFIG"), _T("DLL"), _T("pkpmappx.dll"), dll_name, sizeof(dll_name), GetPkpmXXXXiniPathName());
	CString dll_path = GetAppPath() + dll_name;
	hd = LoadLibrary(dll_path);
	if (hd == NULL)
	{
		//LOG_FATAL<<
		return false;
	}
	fuc_InitPkpmApp = (FP_INITPKPMAPP)GetProcAddress(hd, "InitPkpmApp");
	fuc_RunCommand = (FP_RUNCOMMAND)GetProcAddress(hd, "RunCommand");
	fuc_GetMenu = (FP_GETMENUTXT)GetProcAddress(hd, ("GetMenuTxtBuf"));
	fuc_AdjustProOrder = (FP_ADJUSTPROORDER)GetProcAddress(hd, ("AdjustProOrder"));
	if (!fuc_InitPkpmApp || !fuc_RunCommand || !fuc_GetMenu || !fuc_AdjustProOrder)
	{
		//LOG_FATAL<<
		::AfxMessageBox(L"pkpmv51.dll, 动态库错误");
	}
	return true;
}

CString AppDllAdaptor::GetPkpmXXXXiniPathName()
{
	TCHAR path0[MAX_PATH];
	GetModuleFileName(NULL, path0, MAX_PATH);
	CString ini_file = path0;
	ini_file.MakeLower();//不用make了，已经没法更low了
	//我赶时间
	auto index=ini_file.ReverseFind(_T('\\'));
	ini_file = ini_file.Left(index) + _T("\\PKPM2010V51.ini");
	if (_taccess(ini_file, 0) != 0)
	{
		CString fmt;
		fmt = _T("Can not Find \n") + ini_file;
		CString info;
		info.Format(fmt.GetBuffer(), ini_file);
		AfxMessageBox(info, MB_ICONSTOP);
		return _T("");
	}
	//f**k!
	CString iniFile = ini_file;
	return iniFile;
}

CString AppDllAdaptor::GetAppPath()
{
	TCHAR path[256];
	GetModuleFileName(NULL, path, 256);
	PathRemoveFileSpec(path);
	return CString(path) + _T("\\");
}


