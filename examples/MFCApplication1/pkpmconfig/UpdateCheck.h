#pragma once


// CUpdateCheck �Ի���

class CUpdateCheck : public CPropertyPage
{
	DECLARE_DYNAMIC(CUpdateCheck)

public:
	CUpdateCheck();
	virtual ~CUpdateCheck();

// �Ի�������
	enum { IDD = IDD_CONFIG_UPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_ifupdate;
	int m_updateinterval;
	virtual void OnOK();
	virtual BOOL OnInitDialog();
//	afx_msg void OnBnClickedCheck1();
//	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedCheckupdate();
	afx_msg void OnEnChangeEditupdate();
};
