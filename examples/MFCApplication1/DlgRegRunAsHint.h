#pragma once
#include "resource.h"

// CDlgRegRunAsHint �Ի���

class CDlgRegRunAsHint : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRegRunAsHint)

public:
	CDlgRegRunAsHint(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRegRunAsHint();

// �Ի�������
	enum { IDD = IDD_DLGREGRUNASHINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
