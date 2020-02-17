// SetOthers.cpp : 实现文件
//

#include "../stdafx.h"
#include "pkpmconfig.h"
#include "SetOthers.h"
#include "afxdialogex.h"
//#include "PkpmAppXml.h"


// CSetOthers 对话框

IMPLEMENT_DYNAMIC(CSetOthers, CPropertyPage)

CSetOthers::CSetOthers()
	: CPropertyPage(CSetOthers::IDD)
	, m_ifchange02to10(FALSE)
	, m_ifPMCADopenSTS(FALSE)
	, m_ifShowFullPath(FALSE)
	, m_nSelF1(0)
{

}

CSetOthers::~CSetOthers()
{
}

void CSetOthers::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK02TO10, m_ifchange02to10);
	DDX_Check(pDX, ID_CHECK_PMCADOPENSTS, m_ifPMCADopenSTS);
	DDX_Check(pDX, ID_CHECK_BTN_SHOWFULLPATH, m_ifShowFullPath);
	DDX_CBIndex(pDX, IDC_CB_F1, m_nSelF1);
}


BEGIN_MESSAGE_MAP(CSetOthers, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK02TO10, &CSetOthers::OnBnClickedCheck02to10)
	ON_BN_CLICKED(ID_CHECK_PMCADOPENSTS, &CSetOthers::OnBnClickedCheck1)
	ON_BN_CLICKED(ID_CHECK_BTN_SHOWFULLPATH, &CSetOthers::OnBnClickedCheckBtnShowfullpath)
END_MESSAGE_MAP()


// CSetOthers 消息处理程序


void CSetOthers::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData(TRUE);
	CString pkpminipath=GetPKPMFileName();
	CString tmp;
	tmp.Format("%d",m_ifchange02to10?1:0);
	::WritePrivateProfileString("PM控制参数","changePM2load",tmp,pkpminipath);
	tmp.Format("%d",m_ifPMCADopenSTS?1:0);
	::WritePrivateProfileString("PM控制参数","readalljws",tmp,pkpminipath);
	tmp.Format("%d",m_ifShowFullPath?1:0);
	::WritePrivateProfileString("PM控制参数","btnShowFullPath",tmp,pkpminipath);

	const CString MAKER_HELP_TYPE = "HELP_TYPE";
	const CString MARKER_F1 = "MARKER_F1";
	CString strTmp;
	if (0==m_nSelF1)
	{
		strTmp = "UNKNOWN";	 
	}
	else if (1==m_nSelF1)
	{
		strTmp = "ONLINE"; 
	}
	else if(2==m_nSelF1)
	{
		strTmp = "LOCAL";
	}
	::WritePrivateProfileString(MAKER_HELP_TYPE,MARKER_F1,strTmp,pkpminipath);
	CPropertyPage::OnOK();
}


BOOL CSetOthers::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CString pkpminipath=GetPKPMFileName();
	int tmpint=GetPrivateProfileInt("PM控制参数","changePM2load",-1,pkpminipath);
	m_ifchange02to10=0;
	if (tmpint>=0)
	{
		m_ifchange02to10=tmpint;
	}

	tmpint=GetPrivateProfileInt("PM控制参数","readalljws",-1,pkpminipath);
	if (tmpint>=0)
	{
		m_ifPMCADopenSTS=tmpint;
	}


	tmpint=GetPrivateProfileInt("PM控制参数","btnShowFullPath",-1,pkpminipath);
	if (tmpint>=0)
	{
		m_ifShowFullPath=tmpint;
	}

	//chenjian 10/28
	//不要暴露 PkpmAppXml::CELL.
	/*
	if (m_PageType == PkpmAppXml::CELL )
	{
		GetDlgItem(ID_CHECK_BTN_SHOWFULLPATH)->EnableWindow(TRUE);
	}
	else
	{
		CString strText;
		GetDlgItem(ID_CHECK_BTN_SHOWFULLPATH)->GetWindowText(strText);
		strText+="\n[仅当\'最近打开工程(十二宫格)\'页面出现时才可设置]";
		GetDlgItem(ID_CHECK_BTN_SHOWFULLPATH)->SetWindowText(strText);
		GetDlgItem(ID_CHECK_BTN_SHOWFULLPATH)->EnableWindow(FALSE);
	}
	*/
	{
		CString strText;
		GetDlgItem(ID_CHECK_BTN_SHOWFULLPATH)->GetWindowText(strText);
		strText+="\n[仅当\'最近打开工程(十二宫格)\'页面出现时才可设置]";
		GetDlgItem(ID_CHECK_BTN_SHOWFULLPATH)->SetWindowText(strText);
		GetDlgItem(ID_CHECK_BTN_SHOWFULLPATH)->EnableWindow(FALSE);
	}
	//chenjian end
	const CString MAKER_HELP_TYPE = "HELP_TYPE";
	const CString MAKER_F1 = "MAKER_F1";

	CString strTmp;
	::GetPrivateProfileString(MAKER_HELP_TYPE,MAKER_F1,"UNKNOWN",strTmp.GetBuffer(20),20,pkpminipath);
	strTmp.ReleaseBuffer();
	strTmp.Trim();
	if (0==strTmp.CompareNoCase("UNKNOWN"))
	{
		m_nSelF1 = 0;
	}
	else if (0==strTmp.CompareNoCase("ONLINE"))
	{
		m_nSelF1 = 1;
	}
	else if(0==strTmp.CompareNoCase("LOCAL"))
	{
		m_nSelF1 = 2;
	}
	else
	{
		m_nSelF1 = 0;
	}


	// TODO:  在此添加额外的初始化
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CSetOthers::OnBnClickedCheck02to10()
{
	SetModified();
	// TODO: 在此添加控件通知处理程序代码
}




void CSetOthers::OnBnClickedCheck1()
{
	SetModified();
	// TODO: 在此添加控件通知处理程序代码
}


void CSetOthers::OnBnClickedCheckBtnShowfullpath()
{
	SetModified();
	 
}
