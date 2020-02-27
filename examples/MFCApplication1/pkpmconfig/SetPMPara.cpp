// SetPMPara.cpp : ʵ���ļ�
//

#include "../stdafx.h"
#include "pkpmconfig.h"
#include "SetPMPara.h"
#include "afxdialogex.h"


// CSetPMPara �Ի���

IMPLEMENT_DYNAMIC(CSetPMPara, CPropertyPage)

CSetPMPara::CSetPMPara()
	: CPropertyPage(CSetPMPara::IDD)
	, m_earthgroup(0)
	, m_earthintense(0)
	, m_groudlevel(0)
	, m_ifchange02to10(FALSE)
	, m_windpressure(0.5)
	, m_maxcornerdiameter(14)
{

}

CSetPMPara::~CSetPMPara()
{
}

void CSetPMPara::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBOEARTHGROUP, m_earthgroup);
	DDX_CBIndex(pDX, IDC_COMBOEARTHINTENSE, m_earthintense);
	DDX_CBIndex(pDX, IDC_COMBOGROUNDLEVEL, m_groudlevel);
	DDX_Text(pDX, IDC_EDITWINDPRESS, m_windpressure);
	DDV_MinMaxDouble(pDX, m_windpressure, 0, 100000);
	DDX_CBIndex(pDX, IDC_COMBO_DIAMETER, m_maxcornerdiameter);
}


BEGIN_MESSAGE_MAP(CSetPMPara, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBOEARTHGROUP, &CSetPMPara::OnCbnSelchangeComboearthgroup)
	ON_CBN_SELCHANGE(IDC_COMBOEARTHINTENSE, &CSetPMPara::OnCbnSelchangeComboearthintense)
	ON_CBN_SELCHANGE(IDC_COMBOGROUNDLEVEL, &CSetPMPara::OnCbnSelchangeCombogroundlevel)
	ON_EN_CHANGE(IDC_EDITWINDPRESS, &CSetPMPara::OnEnChangeEditwindpress)
	ON_CBN_SELCHANGE(IDC_COMBO_DIAMETER, &CSetPMPara::OnCbnSelchangeComboDiameter)
END_MESSAGE_MAP()


// CSetPMPara ��Ϣ�������

//
//�������=0
//	�����Ҷ�=1
//	���������=1
//	������ѹ=0.10
//	˫ƫѹ�ǽ����ֱ��=50
//;;ת��02�������10�����ݸ�ʽ���� 0(��ת)��1;changePM2load=0
BOOL CSetPMPara::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString pkpminipath=GetPKPMFileName();
	int tmpint;
	tmpint=GetPrivateProfileInt(_T("PM���Ʋ���"),_T("�������"),-1,pkpminipath);
	m_earthgroup=0;
	if (tmpint>=0 && tmpint<=2)
	{
		m_earthgroup=tmpint;
	}
	tmpint=GetPrivateProfileInt(_T("PM���Ʋ���"),_T("�����Ҷ�"),-1,pkpminipath);
	m_earthintense=0;
	if (tmpint>=0 && tmpint<=6)
	{
		m_earthintense=tmpint;
	}
	tmpint=GetPrivateProfileInt(_T("PM���Ʋ���"),_T("���������"),-1,pkpminipath);
	m_groudlevel=0;
	if (tmpint>=0 && tmpint<=5)
	{
		m_groudlevel=tmpint;
	}
	CString tmp;
	GetPrivateProfileString(_T("PM���Ʋ���"),_T("������ѹ"),_T("-1"),tmp.GetBuffer(500),500,pkpminipath);
	m_windpressure=0.5;
	if (tmp!="-1")
	{
		m_windpressure= _tstof(tmp.GetBuffer(500));
	}
	int diameter[15]={6,8,10,12,14,16,18,20,22,25,28,32,36,40,50};
	tmpint=GetPrivateProfileInt(_T("PM���Ʋ���"),_T("˫ƫѹ�ǽ����ֱ��"),-1,pkpminipath);
	for (int i=0;i<15;i++)
	{
		if (diameter[i]==tmpint)
		{
			m_maxcornerdiameter=i;
		}
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CSetPMPara::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	UpdateData(TRUE);
	CString pkpminipath=GetPKPMFileName();
	CString tmp;
	tmp.Format(_T("%d"),m_earthgroup);
	::WritePrivateProfileString(_T("PM���Ʋ���"),_T("�������"),tmp,pkpminipath);
	tmp.Format(_T("%d"),m_earthintense);
	::WritePrivateProfileString(_T("PM���Ʋ���"),_T("�����Ҷ�"),tmp,pkpminipath);
	tmp.Format(_T("%d"),m_groudlevel);
	::WritePrivateProfileString(_T("PM���Ʋ���"),_T("���������"),tmp,pkpminipath);
	tmp.Format(_T("%d"),m_windpressure);
	::WritePrivateProfileString(_T("PM���Ʋ���"),_T("������ѹ"),tmp,pkpminipath);
	int diameter[15]={6,8,10,12,14,16,18,20,22,25,28,32,36,40,50};
	tmp.Format(_T("%d"),diameter[m_maxcornerdiameter]);
	::WritePrivateProfileString(_T("PM���Ʋ���"),_T("˫ƫѹ�ǽ����ֱ��"),tmp,pkpminipath);
	CPropertyPage::OnOK();
}


void CSetPMPara::OnCbnSelchangeComboearthgroup()
{
	SetModified();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CSetPMPara::OnCbnSelchangeComboearthintense()
{
	SetModified();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CSetPMPara::OnCbnSelchangeCombogroundlevel()
{
	SetModified();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CSetPMPara::OnEnChangeEditwindpress()
{
	SetModified();
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CPropertyPage::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CSetPMPara::OnCbnSelchangeComboDiameter()
{
	SetModified();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
