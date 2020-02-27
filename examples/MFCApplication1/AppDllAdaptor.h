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




CString GetAppPath();
CString GetPkpmXXXXiniPathName();
bool InitPkpmAppFuncPtr();


#endif
