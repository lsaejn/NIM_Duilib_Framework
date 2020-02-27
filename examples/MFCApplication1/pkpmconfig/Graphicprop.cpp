// Graphicprop.cpp : ʵ���ļ�
//
#include "../stdafx.h"
#include "pkpmconfig.h"
#include "Graphicprop.h"
#include "afxdialogex.h"
#include "../json/json.h"
//#include "PkpmmainJson.h"
// CGraphicprop �Ի���

IMPLEMENT_DYNAMIC(CGraphicprop, CPropertyPage)

CGraphicprop::CGraphicprop()
	: CPropertyPage(CGraphicprop::IDD)
	, m_ifsavedimstyle(FALSE)
	, m_Fontstyle(0)
	, m_iftriangle(FALSE)
	, m_nSTS_T2Dwg(0)
	, m_nSelDwgVersion(0)
{

}

CGraphicprop::~CGraphicprop()
{
}

void CGraphicprop::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECKSAVETDIM, m_ifsavedimstyle);
	DDX_CBIndex(pDX, IDC_COMBOFONT, m_Fontstyle);
	DDX_Check(pDX, IDC_CHECKTRIANGLE, m_iftriangle);
	DDX_CBIndex(pDX, IDC_COMB_STS_T2DWG, m_nSTS_T2Dwg);
	DDX_CBIndex(pDX, IDC_COMB_DWG_VER, m_nSelDwgVersion);
	DDX_Control(pDX, IDC_COMB_DWG_VER, m_combDwgVer);
}


BEGIN_MESSAGE_MAP(CGraphicprop, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBOFONT, &CGraphicprop::OnCbnSelchangeCombofont)
	ON_BN_CLICKED(IDC_CHECKSAVETDIM, &CGraphicprop::OnBnClickedChecksavetdim)
	ON_BN_CLICKED(IDC_CHECKTRIANGLE, &CGraphicprop::OnBnClickedChecktriangle)
	ON_CBN_SELCHANGE(IDC_COMB_STS_T2DWG, &CGraphicprop::OnCbnSelchangeCombStsT2dwg)
	ON_CBN_SELCHANGE(IDC_COMB_DWG_VER, &CGraphicprop::OnCbnSelchangeCombDwgVer)
END_MESSAGE_MAP()


// CGraphicprop ��Ϣ�������


//void CGraphicprop::OnEnChangeEdit1()
//{
//	SetModified();
//	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
//	// ���ʹ�֪ͨ��������д CPropertyPage::OnInitDialog()
//	// ���������� CRichEditCtrl().SetEventMask()��
//	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
//
//	// TODO:  �ڴ���ӿؼ�֪ͨ����������
//}

//;;��ѡ���������ԡ������ġ�ƴ��;
//����=����
//
//	;;��ѡ���Tͼ��ת��Autocadͼ��DWG��ȱʡ���͡���DOS��WINDOWS;
//AUTOCAD_����=WINDOWS
//
//	;;Tͼ��ת��Autocadͼ��DWG�ĳߴ��߷�ʽ�Ƿ�������0��ը������1��������;
//AUTOCAD_Dim=0
//
//	;;Tͼ��ת��Autocadͼ�ζ��������������Ԫ��ʽ���Ƿ񡪡�0��ȱʡ�ı�Ԫ����1������Ԫ��;
//AUTOCAD_FILL3=0
//

bool CGraphicprop::OnInitVerComb()
{

	m_dwgSelOption.RemoveAll();
	//m_dwgSelOption.Add("Acad R10");
	m_dwgSelOption.Add(L"Acad R12");
	m_dwgSelOption.Add(L"Acad R13");
	m_dwgSelOption.Add(L"Acad R14");

	m_dwgSelOption.Add(L"Acad2000");
	m_dwgSelOption.Add(L"Acad2004");
	m_dwgSelOption.Add(L"Acad2007");
	m_dwgSelOption.Add(L"Acad2010");
	m_dwgSelOption.Add(L"Acad2013");



	m_dwgSelUI.RemoveAll();
	m_dwgSelUI.Add(L"Acad R12(Acad R12)");
	m_dwgSelUI.Add(L"Acad R13(Acad R13)");
	m_dwgSelUI.Add(L"Acad R14(Acad R14)");

	m_dwgSelUI.Add(L"Acad2000(Acad2000-2002)");
	m_dwgSelUI.Add(L"Acad2004(Acad2004-2006)");
	m_dwgSelUI.Add(L"Acad2007(Acad2007-2009)");
	m_dwgSelUI.Add(L"Acad2010(Acad2010-2012)");
	m_dwgSelUI.Add(L"Acad2013(Acad2013-2017)");

	for (int i=0;i<m_dwgSelUI.GetSize();i++)
	{
		m_combDwgVer.AddString(m_dwgSelUI[i]);
	}
	return true;
}
BOOL CGraphicprop::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	OnInitVerComb();

	//PkpmmainConfigJson::Get().Load();


	const wchar_t* MARKER_NOWTODWG =  L"NOWTODWG";
	const wchar_t* MARKER_STS_NEWDWGTRANS= L"STS_NEWDWGTRANS_2";
	const wchar_t* MARKER_DWGTRANSNAME_APPENDIX=  L"DWGTRANSNAME_APPENDIX";
	const wchar_t* MARKER_DWG_VERSION = L"DWG_VERSION_2";

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString pkpminipath=GetPKPMFileName();
	CString tmp;
	GetPrivateProfileString(L"�������",L"AUTOCAD_����",L"0",tmp.GetBuffer(500),500,pkpminipath);
	m_Fontstyle=0;
	if (tmp!="0")
	{
		if (tmp=="DOS")
		{
			m_Fontstyle=0;
		}
		else
			m_Fontstyle=1;
		
	}
	int tmpint;
	tmpint=GetPrivateProfileInt(L"�������",L"AUTOCAD_Dim",-1,pkpminipath);
	m_ifsavedimstyle=0;
	if (tmpint!=-1)
	{
		if (tmpint==0)
		{
			m_ifsavedimstyle=0;
		}
		else
			m_ifsavedimstyle=1;
	}
	m_iftriangle=0;
	tmpint=GetPrivateProfileInt(L"�������",L"AUTOCAD_FILL3",-1,pkpminipath);
	m_iftriangle=0;
	if (tmpint!=-1)
	{
		if (tmpint==0)
		{
			m_iftriangle=0;
		}
		else
			m_iftriangle=1;
	}

	//CString tmp;
	GetPrivateProfileString(MARKER_NOWTODWG,MARKER_STS_NEWDWGTRANS,L"1",tmp.GetBuffer(500),500,pkpminipath);
	tmp.ReleaseBuffer();
	tmp.Trim();
	if (0==tmp.CompareNoCase(L"0"))//0=false  1=true
	{
		m_nSTS_T2Dwg = 0;
	}
	else
	{
		m_nSTS_T2Dwg = 1;
	}
	 
	GetPrivateProfileString(MARKER_NOWTODWG,MARKER_DWG_VERSION,L"0",tmp.GetBuffer(500),500,pkpminipath);
	tmp.ReleaseBuffer();
	tmp.Trim();
	bool bFind = false;
	for (int i=0;i<m_dwgSelOption.GetSize();i++)
	{
		CString strone = m_dwgSelOption[i];
		if (0 == strone.CompareNoCase(tmp))
		{
			m_nSelDwgVersion = i;
			bFind = true;
			break;
		}
	}
	if (!bFind)
	{
		m_nSelDwgVersion = 5;//Acad2007
	}
 
	//m_nSTS_T2Dwg = PkpmmainConfigJson::Get().IsStsT2DwgNewVersion()==false?0:1;

	//CString str = PkpmmainConfigJson::Get().DwgVersion();
	//for (int i=0;i<m_dwgSelOption.GetSize();i++)
	//{
	//	if(0==m_dwgSelOption[i].CompareNoCase(str))
	//	{
	//		m_nSelDwgVersion = i;
	//		break;
	//	}
	//}

	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}




//void CGraphicprop::OnBnClickedCheck1()
//{
//	SetModified();
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//}


//void CGraphicprop::OnBnClickedCheck2()
//{
//	SetModified();
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//}


void CGraphicprop::OnOK()
{
	const wchar_t* MARKER_NOWTODWG =  L"NOWTODWG";
	const wchar_t* MARKER_STS_NEWDWGTRANS= L"STS_NEWDWGTRANS_2"; //�ܿ���
	const wchar_t* MARKER_DWGTRANSNAME_APPENDIX=  L"DWGTRANSNAME_APPENDIX";
	const wchar_t* MARKER_DWG_VERSION = L"DWG_VERSION_2";


	// TODO: �ڴ����ר�ô����/����û���
	UpdateData(TRUE);
	CString pkpminipath=GetPKPMFileName();
	CString tmp;
	tmp="WINDOWS";
	if (m_Fontstyle==0)
	{
		tmp="DOS";
	}
	::WritePrivateProfileString(L"�������",L"AUTOCAD_����",tmp,pkpminipath);
	tmp.Format(L"%d",m_ifsavedimstyle?1:0);
	::WritePrivateProfileString(L"�������",L"AUTOCAD_Dim",tmp,pkpminipath);
	tmp.Format(L"%d",m_iftriangle?1:0);
	::WritePrivateProfileString(L"�������",L"AUTOCAD_FILL3",tmp,pkpminipath);
/*
	//if (m_nSTS_T2Dwg==0)
	{
		PkpmmainConfigJson::Get().SetStsT2DwgNewVersion(false);
	}
	else  if (m_nSTS_T2Dwg==1)
	{
		PkpmmainConfigJson::Get().SetStsT2DwgNewVersion(true);
	}
*/
	CString str = m_dwgSelOption[m_nSelDwgVersion];
	//PkpmmainConfigJson::Get().SetDwgVersion(str);
	//PkpmmainConfigJson::Get().save();

	tmp.Format(L"%d",m_nSTS_T2Dwg);//0=false  1=true
	::WritePrivateProfileString(L"NOWTODWG",MARKER_STS_NEWDWGTRANS,tmp,pkpminipath);
	::WritePrivateProfileString(L"NOWTODWG",MARKER_DWG_VERSION,str,pkpminipath);

	CPropertyPage::OnOK();
}


void CGraphicprop::OnCbnSelchangeCombofont()
{
	SetModified();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CGraphicprop::OnBnClickedChecksavetdim()
{
	SetModified();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CGraphicprop::OnBnClickedChecktriangle()
{
	SetModified();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CGraphicprop::OnCbnSelchangeCombStsT2dwg()
{
	SetModified();
}


void CGraphicprop::OnCbnSelchangeCombDwgVer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetModified();
}
