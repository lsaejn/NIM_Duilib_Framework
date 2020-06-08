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
	GetPrivateProfileString(_T("CONFIG"), _T("DLL"), _T("pkpmappx.dll"), dll_name, sizeof(dll_name), GetPkpmXXXXiniPathName().c_str());
	auto dll_path = GetAppPath() + dll_name;
	hd = LoadLibrary(dll_path.c_str());
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
		::AfxMessageBox(L"pkpmv51.dll, ¶¯Ì¬¿â´íÎó");
	}
	return true;
}

std::wstring AppDllAdaptor::GetPkpmXXXXiniPathName()
{
	TCHAR path0[MAX_PATH];
	GetModuleFileName(NULL, path0, MAX_PATH);
	std::wstring ini_file = path0;
	auto index=ini_file.rfind(_T('\\'));
	ini_file = ini_file.substr(0,index) + _T("\\PKPM2010V51.ini");
	if (_taccess(ini_file.c_str(), 0) != 0)
	{
		auto fmt = _T("Can not Find \n") + ini_file;
		AfxMessageBox(fmt.c_str(), MB_ICONSTOP);
		return _T("");
	}
	return  ini_file;
}

std::wstring AppDllAdaptor::GetAppPath()
{
	TCHAR path[256];
	GetModuleFileName(NULL, path, 256);
	PathRemoveFileSpec(path);
	std::wstring result(path);
	return result + _T("\\");
}


