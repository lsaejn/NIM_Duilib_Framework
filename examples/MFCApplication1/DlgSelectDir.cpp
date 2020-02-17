// DlgSelectDir.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSelectDir.h"
#include "svr.h"
#include "io.h"
#include "tchar.h"
#include "MultLanguageStringMgr.h"
#include "pkpm2014svr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSelectDir dialog


DlgSelectDir::DlgSelectDir(CWnd* pParent /*=NULL*/)
	: DlgResize(DlgSelectDir::IDD, pParent),
	m_treeAd()
{
	//{{AFX_DATA_INIT(DlgSelectDir)
	m_sCurPath = _T("");
	//}}AFX_DATA_INIT
	m_iconCorner=AfxGetApp()->LoadIcon(IDI_ICON_CORNER);
	ASSERT(m_iconCorner);
}


void DlgSelectDir::DoDataExchange(CDataExchange* pDX)
{
	DlgResize::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgSelectDir)
	DDX_Control(pDX, IDOK, m_bnOk);
	DDX_Control(pDX, IDCANCEL, m_bnCancel);
	DDX_Control(pDX, ID_NEW_DIR, m_bnNewDir);
	//DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Text(pDX, IDC_EDIT, m_sCurPath);
	DDX_Control(pDX, IDC_TREE, m_treeAd);

	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT, m_editCurPath);
}


BEGIN_MESSAGE_MAP(DlgSelectDir, DlgResize)
	//{{AFX_MSG_MAP(DlgSelectDir)
	//ON_BN_CLICKED(ID_NEW_DIR, OnNewDir)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT, &DlgSelectDir::OnChangeEdit)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, &DlgSelectDir::OnRclickTree)
	ON_NOTIFY(NM_CLICK, IDC_TREE, &DlgSelectDir::OnClickTree)
END_MESSAGE_MAP()


BOOL DlgSelectDir::OnInitDialog() 
{
	SetDlgItemSZInfo(IDC_TREE,sz_bottom|sz_right);
	SetDlgItemSZInfo(ID_NEW_DIR,pos_bottom);
	SetDlgItemSZInfo(IDOK,pos_bottom|pos_right);
	SetDlgItemSZInfo(IDCANCEL,pos_bottom|pos_right);
	SetDlgItemSZInfo(IDC_EDIT,pos_bottom|sz_right);
	SetDlgItemSZInfo(IDC_STATIC_folder,pos_bottom);
	SetDlgItemSZInfo(IDC_PIC_VIEW,pos_right);
	SetDlgItemSZInfo(IDC_TXT_PROJ_VIEW,pos_right);


	DlgResize::OnInitDialog();

	

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_NEWPRJ_ICON);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);		// 设置小图标
	//

	if(0 != _taccess_s(m_initPath,0))
	{
		m_initPath = _T("C:\\");
	}

	m_treeAd.SelectPath(m_initPath);

	SetTreeFont();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL DlgSelectDir::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);

	if (pMsg->hwnd == m_editCurPath.m_hWnd && pMsg->message == WM_LBUTTONDBLCLK)
	{
		m_editCurPath.SetSel(0,-1);
		return TRUE;
	}
	else if (pMsg->message==WM_KEYDOWN)
	{
		if (GetKeyState('V')<0||GetKeyState('v')<0)
		{
			if (GetKeyState(VK_CONTROL)<0)
			{
				m_editCurPath.SetSel(0,-1);
				m_editCurPath.Paste();
				return TRUE;
				//return CDialog::PreTranslateMessage(pMsg);	
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
bool DlgSelectDir::DrawPreviewPic(CString strPath,bool bRedraw)
{
	m_folderHasPic;
	CString strName;
	//这里加载不上，不要怪我~~~
	//strName = g_stringMgr.LoadString(IDS_PICVIEW_NAME);
	strName="BuildUp.bmp";
	CString picFileName = strPath + strName;

	m_folderHasPic = m_bmpPreview.Load(picFileName,CXIMAGE_FORMAT_BMP);
	if (m_folderHasPic)
	{
		m_picSize.SetSize(m_bmpPreview.GetWidth(),m_bmpPreview.GetHeight()); 
	}
	CRect rcView;
	GetDlgItem(IDC_PIC_VIEW)->GetWindowRect(&rcView);
	ScreenToClient(&rcView);

	if (bRedraw)
	{
		InvalidateRect(rcView);
	}

	return m_folderHasPic;

}
void DlgSelectDir::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_treeAd.GetItemPath(m_sCurPath);
	if (m_sCurPath.ReverseFind(_T('\\'))!= m_sCurPath.GetLength()-1)
	{
		m_sCurPath += _T("\\");
	}
	//UpdateData(FALSE);
	GetDlgItem(IDC_EDIT)->SetWindowText(m_sCurPath);


	DrawPreviewPic(m_sCurPath);

	*pResult = 0;
}

void DlgSelectDir::OnOK() 
{
	CString strTmp = m_sCurPath;
	UpdateData();
	if(!m_sCurPath.IsEmpty())
	{
		if (0==_taccess_s(m_sCurPath,0))
		{
			DlgResize::OnOK();
		}
		else
		{
			CString strCreateDir = "是否需要创建文件夹";
			if(IDOK != MessageBox(strCreateDir,_T(""),MB_OKCANCEL))
			{
				m_sCurPath = strTmp;
				UpdateData(FALSE);
				m_treeAd.SelectPath(m_sCurPath);
				return ;
			}
			if(svr::CreateDirtory(m_sCurPath))
			{
				DlgResize::OnOK();
			}
			else
			{
				MessageBox("ERROR WORKPATH");

				m_sCurPath = strTmp;
				UpdateData(FALSE);
				m_treeAd.SelectPath(m_sCurPath);
			}
		}
		
	}
	else
	{
		CString str = "ERROR WORKPATH";
		MessageBox(str);
	}
}

void DlgSelectDir::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if(m_iconCorner)
	{
		CRect rc;
		GetClientRect(rc);
		::DrawIconEx(dc.GetSafeHdc(),rc.right-34,rc.bottom-34,m_iconCorner,32,32,0,NULL,DI_NORMAL);
	}

	CRect rcView;
	GetDlgItem(IDC_PIC_VIEW)->GetWindowRect(&rcView);
	ScreenToClient(&rcView);

	double xRat,yRat,rat;
	xRat = 1.0*rcView.Width()/m_picSize.cx;
	yRat = 1.0*rcView.Height()/m_picSize.cy;
	rat = xRat<yRat?xRat:yRat;

	CRect rcDraw;
	rcDraw = rcView;
	rcDraw.right = rcDraw.left + m_picSize.cx * rat;
	rcDraw.bottom = rcDraw.top+ m_picSize.cy * rat;
	rcDraw.OffsetRect((rcView.Width()-rcDraw.Width())/2.0,(rcView.Height()-rcDraw.Height())/2.0);
	if (m_bmpPreview.IsValid()&&m_folderHasPic)
	{
		m_bmpPreview.Draw(dc.GetSafeHdc(),rcDraw);
	}
	else
	{
		OnPaintNoView(&dc);
	}


}

void DlgSelectDir::OnPaintNoView(CDC* pDC)
{
	CRect rcView;
	GetDlgItem(IDC_PIC_VIEW)->GetWindowRect(&rcView);
	ScreenToClient(rcView);

	CBrush br(RGB(0,0,0));
	pDC->FillRect(&rcView,&br);

	CString strNoView;
	strNoView = "无预览";
	
	if (NULL == m_viewfont.GetSafeHandle())
	{
		CString fntName;
		fntName = "微软雅黑";
		int height = 25;
		int weight = 330;

		m_viewfont.CreateFont(height,0,0,0,weight,0,0,0,DEFAULT_CHARSET,OUT_DEVICE_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH|FF_DONTCARE,fntName);
	}

	CFont *oldFont = pDC->SelectObject(&m_viewfont);
	int oldMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldCol = pDC->SetTextColor(RGB(255,255,255));
	CRect rcText = rcView;

	pDC->DrawText(strNoView,&rcText,DT_SINGLELINE|DT_VCENTER|DT_CENTER);


	pDC->SelectObject(oldFont);
	pDC->SetBkMode(oldMode);
	pDC->SetTextColor(oldCol);
}


void DlgSelectDir::SetTreeFont()
{
	if (NULL==m_cbfont.GetSafeHandle())
	{
		CString fntName;
		int height = 12;
		int weight= 350;

		OSVERSIONINFO osv;
		osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osv);
		if (osv.dwMajorVersion<=5)
		{
			fntName = _T("宋体");
			height = 16;
			weight= 300;
		}
		else
		{
			fntName = _T("微软雅黑");
			height = 24;
			weight= 350;
		}

		BOOL rt=m_cbfont.CreateFont(height,0,0,0,weight,0,0,0,DEFAULT_CHARSET,OUT_DEVICE_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH|FF_DONTCARE,fntName);
	}
	SetFont(&m_cbfont,TRUE);
}


void DlgSelectDir::OnChangeEdit()
{
	CString strEdit;
	GetDlgItem(IDC_EDIT)->GetWindowText(strEdit);
	CDC *pDc = GetDC();
	CRect txtRect;
	pDc->DrawText(strEdit,&txtRect,DT_CALCRECT|DT_SINGLELINE);
	
	CRect rc ;
	GetDlgItem(IDC_EDIT)->GetWindowRect(&rc) ;
	ScreenToClient(&rc);

	CRect rcPic;
	GetDlgItem(IDC_PIC_VIEW)->GetWindowRect(&rcPic);
	ScreenToClient(&rcPic);
	if (txtRect.Width() >=rc.Width())
	{
		GetDlgItem(IDC_EDIT)->SetWindowPos(NULL,rc.left,rc.top,rcPic.right-rc.left,rc.Height(),SWP_NOMOVE);
	}

	FixDlgItemRect(IDC_EDIT);
}


void DlgSelectDir::OnRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void DlgSelectDir::OnClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_treeAd.GetItemPath(m_sCurPath);
	if (m_sCurPath.ReverseFind(_T('\\'))!= m_sCurPath.GetLength()-1)
	{
		m_sCurPath += _T("\\");
	}
	UpdateData(FALSE);


	DrawPreviewPic(m_sCurPath);

	*pResult = 0;

	 

}


