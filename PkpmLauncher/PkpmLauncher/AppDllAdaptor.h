#pragma once
#ifndef _APP_DLL_ADAPTOR_H_
#define  _APP_DLL_ADAPTOR_H_

/// <summary>
/// ���ڣ�����dllֻ��Ҫ�ṩ�����ӿڼ���, ���ĵ����µľɴ��롣
/// </summary>
typedef  const char* (*FP_INITPKPMAPP)();
typedef  void  (*FP_RUNCOMMAND)(CHAR* strCmd);

class AppDllAdaptor
{
public:
	AppDllAdaptor();
	~AppDllAdaptor();
	bool InitPkpmAppFuncPtr();

	//����ֵӦ��Ϊ���ô���
	const char* Invoke_InitPkpmApp()
	{
		if (fuc_InitPkpmApp)
			return fuc_InitPkpmApp();
		return nullptr;
	}

	//����Ӧʹ��const char*
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
