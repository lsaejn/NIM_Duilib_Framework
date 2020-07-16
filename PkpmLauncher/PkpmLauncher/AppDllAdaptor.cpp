#include "pch.h"

#include <tchar.h>
#include <io.h>

#include "AppDllAdaptor.h"
#include "ConfigFileManager.h"
#include "string_util.h"


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
	std::wstring dll_path = application_utily::GetExePath() + ConfigManager::GetInstance().GetLaunchDllName();
	hd = LoadLibrary(dll_path.c_str());
	if (hd == NULL)
	{
		::AfxMessageBox((L"��̬�����ʧ��"+ dll_path).c_str());
		return false;
	}
	fuc_InitPkpmApp = (FP_INITPKPMAPP)GetProcAddress(hd, "InitPkpmApp");
	fuc_RunCommand = (FP_RUNCOMMAND)GetProcAddress(hd, "RunCommand");
	if (!fuc_InitPkpmApp || !fuc_RunCommand)
	{
		::AfxMessageBox(L"AppDllAdaptor, �޷��Ӷ�̬�⵼�뺯��");
	}
	return true;
}


