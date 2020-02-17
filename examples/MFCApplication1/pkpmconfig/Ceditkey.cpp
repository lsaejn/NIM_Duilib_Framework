// Ceditkey.cpp : 实现文件
//

#include "../stdafx.h"
#include "Ceditkey.h"
#include "afxdialogex.h"


// Ceditkey 对话框

IMPLEMENT_DYNAMIC(Ceditkey, CDialogEx)

Ceditkey::Ceditkey(CWnd* pParent /*=NULL*/)
	: CDialog(Ceditkey::IDD, pParent)
	, m_keyvalue(_T(""))
	, m_comment(_T(""))
{
	m_strcaption="修改";
}

Ceditkey::~Ceditkey()
{
}

void Ceditkey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOCK, m_keyvalue);
	DDX_Control(pDX, IDC_EDIT_LOCK, m_edit);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_comment);
}


BEGIN_MESSAGE_MAP(Ceditkey, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, &Ceditkey::OnBnClickedOk)
END_MESSAGE_MAP()


// Ceditkey 消息处理程序


BOOL Ceditkey::OnInitDialog()
{
	__super::OnInitDialog();
	SetWindowTextA(m_strcaption);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void Ceditkey::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	m_edit.SetSel(0,-1);
	m_edit.SetFocus();
	// TODO: 在此处添加消息处理程序代码
}


void Ceditkey::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if(m_keyvalue.IsEmpty())
	{
		MessageBox(_T("请您输入锁码。"));
		return;
	}
	__super::OnOK();
}
