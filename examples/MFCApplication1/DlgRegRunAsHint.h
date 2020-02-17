#pragma once
#include "resource.h"

// CDlgRegRunAsHint 对话框

class CDlgRegRunAsHint : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRegRunAsHint)

public:
	CDlgRegRunAsHint(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRegRunAsHint();

// 对话框数据
	enum { IDD = IDD_DLGREGRUNASHINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
