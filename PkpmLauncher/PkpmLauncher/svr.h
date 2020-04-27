#pragma once


//旧代码，大量MFC代码，要抽空干掉。
struct MODULE_PATH //在主对话框中有一个类似的  这个是公用的
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
