// UpdateCheck.cpp : ʵ���ļ�
//

#include "../stdafx.h"
#include "pkpmconfig.h"
#include "UpdateCheck.h"
#include "afxdialogex.h"


// CUpdateCheck �Ի���

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


// CUpdateCheck ��Ϣ�������


void CUpdateCheck::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	UpdateData(TRUE);
	CString pkpminipath=GetPKPMFileName();
	CString tmp;
	tmp.Format("%d",m_ifupdate?1:0);
	::WritePrivateProfileString("�Զ�����","�Ƿ����Զ�����",tmp,pkpminipath);
	tmp.Format("%d",m_updateinterval);
	::WritePrivateProfileString("�Զ�����","�����µļ��",tmp,pkpminipath);
	CPropertyPage::OnOK();
}

//[�Զ�����]
//�Ƿ����Զ�����=1
//	�����µļ��=15
BOOL CUpdateCheck::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString pkpminipath=GetPKPMFileName();
	int tmpint;
	tmpint=GetPrivateProfileInt("�Զ�����","�Ƿ����Զ�����",-1,pkpminipath);
	m_ifupdate=0;
	if (tmpint>=0)
	{
		m_ifupdate=tmpint;
	}

	tmpint=GetPrivateProfileInt("�Զ�����","�����µļ��",-1,pkpminipath);
	m_updateinterval=0;
	if (tmpint>=0 && tmpint<10000)
	{
		m_updateinterval=tmpint;
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


//void CUpdateCheck::OnBnClickedCheck1()
//{
//	SetModified();
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//}


//void CUpdateCheck::OnEnChangeEdit1()
//{
//	SetModified();
//	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
//	// ���ʹ�֪ͨ��������д CPropertyPage::OnInitDialog()
//	// ���������� CRichEditCtrl().SetEventMask()��
//	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
//
//	// TODO:  �ڴ���ӿؼ�֪ͨ����������
//}


void CUpdateCheck::OnBnClickedCheckupdate()
{
	SetModified();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CUpdateCheck::OnEnChangeEditupdate()
{
	SetModified();
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CPropertyPage::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
