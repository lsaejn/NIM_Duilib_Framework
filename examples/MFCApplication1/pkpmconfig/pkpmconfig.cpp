// pkpmconfig.cpp : ���� DLL �ĳ�ʼ�����̡�
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
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CpkpmconfigApp

// BEGIN_MESSAGE_MAP(CpkpmconfigApp, CWinApp)
// END_MESSAGE_MAP()


// CpkpmconfigApp ����

// CpkpmconfigApp::CpkpmconfigApp()
// {
// 	// TODO: �ڴ˴���ӹ�����룬
// 	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
// }


// Ψһ��һ�� CpkpmconfigApp ����

/*CpkpmconfigApp theApp;*/


// CpkpmconfigApp ��ʼ��

// BOOL CpkpmconfigApp::InitInstance()
// {
// 	CWinApp::InitInstance();
// 	AfxEnableControlContainer();
// 	CoInitialize(NULL);
// 
// 	return TRUE;
// }
