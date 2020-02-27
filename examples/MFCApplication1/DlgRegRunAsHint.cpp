// DlgRegRunAsHint.cpp : 实现文件
//

#include "pch.h"
#include "DlgRegRunAsHint.h"


// CDlgRegRunAsHint 对话框

IMPLEMENT_DYNAMIC(CDlgRegRunAsHint, CDialogEx)

CDlgRegRunAsHint::CDlgRegRunAsHint(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRegRunAsHint::IDD, pParent)
{

}

CDlgRegRunAsHint::~CDlgRegRunAsHint()
{
}

void CDlgRegRunAsHint::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRegRunAsHint, CDialogEx)
END_MESSAGE_MAP()


// CDlgRegRunAsHint 消息处理程序
