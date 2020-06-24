#include "pch.h"

#include <tchar.h>
#include <io.h>

#include "AppDllAdaptor.h"
#include "ConfigFileManager.h"


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
	auto dll_path = GetAppPath() + ConfigManager::GetInstance().GetLaunchDllName();
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
		::AfxMessageBox(L"AppDllAdaptor, 无法从动态库导入函数");
	}
	return true;
}

std::wstring AppDllAdaptor::GetPkpmXXXXiniPathName()
{
	TCHAR path0[MAX_PATH];
	GetModuleFileName(NULL, path0, MAX_PATH);
	std::wstring ini_file = path0;
	auto index=ini_file.rfind(_T('\\'));
	ini_file = ini_file.substr(0,index) + _T("\\")+ConfigManager::GetInstance().GetLaunchDllName();
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


