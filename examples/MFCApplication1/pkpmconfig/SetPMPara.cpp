// SetPMPara.cpp : 实现文件
//

#include "../stdafx.h"
#include "pkpmconfig.h"
#include "SetPMPara.h"
#include "afxdialogex.h"


// CSetPMPara 对话框

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


// CSetPMPara 消息处理程序

//
//地震分组=0
//	地震烈度=1
//	场地土类别=1
//	基本风压=0.10
//	双偏压角筋最大直径=50
//;;转换02版荷载至10版数据格式—— 0(不转)、1;changePM2load=0
BOOL CSetPMPara::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString pkpminipath=GetPKPMFileName();
	int tmpint;
	tmpint=GetPrivateProfileInt("PM控制参数","地震分组",-1,pkpminipath);
	m_earthgroup=0;
	if (tmpint>=0 && tmpint<=2)
	{
		m_earthgroup=tmpint;
	}
	tmpint=GetPrivateProfileInt("PM控制参数","地震烈度",-1,pkpminipath);
	m_earthintense=0;
	if (tmpint>=0 && tmpint<=6)
	{
		m_earthintense=tmpint;
	}
	tmpint=GetPrivateProfileInt("PM控制参数","场地土类别",-1,pkpminipath);
	m_groudlevel=0;
	if (tmpint>=0 && tmpint<=5)
	{
		m_groudlevel=tmpint;
	}
	CString tmp;
	GetPrivateProfileString("PM控制参数","基本风压","-1",tmp.GetBuffer(500),500,pkpminipath);
	m_windpressure=0.5;
	if (tmp!="-1")
	{
		m_windpressure=atof(tmp.GetBuffer(500));
	}
	int diameter[15]={6,8,10,12,14,16,18,20,22,25,28,32,36,40,50};
	tmpint=GetPrivateProfileInt("PM控制参数","双偏压角筋最大直径",-1,pkpminipath);
	for (int i=0;i<15;i++)
	{
		if (diameter[i]==tmpint)
		{
			m_maxcornerdiameter=i;
		}
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CSetPMPara::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData(TRUE);
	CString pkpminipath=GetPKPMFileName();
	CString tmp;
	tmp.Format("%d",m_earthgroup);
	::WritePrivateProfileString("PM控制参数","地震分组",tmp,pkpminipath);
	tmp.Format("%d",m_earthintense);
	::WritePrivateProfileString("PM控制参数","地震烈度",tmp,pkpminipath);
	tmp.Format("%d",m_groudlevel);
	::WritePrivateProfileString("PM控制参数","场地土类别",tmp,pkpminipath);
	tmp.Format("%.2f",m_windpressure);
	::WritePrivateProfileString("PM控制参数","基本风压",tmp,pkpminipath);
	int diameter[15]={6,8,10,12,14,16,18,20,22,25,28,32,36,40,50};
	tmp.Format("%d",diameter[m_maxcornerdiameter]);
	::WritePrivateProfileString("PM控制参数","双偏压角筋最大直径",tmp,pkpminipath);
	CPropertyPage::OnOK();
}


void CSetPMPara::OnCbnSelchangeComboearthgroup()
{
	SetModified();
	// TODO: 在此添加控件通知处理程序代码
}


void CSetPMPara::OnCbnSelchangeComboearthintense()
{
	SetModified();
	// TODO: 在此添加控件通知处理程序代码
}


void CSetPMPara::OnCbnSelchangeCombogroundlevel()
{
	SetModified();
	// TODO: 在此添加控件通知处理程序代码
}


void CSetPMPara::OnEnChangeEditwindpress()
{
	SetModified();
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CSetPMPara::OnCbnSelchangeComboDiameter()
{
	SetModified();
	// TODO: 在此添加控件通知处理程序代码
}
