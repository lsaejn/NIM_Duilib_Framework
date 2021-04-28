#pragma once
#ifndef _APP_DLL_ADAPTOR_H_
#define  _APP_DLL_ADAPTOR_H_

/// <summary>
/// 在华为云版程序发行之前，所有的exe都是从pkpmappv5x.dll启动。
/// 这造成了大量的问题。不仅调试过程繁琐，而且dll也没有复用的价值。
/// 随着华为云程序的发布，启动界面对应的xml也进行了大改。pkpmappv5x.dll无法兼容新菜单。
/// 
/// 如果你仍然打算以dll的方式启动，那么只需要提供私有变量对应的两个接口指针即可，
/// 但这个兼容性工作我肯定不会做。
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
