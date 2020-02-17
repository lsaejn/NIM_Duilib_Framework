#if !defined(AFX_DLGRESIZE_H__1DF8FE44_5884_45B7_8946_18CA305C6618__INCLUDED_)
#define AFX_DLGRESIZE_H__1DF8FE44_5884_45B7_8946_18CA305C6618__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgResize.h : header file
//
#include <vector>
/////////////////////////////////////////////////////////////////////////////
// DlgResize dialog

class DlgResize : public CDialog
{
public:
	enum xxx_op
	{
		sz_right   =1<<0,
		sz_bottom  =1<<1,
		pos_right  =1<<2,
		pos_bottom =1<<3,
	};
private:
	void adjust_ctrl_pos();
private:
	struct dlg_item
	{
		DWORD  m_flags;
		DWORD  m_id;
		CRect  m_rect;
		CSize  m_szBorder;
		dlg_item(UINT id,DWORD flags)
		{
			m_id=id;m_flags=flags;
		}
	};
	std::vector<dlg_item> m_sz_items;
	CRect                 m_rcDlg;
	BOOL                  m_bInit;
	BOOL                  m_bExpandWin;
	CRect                 m_rectWin;
	BOOL                  m_bWinFree;
	BOOL                  m_bLockMinSize;
	// Construction
public:
	void SetDlgItemSZInfo(UINT id,DWORD flags);
	DlgResize(UINT id,CWnd* pParent = NULL);   // standard constructor
	bool FixDlgItemRect(UINT id);
	// Dialog Data
	//{{AFX_DATA(DlgResize)
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgResize)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgResize)
	virtual BOOL OnInitDialog();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRESIZE_H__1DF8FE44_5884_45B7_8946_18CA305C6618__INCLUDED_)
