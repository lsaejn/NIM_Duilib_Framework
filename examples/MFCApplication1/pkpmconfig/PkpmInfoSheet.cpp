// PkpmInfoSheet.cpp : 实现文件
//

#include "../stdafx.h"
#include "pkpmconfig.h"
#include "PkpmInfoSheet.h"
#include "../resource.h"
#include "../CommonUtil.h"
#include "../svr.h"
#include "../PublicMacroHeader.h"


// CPkpmInfoSheet

//chenjian 原先对话框的menu_data已经没有用了
//等待测试通过


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


// CPkpmInfoSheet 消息处理程序


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

		m_WriteOut.Create(_T("导出"),BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,rcOut,this,IDC_BT_WRITE_OUT);
		m_LoadIn.Create(_T("导入"),BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,rcIn,this,IDC_BT_LOAD_IN);

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
		svr::getPathByMaker(_T("CFG"),defaultDir);
		CString fileN  = _T("PkpmV2010");
		BOOL isOpen =FALSE;
		CString filter;
		filter = "配置文件(*.config )|*.config||";
		
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

				strAll.Add(_T("----PKPM.INI----"));
				while(f1.ReadString(strLoop))
				{
					strAll.Add(strLoop);
				}

				strAll.Add(_T("----CLICFG6.INI----"));
				while(f2.ReadString(strLoop))
				{
					strAll.Add(strLoop);
				}
				strAll.Add(_T("----END----"));

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
					MessageBox(_T("无法保存配置文件. "));
					return;
				}
				
			}
			else
			{
				MessageBox(_T("无法打开配置文件 pkpm.ini 和/或 Clicfg6.ini "));
				return;
			}


		}
		else
		{
			MessageBox(_T("无法找到配置文件 pkpm.ini 和/或 Clicfg6.ini.请确认文件存在或重新安装."));
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
		CString fileN = _T("");

		const int nMaxFiles = 300;
		const int nMaxPathBuffer = (nMaxFiles*(MAX_PATH+1))+1;
		LPTSTR pc = (LPTSTR)malloc(nMaxPathBuffer*sizeof(TCHAR));

		CString filter;
		filter = "配置文件(*.config )|*.config||";
		CStringArray filePath;
		if (pc)
		{
			CFileDialog openFileDlg(isOpen,defaultDir,fileN,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_ALLOWMULTISELECT,filter);
			openFileDlg.GetOFN().lpstrFile = pc;
			openFileDlg.GetOFN().lpstrFile[0] = NULL;
			openFileDlg.GetOFN().nMaxFile = nMaxPathBuffer;

			openFileDlg.GetOFN().lpstrInitialDir = defaultDir;
			openFileDlg.GetOFN().lpstrTitle = _T("请选择配置文件");
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
				if (0!=strTmp.CompareNoCase(_T("----PKPM.INI----")))
				{
					MessageBox(_T("无法打开配置文件. "));
					return;
				}

				bool bcontinue=true;
				while(bcontinue)
				{
					if(f3.ReadString(strTmp))
					{
						if (0==strTmp.CompareNoCase(_T("----CLICFG6.INI----")))
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
						if (0==strTmp.CompareNoCase(_T("----END----")))
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
					MessageBox(_T("导入成功! 请重新启动程序"));

					EndDialog(CONFIG_SHEET_LOAD_IN);//
				}
				else
				{
					MessageBox(_T("导入失败 "));

					EndDialog(IDCANCEL);
				}
			}
			
		}
		else
		{
			MessageBox(_T("无法打开配置文件. "));
		}
	}
	



}
	 

