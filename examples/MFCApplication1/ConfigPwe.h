#pragma once
#include "afxcmn.h"


// CConfigPwe 对话框

class CConfigPwe : public CPropertyPage
{
	DECLARE_DYNAMIC(CConfigPwe)

public:
	CConfigPwe(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfigPwe();

	// 对话框数据
	enum { IDD = IDD_CONFIG_PWE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_Server;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CString m_IP;
};
