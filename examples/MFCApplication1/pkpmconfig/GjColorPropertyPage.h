#pragma once
#include "pgcell.h"
#include "afxwin.h"
#include "../Resource.h"
// CGjColorPropertyPage 对话框

class CGjColorPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGjColorPropertyPage)

public:
	CGjColorPropertyPage();
	virtual ~CGjColorPropertyPage();
	void GetGjColor();
	void GetDefautColor();
	void SetGjColor();
	void InitIDName();
	// 对话框数据
	enum { IDD = IDD_CONFIG_GJCOLORDIALOG };
	int m_color[50];
	int m_ncolor;
	int m_KPASTE[50];  //GHF 2011-09-26，透明度0-100
	// Dialog Data
	//{{AFX_DATA(CDlgSelColor)
	CPgCell	m_Grid;
	CStringArray m_gjname;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedDefault();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
//	virtual BOOL OnApply();
	DECLARE_EVENTSINK_MAP()
	void OnEditCharPgcellcolorctrl(short nCurRow, short nCurCOl, long nChar, short nPage);
	void OnBeginEditCellPgcellcolorctrl(long lCurRow, long lCurCol, LPCTSTR strCellString, short nPage);
	void OnColorBtnEndSelectPgcellcolorctrl(short nRow, short nCol, long ulBkClr);

	int m_Solution;
	afx_msg void OnCbnSelchangeCombocolor();
	int m_ToolbarColorSchem;
	afx_msg void OnCbnSelchangeCombo1();
};
