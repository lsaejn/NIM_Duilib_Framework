#pragma once
#ifndef _APP_DLL_ADAPTOR_H_
#define  _APP_DLL_ADAPTOR_H_



typedef  const char* (*FP_INITPKPMAPP)();
typedef  void  (*FP_RUNCOMMAND)(CHAR* strCmd); 
typedef  void  (*FP_PKPMCONFIG)(); //参数设置
typedef  void  (*FP_GETMENUTXT)(CHAR* filebuf,int &nLen);//得到菜单文件 
typedef void (*FP_GENCOMPFILE)();
typedef bool (*FP_ADJUSTPROORDER)(const CHAR* profess,CHAR** names,int nCount);
typedef bool (*FP_ISPROJECTWISEVAILED)(CHAR* pwPath,int& pwPathLen);

class AppDllAdaptor
{
public:
	AppDllAdaptor();
	~AppDllAdaptor();
	bool InitPkpmAppFuncPtr();
	std::wstring GetPkpmXXXXiniPathName();
	std::wstring GetAppPath();

	//返回值应改为引用传回
	const char* Invoke_InitPkpmApp()
	{
		if (fuc_InitPkpmApp)
			return fuc_InitPkpmApp();
		return nullptr;
	}

	//参数应使用const char*
	void Invoke_RunCommand(char* strCmd)
	{
		if (fuc_RunCommand)
			fuc_RunCommand(strCmd);
	}

	//参数应为const char*
	bool Invoke_IsProjectWiseVailed(char* pwPath, int& pwPathLen)
	{
		if(fuc_IsProjectWiseVailed)
			return fuc_IsProjectWiseVailed(pwPath, pwPathLen);
		return false;
	}

private:
	FP_INITPKPMAPP fuc_InitPkpmApp;
	FP_RUNCOMMAND  fuc_RunCommand;
	FP_PKPMCONFIG fuc_PkpmConfig;
	FP_GETMENUTXT fuc_GetMenu;
	FP_GENCOMPFILE fuc_GenCompFile;
	FP_ADJUSTPROORDER fuc_AdjustProOrder;
	FP_ISPROJECTWISEVAILED fuc_IsProjectWiseVailed;
	HMODULE hd;
};

#endif
