#pragma once
#include "afxwin.h"
#include "../Resource.h"

// Ceditkey �Ի���

class Ceditkey : public CDialog
{
	DECLARE_DYNAMIC(Ceditkey)

public:
	Ceditkey(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Ceditkey();
	CString m_strcaption;
// �Ի�������
	enum { IDD = IDD_CONFIG_DLGLOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_keyvalue;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CEdit m_edit;
	afx_msg void OnBnClickedOk();
	CString m_comment;
};
