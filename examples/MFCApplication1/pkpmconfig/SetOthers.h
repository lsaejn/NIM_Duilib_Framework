#pragma once


// CSetOthers �Ի���

class CSetOthers : public CPropertyPage
{
	DECLARE_DYNAMIC(CSetOthers)

public:
	CSetOthers();
	virtual ~CSetOthers();

// �Ի�������
	enum { IDD = IDD_CONFIG_OTHERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_ifchange02to10;
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheck02to10();
	BOOL m_ifPMCADopenSTS;
	afx_msg void OnBnClickedCheck1();
	BOOL m_ifShowFullPath;
	afx_msg void OnBnClickedCheckBtnShowfullpath();

	void SetPageType(int tp) {m_PageType = tp;}
private:
	int m_PageType;
public:
	int m_nSelF1;
};
