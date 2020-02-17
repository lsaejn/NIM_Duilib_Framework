
// lockconfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "lockconfigDlg.h"
#include "afxdialogex.h"
#include "Ceditkey.h"
#include "../Resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "pkpmconfig.h"
#include <tchar.h>
#include "io.h"
#include "../svr.h"
#include "NetLockName.h"
// ClockconfigDlg 对话框




ClockconfigDlg::ClockconfigDlg()
	: CPropertyPage(ClockconfigDlg::IDD)
	, m_timeout(1)
	, m_lockip(_T(""))
	, m_seachflag(1)
	, m_nAuthorizeType(0)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClockconfigDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_locklist);
	DDX_Text(pDX, IDC_TIMEOUT, m_timeout);
	DDV_MinMaxInt(pDX, m_timeout, 1, 1000000);
	DDX_Text(pDX, IDC_EDIT2, m_lockip);
	DDX_CBIndex(pDX, ID_COMMBO_SEARCH_FLAG, m_seachflag);
	DDV_MinMaxInt(pDX, m_seachflag, 0, 2);
	DDX_Radio(pDX, IDC_RAD_AUTHORIZE_PHYSICAL, m_nAuthorizeType);
	//DDX_Control(pDX, IDC_PROGRESS_SEARCH, m_Pro1);
}

BEGIN_MESSAGE_MAP(ClockconfigDlg, CPropertyPage)
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHANGE, &ClockconfigDlg::OnBnClickedChange)
	ON_BN_CLICKED(IDC_ADD, &ClockconfigDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_MOVEUP, &ClockconfigDlg::OnBnClickedMoveup)
	ON_BN_CLICKED(IDC_MOVEDOWN, &ClockconfigDlg::OnBnClickedMovedown)
	ON_BN_CLICKED(IDC_DELETE, &ClockconfigDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDOK, &ClockconfigDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_TIMEOUT, &ClockconfigDlg::OnEnChangeTimeout)
	ON_EN_CHANGE(IDC_EDIT2, &ClockconfigDlg::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &ClockconfigDlg::OnEnChangeEdit3)
	ON_CBN_SELCHANGE(ID_COMMBO_SEARCH_FLAG, &ClockconfigDlg::OnCbnSelchangeCombo1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &ClockconfigDlg::OnItemchangedList1)
	ON_BN_CLICKED(IDC_RAD_AUTHORIZE_PHYSICAL, &ClockconfigDlg::OnBnClickedRadAuthorizePhysical)
	ON_BN_CLICKED(IDC_RAD_AUTHORIZE_CODE, &ClockconfigDlg::OnBnClickedRadAuthorizeCode)
	ON_BN_CLICKED(IDC_RAD_AUTHORIZE_AUTO, &ClockconfigDlg::OnBnClickedRadAuthorizeAuto)
	ON_BN_CLICKED(IDC_BTN_AUTHORIZE_CODE, &ClockconfigDlg::OnBnClickedBtnAuthorizeCode)
	ON_BN_CLICKED(IDC_BTN_LOCK_MANAGER, &ClockconfigDlg::OnBnClickedBtnLockManager)
	ON_BN_CLICKED(IDC_LOCK_INFO, &ClockconfigDlg::OnBnClickedLockInfo)
END_MESSAGE_MAP()



extern CString GetClicfg6file();
// ClockconfigDlg 消息处理程序
extern "C" void __stdcall GET_PKPM_REGISTRY_PATH( char pathname[]);
BOOL ClockconfigDlg::OnInitDialog()
{
	__super::OnInitDialog();
	CRect arect;
	m_locklist.GetClientRect(&arect);
	m_locklist.InsertColumn(0,_T("序号"),0,arect.Width()/8.0);
	m_locklist.InsertColumn(1,_T("锁码"),0,arect.Width()*5/8.0);
	m_locklist.InsertColumn(2,_T("备注"),0,arect.Width()*2/8.0);

	m_locklist.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	// 将“关于...”菜单项添加到系统菜单中。
	
	PhysicalCtrlStatusFromIni();//首先获得硬件锁参数

	AuthorizeTypeFromeIni(true);
	if (AUTHORIZE_PHYSICAL == m_nAuthorizeType)
	{
		EnableBtnCodeCtrlStatus(false);
		EnableBtnLockMgCtrlStatus(true);
	}
	else if (AUTHORIZE_CODE == m_nAuthorizeType)
	{
		EablePhysicalCtrlStatus(false);
		EnableBtnCodeCtrlStatus(true);
		EnableBtnLockMgCtrlStatus(false);
	}
	else if (AUTHORIZE_AUTO == m_nAuthorizeType)
	{
		EablePhysicalCtrlStatus(true);
		EnableBtnCodeCtrlStatus(true);
		EnableBtnLockMgCtrlStatus(true);
	}

	

	UpdateData(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR ClockconfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void ClockconfigDlg::OnBnClickedChange()
{
	int k=m_locklist.GetSelectionMark();
	if (k==-1)
	{
		m_locklist.SetFocus();
		return;
	}
	Ceditkey a;


	a.m_strcaption=_T("修改");

	CString strLockId ;
	CString  strLockComment;
	int nSign = keys[k].Find(_T(";"));
	if (nSign>0)
	{
		strLockId =  keys[k].Left(nSign);
		strLockComment = keys[k].Mid(nSign+1);
	}
	else
	{
		strLockId = keys[k];
	}


	a.m_keyvalue=strLockId;
	a.m_comment = strLockComment;
	if (a.DoModal()!=IDOK)
	{
		return;
	}
	if (a.m_keyvalue!="")
	{
		keys.SetAt(k,a.m_keyvalue+_T(";")+a.m_comment);
	}
	updateList();
	m_locklist.SetItemState(k,LVIS_FOCUSED|LVIS_SELECTED|LVIS_ACTIVATING,LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING);
	m_locklist.SetFocus();
	// TODO: 在此添加控件通知处理程序代码
	SetModified(TRUE);
}
void ClockconfigDlg::updateList()
{
	int n=keys.GetSize();
	CString strLockId;
	CString strLockComment;

	m_locklist.DeleteAllItems();
	for (int i=0;i<n;i++)
	{

		int nSign = keys[i].Find(_T(";"));
		if (nSign>0)
		{
			strLockId =  keys[i].Left(nSign);
			strLockComment = keys[i].Mid(nSign+1);
		}
		else
		{
			strLockId = keys[i];
		}
		CString tmp;
		m_locklist.InsertItem(i,"");
		tmp.Format("%d",i+1);
		m_locklist.SetItemText(i,0,tmp);
		m_locklist.SetItemText(i,1,strLockId);
		m_locklist.SetItemText(i,2,strLockComment);
	}
}


void ClockconfigDlg::OnBnClickedAdd()
{
	Ceditkey a;
	a.m_strcaption=_T("添加");
	if (a.DoModal()!=IDOK)
	{
		return;
	}
	if (a.m_keyvalue!="")
	{
		keys.Add(a.m_keyvalue+_T("         ;")+a.m_comment);
	}
	updateList();
	int n=keys.GetSize();
	m_locklist.SetItemState(n-1,LVIS_FOCUSED|LVIS_SELECTED|LVIS_ACTIVATING,LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING);
	m_locklist.SetFocus();
	// TODO: 在此添加控件通知处理程序代码
	SetModified(TRUE);
}


void ClockconfigDlg::OnBnClickedMoveup()
{
	int k=m_locklist.GetSelectionMark();
	if (k==-1 || k==0)
	{
		m_locklist.SetFocus();
		return;
	}
	CString tmp;
	tmp=keys[k-1];
	keys[k-1]=keys[k];
	keys[k]=tmp;
	updateList();
	m_locklist.SetItemState(k-1,LVIS_FOCUSED|LVIS_SELECTED|LVIS_ACTIVATING,LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING);
	m_locklist.SetFocus();
	// TODO: 在此添加控件通知处理程序代码
	SetModified(TRUE);
}


void ClockconfigDlg::OnBnClickedMovedown()
{
	int n=keys.GetSize();
	int k=m_locklist.GetSelectionMark();
	if (k>=n-1 || k==-1)
	{
		m_locklist.SetFocus();
		return;
	}
	CString tmp;
	tmp=keys[k+1];
	keys[k+1]=keys[k];
	keys[k]=tmp;
	updateList();
	m_locklist.SetItemState(k+1,LVIS_FOCUSED|LVIS_SELECTED|LVIS_ACTIVATING,LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING);
	m_locklist.SetFocus();
	// TODO: 在此添加控件通知处理程序代码
	SetModified(TRUE);
}


void ClockconfigDlg::OnBnClickedDelete()
{
	int n=keys.GetSize();
	int k=m_locklist.GetSelectionMark();
	if (k==-1)
	{
		m_locklist.SetFocus();
		return;
	}
	keys.RemoveAt(k);
	updateList();
	if (k+1<=n-1)
	{
		m_locklist.SetItemState(k,LVIS_FOCUSED|LVIS_SELECTED|LVIS_ACTIVATING,LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING);
	}
	else if (k-1>=0)
	{
		m_locklist.SetItemState(k-1,LVIS_FOCUSED|LVIS_SELECTED|LVIS_ACTIVATING,LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING);
	}
	m_locklist.SetFocus();

	// TODO: 在此添加控件通知处理程序代码
	SetModified(TRUE);
}


void ClockconfigDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString tmp;
	int n=keys.GetSize();
	{
		//首先是删除ini文件中的所有的key
		for (int i=1;i<200;i++)
		{
			if (i<10)
			{
				tmp.Format(_T("NetKeyID0%d"),i);
			}
			else
			{
				tmp.Format(_T("NetKeyID%d"),i);
			}
			WritePrivateProfileString("CFG_DEVICE",tmp,NULL,m_filename);
		}
	}
	for (int i=1;i<n+1;i++)
	{
		if (i<10)
		{
			tmp.Format(_T("NetKeyID0%d"),i);
		}
		else
		{
			tmp.Format(_T("NetKeyID%d"),i);
		}
		WritePrivateProfileString("CFG_DEVICE",tmp,keys[i-1].GetBuffer(500),m_filename);
	}
	CString cfg6path=GetClicfg6file();
	tmp.Format("%d",m_timeout);
	::WritePrivateProfileString("Common","Timeout",tmp,cfg6path);
	tmp.Format("%d",m_seachflag);
	::WritePrivateProfileString("Common","SearchFlag",tmp,cfg6path);
	::WritePrivateProfileString("TCPUDP","SearchList",m_lockip,cfg6path);

	CString cfgPkpmIni=GetPKPMFileName();
	CString strIntVal;
	strIntVal.Format(_T("%d"),m_nAuthorizeType);
	::WritePrivateProfileString(_T("CFG_DEVICE"),"AuthorizeType",strIntVal,cfgPkpmIni);
	__super::OnOK();
}

void ClockconfigDlg::regpathini()
{
	char pathname[1024];
	GET_PKPM_REGISTRY_PATH(pathname);
	CString filename(pathname);
	filename+="\\pkpm.ini";
	m_filename=filename;
	keys.RemoveAll();
	CString tmp;
	for (int i=1;i<100;i++)
	{
		if (i<10)
		{
			tmp.Format("NetKeyID0%d",i);
		}
		else
			tmp.Format("NetKeyID%d",i);
		char tmpget[200];
		GetPrivateProfileString("CFG_DEVICE",tmp,"",tmpget,200,filename);
		CString res(tmpget);
		if (res!="")
		{
			keys.Add(res);
		}
	}
	updateList();
}

void ClockconfigDlg::OnOK()
{
	OnBnClickedOk();
	//CPropertyPage::OnOK();
}


void ClockconfigDlg::OnEnChangeTimeout()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	SetModified();
}


void ClockconfigDlg::OnEnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	SetModified();
}


void ClockconfigDlg::OnEnChangeEdit3()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CPropertyPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	SetModified();
}


void ClockconfigDlg::OnCbnSelchangeCombo1()
{
	SetModified();
	UpdateData(TRUE);
	if ( 1== m_seachflag||2 == m_seachflag) //0 自动搜索 1 手动 2 半自动
	{
		GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
	}
	SetModified();
}

void ClockconfigDlg::SetBtnStatusBySel()
{
	POSITION pos = m_locklist.GetFirstSelectedItemPosition(); //获取被选中的行
	int nIdx = -1;
	if (NULL == pos)
	{
		nIdx = -1;
	}
	nIdx= m_locklist.GetNextSelectedItem(pos);

	GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
	if (-1 == nIdx)
	{
		GetDlgItem(IDC_CHANGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVEUP)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVEDOWN)->EnableWindow(FALSE);
		GetDlgItem(IDC_LOCK_INFO)->EnableWindow(TRUE);
	}
	else if (0 == nIdx)
	{
		GetDlgItem(IDC_CHANGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
		GetDlgItem(IDC_MOVEUP)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVEDOWN)->EnableWindow(TRUE);
		GetDlgItem(IDC_LOCK_INFO)->EnableWindow(TRUE);
	}
	else if(m_locklist.GetItemCount()-1 == nIdx)
	{
		GetDlgItem(IDC_CHANGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
		GetDlgItem(IDC_MOVEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_MOVEDOWN)->EnableWindow(FALSE);
		GetDlgItem(IDC_LOCK_INFO)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHANGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
		GetDlgItem(IDC_MOVEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_MOVEDOWN)->EnableWindow(TRUE);
		GetDlgItem(IDC_LOCK_INFO)->EnableWindow(TRUE);
	}

	return;
	
}

void ClockconfigDlg::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	SetBtnStatusBySel();
	*pResult = 0;
}


void ClockconfigDlg::OnBnClickedRadAuthorizePhysical()
{
	UpdateData(TRUE);
	EablePhysicalCtrlStatus(true);
	EnableBtnCodeCtrlStatus(false);
	EnableBtnLockMgCtrlStatus(true);
	SetModified();
}


void ClockconfigDlg::OnBnClickedRadAuthorizeCode()
{
	UpdateData(TRUE);
	EablePhysicalCtrlStatus(false);
	EnableBtnCodeCtrlStatus(true);
	EnableBtnLockMgCtrlStatus(false);
	SetModified();
}


void ClockconfigDlg::OnBnClickedRadAuthorizeAuto()
{
	UpdateData(TRUE);
	EablePhysicalCtrlStatus(true);
	EnableBtnCodeCtrlStatus(true);
	EnableBtnLockMgCtrlStatus(true);
	SetModified();
}
void ClockconfigDlg::OnBnClickedBtnAuthorizeCode()
{
	CString cfgpath;
	GET_PKPM_REGISTRY_PATH(cfgpath.GetBuffer(MAX_PATH));
	cfgpath.ReleaseBuffer();
	CString exePathName = cfgpath + _T("PKPMAuthorize.exe");
	if(-1==_taccess(exePathName,0))
	{
		CString strHint;
		strHint += _T("File belowed is not existed.\n");
		strHint += exePathName;
		MessageBox(strHint,_T("lockconfig"));
		return ;
	}

	
// 	HINSTANCE hinst = ShellExecute(m_hWnd,_T("open"),exePathName,NULL,NULL,SW_SHOWNORMAL);
// 
// 	WaitForSingleObject(hinst,-1);

	TCHAR STRPATH[MAX_PATH];
	ZeroMemory(STRPATH,sizeof(TCHAR)*MAX_PATH);
	_tcscpy_s(STRPATH,MAX_PATH,exePathName.GetBuffer());
	STARTUPINFO info = {0};
	info.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION prinfo;
	CreateProcess(NULL,STRPATH,NULL,NULL,FALSE,0,NULL,NULL,&info,&prinfo);

	WaitForSingleObject(prinfo.hProcess,-1);

 	CloseHandle(prinfo.hThread);
 	CloseHandle(prinfo.hProcess);
	
	SetModified();
}

void ClockconfigDlg::PhysicalCtrlStatusFromIni(bool bUpdate)
{
	regpathini();//从ini文件 b中得到锁列表并更新锁控件

	GetDlgItem(IDC_TIMEOUT)->EnableWindow(TRUE);
	GetDlgItem(IDC_LIST1)->EnableWindow(TRUE);
	CString cfg6path=GetClicfg6file();
	int tmpint=GetPrivateProfileInt("Common","Timeout",-1,cfg6path);

	//超时设置
	m_timeout=1;
	if (tmpint>0)
	{
		m_timeout=tmpint;
	}
	//硬件锁搜索方法
	tmpint=GetPrivateProfileInt("Common","SearchFlag",-1,cfg6path);
	m_seachflag=0;
	if (tmpint>=0)
	{
		m_seachflag=tmpint;
	}
	GetDlgItem(ID_COMMBO_SEARCH_FLAG)->EnableWindow(TRUE);

	GetPrivateProfileString("TCPUDP","SearchList","",m_lockip.GetBuffer(256),256,cfg6path);
	if (m_seachflag==1)
	{
		GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
	}
	//更新控制锁列表的按钮

	

	SetBtnStatusBySel();

	
	if (bUpdate)
	{
		UpdateData(FALSE);
	}

}

void ClockconfigDlg::DisablePhysicalCtrlStatus(  )
{
	GetDlgItem(IDC_TIMEOUT)->EnableWindow(FALSE);
	GetDlgItem(ID_COMMBO_SEARCH_FLAG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIST1)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
	GetDlgItem(IDC_MOVEUP)->EnableWindow(FALSE);
	GetDlgItem(IDC_MOVEDOWN)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOCK_INFO)->EnableWindow(FALSE);
}

void ClockconfigDlg::AuthorizeTypeFromeIni( bool bUpdate/*= true*/ )
{
	CString pathPkpmIni = GetPKPMFileName();

	int authType=GetPrivateProfileInt(_T("CFG_DEVICE"),_T("AuthorizeType"),0,pathPkpmIni);

	m_nAuthorizeType = authType;
	if (bUpdate)
	{
		UpdateData(FALSE);
	}
}

void ClockconfigDlg::EnableBtnCodeCtrlStatus(bool bEanble )
{
	GetDlgItem(IDC_BTN_AUTHORIZE_CODE)->EnableWindow(bEanble?TRUE:FALSE);
}
void ClockconfigDlg::EnableBtnLockMgCtrlStatus(bool bEanble )
{
	GetDlgItem(IDC_BTN_LOCK_MANAGER)->EnableWindow(bEanble?TRUE:FALSE);
}


void ClockconfigDlg::EablePhysicalCtrlStatus(bool bEnable )
{
	if (bEnable)
	{
		PhysicalCtrlStatusFromIni(true);
	}
	else
	{
		DisablePhysicalCtrlStatus();
	}
}


void ClockconfigDlg::OnBnClickedBtnLockManager()
{
	CString cfgpath;
	GET_PKPM_REGISTRY_PATH(cfgpath.GetBuffer(MAX_PATH));
	cfgpath.ReleaseBuffer();
	CString exePathName = cfgpath + _T("用户锁管理程序.exe");
	if(-1==_taccess(exePathName,0))
	{
		CString strHint;
		strHint += _T("File belowed is not existed.\n");
		strHint += exePathName;
		MessageBox(strHint,_T("lockconfig"));
		return ;
	}

	TCHAR STRPATH[MAX_PATH];
	ZeroMemory(STRPATH,sizeof(TCHAR)*MAX_PATH);
	_tcscpy_s(STRPATH,MAX_PATH,exePathName.GetBuffer());
	STARTUPINFO info = {0};
	info.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION prinfo;
	CreateProcess(NULL,STRPATH,NULL,NULL,FALSE,0,NULL,NULL,&info,&prinfo);

	WaitForSingleObject(prinfo.hProcess,-1);

	CloseHandle(prinfo.hThread);
	CloseHandle(prinfo.hProcess);

	SetModified();
}





// extern CString get_cfg_path_reg_key();
// extern "C" void __stdcall RESET_PKPMREGKEY(char* strCfgReg,long n);
// extern "C" void __stdcall GET_KEY_AUTHORIZE_NO(const int &iNo,short &IPSW1,short &IPSW2,short &IPSW3,short &IPSW4);
// extern "C" void __stdcall TESTDG0_DL(short &IK11,short &IK12,short &IK13,short &IK14,int &INTMI);
// extern "C" void __stdcall GET_KEY_MODULE_NAME(const int &iModule,char *strName,long n1);
// extern "C" void __stdcall SET_IF_SHOW_NO_KEY(const int &iShow);
// extern "C" void __stdcall CHECK_AUTHORIZE(short &IK11,short &IK12,short &IK13,short &IK14,int &iRet);
// extern "C" void TESTDG2_NOT_STOP(int &IRet);
// extern "C" void __stdcall TESTDG9();
// extern bool FixPathStr(CString &strPath);
void ClockconfigDlg::OnBnClickedLockInfo()
{
	CNetLockName dlg;
	dlg.DoModal();
// 
// 	CStringArray Names;
// 	CString strDebug;
// 
// 	m_Pro1.ShowWindow(SW_SHOWNORMAL);
// 	m_Pro1.SetRange(0,179);
// 	m_Pro1.SetPos(0);
// 
// 	MSG msg;
// 	while (PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))         
// 	{            
// 		TranslateMessage(&msg);            
// 		DispatchMessage(&msg);         
// 	}
// 
// 	CString strCFGMarker = get_cfg_path_reg_key();
// 	char ccfg[512];
// 	_tcscpy_s(ccfg,512,strCFGMarker);
// 	RESET_PKPMREGKEY(ccfg,strCFGMarker.GetLength()+1);
// 	 
// 	SET_IF_SHOW_NO_KEY(0);
// 
// 	//第一次
// 	int nModule=1;
// 	short K1=0,K2=0,K3=0,K4=0;
// 	GET_KEY_AUTHORIZE_NO(nModule,K1,K2,K3,K4);
// 
// 	bool bGet0=false;
// 	int nRet;
// 	TESTDG0_DL(K1,K2,K3,K4,nRet);
// 	int IRet = 1;
// 	TESTDG2_NOT_STOP(IRet);
// 	
// 
// 	m_Pro1.SetPos(1); 
// 	if (IRet>0)
// 	{
// 		bGet0=true;
// 	}
// 	if (bGet0)
// 	{
// 		char czName[100];
// 		GET_KEY_MODULE_NAME(nModule,czName,100);
// 
// 		CString strNa ;
// 		strNa = czName;
// 		strNa.Trim();
// 		Names.Add(strNa);
// 	}
// 
// 	//循环次
// 	for (int i=2;i<180;i++)
// 	{
// 		nModule = i;
// 		GET_KEY_AUTHORIZE_NO(nModule,K1,K2,K3,K4);
// 		CHECK_AUTHORIZE(K1,K2,K3,K4,nRet);
// 
// 		if (nRet>0)
// 		{
// 			char czName[100];
// 			GET_KEY_MODULE_NAME(nModule,czName,100);
// 			CString strNa ;
// 			strNa = czName;
// 			strNa.Trim();
// 			if (strNa.IsEmpty())
// 			{
// 				continue;
// 			}
// 			Names.Add(strNa);
// 		}
// 
// 
// 		m_Pro1.SetPos(i); 
// 		while (PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))         
// 		{            
// 			TranslateMessage(&msg);            
// 			DispatchMessage(&msg);         
// 		}
// 	}
// 
// 	TESTDG9();
// 
// 
// 
// 	m_Pro1.ShowWindow(SW_HIDE);
// 
// 
// 	CString strLock;
// 	for (int i=0;i<Names.GetSize();i++)
// 	{
// 		strLock+= (" "+ Names[i]+";");
// 	}
// 	if (Names.GetSize()>0)
// 	{
// 		MessageBox(strLock,"查询锁");
// 	}
// 	else
// 	{
// 		MessageBox("未查询到任何锁","查询锁");
// 	}
// 	
}
