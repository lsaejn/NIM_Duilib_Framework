// PkpmInfoSheet.cpp : ʵ���ļ�
//

#include "../stdafx.h"
#include "pkpmconfig.h"
#include "PkpmInfoSheet.h"
#include "../resource.h"
#include "../CommonUtil.h"
#include "../svr.h"
#include "../PublicMacroHeader.h"


// CPkpmInfoSheet

//chenjian ԭ�ȶԻ����menu_data�Ѿ�û������
//�ȴ�����ͨ��


IMPLEMENT_DYNAMIC(CPkpmInfoSheet, CPropertySheet)

CPkpmInfoSheet::CPkpmInfoSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	
}

CPkpmInfoSheet::CPkpmInfoSheet(LPCTSTR pszCaption,menu_data* pData,bool bHasPwe,UINT iSelectPage)
	:CPropertySheet(pszCaption, NULL, iSelectPage)
{

	m_psh.dwFlags &= ~(PSH_HASHELP);

	m_graghpage.m_psp.dwFlags &= ~(PSP_HASHELP);	AddPage(&m_graghpage);
	m_pmctrlpage.m_psp.dwFlags &= ~(PSP_HASHELP);	AddPage(&m_pmctrlpage);
	m_colorpage.m_psp.dwFlags &= ~(PSP_HASHELP);	AddPage(&m_colorpage);
	//m_lockPage.m_psp.dwFlags &= ~(PSP_HASHELP);		AddPage(&m_lockPage);
	
	
	//m_SeqPage.m_psp.dwFlags &= ~(PSP_HASHELP);	m_SeqPage.setMenuData(pData);  AddPage(&m_SeqPage);
	if (bHasPwe)
	{
		m_Pwe.m_psp.dwFlags &= ~(PSP_HASHELP);		AddPage(&m_Pwe);
	}
	m_otherspage.m_psp.dwFlags &= ~(PSP_HASHELP);	AddPage(&m_otherspage);
	
	

	 
}

CPkpmInfoSheet::~CPkpmInfoSheet()
{

}


BEGIN_MESSAGE_MAP(CPkpmInfoSheet, CPropertySheet)
	ON_BN_CLICKED(IDC_BT_WRITE_OUT, OnBnClickedBtnWriteOut)
	ON_BN_CLICKED(IDC_BT_LOAD_IN, OnBnClickedBtnLoadIn)
END_MESSAGE_MAP()


// CPkpmInfoSheet ��Ϣ�������


BOOL CPkpmInfoSheet::OnInitDialog()
{

	BOOL bResult = CPropertySheet::OnInitDialog();


	{
		CRect rc;
		CWnd *pBt = GetDlgItem(IDOK);
		pBt->GetWindowRect(&rc);
		ScreenToClient(&rc);

		int wid = rc.Width();

		CRect rcOut =rc;
		rcOut.left = 6;
		rcOut.right = rcOut.left+wid;

		CRect rcIn =rc;
		rcIn.left = rcOut.right + 6;
		rcIn.right = rcIn.left+wid;

		m_WriteOut.Create("����",BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,rcOut,this,IDC_BT_WRITE_OUT);
		m_LoadIn.Create("����",BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,rcIn,this,IDC_BT_LOAD_IN);

		m_WriteOut.SetFont(pBt->GetFont());
		m_LoadIn.SetFont(pBt->GetFont());
	}


	

	return bResult;
}

 
void CPkpmInfoSheet::OnBnClickedBtnWriteOut()
{


	auto f_SaveAsDlg = [](CString& zfname)->bool
	{
		CString defaultDir;
		svr::getPathByMaker("CFG",defaultDir);
		CString fileN  = "PkpmV2010";
		BOOL isOpen =FALSE;
		CString filter;
		filter = "�����ļ�(*.config )|*.config||";
		
		CFileDialog openFiDlg(isOpen,defaultDir,fileN,
			OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,filter);

		if(IDOK == openFiDlg.DoModal())
		{
			zfname = openFiDlg.GetOFN().lpstrFile;
			return true;
		}
		return false;
	};


	CString fname;
	if(f_SaveAsDlg(fname))
	{
		CString pathPathPkpmIni = sjx::GetModuleFolder()+"CFG\\PKPM.INI";
		CString pathPathCli = sjx::GetModuleFolder()+"CFG\\Clicfg6.ini";

		if(sjx::IsFileExist(pathPathPkpmIni)
			&&sjx::IsFileExist(pathPathCli))
		{

			CStdioFile f1;
			CStdioFile f2;
			CStdioFile f3;
			bool bOpen1=false,bOpen2=false;
			if(f1.Open(pathPathPkpmIni,CFile::modeRead))
			{
				bOpen1=true;
			}

			
			if(f2.Open(pathPathCli,CFile::modeRead))
			{
				bOpen2=true;
			}

			CStringArray strAll;
			if (bOpen1&&bOpen2)
			{
				CString strLoop;

				strAll.Add("----PKPM.INI----");
				while(f1.ReadString(strLoop))
				{
					strAll.Add(strLoop);
				}

				strAll.Add("----CLICFG6.INI----");
				while(f2.ReadString(strLoop))
				{
					strAll.Add(strLoop);
				}
				strAll.Add("----END----");

				f1.Close();
				f2.Close();

				if(f3.Open(fname,CFile::modeWrite|CFile::modeCreate))
				{
					for (int i=0;i<strAll.GetSize();i++)
					{
						f3.WriteString(strAll[i]+"\n");
					}

					f3.Close();
				}
				else
				{
					MessageBox("�޷����������ļ�. ");
					return;
				}
				
			}
			else
			{
				MessageBox("�޷��������ļ� pkpm.ini ��/�� Clicfg6.ini ");
				return;
			}


		}
		else
		{
			MessageBox("�޷��ҵ������ļ� pkpm.ini ��/�� Clicfg6.ini.��ȷ���ļ����ڻ����°�װ.");
			return;
		}
	}

}
void CPkpmInfoSheet::OnBnClickedBtnLoadIn()
{

	
	auto f_OpenFile = [](CString &zFileName)->bool
	{
		BOOL isOpen=TRUE;
		CString defaultDir = sjx::GetModuleFolder(); 
		CString fileN = "";

		const int nMaxFiles = 300;
		const int nMaxPathBuffer = (nMaxFiles*(MAX_PATH+1))+1;
		LPTSTR pc = (LPTSTR)malloc(nMaxPathBuffer*sizeof(TCHAR));

		CString filter;
		filter = "�����ļ�(*.config )|*.config||";
		CStringArray filePath;
		if (pc)
		{
			CFileDialog openFileDlg(isOpen,defaultDir,fileN,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_ALLOWMULTISELECT,filter);
			openFileDlg.GetOFN().lpstrFile = pc;
			openFileDlg.GetOFN().lpstrFile[0] = NULL;
			openFileDlg.GetOFN().nMaxFile = nMaxPathBuffer;

			openFileDlg.GetOFN().lpstrInitialDir = defaultDir;
			openFileDlg.GetOFN().lpstrTitle = "��ѡ�������ļ�";
			INT_PTR resu = openFileDlg.DoModal();
			if (IDOK == resu)
			{
				zFileName =  openFileDlg.GetOFN().lpstrFile;
				return true;
			}
			return false;
		}

		return false;
	};

	 
	///////////////////////////////////////////////////////
	CString strFName;
	CStdioFile f1,f2,f3;
	CStringArray strArr1,strArr2;
	if (f_OpenFile(strFName))
	{
		if (f3.Open(strFName,CFile::modeRead))
		{
			CString strTmp;
			if (f3.ReadString(strTmp))
			{
				if (0!=strTmp.CompareNoCase("----PKPM.INI----"))
				{
					MessageBox("�޷��������ļ�. ");
					return;
				}

				bool bcontinue=true;
				while(bcontinue)
				{
					if(f3.ReadString(strTmp))
					{
						if (0==strTmp.CompareNoCase("----CLICFG6.INI----"))
						{ 
							bcontinue=false;
							break;;
						}
						strArr1.Add(strTmp);
						 
					}
				}

				bcontinue=true;
				while(bcontinue)
				{
					if(f3.ReadString(strTmp))
					{
						if (0==strTmp.CompareNoCase("----END----"))
						{ 
							bcontinue=false;
							break;;
						}
						strArr2.Add(strTmp);
					}
				}

				CString CfgDir = sjx::GetModuleFolder()+"CFG\\"; 
				bool b1 = f1.Open(CfgDir+"PKPM.INI",CFile::modeCreate|CFile::modeWrite);
				if (b1)
				{
					for (int i=0;i<strArr1.GetSize();i++)
					{
						f1.WriteString(strArr1[i]+"\n");
					}
					f1.Close();
				}
				

				bool b2 = f2.Open(CfgDir+"Clicfg6.ini",CFile::modeCreate|CFile::modeWrite);
				if (b2)
				{
					for (int i=0;i<strArr2.GetSize();i++)
					{
						f2.WriteString(strArr2[i]+"\n");
					}
					f2.Close();
				}
				

				if (b1==true&&b2==true)
				{
					MessageBox("����ɹ�! ��������������");

					EndDialog(CONFIG_SHEET_LOAD_IN);//
				}
				else
				{
					MessageBox("����ʧ�� ");

					EndDialog(IDCANCEL);
				}
			}
			
		}
		else
		{
			MessageBox("�޷��������ļ�. ");
		}
	}
	



}
	 

