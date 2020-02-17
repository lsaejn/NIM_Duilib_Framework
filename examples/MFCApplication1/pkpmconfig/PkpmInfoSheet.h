#pragma once

#include "GjColorPropertyPage.h"
#include "Graphicprop.h"
#include "SetPMPara.h"
//#include "UpdateCheck.h"
#include "SetOthers.h"
//#include "lockconfigDlg.h"
//#include "ConfigPanelSeq.h"
#include "../ConfigPwe.h"//这个好像也没啥用0.0
// CPkpmInfoSheet

class menu_data
{

};

class CPkpmInfoSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPkpmInfoSheet)

public:
	CPkpmInfoSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPkpmInfoSheet(LPCTSTR pszCaption,menu_data* pData,bool bHasPwe,UINT iSelectPage);

	virtual ~CPkpmInfoSheet();
	CGjColorPropertyPage m_colorpage;
	CGraphicprop m_graghpage;
	CSetPMPara m_pmctrlpage;
	//CUpdateCheck m_updatepage;
	CSetOthers m_otherspage;
	//ClockconfigDlg m_lockPage;
	//CConfigPanelSeq m_SeqPage;
	CConfigPwe m_Pwe;

	CButton m_WriteOut;
	CButton m_LoadIn;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void OnBnClickedBtnWriteOut();
	void OnBnClickedBtnLoadIn();
};


