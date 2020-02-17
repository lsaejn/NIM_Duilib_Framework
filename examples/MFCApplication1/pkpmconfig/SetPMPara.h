#pragma once


// CSetPMPara �Ի���

class CSetPMPara : public CPropertyPage
{
	DECLARE_DYNAMIC(CSetPMPara)

public:
	CSetPMPara();
	virtual ~CSetPMPara();

// �Ի�������
	enum { IDD = IDD_CONFIG_PMCTRL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_earthgroup;
	int m_earthintense;
	int m_groudlevel;
	BOOL m_ifchange02to10;
	double m_windpressure;
	int m_alldiameter;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCbnSelchangeComboearthgroup();
	afx_msg void OnCbnSelchangeComboearthintense();
	afx_msg void OnCbnSelchangeCombogroundlevel();
	afx_msg void OnEnChangeEditwindpress();
	int m_maxcornerdiameter;
	afx_msg void OnCbnSelchangeComboDiameter();
};
