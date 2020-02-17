
// lockconfigDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../Resource.h"

// ClockconfigDlg 对话框
class ClockconfigDlg : public CPropertyPage
{
protected:
	enum
	{
		AUTHORIZE_NULL = -1,
		AUTHORIZE_PHYSICAL = 0,
		AUTHORIZE_CODE = 1,
		AUTHORIZE_AUTO = 2

	};
// 构造
public:
	ClockconfigDlg();	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CONFIG_LOCKCONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	CStringArray keys;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg HCURSOR OnQueryDragIcon();
	
public:
	CString m_filename;
	CListCtrl m_locklist;
	afx_msg void OnBnClickedChange();
	void updateList();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedMoveup();
	afx_msg void OnBnClickedMovedown();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedOk();
	void regpathini();
	virtual void OnOK();
	afx_msg void OnEnChangeTimeout();
	int m_timeout;
	CString m_lockip;
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnCbnSelchangeCombo1();
	int m_seachflag;
	void SetBtnStatusBySel();
	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadAuthorizePhysical();
	afx_msg void OnBnClickedRadAuthorizeCode();
	afx_msg void OnBnClickedRadAuthorizeAuto();
	afx_msg void OnBnClickedBtnAuthorizeCode();
	DECLARE_MESSAGE_MAP()

protected:
	void AuthorizeTypeFromeIni(bool bUpdate= true);
	void PhysicalCtrlStatusFromIni(bool bUpdate= true);
	void DisablePhysicalCtrlStatus();
	void EablePhysicalCtrlStatus(bool bEnable = true);
	void EnableBtnCodeCtrlStatus(bool bEanble = true);
public:
	int m_nAuthorizeType;
	afx_msg void OnBnClickedBtnLockManager();
	void EnableBtnLockMgCtrlStatus(bool bEanble );
	afx_msg void OnBnClickedLockInfo();
	CProgressCtrl m_Pro1;
};
