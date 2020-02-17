#pragma once
#ifndef _APP_DLL_ADAPTOR_H_
#define  _APP_DLL_ADAPTOR_H_



typedef  const char* (*FP_INITPKPMAPP)();
typedef  void  (*FP_RUNCOMMAND)(TCHAR* strCmd); 
typedef  void  (*FP_PKPMCONFIG)(); //��������
typedef  void  (*FP_GETMENUTXT)(TCHAR* filebuf,int &nLen);//�õ��˵��ļ� 
typedef void (*FP_GENCOMPFILE)();
typedef bool (*FP_ADJUSTPROORDER)(const TCHAR* profess,TCHAR** names,int nCount);
typedef bool (*FP_ISPROJECTWISEVAILED)(TCHAR* pwPath,int& pwPathLen);




CString GetAppPath();
CString GetPkpmXXXXiniPathName();
bool InitPkpmAppFuncPtr();


#endif
