// ConfigPwe.cpp : 实现文件
//

#include "stdafx.h"
//#include "pkpm2010_vc6.h"
#include "Resource.h"
#include "ConfigPwe.h"
#include "afxdialogex.h"
#include <winbase.h>
#include "svr.h"
#include <shlwapi.h>

// CConfigPwe 对话框
CString _COLDLG_INI_FILE_NAME = _T("PKPMBentleyCollabModuleConfig.ini");
const CString dlg_SEC_INI_ASSIGNSERVER = _T("ASSIGNSERVER");
const CString dlg_MARKER_INI_SERVER= _T("SERVER") ;
const CString dlg_MARKER_INI_ADDRESS= _T("ADDRESS") ;
IMPLEMENT_DYNAMIC(CConfigPwe, CPropertyPage)

CConfigPwe::CConfigPwe(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CConfigPwe::IDD)
	, m_Server(_T(""))
	, m_IP(_T(""))
{

}

CConfigPwe::~CConfigPwe()
{

}

void CConfigPwe::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_Server);
	DDX_Text(pDX, IDC_EDIT_IP, m_IP);
}


BEGIN_MESSAGE_MAP(CConfigPwe, CPropertyPage)
END_MESSAGE_MAP()


// CConfigPwe 消息处理程序


BOOL CConfigPwe::OnInitDialog()
{
	CDialog::OnInitDialog();


	CString UserCachePath = svr::GetRegCFGPath();
	CString iniFile = UserCachePath + _COLDLG_INI_FILE_NAME;

	if (PathFileExists(iniFile))
	{
		TCHAR szBuf[200];
		GetPrivateProfileString(dlg_SEC_INI_ASSIGNSERVER, dlg_MARKER_INI_SERVER,_T(""),szBuf,sizeof(szBuf),iniFile);m_Server=szBuf;
		GetPrivateProfileString(dlg_SEC_INI_ASSIGNSERVER, dlg_MARKER_INI_ADDRESS,_T(""),szBuf,sizeof(szBuf),iniFile);m_IP=szBuf;

	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CConfigPwe::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData(TRUE);

	CString UserCachePath = svr::GetRegCFGPath();
	CString iniFile = UserCachePath + _COLDLG_INI_FILE_NAME;

	if (FALSE == PathFileExists(iniFile))
	{
		return ;
	}

	WritePrivateProfileString(dlg_SEC_INI_ASSIGNSERVER, dlg_MARKER_INI_SERVER,m_Server,iniFile);
	WritePrivateProfileString(dlg_SEC_INI_ASSIGNSERVER, dlg_MARKER_INI_ADDRESS,m_IP,iniFile);
}
