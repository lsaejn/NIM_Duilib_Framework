#if !defined(AFX_DLGSELECTDIR_H__15D5DB63_1354_49CC_8073_EAECA757FFFC__INCLUDED_)
#define AFX_DLGSELECTDIR_H__15D5DB63_1354_49CC_8073_EAECA757FFFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectDir.h : header file
//

#include "DlgResize.h"
#include "ximage.h"
#include "PublicMacroHeader.h"
#include "afxshelltreectrl.h "
#include "afxwin.h"
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// DlgSelectDir dialog

class DlgSelectDir : public DlgResize
{
// Construction
public:
	DlgSelectDir(CWnd* pParent = NULL);   // standard constructor
	void SetInitPath(const CString name)
	{
		m_initPath=name;
	}

	CString m_initPath;
// Dialog Data
	//{{AFX_DATA(DlgSelectDir)
	enum { IDD = IDD_SELECT_DIR };
	CButton	m_bnOk;
	CButton	m_bnCancel;
	CButton	m_bnNewDir;
	CString	m_sCurPath;
	//}}AFX_DATA

	CMFCShellTreeCtrl m_treeAd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgSelectDir)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
private:
	HICON m_iconCorner;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgSelectDir)
	//afx_msg void OnNewDir();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnChangeEdit();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	CFont m_viewfont;
	CxImage m_bmpPreview;
	CSize m_picSize;
	bool m_bLoadPrevew;
	bool m_folderHasPic;
	void OnPaintNoView(CDC* pDC);
	bool DrawPreviewPic(CString strPath,bool bRedraw=true);
public:
	void SetTreeFont();
	CFont m_cbfont;

	
	afx_msg void OnRclickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_editCurPath;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTDIR_H__15D5DB63_1354_49CC_8073_EAECA757FFFC__INCLUDED_)
