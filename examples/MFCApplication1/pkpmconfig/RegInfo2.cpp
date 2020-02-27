//安装程序的注册信息
#include "../stdafx.h"
#include "string.h"
#include "stdlib.h"
//#include <afxwin.h>         // MFC core and standard components
#include "../string_utility.h"

bool Fix_Path_Str2(CString &strPath);
bool Get_AssignRegCFGpath2(CString& strPath,CString &StrReg,const HKEY &hPos);

//得到应用程序的CFG文件夹的路径  该文件夹的路径在注册表中保存
//值 - 在PKPMAPP.dll同名的的XML文件中
extern CString get_cfg_path_reg_key();
extern "C" void __stdcall GET_PKPM_REGISTRY_PATH( char pathname[])
{
	//找出注册表中的对应键值   键由CFG()函数提供
	CString CFGpath;
	CFGpath.Empty();
	//CString StrReg=_T("CFG");
	CString StrReg= get_cfg_path_reg_key();
	//从当前用户注册信息中取
	if (!Get_AssignRegCFGpath2(CFGpath,StrReg,HKEY_CURRENT_USER))
	{
		//从计算机注册信息中取
		if(!Get_AssignRegCFGpath2(CFGpath,StrReg,HKEY_LOCAL_MACHINE))
		{
			//从执行程序目录中取
			HMODULE hModule=NULL;
			char Filename[_MAX_PATH]; 
			DWORD nSize=_MAX_PATH;
			DWORD LPath=GetModuleFileNameA( hModule, Filename, nSize);
			CStringA StrExeFile=Filename;
			int iSize=StrExeFile.ReverseFind('\\');
			StrExeFile=StrExeFile.Right(StrExeFile.GetLength()-(iSize+1));
		}
	}

	Fix_Path_Str2(CFGpath);

	int LenStr=CFGpath.GetLength();
	strcpy(pathname,string_utility::WStringToString(CFGpath.GetBuffer()).c_str());
	pathname[LenStr]=char(0);

}

bool Get_AssignRegCFGpath2(CString& strPath,CString &StrReg,const HKEY &hPos)
{
	CString  CFGpath= _T("");

	char SoftKeyName[256];
	HKEY hkRoot;
	char SoftValue[_MAX_PATH], szValue[128];
	ULONG cb, cs;
	DWORD i;

	// Get the key name from the following string
#ifdef _X64
	if (hPos==HKEY_CURRENT_USER)
		strcpy( SoftKeyName, "SOFTWARE\\PKPM\\MAIN\\PATH" );
	else
		strcpy( SoftKeyName, "SOFTWARE\\Wow6432Node\\PKPM\\MAIN\\PATH" );
#else
	strcpy( SoftKeyName, "SOFTWARE\\PKPM\\MAIN\\PATH" );
#endif

	bool bFind=false;
	if(RegOpenKeyA(hPos, SoftKeyName, &hkRoot)==ERROR_SUCCESS)
	{
		cb = sizeof(SoftValue);
		cs = sizeof(szValue);

		LPBYTE pSoftValue = LPBYTE(SoftValue);
		//先搜索给定注册名称
		for(i=0;RegEnumValueA(hkRoot,i,szValue,&cs,NULL,NULL,pSoftValue,&cb)==ERROR_SUCCESS;++i)
		{
			if( !strcmp(szValue, string_utility::WStringToString(StrReg.GetBuffer()).c_str()))
			{
				CFGpath=SoftValue;
				bFind=true;
				break;
			}
			cb = sizeof(SoftValue);
			cs = sizeof(szValue);
		}

		RegCloseKey(hkRoot);
	}

	strPath = CFGpath;
	return  CFGpath.IsEmpty()?false:true;
}



bool Fix_Path_Str2(CString &strPath)
{
	if (strPath.IsEmpty())
	{
		return false;
	}

	if ((strPath.GetLength() -1) != strPath.ReverseFind(_T('\\')))
	{
		strPath += _T('\\');
	}

	return true;
}
