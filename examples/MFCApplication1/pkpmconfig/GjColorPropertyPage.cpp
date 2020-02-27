// GjColorPropertyPage.cpp : 实现文件
//
#include "../stdafx.h"
#include "GjColorPropertyPage.h"
#include "afxdialogex.h"
#include "pkpmconfig.h"
// CGjColorPropertyPage 对话框

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


// CGjColorPropertyPage 消息处理程序

BEGIN_EVENTSINK_MAP(CGjColorPropertyPage, CPropertyPage)
	ON_EVENT(CGjColorPropertyPage, IDC_PGCELLCOLORCTRL, 5, CGjColorPropertyPage::OnEditCharPgcellcolorctrl, VTS_I2 VTS_I2 VTS_I4 VTS_I2)
	ON_EVENT(CGjColorPropertyPage, IDC_PGCELLCOLORCTRL, 28, CGjColorPropertyPage::OnBeginEditCellPgcellcolorctrl, VTS_I4 VTS_I4 VTS_BSTR VTS_I2)
	ON_EVENT(CGjColorPropertyPage, IDC_PGCELLCOLORCTRL, 13, CGjColorPropertyPage::OnColorBtnEndSelectPgcellcolorctrl, VTS_I2 VTS_I2 VTS_I4)
END_EVENTSINK_MAP()


void CGjColorPropertyPage::OnEditCharPgcellcolorctrl(short nCurRow, short nCurCOl, long nChar, short nPage)
{
	SetModified();
	// TODO: 在此处添加消息处理程序代码
}


void CGjColorPropertyPage::OnBeginEditCellPgcellcolorctrl(long lCurRow, long lCurCol, LPCTSTR strCellString, short nPage)
{
	SetModified();
	// TODO: 在此处添加消息处理程序代码
}


void CGjColorPropertyPage::OnColorBtnEndSelectPgcellcolorctrl(short nRow, short nCol, long ulBkClr)
{
	SetModified();
	// TODO: 在此处添加消息处理程序代码
}

/*********************************************************************
颜色分类
	1-烧结砖墙		2-混凝土墙		3-蒸压砖墙      4-空心砌块墙	5-轻骨料墙
	6-砌体柱		7-钢柱			8-混凝土柱		9-刚性杆柱		10-轻骨料柱       
	11-钢主梁		12-混凝土主梁	13-刚性杆主梁	14-轻骨料主梁	15-门窗洞口
	16-楼板			17-楼板洞口		18-钢斜杆		19-混凝土斜杆	20-刚性杆斜杆     
	21-轻骨料斜杆	22-钢次梁		23-混凝土次梁	24-刚性杆次梁	25-轻骨料次梁
	26-楼梯示意		27-混凝土圈梁	28-钢圈梁       29-地面         30-悬挑板
*********************************************************************/


void CGjColorPropertyPage::InitIDName()
{
	m_gjname.RemoveAll();
	m_gjname.Add(L"烧结砖墙");      //1
	m_gjname.Add(L"混凝土墙");
	m_gjname.Add(L"蒸压砖墙");
	m_gjname.Add(L"空心砌块墙");
	m_gjname.Add(L"轻骨料墙");
	m_gjname.Add(L"砌体柱");
	m_gjname.Add(L"钢柱");
	m_gjname.Add(L"混凝土柱");
	m_gjname.Add(L"刚性杆柱");
	m_gjname.Add(L"轻骨料柱");       //10
	m_gjname.Add(L"钢主梁");
	m_gjname.Add(L"混凝土主梁");
	m_gjname.Add(L"刚性杆主梁");
	m_gjname.Add(L"轻骨料主梁");
	m_gjname.Add(L"门窗洞口");
	m_gjname.Add(L"楼板");
	m_gjname.Add(L"楼板洞口");
	m_gjname.Add(L"钢斜杆");
	m_gjname.Add(L"混凝土斜杆");
	m_gjname.Add(L"刚性杆斜杆");     //20
	m_gjname.Add(L"轻骨料斜杆");
	m_gjname.Add(L"钢次梁");
	m_gjname.Add(L"混凝土次梁");
	m_gjname.Add(L"刚性杆次梁");
	m_gjname.Add(L"轻骨料次梁");
	m_gjname.Add(L"楼梯示意");
	m_gjname.Add(L"混凝土圈梁");
	m_gjname.Add(L"钢圈梁");
	m_gjname.Add(L"地面");
	m_gjname.Add(L"悬挑板");
	m_gjname.Add(L"层间板");
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
		if(m_color[0]>=0){					//烧结砖墙
			m_color[0] = 8671917;
			//CFGRGB(cfgcolor,m_color[0]);
			m_KPASTE[0] = 30;
		}
		if(m_color[1]>=0){					//混凝土墙
			m_color[1] = 6734218;
			//CFGRGB(cfgcolor,m_color[1]);
			m_KPASTE[1] = 30;
		}
		if(m_color[2]>=0){					//蒸压砖墙
			m_color[2] = 3368584;
			//CFGRGB(cfgcolor,m_color[2]);
			m_KPASTE[2] = 30;
		}
		if(m_color[3]>=0){					//空心砌块墙
			m_color[3] = 749522;
			//CFGRGB(cfgcolor,m_color[3]);
			m_KPASTE[3] = 30;
		}
		if(m_color[4]>=0){					//轻骨料墙
			m_color[4] = 13468552;
			//CFGRGB(cfgcolor,m_color[4]);
			m_KPASTE[4] = 30;
		}
		if(m_color[5]>=0){					//砌体柱
			m_color[5] = 6710886;
			//CFGRGB(cfgcolor,m_color[5]);
		}
		if(m_color[6]>=0){					//钢柱
			m_color[6] = 8505786;
			//CFGRGB(cfgcolor,m_color[6]);
		}
		if(m_color[7]>=0){					//混凝土柱
			m_color[7] = 13684834;
			//CFGRGB(cfgcolor,m_color[7]);
		}
		if(m_color[8]>=0){					//刚性杆柱
			m_color[8] = 10374774;
			//CFGRGB(cfgcolor,m_color[8]);
		}
		if(m_color[9]>=0){					//轻骨料柱
			m_color[9] = 16379566;
			//CFGRGB(cfgcolor,m_color[9]);
		}
		if(m_color[10]>=0){					//钢主梁
			m_color[10] = 10518589;
			//CFGRGB(cfgcolor,m_color[10]);
		}
		if(m_color[11]>=0){					//混凝土主梁
			m_color[11] = 13988921;
			//CFGRGB(cfgcolor,m_color[11]);
		}
		if(m_color[12]>=0){					//刚性杆主梁
			m_color[12] =5532963;
			//CFGRGB(cfgcolor,m_color[12]);
		}
		if(m_color[13]>=0){					//轻骨料主梁
			m_color[13] = 5159080;
			//CFGRGB(cfgcolor,m_color[13]);
		}
		if(m_color[14]>=0){					//门窗洞口
			m_color[14] = 13688896;
			//CFGRGB(cfgcolor,m_color[14]);
			m_KPASTE[14]=30;
		}
		if(m_color[15]>=0){					//楼板
			m_color[15] = 12566463;
			//CFGRGB(cfgcolor,m_color[15]);
			m_KPASTE[15]=50;
		}
		if(m_color[16]>=0){					//楼板洞口
			m_color[16] = 12118515;
			//CFGRGB(cfgcolor,m_color[16]);
			m_KPASTE[16]=30;
		}
		if(m_color[17]>=0){					//钢斜杆
			m_color[17] = 14967157;
			//CFGRGB(cfgcolor,m_color[17]);
		}
		if(m_color[18]>=0){					//混凝土斜杆
			m_color[18] = 3295670;
			//CFGRGB(cfgcolor,m_color[18]);
		}
		if(m_color[19]>=0){					//刚性杆斜杆
			m_color[19] = 13453244;
			//CFGRGB(cfgcolor,m_color[19]);
		}
		if(m_color[20]>=0){					//轻骨料斜杆
			m_color[20] = 8421631;
			//CFGRGB(cfgcolor,m_color[20]);
		}
		if(m_color[21]>=0){					//钢次梁
			m_color[21] = 15188662;
			//CFGRGB(cfgcolor,m_color[21]);
		}
		if(m_color[22]>=0){					//混凝土次梁
			m_color[22] = 12623485;
			//CFGRGB(cfgcolor,m_color[22]);
		}
		if(m_color[23]>=0){					//刚性杆次梁
			m_color[23] = 12614523;
			//CFGRGB(cfgcolor,m_color[23]);
		}
		if(m_color[24]>=0){					//轻骨料次梁
			m_color[24] = 12382580;
			//CFGRGB(cfgcolor,m_color[24]);
		}
		if(m_color[25]>=0){					//楼梯示意
			m_color[25] = 8895429;
			//CFGRGB(cfgcolor,m_color[25]);
			m_KPASTE[25] = 20;
		}
		if(m_color[26]>=0){					//混凝土圈梁
			m_color[26] = 14369894;
			//CFGRGB(cfgcolor,m_color[26]);
		}
		if(m_color[27]>=0){					//钢圈梁
			m_color[27] = 12614523;
			//CFGRGB(cfgcolor,m_color[27]);
		}
		if(m_color[28]>=0){					//地面
			m_color[28] = 3156001;
			//CFGRGB(cfgcolor,m_color[28]);
			m_KPASTE[28]=60;
		}
		if(m_color[29]>=0){					//悬挑板
			m_color[29] = 14054563;
			//CFGRGB(cfgcolor,m_color[29]);
		}
		if(m_color[29]>=0){					//悬挑板
			m_color[29] = 14054563;
			//CFGRGB(cfgcolor,m_color[29]);
		}
		if(m_color[30]>=0){					//层间板
			m_color[30] = 14054563;
			//CFGRGB(cfgcolor,m_color[29]);
		}
		
		break;
	case 1:
		if(m_color[0]>=0){					//烧结砖墙
			m_color[0] = 12550143;
			m_KPASTE[0] = 10;
		}
		if(m_color[1]>=0){					//混凝土墙
			m_color[1] = 5635925;
			m_KPASTE[1] = 10;
		}
		if(m_color[2]>=0){					//蒸压砖墙
			m_color[2] = 5603327;
			m_KPASTE[2] = 10;
		}
		if(m_color[3]>=0){					//空心砌块墙
			m_color[3] = 5603327;
			m_KPASTE[3] = 10;
		}
		if(m_color[4]>=0){					//轻骨料墙
			m_color[4] = 13238217;
			m_KPASTE[4] = 10;
		}
		if(m_color[5]>=0){					//砌体柱
			m_color[5] = 65535;
		}
		if(m_color[6]>=0){					//钢柱
			m_color[6] = 9764863;
		}
		if(m_color[7]>=0){					//混凝土柱
			m_color[7] = 65535;
		}
		if(m_color[8]>=0){					//刚性杆柱
			m_color[8] = 65535;
		}
		if(m_color[9]>=0){					//轻骨料柱
			m_color[9] = 16379566;
		}
		if(m_color[10]>=0){					//钢主梁
			m_color[10] = 16379566;
		}
		if(m_color[11]>=0){					//混凝土主梁
			m_color[11] = 16776960;
		}
		if(m_color[12]>=0){					//刚性杆主梁
			m_color[12] =16776960;
		}
		if(m_color[13]>=0){					//轻骨料主梁
			m_color[13] = 9764863;
		}
		if(m_color[14]>=0){					//门窗洞口
			m_color[14] = 16777215;
			m_KPASTE[14]=50;
		}
		if(m_color[15]>=0){					//楼板
			m_color[15] = 12566463;
			m_KPASTE[15]=50;
		}
		if(m_color[16]>=0){					//楼板洞口
			m_color[16] = 16777215;
			m_KPASTE[16]=50;
		}
		if(m_color[17]>=0){					//钢斜杆
			m_color[17] = 16711935;
		}
		if(m_color[18]>=0){					//混凝土斜杆
			m_color[18] = 16711935;
		}
		if(m_color[19]>=0){					//刚性杆斜杆
			m_color[19] = 16711935;
		}
		if(m_color[20]>=0){					//轻骨料斜杆
			m_color[20] = 13224447;
		}
		if(m_color[21]>=0){					//钢次梁
			m_color[21] = 15315828;
		}
		if(m_color[22]>=0){					//混凝土次梁
			m_color[22] = 11184810;
		}
		if(m_color[23]>=0){					//刚性杆次梁
			m_color[23] = 11184810;
		}
		if(m_color[24]>=0){					//轻骨料次梁
			m_color[24] = 11184810;
		}
		if(m_color[25]>=0){					//楼梯示意
			m_color[25] = 16777215;
			m_KPASTE[25] = 20;
		}
		if(m_color[26]>=0){					//混凝土圈梁
			m_color[26] = 5635925;
		}
		if(m_color[27]>=0){					//钢圈梁
			m_color[27] = 5635925;
		}
		if(m_color[28]>=0){					//地面
			m_color[28] = 9737354;
			m_KPASTE[28]=60;
		}
		if(m_color[29]>=0){					//悬挑板
			m_color[29] = 16777215;
		}
		if(m_color[30]>=0){					//层间板
			m_color[30] = 16777215;
			//CFGRGB(cfgcolor,m_color[29]);
		}
		break;
	}
	
	if(m_color[0]==0){					//烧结砖墙
		m_color[0] = 8671917;
		//CFGRGB(cfgcolor,m_color[0]);
		m_KPASTE[0] = 30;
	}
	if(m_color[1]==0){					//混凝土墙
		m_color[1] = 6734218;
		//CFGRGB(cfgcolor,m_color[1]);
		m_KPASTE[1] = 30;
	}
	if(m_color[2]==0){					//蒸压砖墙
		m_color[2] = 3368584;
		//CFGRGB(cfgcolor,m_color[2]);
		m_KPASTE[2] = 30;
	}
	if(m_color[3]==0){					//空心砌块墙
		m_color[3] = 749522;
		//CFGRGB(cfgcolor,m_color[3]);
		m_KPASTE[3] = 30;
	}
	if(m_color[4]==0){					//轻骨料墙
		m_color[4] = 13468552;
		//CFGRGB(cfgcolor,m_color[4]);
		m_KPASTE[4] = 30;
	}
	if(m_color[5]==0){					//砌体柱
		m_color[5] = 6710886;
		//CFGRGB(cfgcolor,m_color[5]);
	}
	if(m_color[6]==0){					//钢柱
		m_color[6] = 8280859;
		//CFGRGB(cfgcolor,m_color[6]);
	}
	if(m_color[7]==0){					//混凝土柱
		m_color[7] = 13988921;
		//CFGRGB(cfgcolor,m_color[7]);
	}
	if(m_color[8]==0){					//刚性杆柱
		m_color[8] = 10374774;
		//CFGRGB(cfgcolor,m_color[8]);
	}
	if(m_color[9]==0){					//轻骨料柱
		m_color[9] = 16379566;
		//CFGRGB(cfgcolor,m_color[9]);
	}
	if(m_color[10]==0){					//钢主梁
		m_color[10] = 12556625;
		//CFGRGB(cfgcolor,m_color[10]);
	}
	if(m_color[11]==0){					//混凝土主梁
		m_color[11] = 11972869;
		//CFGRGB(cfgcolor,m_color[11]);
	}
	if(m_color[12]==0){					//刚性杆主梁
		m_color[12] =9090060;
		//CFGRGB(cfgcolor,m_color[12]);
	}
	if(m_color[13]==0){					//轻骨料主梁
		m_color[13] = 2465435;
		//CFGRGB(cfgcolor,m_color[13]);
	}
	if(m_color[14]==0){					//门窗洞口
		m_color[14] = 13688896;
		//CFGRGB(cfgcolor,m_color[14]);
		m_KPASTE[14]=30;
	}
	if(m_color[15]==0){					//楼板
		m_color[15] = 12566463;
		//CFGRGB(cfgcolor,m_color[15]);
		m_KPASTE[15]=50;
	}
	if(m_color[16]==0){					//楼板洞口
		m_color[16] = 12118515;
		//CFGRGB(cfgcolor,m_color[16]);
		m_KPASTE[16]=30;
	}
	if(m_color[17]==0){					//钢斜杆
		m_color[17] = 10312658;
		//CFGRGB(cfgcolor,m_color[17]);
	}
	if(m_color[18]==0){					//混凝土斜杆
		m_color[18] = 13391727;
		//CFGRGB(cfgcolor,m_color[18]);
	}
	if(m_color[19]==0){					//刚性杆斜杆
		m_color[19] = 13453244;
		//CFGRGB(cfgcolor,m_color[19]);
	}
	if(m_color[20]==0){					//轻骨料斜杆
		m_color[20] = 13224447;
		//CFGRGB(cfgcolor,m_color[20]);
	}
	if(m_color[21]==0){					//钢次梁
		m_color[21] = 12357157;
		//CFGRGB(cfgcolor,m_color[21]);
	}
	if(m_color[22]==0){					//混凝土次梁
		m_color[22] = 12623485;
		//CFGRGB(cfgcolor,m_color[22]);
	}
	if(m_color[23]==0){					//刚性杆次梁
		m_color[23] = 12614523;
		//CFGRGB(cfgcolor,m_color[23]);
	}
	if(m_color[24]==0){					//轻骨料次梁
		m_color[24] = 9143144;
		//CFGRGB(cfgcolor,m_color[24]);
	}
	if(m_color[25]==0){					//楼梯示意
		m_color[25] = 8895429;
		//CFGRGB(cfgcolor,m_color[25]);
		m_KPASTE[25] = 20;
	}
	if(m_color[26]==0){					//混凝土圈梁
		m_color[26] = 14369894;
		//CFGRGB(cfgcolor,m_color[26]);
	}
	if(m_color[27]==0){					//钢圈梁
		m_color[27] = 12614523;
		//CFGRGB(cfgcolor,m_color[27]);
	}
	if(m_color[28]==0){					//地面
		m_color[28] = 3156001;
		//CFGRGB(cfgcolor,m_color[28]);
		m_KPASTE[28]=60;
	}
	if(m_color[29]==0){					//悬挑板
		m_color[29] = 14054563;
		//CFGRGB(cfgcolor,color[29]);
	}
	if(m_color[30]>=0){					//层间板
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
	// TODO: 在此添加控件通知处理程序代码
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

	m_Grid.SetItemText(0,0,L"序号");
	m_Grid.SetItemText(0,1,L"类别");
	m_Grid.SetItemText(0,2,L"颜色");
	m_Grid.SetItemText(0,3,L"透明度");

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
	int tmpint=GetPrivateProfileInt(L"PKPMMENU",L"配色方案",-1,pkpminipath);
	if (tmpint>=0)
	{
		m_ToolbarColorSchem=tmpint;
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CGjColorPropertyPage::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
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
	::WritePrivateProfileString(L"PKPMMENU",L"配色方案",tmp,pkpminipath);
	UpdateData(FALSE);
	CPropertyPage::OnOK();
}

void CGjColorPropertyPage::OnCbnSelchangeCombocolor()
{
		UpdateData(TRUE);
		SetModified();
		int presolution=m_Solution;
		// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
}


void CGjColorPropertyPage::OnCbnSelchangeCombo1()
{
	SetModified();
	// TODO: 在此添加控件通知处理程序代码
}
