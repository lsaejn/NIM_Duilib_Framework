#include "StdAfx.h"
#include "AppDllAdaptor.h"
#include <tchar.h>
#include <io.h>



extern FP_INITPKPMAPP fuc_InitPkpmApp ;
extern FP_RUNCOMMAND  fuc_RunCommand;
extern FP_PKPMCONFIG fuc_PkpmConfig;
extern FP_GETMENUTXT fuc_GetMenu;
extern FP_GENCOMPFILE fuc_GenCompFile;
extern FP_ADJUSTPROORDER fuc_AdjustProOrder;
extern FP_ISPROJECTWISEVAILED fuc_IsProjectWiseVailed ;



CString GetAppPath()
{
	TCHAR path[256];
	GetModuleFileName(NULL,path,256);
	PathRemoveFileSpec(path);
	return CString(path)+_T("\\");
}

//知道cache一下吗
CString GetPkpmXXXXiniPathName()
{
	TCHAR path0[MAX_PATH];
	GetModuleFileName(NULL,path0,MAX_PATH);
	CString ini_file=path0;
	ini_file.MakeLower();//不用make了，已经没法更low了
	//我赶时间
#ifdef _DEBUG
	ini_file.Replace(_T("pkpm2010v51.exe"),_T("\\PKPM2010V51.ini"));
#else
	ini_file.Replace(_T("pkpm2010v51.exe"),_T("\\PKPM2010V51.ini"));
#endif // _DEBUG
	if(_taccess(ini_file,0)!=0 )
	{
		CString fmt;
		fmt = _T("Can not Find \n") + ini_file;
		CString info;
		info.Format(fmt,ini_file);
		AfxMessageBox(info,MB_ICONSTOP);
		return _T("");
	}
	//f**k!
	CString iniFile = ini_file;
	return iniFile;
}

/*
是这样的，几乎每一个函数都有问题。

*/

bool InitPkpmAppFuncPtr()
{
	TCHAR dll_name[40];
	DWORD nRead= GetPrivateProfileString(_T("CONFIG"), _T("DLL"),	_T("pkpmappx.dll"), dll_name, sizeof(dll_name), GetPkpmXXXXiniPathName());
	CString dll_path=GetAppPath()+dll_name;
	HMODULE hd=LoadLibrary(dll_path);
	if(hd==NULL)
	{
		//LOG_FATAL<<
		return false;
	}
	fuc_InitPkpmApp=(FP_INITPKPMAPP)GetProcAddress(hd,"InitPkpmApp");
	fuc_RunCommand=(FP_RUNCOMMAND)GetProcAddress(hd,"RunCommand");
	fuc_GetMenu=(FP_GETMENUTXT)GetProcAddress(hd,("GetMenuTxtBuf"));
	fuc_AdjustProOrder=(FP_ADJUSTPROORDER)GetProcAddress(hd,("AdjustProOrder"));
	if(!fuc_InitPkpmApp|| !fuc_RunCommand|| !fuc_GetMenu|| !fuc_AdjustProOrder)
	{
		//LOG_FATAL<<
		return false;
	}
	return true;
}

