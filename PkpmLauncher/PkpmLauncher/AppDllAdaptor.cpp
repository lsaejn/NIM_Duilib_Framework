#include "pch.h"

#include <tchar.h>
#include <io.h>

#include "AppDllAdaptor.h"
#include "ConfigFileManager.h"
#include "utility.h"


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
	std::wstring dll_path = application_utily::GetExeFolderPath() + ConfigManager::GetInstance().GetLaunchDllName();
	hd = LoadLibrary(dll_path.c_str());
	if (hd == NULL)
	{
		if(!ConfigManager::GetInstance().isStartPkpmmainDirect())
			MsgBox::ShowViaIDWithSpecifiedCtn(dll_path, L"TITLE_DLL_ERROR");
		return false;
	}
	fuc_InitPkpmApp = (FP_INITPKPMAPP)GetProcAddress(hd, "InitPkpmApp");
	fuc_RunCommand = (FP_RUNCOMMAND)GetProcAddress(hd, "RunCommand");
	if (!fuc_InitPkpmApp || !fuc_RunCommand)
	{
		if (!ConfigManager::GetInstance().isStartPkpmmainDirect())
			MsgBox::ShowViaIDWithSpecifiedTitle(L"ERROR_FAIL_TO_DEC_FUNCTION_FROM_DLL", L"AppDllAdaptor.dll");
	}
	return true;
}


