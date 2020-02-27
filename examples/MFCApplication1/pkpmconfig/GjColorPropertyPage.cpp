// GjColorPropertyPage.cpp : ʵ���ļ�
//
#include "../stdafx.h"
#include "GjColorPropertyPage.h"
#include "afxdialogex.h"
#include "pkpmconfig.h"
// CGjColorPropertyPage �Ի���

IMPLEMENT_DYNAMIC(CGjColorPropertyPage, CPropertyPage)

CGjColorPropertyPage::CGjColorPropertyPage()
	: CPropertyPage(CGjColorPropertyPage::IDD)
	, m_Solution(0)
	, m_ToolbarColorSchem(0)
{
	m_Solution=0;
	ZeroMemory(m_color,sizeof(m_color));
	ZeroMemory(m_KPASTE,sizeof(m_KPASTE));
}

CGjColorPropertyPage::~CGjColorPropertyPage()
{
}

void CGjColorPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PGCELLCOLORCTRL, m_Grid);
	DDX_CBIndex(pDX, IDC_COMBOCOLOR, m_Solution);
	DDX_CBIndex(pDX, ID_COMBO_COLOR_SCHEM, m_ToolbarColorSchem);
}


BEGIN_MESSAGE_MAP(CGjColorPropertyPage, CPropertyPage)
	ON_BN_CLICKED(IDDEFAULT, &CGjColorPropertyPage::OnBnClickedDefault)
	ON_CBN_SELCHANGE(IDC_COMBOCOLOR, &CGjColorPropertyPage::OnCbnSelchangeCombocolor)
	ON_CBN_SELCHANGE(ID_COMBO_COLOR_SCHEM, &CGjColorPropertyPage::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CGjColorPropertyPage ��Ϣ�������

BEGIN_EVENTSINK_MAP(CGjColorPropertyPage, CPropertyPage)
	ON_EVENT(CGjColorPropertyPage, IDC_PGCELLCOLORCTRL, 5, CGjColorPropertyPage::OnEditCharPgcellcolorctrl, VTS_I2 VTS_I2 VTS_I4 VTS_I2)
	ON_EVENT(CGjColorPropertyPage, IDC_PGCELLCOLORCTRL, 28, CGjColorPropertyPage::OnBeginEditCellPgcellcolorctrl, VTS_I4 VTS_I4 VTS_BSTR VTS_I2)
	ON_EVENT(CGjColorPropertyPage, IDC_PGCELLCOLORCTRL, 13, CGjColorPropertyPage::OnColorBtnEndSelectPgcellcolorctrl, VTS_I2 VTS_I2 VTS_I4)
END_EVENTSINK_MAP()


void CGjColorPropertyPage::OnEditCharPgcellcolorctrl(short nCurRow, short nCurCOl, long nChar, short nPage)
{
	SetModified();
	// TODO: �ڴ˴������Ϣ����������
}


void CGjColorPropertyPage::OnBeginEditCellPgcellcolorctrl(long lCurRow, long lCurCol, LPCTSTR strCellString, short nPage)
{
	SetModified();
	// TODO: �ڴ˴������Ϣ����������
}


void CGjColorPropertyPage::OnColorBtnEndSelectPgcellcolorctrl(short nRow, short nCol, long ulBkClr)
{
	SetModified();
	// TODO: �ڴ˴������Ϣ����������
}

/*********************************************************************
��ɫ����
	1-�ս�שǽ		2-������ǽ		3-��ѹשǽ      4-��������ǽ	5-�����ǽ
	6-������		7-����			8-��������		9-���Ը���		10-�������       
	11-������		12-����������	13-���Ը�����	14-���������	15-�Ŵ�����
	16-¥��			17-¥�嶴��		18-��б��		19-������б��	20-���Ը�б��     
	21-�����б��	22-�ִ���		23-����������	24-���Ը˴���	25-����ϴ���
	26-¥��ʾ��		27-������Ȧ��	28-��Ȧ��       29-����         30-������
*********************************************************************/


void CGjColorPropertyPage::InitIDName()
{
	m_gjname.RemoveAll();
	m_gjname.Add(L"�ս�שǽ");      //1
	m_gjname.Add(L"������ǽ");
	m_gjname.Add(L"��ѹשǽ");
	m_gjname.Add(L"��������ǽ");
	m_gjname.Add(L"�����ǽ");
	m_gjname.Add(L"������");
	m_gjname.Add(L"����");
	m_gjname.Add(L"��������");
	m_gjname.Add(L"���Ը���");
	m_gjname.Add(L"�������");       //10
	m_gjname.Add(L"������");
	m_gjname.Add(L"����������");
	m_gjname.Add(L"���Ը�����");
	m_gjname.Add(L"���������");
	m_gjname.Add(L"�Ŵ�����");
	m_gjname.Add(L"¥��");
	m_gjname.Add(L"¥�嶴��");
	m_gjname.Add(L"��б��");
	m_gjname.Add(L"������б��");
	m_gjname.Add(L"���Ը�б��");     //20
	m_gjname.Add(L"�����б��");
	m_gjname.Add(L"�ִ���");
	m_gjname.Add(L"����������");
	m_gjname.Add(L"���Ը˴���");
	m_gjname.Add(L"����ϴ���");
	m_gjname.Add(L"¥��ʾ��");
	m_gjname.Add(L"������Ȧ��");
	m_gjname.Add(L"��Ȧ��");
	m_gjname.Add(L"����");
	m_gjname.Add(L"������");
	m_gjname.Add(L"����");
}
void CGjColorPropertyPage::GetGjColor()
{
	CString strColorfilename;
	CString strName;
	strColorfilename = GetColorFileName();
	CFileStatus filestat;
	int exitstfile=CFile::GetStatus(strColorfilename,filestat);
	if (!exitstfile)
	{
		GetDefautColor();
		SetGjColor();
	}
	m_ncolor=GetPrivateProfileInt(_T("color"),_T("ncnt"),0,strColorfilename);
	m_Solution=GetPrivateProfileInt(_T("solution"),_T("solution"),0,strColorfilename);
	if (m_Solution==0)
	{
		for(int i=0;i<m_ncolor&&i<m_gjname.GetSize();i++){
			strName = m_gjname.ElementAt(i);
			m_color[i] = GetPrivateProfileInt(_T("color0"),strName,0,strColorfilename);
			m_KPASTE[i] = GetPrivateProfileInt(_T("transparence0"),strName,0,strColorfilename);
		}
	}
	else if(m_Solution==1)
	{
		for(int i=0;i<m_ncolor&&i<m_gjname.GetSize();i++){
			strName = m_gjname.ElementAt(i);
			m_color[i] = GetPrivateProfileInt(_T("color1"),strName,0,strColorfilename);
			m_KPASTE[i] = GetPrivateProfileInt(_T("transparence1"),strName,0,strColorfilename);
		}
	}
	else if(m_Solution==2)
	{
		for(int i=0;i<m_ncolor&&i<m_gjname.GetSize();i++){
			strName = m_gjname.ElementAt(i);
			m_color[i] = GetPrivateProfileInt(_T("colorUser1"),strName,0,strColorfilename);
			m_KPASTE[i] = GetPrivateProfileInt(_T("transparenceUser1"),strName,0,strColorfilename);
		}
	}
	else if(m_Solution==3)
	{
		for(int i=0;i<m_ncolor&&i<m_gjname.GetSize();i++){
			strName = m_gjname.ElementAt(i);
			m_color[i] = GetPrivateProfileInt(_T("colorUser2"),strName,0,strColorfilename);
			m_KPASTE[i] = GetPrivateProfileInt(_T("transparenceUser2"),strName,0,strColorfilename);
		}
	}



	if(m_color[0]==0) GetDefautColor();
}
void CGjColorPropertyPage::GetDefautColor(/*int* m_ncolor,int color[],int KPASTE[],int &m_Solution*/)	
{
	if(m_ncolor==0){
		m_ncolor = 31;
		for(int i=0;i<m_ncolor;i++){
			m_color[i] = 0;
			m_KPASTE[i] = 0;
		}
	}
	switch(m_Solution) 
	{
		case 0:
		if(m_color[0]>=0){					//�ս�שǽ
			m_color[0] = 8671917;
			//CFGRGB(cfgcolor,m_color[0]);
			m_KPASTE[0] = 30;
		}
		if(m_color[1]>=0){					//������ǽ
			m_color[1] = 6734218;
			//CFGRGB(cfgcolor,m_color[1]);
			m_KPASTE[1] = 30;
		}
		if(m_color[2]>=0){					//��ѹשǽ
			m_color[2] = 3368584;
			//CFGRGB(cfgcolor,m_color[2]);
			m_KPASTE[2] = 30;
		}
		if(m_color[3]>=0){					//��������ǽ
			m_color[3] = 749522;
			//CFGRGB(cfgcolor,m_color[3]);
			m_KPASTE[3] = 30;
		}
		if(m_color[4]>=0){					//�����ǽ
			m_color[4] = 13468552;
			//CFGRGB(cfgcolor,m_color[4]);
			m_KPASTE[4] = 30;
		}
		if(m_color[5]>=0){					//������
			m_color[5] = 6710886;
			//CFGRGB(cfgcolor,m_color[5]);
		}
		if(m_color[6]>=0){					//����
			m_color[6] = 8505786;
			//CFGRGB(cfgcolor,m_color[6]);
		}
		if(m_color[7]>=0){					//��������
			m_color[7] = 13684834;
			//CFGRGB(cfgcolor,m_color[7]);
		}
		if(m_color[8]>=0){					//���Ը���
			m_color[8] = 10374774;
			//CFGRGB(cfgcolor,m_color[8]);
		}
		if(m_color[9]>=0){					//�������
			m_color[9] = 16379566;
			//CFGRGB(cfgcolor,m_color[9]);
		}
		if(m_color[10]>=0){					//������
			m_color[10] = 10518589;
			//CFGRGB(cfgcolor,m_color[10]);
		}
		if(m_color[11]>=0){					//����������
			m_color[11] = 13988921;
			//CFGRGB(cfgcolor,m_color[11]);
		}
		if(m_color[12]>=0){					//���Ը�����
			m_color[12] =5532963;
			//CFGRGB(cfgcolor,m_color[12]);
		}
		if(m_color[13]>=0){					//���������
			m_color[13] = 5159080;
			//CFGRGB(cfgcolor,m_color[13]);
		}
		if(m_color[14]>=0){					//�Ŵ�����
			m_color[14] = 13688896;
			//CFGRGB(cfgcolor,m_color[14]);
			m_KPASTE[14]=30;
		}
		if(m_color[15]>=0){					//¥��
			m_color[15] = 12566463;
			//CFGRGB(cfgcolor,m_color[15]);
			m_KPASTE[15]=50;
		}
		if(m_color[16]>=0){					//¥�嶴��
			m_color[16] = 12118515;
			//CFGRGB(cfgcolor,m_color[16]);
			m_KPASTE[16]=30;
		}
		if(m_color[17]>=0){					//��б��
			m_color[17] = 14967157;
			//CFGRGB(cfgcolor,m_color[17]);
		}
		if(m_color[18]>=0){					//������б��
			m_color[18] = 3295670;
			//CFGRGB(cfgcolor,m_color[18]);
		}
		if(m_color[19]>=0){					//���Ը�б��
			m_color[19] = 13453244;
			//CFGRGB(cfgcolor,m_color[19]);
		}
		if(m_color[20]>=0){					//�����б��
			m_color[20] = 8421631;
			//CFGRGB(cfgcolor,m_color[20]);
		}
		if(m_color[21]>=0){					//�ִ���
			m_color[21] = 15188662;
			//CFGRGB(cfgcolor,m_color[21]);
		}
		if(m_color[22]>=0){					//����������
			m_color[22] = 12623485;
			//CFGRGB(cfgcolor,m_color[22]);
		}
		if(m_color[23]>=0){					//���Ը˴���
			m_color[23] = 12614523;
			//CFGRGB(cfgcolor,m_color[23]);
		}
		if(m_color[24]>=0){					//����ϴ���
			m_color[24] = 12382580;
			//CFGRGB(cfgcolor,m_color[24]);
		}
		if(m_color[25]>=0){					//¥��ʾ��
			m_color[25] = 8895429;
			//CFGRGB(cfgcolor,m_color[25]);
			m_KPASTE[25] = 20;
		}
		if(m_color[26]>=0){					//������Ȧ��
			m_color[26] = 14369894;
			//CFGRGB(cfgcolor,m_color[26]);
		}
		if(m_color[27]>=0){					//��Ȧ��
			m_color[27] = 12614523;
			//CFGRGB(cfgcolor,m_color[27]);
		}
		if(m_color[28]>=0){					//����
			m_color[28] = 3156001;
			//CFGRGB(cfgcolor,m_color[28]);
			m_KPASTE[28]=60;
		}
		if(m_color[29]>=0){					//������
			m_color[29] = 14054563;
			//CFGRGB(cfgcolor,m_color[29]);
		}
		if(m_color[29]>=0){					//������
			m_color[29] = 14054563;
			//CFGRGB(cfgcolor,m_color[29]);
		}
		if(m_color[30]>=0){					//����
			m_color[30] = 14054563;
			//CFGRGB(cfgcolor,m_color[29]);
		}
		
		break;
	case 1:
		if(m_color[0]>=0){					//�ս�שǽ
			m_color[0] = 12550143;
			m_KPASTE[0] = 10;
		}
		if(m_color[1]>=0){					//������ǽ
			m_color[1] = 5635925;
			m_KPASTE[1] = 10;
		}
		if(m_color[2]>=0){					//��ѹשǽ
			m_color[2] = 5603327;
			m_KPASTE[2] = 10;
		}
		if(m_color[3]>=0){					//��������ǽ
			m_color[3] = 5603327;
			m_KPASTE[3] = 10;
		}
		if(m_color[4]>=0){					//�����ǽ
			m_color[4] = 13238217;
			m_KPASTE[4] = 10;
		}
		if(m_color[5]>=0){					//������
			m_color[5] = 65535;
		}
		if(m_color[6]>=0){					//����
			m_color[6] = 9764863;
		}
		if(m_color[7]>=0){					//��������
			m_color[7] = 65535;
		}
		if(m_color[8]>=0){					//���Ը���
			m_color[8] = 65535;
		}
		if(m_color[9]>=0){					//�������
			m_color[9] = 16379566;
		}
		if(m_color[10]>=0){					//������
			m_color[10] = 16379566;
		}
		if(m_color[11]>=0){					//����������
			m_color[11] = 16776960;
		}
		if(m_color[12]>=0){					//���Ը�����
			m_color[12] =16776960;
		}
		if(m_color[13]>=0){					//���������
			m_color[13] = 9764863;
		}
		if(m_color[14]>=0){					//�Ŵ�����
			m_color[14] = 16777215;
			m_KPASTE[14]=50;
		}
		if(m_color[15]>=0){					//¥��
			m_color[15] = 12566463;
			m_KPASTE[15]=50;
		}
		if(m_color[16]>=0){					//¥�嶴��
			m_color[16] = 16777215;
			m_KPASTE[16]=50;
		}
		if(m_color[17]>=0){					//��б��
			m_color[17] = 16711935;
		}
		if(m_color[18]>=0){					//������б��
			m_color[18] = 16711935;
		}
		if(m_color[19]>=0){					//���Ը�б��
			m_color[19] = 16711935;
		}
		if(m_color[20]>=0){					//�����б��
			m_color[20] = 13224447;
		}
		if(m_color[21]>=0){					//�ִ���
			m_color[21] = 15315828;
		}
		if(m_color[22]>=0){					//����������
			m_color[22] = 11184810;
		}
		if(m_color[23]>=0){					//���Ը˴���
			m_color[23] = 11184810;
		}
		if(m_color[24]>=0){					//����ϴ���
			m_color[24] = 11184810;
		}
		if(m_color[25]>=0){					//¥��ʾ��
			m_color[25] = 16777215;
			m_KPASTE[25] = 20;
		}
		if(m_color[26]>=0){					//������Ȧ��
			m_color[26] = 5635925;
		}
		if(m_color[27]>=0){					//��Ȧ��
			m_color[27] = 5635925;
		}
		if(m_color[28]>=0){					//����
			m_color[28] = 9737354;
			m_KPASTE[28]=60;
		}
		if(m_color[29]>=0){					//������
			m_color[29] = 16777215;
		}
		if(m_color[30]>=0){					//����
			m_color[30] = 16777215;
			//CFGRGB(cfgcolor,m_color[29]);
		}
		break;
	}
	
	if(m_color[0]==0){					//�ս�שǽ
		m_color[0] = 8671917;
		//CFGRGB(cfgcolor,m_color[0]);
		m_KPASTE[0] = 30;
	}
	if(m_color[1]==0){					//������ǽ
		m_color[1] = 6734218;
		//CFGRGB(cfgcolor,m_color[1]);
		m_KPASTE[1] = 30;
	}
	if(m_color[2]==0){					//��ѹשǽ
		m_color[2] = 3368584;
		//CFGRGB(cfgcolor,m_color[2]);
		m_KPASTE[2] = 30;
	}
	if(m_color[3]==0){					//��������ǽ
		m_color[3] = 749522;
		//CFGRGB(cfgcolor,m_color[3]);
		m_KPASTE[3] = 30;
	}
	if(m_color[4]==0){					//�����ǽ
		m_color[4] = 13468552;
		//CFGRGB(cfgcolor,m_color[4]);
		m_KPASTE[4] = 30;
	}
	if(m_color[5]==0){					//������
		m_color[5] = 6710886;
		//CFGRGB(cfgcolor,m_color[5]);
	}
	if(m_color[6]==0){					//����
		m_color[6] = 8280859;
		//CFGRGB(cfgcolor,m_color[6]);
	}
	if(m_color[7]==0){					//��������
		m_color[7] = 13988921;
		//CFGRGB(cfgcolor,m_color[7]);
	}
	if(m_color[8]==0){					//���Ը���
		m_color[8] = 10374774;
		//CFGRGB(cfgcolor,m_color[8]);
	}
	if(m_color[9]==0){					//�������
		m_color[9] = 16379566;
		//CFGRGB(cfgcolor,m_color[9]);
	}
	if(m_color[10]==0){					//������
		m_color[10] = 12556625;
		//CFGRGB(cfgcolor,m_color[10]);
	}
	if(m_color[11]==0){					//����������
		m_color[11] = 11972869;
		//CFGRGB(cfgcolor,m_color[11]);
	}
	if(m_color[12]==0){					//���Ը�����
		m_color[12] =9090060;
		//CFGRGB(cfgcolor,m_color[12]);
	}
	if(m_color[13]==0){					//���������
		m_color[13] = 2465435;
		//CFGRGB(cfgcolor,m_color[13]);
	}
	if(m_color[14]==0){					//�Ŵ�����
		m_color[14] = 13688896;
		//CFGRGB(cfgcolor,m_color[14]);
		m_KPASTE[14]=30;
	}
	if(m_color[15]==0){					//¥��
		m_color[15] = 12566463;
		//CFGRGB(cfgcolor,m_color[15]);
		m_KPASTE[15]=50;
	}
	if(m_color[16]==0){					//¥�嶴��
		m_color[16] = 12118515;
		//CFGRGB(cfgcolor,m_color[16]);
		m_KPASTE[16]=30;
	}
	if(m_color[17]==0){					//��б��
		m_color[17] = 10312658;
		//CFGRGB(cfgcolor,m_color[17]);
	}
	if(m_color[18]==0){					//������б��
		m_color[18] = 13391727;
		//CFGRGB(cfgcolor,m_color[18]);
	}
	if(m_color[19]==0){					//���Ը�б��
		m_color[19] = 13453244;
		//CFGRGB(cfgcolor,m_color[19]);
	}
	if(m_color[20]==0){					//�����б��
		m_color[20] = 13224447;
		//CFGRGB(cfgcolor,m_color[20]);
	}
	if(m_color[21]==0){					//�ִ���
		m_color[21] = 12357157;
		//CFGRGB(cfgcolor,m_color[21]);
	}
	if(m_color[22]==0){					//����������
		m_color[22] = 12623485;
		//CFGRGB(cfgcolor,m_color[22]);
	}
	if(m_color[23]==0){					//���Ը˴���
		m_color[23] = 12614523;
		//CFGRGB(cfgcolor,m_color[23]);
	}
	if(m_color[24]==0){					//����ϴ���
		m_color[24] = 9143144;
		//CFGRGB(cfgcolor,m_color[24]);
	}
	if(m_color[25]==0){					//¥��ʾ��
		m_color[25] = 8895429;
		//CFGRGB(cfgcolor,m_color[25]);
		m_KPASTE[25] = 20;
	}
	if(m_color[26]==0){					//������Ȧ��
		m_color[26] = 14369894;
		//CFGRGB(cfgcolor,m_color[26]);
	}
	if(m_color[27]==0){					//��Ȧ��
		m_color[27] = 12614523;
		//CFGRGB(cfgcolor,m_color[27]);
	}
	if(m_color[28]==0){					//����
		m_color[28] = 3156001;
		//CFGRGB(cfgcolor,m_color[28]);
		m_KPASTE[28]=60;
	}
	if(m_color[29]==0){					//������
		m_color[29] = 14054563;
		//CFGRGB(cfgcolor,color[29]);
	}
	if(m_color[30]>=0){					//����
		m_color[30] = 14054563;
		//CFGRGB(cfgcolor,m_color[29]);
	}
// 	if(m_ncolor<m_gjname.GetSize())
// 		m_ncolor = m_gjname.GetSize();
}

void CGjColorPropertyPage::SetGjColor()
{
	CString strColorfilename;
	CString strName,strTmp;

	strColorfilename = GetColorFileName();

	strTmp.Format(L"%d",m_ncolor);
	::WritePrivateProfileString(L"color",L"ncnt",strTmp,strColorfilename);
	for(int i=0;i<m_ncolor;i++){
		strName = m_gjname.ElementAt(i);
		strTmp.Format(L"%d",m_color[i]);
		::WritePrivateProfileString(L"color",strName,strTmp,strColorfilename);
		strTmp.Format(L"%d",m_KPASTE[i]);
		::WritePrivateProfileString(L"transparence",strName,strTmp,strColorfilename);
	}
	strTmp.Format(L"%d",m_Solution);
	::WritePrivateProfileString(L"solution",L"solution",strTmp,strColorfilename);
	if (m_Solution==0)
	{
		for(int i=0;i<m_ncolor;i++){
			strName = m_gjname.ElementAt(i);
			strTmp.Format(L"%d",m_color[i]);
			::WritePrivateProfileString(L"color0",strName,strTmp,strColorfilename);
			strTmp.Format(L"%d",m_KPASTE[i]);
			::WritePrivateProfileString(L"transparence0",strName,strTmp,strColorfilename);
		}
	}
	else if(m_Solution==1)
	{
		for(int i=0;i<m_ncolor;i++){
			strName = m_gjname.ElementAt(i);
			strTmp.Format(L"%d",m_color[i]);
			::WritePrivateProfileString(L"color1",strName,strTmp,strColorfilename);
			strTmp.Format(L"%d",m_KPASTE[i]);
			::WritePrivateProfileString(L"transparence1",strName,strTmp,strColorfilename);
		}
	}
	else if(m_Solution==2)
	{
		for(int i=0;i<m_ncolor;i++){
			strName = m_gjname.ElementAt(i);
			strTmp.Format(L"%d",m_color[i]);
			::WritePrivateProfileString(L"colorUser1",strName,strTmp,strColorfilename);
			strTmp.Format(L"%d",m_KPASTE[i]);
			::WritePrivateProfileString(L"transparenceUser1",strName,strTmp,strColorfilename);
		}
	}
	else if(m_Solution==3)
	{
		for(int i=0;i<m_ncolor;i++){
			strName = m_gjname.ElementAt(i);
			strTmp.Format(L"%d",m_color[i]);
			::WritePrivateProfileString(L"colorUser2",strName,strTmp,strColorfilename);
			strTmp.Format(L"%d",m_KPASTE[i]);
			::WritePrivateProfileString(L"transparenceUser2",strName,strTmp,strColorfilename);
		}
	}
}



void CGjColorPropertyPage::OnBnClickedDefault()
{
	UpdateData(TRUE);
	if (m_Solution!=0 && m_Solution!=1)
	{
		return;
	}
	SetModified();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strTmp;
	GetDefautColor();
	for(int i=0;i<m_ncolor;i++){
		m_Grid.SetCellType(i+1,2,7);
		m_Grid.SetItemBkColour(i+1,2,m_color[i]);
		m_Grid.SetItemText(i+1,1,m_gjname.ElementAt(i));
		strTmp.Format(L"%d",i+1);
		m_Grid.SetItemText(i+1,0,strTmp);
		strTmp.Format(L"%d",m_KPASTE[i]);
		m_Grid.SetItemText(i+1,3,strTmp);
	}
	UpdateData(FALSE);
}

BOOL CGjColorPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CRect rect;
	CString strTmp;
	InitIDName();
	GetGjColor();
	m_Grid.GetClientRect(&rect);

	m_Grid.SetRowCount(m_ncolor+1);
	m_Grid.SetColCount(4);

	int wd=rect.Width()/12;
	m_Grid.SetColWidth(0,wd*2);
	m_Grid.SetColWidth(1,(short)(wd)*4);
	m_Grid.SetColWidth(2,(short)(wd)*3);
	m_Grid.SetColWidth(3,(short)(wd)*2+6);

	//	m_Grid.SetColWidth(0,rect.Width()/5);
	//	m_Grid.SetColWidth(1,(short)(rect.Width()*0.3-12));
	//	m_Grid.SetColWidth(2,(short)(rect.Width()*0.3-12));
	//	m_Grid.SetColWidth(3,(short)(rect.Width()*0.2));

	m_Grid.SetItemText(0,0,L"���");
	m_Grid.SetItemText(0,1,L"���");
	m_Grid.SetItemText(0,2,L"��ɫ");
	m_Grid.SetItemText(0,3,L"͸����");

	for(int i=0;i<m_ncolor;i++){
		m_Grid.SetCellType(i+1,2,7);
		m_Grid.SetItemBkColour(i+1,2,m_color[i]);
		m_Grid.SetItemText(i+1,1,m_gjname.ElementAt(i));
		strTmp.Format(L"%d",i+1);
		m_Grid.SetItemText(i+1,0,strTmp);
		strTmp.Format(L"%d",m_KPASTE[i]);
		m_Grid.SetItemText(i+1,3,strTmp);
		//m_Grid.SetReadOnly(TRUE);
	}

	if(m_Solution<0 || m_Solution>3)
	m_Solution=0;
	CString pkpminipath=GetPKPMFileName();
	int tmpint=GetPrivateProfileInt(L"PKPMMENU",L"��ɫ����",-1,pkpminipath);
	if (tmpint>=0)
	{
		m_ToolbarColorSchem=tmpint;
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CGjColorPropertyPage::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	UpdateData(TRUE);
	CString strTmp;int ntemp=0;
	int nCount = m_Grid.GetRowCount();
	for(int i=0;i<m_ncolor&&i<m_Grid.GetRowCount()+1;i++){
		m_color[i] = m_Grid.GetItemBkColour(i+1,2);
		strTmp=m_Grid.GetCellString(i+1,3);
		ntemp= _tstoi(strTmp);
		if(ntemp>100 || ntemp<0) ntemp=0;
		m_KPASTE[i] = ntemp;
	}
	SetGjColor();
	CString pkpminipath=GetPKPMFileName();
	CString tmp;
	tmp.Format(L"%d",m_ToolbarColorSchem);
	::WritePrivateProfileString(L"PKPMMENU",L"��ɫ����",tmp,pkpminipath);
	UpdateData(FALSE);
	CPropertyPage::OnOK();
}

void CGjColorPropertyPage::OnCbnSelchangeCombocolor()
{
		UpdateData(TRUE);
		SetModified();
		int presolution=m_Solution;
		// TODO: �ڴ���ӿؼ�֪ͨ����������
		if (m_Solution==0 || m_Solution==1)
		{
			OnBnClickedDefault();
		}
		else
		{
			GetGjColor();
			CString strTmp;
			for(int i=0;i<m_ncolor;i++){
				m_Grid.SetCellType(i+1,2,7);
				m_Grid.SetItemBkColour(i+1,2,m_color[i]);
				m_Grid.SetItemText(i+1,1,m_gjname.ElementAt(i));
				strTmp.Format(L"%d",i+1);
				m_Grid.SetItemText(i+1,0,strTmp);
				strTmp.Format(L"%d",m_KPASTE[i]);
				m_Grid.SetItemText(i+1,3,strTmp);
			}
		}
		m_Solution=presolution;
		UpdateData(FALSE);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CGjColorPropertyPage::OnCbnSelchangeCombo1()
{
	SetModified();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
