// pkpmconfig.h : pkpmconfig DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "../resource.h"		// ������


// CpkpmconfigApp
// �йش���ʵ�ֵ���Ϣ������� pkpmconfig.cpp
//

// class CpkpmconfigApp : public CWinApp
// {
// public:
// 	CpkpmconfigApp();
// 
// // ��д
// public:
// 	virtual BOOL InitInstance();
// 
// 	DECLARE_MESSAGE_MAP()
// };


CString GetColorFileName();
CString GetPKPMFileName();
CString GetClicfg6file();