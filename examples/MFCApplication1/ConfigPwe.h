#pragma once
#include "afxcmn.h"


// CConfigPwe �Ի���

class CConfigPwe : public CPropertyPage
{
	DECLARE_DYNAMIC(CConfigPwe)

public:
	CConfigPwe(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigPwe();

	// �Ի�������
	enum { IDD = IDD_CONFIG_PWE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_Server;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CString m_IP;
};
