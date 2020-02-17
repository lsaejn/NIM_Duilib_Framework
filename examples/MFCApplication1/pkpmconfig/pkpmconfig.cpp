// pkpmconfig.cpp : 定义 DLL 的初始化例程。
//

#include "../stdafx.h"
#include "pkpmconfig.h"
#include "GjColorPropertyPage.h"
#include "PkpmInfoSheet.h"
//#include "Ceditkey.h"
#include "string"
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
std::basic_string<TCHAR> s_path = _T("");
extern "C" void __stdcall SET_PKPM_REGISTRY_PATH( TCHAR pathname[])
{
	s_path = pathname;
}
extern "C" void __stdcall GET_PKPM_REGISTRY_PATH( TCHAR pathname[]);
CString GetColorFileName()
{
	TCHAR cfgpath[MAX_PATH];
	CString strPath;

	memset(cfgpath,_T('\0'),sizeof(cfgpath));
	GET_PKPM_REGISTRY_PATH(cfgpath);
	strPath = cfgpath;
	strPath+=_T('\\');
	strPath += _T("GJcolor.ini");

	return strPath;
}
CString GetPKPMFileName()
{
	TCHAR cfgpath[MAX_PATH];
	CString strPath;

	memset(cfgpath,_T('\0'),sizeof(cfgpath));
	GET_PKPM_REGISTRY_PATH(cfgpath);
	strPath = cfgpath;
	strPath+=_T('\\');
	strPath += _T("pkpm.ini");

	return strPath;
}
CString GetClicfg6file()
{
	TCHAR cfgpath[MAX_PATH];
	CString strPath;

	memset(cfgpath,_T('\0'),sizeof(cfgpath));
	GET_PKPM_REGISTRY_PATH(cfgpath);
	strPath = cfgpath;
	strPath += _T("Clicfg6.ini");

	return strPath;
}





//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CpkpmconfigApp

// BEGIN_MESSAGE_MAP(CpkpmconfigApp, CWinApp)
// END_MESSAGE_MAP()


// CpkpmconfigApp 构造

// CpkpmconfigApp::CpkpmconfigApp()
// {
// 	// TODO: 在此处添加构造代码，
// 	// 将所有重要的初始化放置在 InitInstance 中
// }


// 唯一的一个 CpkpmconfigApp 对象

/*CpkpmconfigApp theApp;*/


// CpkpmconfigApp 初始化

// BOOL CpkpmconfigApp::InitInstance()
// {
// 	CWinApp::InitInstance();
// 	AfxEnableControlContainer();
// 	CoInitialize(NULL);
// 
// 	return TRUE;
// }
