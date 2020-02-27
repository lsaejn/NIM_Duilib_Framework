#include "pch.h"
#include "svr.h"
#include "resource.h"
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "pkpm2014svr.h"

bool FixPathStr(CString &strPath)
{
	strPath.Trim();//清理多与字符串
	if (true == strPath.IsEmpty())
	{
		return false;
	}

	if ((strPath.GetLength() -1) != strPath.ReverseFind(_T('\\')))
	{
		strPath += _T('\\');
	}

	return true;
}

bool svr::CreateBmpBrush( UINT idBmp,UINT height,CBrush&br )
{
	ASSERT(br.GetSafeHandle()==0);
	CBitmap bmp;
	BOOL rt=bmp.LoadBitmap(idBmp);
	ASSERT(rt);
	if(!rt)return false;
	BITMAP info;
	bmp.GetBitmap(&info);
	HDC hDC=GetDC(NULL);
	CDC*pDC=CDC::FromHandle(hDC);
	CDC mdc1;
	mdc1.CreateCompatibleDC(pDC);
	mdc1.SelectObject(bmp);
	CDC mdc2;
	mdc2.CreateCompatibleDC(pDC);
	CBitmap bmp2;
	int cx=info.bmWidth;
	int cy=height;
	bmp2.CreateCompatibleBitmap(pDC,cx,cy);
	CBitmap*pBmp=mdc2.SelectObject(&bmp2);
	mdc2.StretchBlt(0,0,cx,cy,&mdc1,0,0,info.bmWidth,info.bmHeight,SRCCOPY);
	mdc2.SelectObject(pBmp);
	br.CreatePatternBrush(&bmp2);
	::ReleaseDC(NULL,hDC);
	return true;
}
//extern bool FixPathStr(CString &strPath);
extern CString get_cfg_path_reg_key();
CString svr::GetRegCFGPath()
{
	CString strCFG;
	strCFG.Empty();
	GetAssignCFGPath(strCFG, false);
	return strCFG;
}

bool svr::GetAssignCFGPath(CString& strCFG,bool bMarchine)
{
	TCHAR path[256];
	GetModuleFileName(NULL, path, 256);
	PathRemoveFileSpec(path);
	strCFG = path;
	strCFG += "\\CFG\\";
	return strCFG.IsEmpty()?false:true;
}
typedef   BOOL   (WINAPI   *SHCreateDirectory2)   (HWND   hwnd, LPCWSTR   pszPath);
bool svr::CreateDirtory(const char*path)
{
	HMODULE   hmodShell   =   LoadLibraryA("shell32.dll");
	if(hmodShell)
	{
		BOOL ok=false;
		SHCreateDirectory2   shcd   =   (SHCreateDirectory2)GetProcAddress(hmodShell,   "SHCreateDirectory");
		if(shcd)
		{
			wchar_t wpath[MAX_PATH];
			int num=::MultiByteToWideChar( CP_ACP, 0, path, MAX_PATH,  wpath,MAX_PATH );
			ok=shcd(NULL,wpath);
		}
		FreeLibrary(hmodShell);     
		return (ok==ERROR_SUCCESS)?true:false ;
	}
	return false;

}
bool svr::SaveWorkPathToFile_CFGPATH(const CString&path)
{
	try{
	ASSERT(!path.IsEmpty());
	CString file_path=svr::GetRegCFGPath();
	ASSERT(!file_path.IsEmpty());
	if(file_path.IsEmpty())return false;
	file_path+=_T("CFGPATH");
	CStdioFile file;
	std::vector<CString>arrItems;
	if(file.Open(file_path,CFile::modeRead))
	{
		CString str;
		for(;;)
		{
			BOOL rt=file.ReadString(str);
			if(rt)
			{
				str.TrimLeft();
				arrItems.push_back(str);
			}
			else
			{
				break;
			}
		}
		file.Close();
	}
	else
	{
		return false;
	}
	//
	bool ok=false;
	int num=arrItems.size();
	for(int i=0;i<num;++i)
	{
		if(arrItems[i]=="WORK")
		{
			if(i+1<num)
			{
				arrItems[i+1]=path;
				ok=true;
				break;
			}
		}
	}
	if(!ok)
	{
		arrItems.push_back(_T("WORK"));
		CString ss=path;
		//ss+="\\";
		FixPathStr(ss);
		arrItems.push_back(ss);
	}
	//
	CStdioFile file2;
	if(file2.Open(file_path,CFile::modeWrite|CFile::modeCreate))
	{
		int num=arrItems.size();
		for(int i=0;i<num;++i)
		{
			file2.WriteString(arrItems[i]);
			file2.WriteString(_T("\n"));
		}
		file2.Close();
	}
	else
	{
		return false;
	}
	return true;
	}catch(...)
	{
	}
	return false;
}
void svr::gradient_rect(HDC hDC,const CRect&rc,COLORREF color_top,COLORREF color_bottom)
{
	TRIVERTEX vts[2];
	vts[0].x=rc.left;
	vts[0].y=rc.top;
	vts[0].Red=GetRValue(color_top)*0xFF;
	vts[0].Green=GetGValue(color_top)*0xFF;
	vts[0].Blue=GetBValue(color_top)*0xFF;
	vts[0].Alpha=0;
	//
	vts[1].x=rc.right;
	vts[1].y=rc.bottom;
	vts[1].Red=GetRValue(color_bottom)*0xFF;
	vts[1].Green=GetGValue(color_bottom)*0xFF;
	vts[1].Blue=GetBValue(color_bottom)*0xFF;
	vts[1].Alpha=0;
	GRADIENT_RECT gr;
	gr.UpperLeft=0;
	gr.LowerRight=1;
	GradientFill(hDC,vts,2,&gr,1,GRADIENT_FILL_RECT_V);
}

void svr::gradient_rect_H(HDC hDC,const CRect&rc,COLORREF color_top,COLORREF color_bottom)
{
	TRIVERTEX vts[2];
	vts[0].x=rc.left;
	vts[0].y=rc.top;
	vts[0].Red=GetRValue(color_top)*0xFF;
	vts[0].Green=GetGValue(color_top)*0xFF;
	vts[0].Blue=GetBValue(color_top)*0xFF;
	vts[0].Alpha=0;
	//
	vts[1].x=rc.right;
	vts[1].y=rc.bottom;
	vts[1].Red=GetRValue(color_bottom)*0xFF;
	vts[1].Green=GetGValue(color_bottom)*0xFF;
	vts[1].Blue=GetBValue(color_bottom)*0xFF;
	vts[1].Alpha=0;
	GRADIENT_RECT gr;
	gr.UpperLeft=0;
	gr.LowerRight=1;
	GradientFill(hDC,vts,2,&gr,1,GRADIENT_FILL_RECT_H);
}

bool svr::getModulePath(CArray<MODULE_PATH>& modPaths)
{
	CString path=svr::GetRegCFGPath();
	if (path.IsEmpty())
	{
		AfxMessageBox(_T("Can not find PKPM Register Infomation"),MB_OK|MB_ICONWARNING);
		return false;
	}

	path+=_T("CFGPATH");


	CFile file;
	try
	{
		if(file.Open(path,CFile::modeRead))
		{
			int len=(int)file.GetLength();
			BYTE*buf=new BYTE[len];
			int nn=file.Read(buf,len);
			ASSERT(nn==len);
			parse_module_path(buf,  modPaths);
			delete buf;
			file.Close();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		return false;
	}
	catch (CFileException* e)
	{
		e->ReportError();
		return false;
	}
	catch (CException* e)
	{
		e->ReportError();
		return false;
	}


	return true;


}

void svr::parse_module_path( const BYTE*buf,CArray<MODULE_PATH>& modPaths )
{
	std::vector<CString>arr_str;
	char ss[320];
	char*s=(char*)buf;
	int i=0;
	while(*s)
	{
		if(*s=='\n')
		{
			ss[i]=0;
			CString s1(ss);
			s1.TrimLeft();
			s1.TrimRight();
			arr_str.push_back(s1);
			i=0;
			++s;
		}
		else
		{
			ss[i++]=*s;
			++s;
		}
	}
	int num=arr_str.size();
	// 	ASSERT(num%2==0);
	num=num/2*2;
	for(i=0;i<num;i+=2)
	{
		MODULE_PATH mp;
		mp.m_name=arr_str[i];
		mp.m_path=arr_str[i+1];
		if (0 == mp.m_name.CompareNoCase(_T("WORK"))
			||0 == mp.m_name.CompareNoCase(_T("CODE"))
			||0 == mp.m_name.CompareNoCase(_T("PAGE")))
		{
			continue;
		}
		
		CString lastChar =CString()+ mp.m_path.GetAt(mp.m_path.GetLength()-1);
		if (0 != lastChar.CompareNoCase(_T("\\")))
		{
			mp.m_path+=_T("\\");
		}
		modPaths.Add(mp);
	}
}
bool svr::isXpSerial()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osv);
	if (5 == osv.dwMajorVersion )
	{
		return true;
	}

	return false;
}
bool svr::isWin7()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osv);
	if (6 == osv.dwMajorVersion&&osv.dwMinorVersion==1)
	{
		return true;
	}

	return false;
}
bool svr::isWin8()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osv);
	if (6 == osv.dwMajorVersion&&osv.dwMinorVersion==2)
	{
		return true;
	}

	return false;
}
bool svr::isWin8dot1()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osv);
	if (6 == osv.dwMajorVersion&&osv.dwMinorVersion==3)
	{
		return true;
	}

	return false;
}

bool svr::getPathByMaker(CString maker,CString& path)
{
	CArray<MODULE_PATH> modPaths;
	CString strPat;
	bool bFind =false;
	if(true == svr::getModulePath(modPaths))
	{
		for (int i=0;i<modPaths.GetSize();i++)
		{
			if(0 == modPaths[i].m_name.CompareNoCase(maker))
			{
				strPat = modPaths[i].m_path;
				toolsvr::FixPathStr(strPat);
				path = strPat;
				bFind=true;
				break;
			}
		} 
	}
	return bFind;
}
