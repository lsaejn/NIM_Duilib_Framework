#pragma once


//�ɴ��룬����MFC���룬Ҫ��ոɵ���
struct MODULE_PATH //�����Ի�������һ�����Ƶ�  ����ǹ��õ�
{
	CString m_name;
	CString m_path;
};

class svr
{
public:
	static bool CreateBmpBrush(UINT idBmp,UINT height,CBrush&br);
	static CString GetRegCFGPath();
	static bool GetAssignCFGPath(CString& strCFG,bool bMarchine);
	static bool CreateDirtory(const char*path);
	static bool SaveWorkPathToFile_CFGPATH(const CString&path);
	static void gradient_rect(HDC hDC,const CRect&rc,COLORREF color_top,COLORREF color_bottom);
	static void gradient_rect_H(HDC hDC,const CRect&rc,COLORREF color_top,COLORREF color_bottom);
	static bool getModulePath(CArray<MODULE_PATH>& modPaths);
	static bool getPathByMaker(CString maker,CString& path);
	static void parse_module_path( const BYTE*buf,CArray<MODULE_PATH>& modPaths );
};
