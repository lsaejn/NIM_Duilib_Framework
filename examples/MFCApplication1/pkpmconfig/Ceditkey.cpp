// Ceditkey.cpp : ʵ���ļ�
//

#include "../stdafx.h"
#include "Ceditkey.h"
#include "afxdialogex.h"


// Ceditkey �Ի���

IMPLEMENT_DYNAMIC(Ceditkey, CDialogEx)

Ceditkey::Ceditkey(CWnd* pParent /*=NULL*/)
	: CDialog(Ceditkey::IDD, pParent)
	, m_keyvalue(_T(""))
	, m_comment(_T(""))
{
	m_strcaption="�޸�";
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


// Ceditkey ��Ϣ�������


BOOL Ceditkey::OnInitDialog()
{
	__super::OnInitDialog();
	SetWindowTextA(m_strcaption);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void Ceditkey::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	m_edit.SetSel(0,-1);
	m_edit.SetFocus();
	// TODO: �ڴ˴������Ϣ����������
}


void Ceditkey::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if(m_keyvalue.IsEmpty())
	{
		MessageBox(_T("�����������롣"));
		return;
	}
	__super::OnOK();
}
