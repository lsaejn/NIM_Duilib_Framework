#pragma once
#include "afxwin.h"
#include "../Resource.h"

// Ceditkey 对话框

class Ceditkey : public CDialog
{
	DECLARE_DYNAMIC(Ceditkey)

public:
	Ceditkey(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Ceditkey();
	CString m_strcaption;
// 对话框数据
	enum { IDD = IDD_CONFIG_DLGLOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_keyvalue;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CEdit m_edit;
	afx_msg void OnBnClickedOk();
	CString m_comment;
};
