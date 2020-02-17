// DlgResize.cpp : implementation file
//

#include "stdafx.h"
#include "DlgResize.h"


/////////////////////////////////////////////////////////////////////////////
// DlgResize dialog


DlgResize::DlgResize(UINT id,CWnd* pParent /*=NULL*/)
	: CDialog(id, pParent)
{
	//{{AFX_DATA_INIT(DlgResize)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInit=FALSE;
	m_bLockMinSize=TRUE;
}


void DlgResize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgResize)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgResize, CDialog)
	//{{AFX_MSG_MAP(DlgResize)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_NCLBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgResize message handlers

void DlgResize::SetDlgItemSZInfo(UINT id, DWORD flags)
{
	ASSERT(GetDlgItem(id));
	dlg_item item(id,flags);
	CRect rc;
	GetClientRect(rc);
	GetDlgItem(id)->GetWindowRect(item.m_rect);
	ScreenToClient(item.m_rect);
	item.m_szBorder.cx=rc.right-item.m_rect.right;
	item.m_szBorder.cy=rc.bottom-item.m_rect.bottom;
	m_sz_items.push_back(item);
}
bool DlgResize::FixDlgItemRect(UINT id)
{
	ASSERT(GetDlgItem(id));

	bool bfind = false;
	int nIdx = -1;
	for (int i=0;i<m_sz_items.size();i++)
	{
		if(m_sz_items[i].m_id == id)
		{
			bfind = true;
			nIdx= i;
			break;
		}
	}
	dlg_item& item =  m_sz_items[nIdx];

	CRect rc;
	GetClientRect(rc);
	GetDlgItem(id)->GetWindowRect(item.m_rect);
	ScreenToClient(item.m_rect);
	item.m_szBorder.cx=rc.right-item.m_rect.right;
	item.m_szBorder.cy=rc.bottom-item.m_rect.bottom;
	//m_sz_items.push_back(item);

	return bfind;

}
BOOL DlgResize::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetWindowRect(m_rcDlg);	
	m_bInit=TRUE;
	m_bExpandWin=TRUE;
	m_bWinFree=TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DlgResize::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	if(m_bWinFree && m_bLockMinSize)
	{
		CSize m_szWnd=m_rcDlg.Size();
		if(lpwndpos[0].cx<m_szWnd.cx)lpwndpos[0].cx=m_szWnd.cx;
		if(lpwndpos[0].cy<m_szWnd.cy)lpwndpos[0].cy=m_szWnd.cy;
	}
	CDialog::OnWindowPosChanging(lpwndpos);
}

void DlgResize::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if(m_bWinFree)
	{
		adjust_ctrl_pos();
 		m_bExpandWin=TRUE;
	}
}

void DlgResize::adjust_ctrl_pos()
{
	if(!m_bInit)
	{
//		AfxMessageBox("没有调用DlgResize::OnInitDialog",MB_ICONSTOP);
		GetWindowRect(m_rcDlg);	
		m_bInit=TRUE;
	}
	CRect rc;
	GetClientRect(rc);
	CRect rc1;
	for(int i=0;i<m_sz_items.size();++i)
	{
		dlg_item&ctrl=m_sz_items[i];
		if(GetDlgItem(ctrl.m_id)==NULL)continue;
		rc1=ctrl.m_rect;
		if(ctrl.m_flags&pos_right)
		{
			rc1.right=rc.right-ctrl.m_szBorder.cx;
			rc1.left=rc1.right-ctrl.m_rect.Width();
		}
		if(ctrl.m_flags&pos_bottom)
		{
			rc1.bottom=rc.bottom-ctrl.m_szBorder.cy;
			rc1.top=rc1.bottom-ctrl.m_rect.Height();
		}
		if(ctrl.m_flags&sz_right)
		{
			rc1.right=rc.right-ctrl.m_szBorder.cx;
		}
		if(ctrl.m_flags&sz_bottom)
		{
			rc1.bottom=rc.bottom-ctrl.m_szBorder.cy;
		}
		GetDlgItem(ctrl.m_id)->MoveWindow(rc1);
	}
	RedrawWindow();
}


void DlgResize::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	CDialog::OnNcLButtonDblClk(nHitTest, point);
	if(nHitTest==HTCAPTION)
	{
		m_bWinFree=FALSE;
		if(m_bExpandWin)
		{
 			GetWindowRect(m_rectWin);
// 			CRect rc3;
// 			GetClientRect(rc3);
// 			ClientToScreen(rc3);
			int hc=GetSystemMetrics(SM_CYSMCAPTION);
// 			int hh=rc3.top-m_rectWin.top;
			CRect rc2=m_rectWin;
 			rc2.bottom=rc2.top+hc;
// 			::MoveWindow(GetSafeHwnd(),rc2.left,rc2.top,rc2.Width(),rc2.Height(),true);
 			::SetWindowPos(GetSafeHwnd(),HWND_TOP,rc2.left,rc2.top,rc2.Width(),rc2.Height(),SWP_NOZORDER|SWP_SHOWWINDOW );
		}
		else
		{
			CRect rc2=m_rectWin;
			::SetWindowPos(GetSafeHwnd(),HWND_TOP,rc2.left,rc2.top,rc2.Width(),rc2.Height(),SWP_NOZORDER|SWP_SHOWWINDOW );
		}
		m_bExpandWin=!m_bExpandWin;
		m_bWinFree=TRUE;
	}
}
