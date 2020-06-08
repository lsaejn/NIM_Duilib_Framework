#pragma once
#ifndef _APP_DLL_ADAPTOR_H_
#define  _APP_DLL_ADAPTOR_H_



typedef  const char* (*FP_INITPKPMAPP)();
typedef  void  (*FP_RUNCOMMAND)(CHAR* strCmd); 
typedef  void  (*FP_PKPMCONFIG)(); //��������
typedef  void  (*FP_GETMENUTXT)(CHAR* filebuf,int &nLen);//�õ��˵��ļ� 
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

	//����ӦΪconst char*
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
