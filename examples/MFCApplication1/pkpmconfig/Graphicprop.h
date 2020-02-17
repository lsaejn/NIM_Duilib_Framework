#pragma once
#include "afxwin.h"


// CGraphicprop 对话框

class CGraphicprop : public CPropertyPage
{
	DECLARE_DYNAMIC(CGraphicprop)

public:
	CGraphicprop();
	virtual ~CGraphicprop();

// 对话框数据
	enum { IDD = IDD_CONFIG_GRAPHIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_ifsavedimstyle;
	int m_Fontstyle;
	virtual BOOL OnInitDialog();
	bool OnInitVerComb();
	BOOL m_iftriangle;
	int m_nSTS_T2Dwg;
//	afx_msg void OnCbnSelchangeCombo1();
//	afx_msg void OnBnClickedCheck1();
//	afx_msg void OnBnClickedCheck2();
	virtual void OnOK();
	afx_msg void OnCbnSelchangeCombofont();
	afx_msg void OnBnClickedChecksavetdim();
	afx_msg void OnBnClickedChecktriangle();
	
	afx_msg void OnCbnSelchangeCombStsT2dwg();
	int m_nSelDwgVersion;
	CComboBox m_combDwgVer;
	CArray<CString> m_dwgSelOption;
	CArray<CString> m_dwgSelUI;
	afx_msg void OnCbnSelchangeCombDwgVer();
};
