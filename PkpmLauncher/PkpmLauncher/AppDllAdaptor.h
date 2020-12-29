#pragma once
#ifndef _APP_DLL_ADAPTOR_H_
#define  _APP_DLL_ADAPTOR_H_

/// <summary>
/// 现在，启动dll只需要提供两个接口即可, 恶心到极致的旧代码。
/// </summary>
typedef  const char* (*FP_INITPKPMAPP)();
typedef  void  (*FP_RUNCOMMAND)(CHAR* strCmd);

class AppDllAdaptor
{
public:
	AppDllAdaptor();
	~AppDllAdaptor();
	bool InitPkpmAppFuncPtr();

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

private:
	FP_INITPKPMAPP fuc_InitPkpmApp=NULL;
	FP_RUNCOMMAND  fuc_RunCommand = NULL;
	HMODULE hd = NULL;
};


#endif
