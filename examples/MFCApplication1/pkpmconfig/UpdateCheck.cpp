// UpdateCheck.cpp : 实现文件
//

#include "../stdafx.h"
#include "pkpmconfig.h"
#include "UpdateCheck.h"
#include "afxdialogex.h"


// CUpdateCheck 对话框

IMPLEMENT_DYNAMIC(CUpdateCheck, CPropertyPage)

CUpdateCheck::CUpdateCheck()
	: CPropertyPage(CUpdateCheck::IDD)
	, m_ifupdate(FALSE)
	, m_updateinterval(3)
{

}

CUpdateCheck::~CUpdateCheck()
{
}

void CUpdateCheck::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECKUPDATE, m_ifupdate);
	DDX_Text(pDX, IDC_EDITUPDATE, m_updateinterval);
	DDV_MinMaxInt(pDX, m_updateinterval, 0, 10000);
}


BEGIN_MESSAGE_MAP(CUpdateCheck, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECKUPDATE, &CUpdateCheck::OnBnClickedCheckupdate)
	ON_EN_CHANGE(IDC_EDITUPDATE, &CUpdateCheck::OnEnChangeEditupdate)
END_MESSAGE_MAP()


// CUpdateCheck 消息处理程序


void CUpdateCheck::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData(TRUE);
	CString pkpminipath=GetPKPMFileName();
	CString tmp;
	tmp.Format("%d",m_ifupdate?1:0);
	::WritePrivateProfileString("自动更新","是否检测自动更新",tmp,pkpminipath);
	tmp.Format("%d",m_updateinterval);
	::WritePrivateProfileString("自动更新","检测更新的间隔",tmp,pkpminipath);
	CPropertyPage::OnOK();
}

//[自动更新]
//是否检测自动更新=1
//	检测更新的间隔=15
BOOL CUpdateCheck::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString pkpminipath=GetPKPMFileName();
	int tmpint;
	tmpint=GetPrivateProfileInt("自动更新","是否检测自动更新",-1,pkpminipath);
	m_ifupdate=0;
	if (tmpint>=0)
	{
		m_ifupdate=tmpint;
	}

	tmpint=GetPrivateProfileInt("自动更新","检测更新的间隔",-1,pkpminipath);
	m_updateinterval=0;
	if (tmpint>=0 && tmpint<10000)
	{
		m_updateinterval=tmpint;
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


//void CUpdateCheck::OnBnClickedCheck1()
//{
//	SetModified();
//	// TODO: 在此添加控件通知处理程序代码
//}


//void CUpdateCheck::OnEnChangeEdit1()
//{
//	SetModified();
//	// TODO:  如果该控件是 RICHEDIT 控件，它将不
//	// 发送此通知，除非重写 CPropertyPage::OnInitDialog()
//	// 函数并调用 CRichEditCtrl().SetEventMask()，
//	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
//
//	// TODO:  在此添加控件通知处理程序代码
//}


void CUpdateCheck::OnBnClickedCheckupdate()
{
	SetModified();
	// TODO: 在此添加控件通知处理程序代码
}


void CUpdateCheck::OnEnChangeEditupdate()
{
	SetModified();
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
