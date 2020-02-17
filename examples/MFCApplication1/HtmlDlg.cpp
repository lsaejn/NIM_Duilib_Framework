// HtmlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HtmlDlg.h"
#include "AppDllAdaptor.h"
#include  "svr.h"
#include "pkpm2014svr.h"
#include "MultLanguageStringMgr.h"
#include "string_utility.h"
#include "BoundedQueue.h"
#include "DlgRegRunAsHint.h"
#include "pkpmconfig/PkpmInfoSheet.h"
#include "ProcessInfo.h"
#include "HttpUtil.h"

#include <stdlib.h>
#include <tchar.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unordered_map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>
#include <cstdio>
#include <functional>
#include <regex>

#include "xml2json.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/memorystream.h"

#ifdef OLE2T
#undef OLE2T
#define OLE2T(x) string_utility::WStringToString(x).c_str()
#endif

using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::Writer;
using namespace rapidjson;
using namespace string_utility;


CString cfg_key_str;
static const CString INI_APP_DLGSTYLE = _T("DLGSTYLE");
static const CString INI_DLGSTYLE_LANG = _T("LANG");
	const int LENGTH_OF_ENV=1024*8;
LPCTSTR UrLOnlineAsk= _T("https://pkpm.udesk.cn/im_client/?web_plugin_id=47258");

const char* separator="\\";
const char* toRead[] = { "navbarIndex", "parentIndex", "childrenIndex","projectIndex" };
const char* UpdateUrl="https://www.pkpm.cn/index.php?m=content&c=index&a=show&catid=70&id=112";
const char* urlAboutPkpm="https://www.pkpm.cn";
const wchar_t serverIp[128]=L"111.230.143.87";
const char* defaultAdvertise="{data:[ \
{\"key\":\"官方网站: www.pkpm.cn\", \"value\":\"www.pkpm.cn\"}, "
"{\"key\":\"官方论坛: www.pkpm.cn/bbs\",\"value\":\"www.pkpm.cn/bbs\"}"
"]}";

std::string GetExePath()
{
	CString strExeFileName;
	GetModuleFileName(NULL, strExeFileName.GetBuffer(MAX_PATH), MAX_PATH);//本程序就是exe
	strExeFileName.ReleaseBuffer(-1);
	int nsp = strExeFileName.ReverseFind('\\');
	return strExeFileName.Left(nsp).GetBuffer();
}

CString FullPathOfPkpmIni()
{
	static CString re=GetAppPath() + "cfg/pkpm.ini";
	return re;
}

CString get_cfg_path_reg_key()
{
	return cfg_key_str;
}

FP_INITPKPMAPP fuc_InitPkpmApp=NULL;
FP_RUNCOMMAND  fuc_RunCommand=NULL;
FP_PKPMCONFIG fuc_PkpmConfig=NULL;
FP_GETMENUTXT fuc_GetMenu = NULL;
FP_GENCOMPFILE fuc_GenCompFile = NULL;
FP_ADJUSTPROORDER fuc_AdjustProOrder= NULL;
FP_ISPROJECTWISEVAILED fuc_IsProjectWiseVailed = NULL;
// CHtmlDlg 对话框

IMPLEMENT_DYNCREATE(CHtmlDlg, CDHtmlDialog)

CHtmlDlg::CHtmlDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(IDD_HTMLDLG),
	miniCx_(GetPrivateProfileInt(_T("Html"), _T("Default_cx"), UINT_MAX, FullPathOfPkpmIni())),
	miniCy_(GetPrivateProfileInt(_T("Html"), _T("Default_cy"), UINT_MAX, FullPathOfPkpmIni())),
	maxPrjNum(GetPrivateProfileInt(_T("WorkPath"), _T("MaxPathName"), UINT_MAX, FullPathOfPkpmIni())),
	prjPaths_(maxPrjNum),
	documentLoaded(false),
	m_introductionUrl(urlAboutPkpm),
	PageInfo()
{
	m_bNetEdition = true;
	if (miniCx_ ==-1 || miniCy_ ==-1 || maxPrjNum ==-1)
	{
		MessageBox("check pkpm.ini", "error");
		std::terminate();
	}
	CheckProjectName();
}

CHtmlDlg::~CHtmlDlg()
{
}

void CHtmlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

//一个由于用户删除工程再新建时，windows字符串不敏感造成的错误
//基本保证是该用户"正常"
void CHtmlDlg::CheckProjectName()
{
	std::vector<std::string> vec;
	char prjPathStr[256]={0};
	for (ULONGLONG i = 0; i < maxPrjNum; ++i)
	{
		auto workPathId = "WorkPath" + std::to_string(i);
		memset(prjPathStr, 0, 256);
		GetPrivateProfileString("WorkPath", workPathId.c_str(), "error", prjPathStr, string_utility::ArraySize(prjPathStr), FullPathOfPkpmIni().GetBuffer());
		if(string_utility::StringPiece(prjPathStr)!="error")
		{
			if(PathFileExists(prjPathStr)&&PathIsDirectory(prjPathStr))
			{
				char longPath[MAX_PATH+1];
				char shortPath[MAX_PATH+1];
				if(GetShortPathName(prjPathStr,shortPath,MAX_PATH)>0)
					if(GetLongPathName(shortPath,longPath,MAX_PATH)>0)
					{
						if(std::islower(longPath[0])&&longPath[1]==':')
							longPath[0]=std::toupper(longPath[0]);
						for(int j=0;j<strlen(longPath);++j)
						{
							 if ('/'==longPath[j])
							{
								longPath[j]='\\';
							}
						}
						std::string stdString(longPath);
						auto endIndex=std::unique(stdString.begin(),stdString.end(),[](const char left,const char right){
							return (left==right)&&(left=='\\');
						});
						stdString.erase(endIndex,stdString.end());
						if(std::find(vec.cbegin(),vec.cend(),stdString)==vec.cend())
							vec.push_back(stdString);		
					}					
			}
		}
	}
	memset(prjPathStr, 0, 256);
	for (ULONGLONG i = 0; i < maxPrjNum; ++i)
	{
		std::string workPathId = std::string("WorkPath") + std::to_string(i);
		if (static_cast<size_t>(i) < vec.size())
			WritePrivateProfileString("WorkPath", workPathId.c_str(), vec[i].c_str(), FullPathOfPkpmIni().GetBuffer());
		else//de a bug 
			WritePrivateProfileString("WorkPath", workPathId.c_str(), NULL, FullPathOfPkpmIni().GetBuffer());
	}
}

/*
程序需要尽量开启高版本的IE以支持网页功能。
受到IE版本影响的功能包括: 拖拽文件，右键菜单，CSS样式。
https://docs.microsoft.com/en-us/previous-versions/windows/internet-explorer/ie-developer/general-info/ee330730(v=vs.85)
像这么改，9000应该就可以了。
HKEY_LOCAL_MACHINE (or HKEY_CURRENT_USER)
   SOFTWARE
	  Microsoft
		 Internet Explorer
			Main
			   FeatureControl
				  FEATURE_BROWSER_EMULATION
					 contoso.exe = (DWORD) 00009000
*/
BOOL CHtmlDlg::OnInitDialog()
{
	//旧代码 begin
	OnInit_gMultLangStrings();
	if(!InitPkpmAppFuncPtr())
	{
		MessageBox("error: pkpmv51.dll");
		EndDialog(IDCANCEL);
		return TRUE;
	}
	cfg_key_str=fuc_InitPkpmApp();	
	if (svr::GetRegCFGPath().IsEmpty())
	{
		CString str;
		str.Format (_T("无法找到注册表 %s" ),cfg_key_str);
		MessageBox(str);
		EndDialog(IDCANCEL);
	}
	SetCfgPmEnv();
	LoadPkpmXXXXIniConfig();
	ReadPrivateProfile();
	//end
	//MaxBrowser();
	//OnClickedResume();
	//CenterWindow();
	//SetHostFlags(DOCHOSTUIFLAG_FLAT_SCROLLBAR | DOCHOSTUIFLAG_NO3DBORDER);//
	EnableAutomation();
	SetExternalDispatch(GetIDispatch(TRUE));
	CDHtmlDialog::OnInitDialog();
	char PathOfHtmlFile[64]={0};
	const char* defaultStr = "error";
	GetPrivateProfileString(_T("Html"), _T("PathOfHtml"), defaultStr, &0[PathOfHtmlFile], sizeof(PathOfHtmlFile), FullPathOfPkpmIni());
	FalseIsFuckingNotTolerable([&]()->bool {return !std::equal(defaultStr, defaultStr + strlen(defaultStr), PathOfHtmlFile);});
	NavigateTo(PathOfHtmlFile);
	m_pBrowserApp->put_Silent(VARIANT_TRUE);
	m_pBrowserApp->put_RegisterAsDropTarget(VARIANT_FALSE);//本地处理
	MoveWindow(0,0,miniCx_,miniCy_,1);
	SetWindowText("PKPM V5.1.1");
	::SetWindowPos(this->m_hWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	SetFocus();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CHtmlDlg, CDHtmlDialog)
	ON_WM_CONTEXTMENU()
	ON_WM_DROPFILES()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


BEGIN_DHTML_EVENT_MAP(CHtmlDlg)
	DHTML_EVENT_ONMOUSEDOWN(_T("headerTest"), OnMoveActive)  //添加的代码
END_DHTML_EVENT_MAP()


/*
函数调用在这里声明
*/
BEGIN_DISPATCH_MAP(CHtmlDlg, CDHtmlDialog)
	//NEWVERSION()
	DISP_FUNCTION(CHtmlDlg, "NEWVERSION", TellMeNewVersionExistOrNot, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CHtmlDlg, "ADVERTISES", TellMeAdvertisement, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CHtmlDlg, "SETIEVERSION", SetIeVersion, VT_EMPTY, VTS_WBSTR)
	DISP_FUNCTION(CHtmlDlg, "CONFIGFILES", OnGetConfig, VT_BSTR, VTS_WBSTR)
	DISP_FUNCTION(CHtmlDlg, "ONNEWPROJECT", OnNewProject, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CHtmlDlg, "ONCLICKMIN", OnClickedMin, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CHtmlDlg, "ONCLICKMAX", OnClickedMax, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CHtmlDlg, "ONCLICKCLOSE", OnClickedClose, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CHtmlDlg, "READWORKPATHS", ReadWorkPaths, VT_BSTR, VTS_WBSTR)
	DISP_FUNCTION(CHtmlDlg, "SHORTCUT", OnShortCut, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CHtmlDlg, "ONDBCLICKHEADER", OnResume, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CHtmlDlg, "ONGETDEFAULTMENUSELECTION", OnGetDefaultMenuSelection, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CHtmlDlg, "ONSETDEFAULTMENUSELECTION", OnSetDefaultMenuSelection, VT_EMPTY, VTS_WBSTR)
	DISP_FUNCTION(CHtmlDlg, "ONSETDEFAULTMENUSELECTION2", OnSetDefaultMenuSelection2, VT_EMPTY, VTS_WBSTR)
	DISP_FUNCTION(CHtmlDlg, "ONRCLICKPRJ", OnRightClickProject, VT_EMPTY, VTS_WBSTR)
	DISP_FUNCTION(CHtmlDlg, "ONOPENPRJ", OnOpenProject, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CHtmlDlg, "DATAFORMAT", DataFormatTransfer, VT_EMPTY, VTS_WBSTR)
	DISP_FUNCTION(CHtmlDlg, "DBCLICKPROJECT", OnDbClickProject, VT_EMPTY, VTS_WBSTR VTS_WBSTR VTS_WBSTR VTS_WBSTR VTS_WBSTR)
	DISP_FUNCTION(CHtmlDlg, "CLICKLISTMENU", OnListMenu, VT_EMPTY, VTS_WBSTR VTS_WBSTR VTS_WBSTR VTS_WBSTR)
	DISP_FUNCTION(CHtmlDlg, "DOCUMENTPATH", OnOpenDocument, VT_EMPTY, VTS_WBSTR VTS_WBSTR)
END_DISPATCH_MAP()

std::unordered_map<std::string, void (CHtmlDlg::*)()> funcMap;
//前端不判断，那只有我来判断字符串咯。
//这样以后改网页可就麻烦了..
#define PREFIXLOCAL funcMap.insert(std::make_pair(
#define SHORTCUTFUNCS(className,JsName,InnerFuncs) \
	PREFIXLOCAL##JsName,&className::InnerFuncs));
#define BEGIN_INNERFUNC(className, helper) \
class className##helper \
{ public: \
	className##helper(){ \
	IPRETENDTONOTUSEIT("a marco to make function check&adding easy");
#define END_INNERFUNC  }}Helper__;


BEGIN_INNERFUNC(CHtmlDlg, Helper)
	SHORTCUTFUNCS(CHtmlDlg,"关于PKPM",OnAboutPkpm)
	SHORTCUTFUNCS(CHtmlDlg,"改进说明",OnImprovement)
	SHORTCUTFUNCS(CHtmlDlg,"模型打包",OnModelPacking)	
	SHORTCUTFUNCS(CHtmlDlg,"注册控件",OnRegiser)
	SHORTCUTFUNCS(CHtmlDlg,"联系我们",OnContactUs)
	SHORTCUTFUNCS(CHtmlDlg,"检查软件完整性",OnIntegrityCheck)
	SHORTCUTFUNCS(CHtmlDlg,"完整性检查",OnIntegrityCheck)
	SHORTCUTFUNCS(CHtmlDlg,"参数设置",OnParameterSettings)
	SHORTCUTFUNCS(CHtmlDlg,"转网络版",OnSwitchToNetVersion)
	SHORTCUTFUNCS(CHtmlDlg,"转单机版",OnSwitchToNetVersion)
	SHORTCUTFUNCS(CHtmlDlg,"锁码设置",OnSwitchToNetVersion)
	SHORTCUTFUNCS(CHtmlDlg,"用户手册",OnUserManual)	
	SHORTCUTFUNCS(CHtmlDlg,"在线更新",OnUpdateOnline)
END_INNERFUNC


void CHtmlDlg::OnUpdateOnline()
{
	ShellExecute(NULL, _T("open"), UpdateUrl, NULL, NULL, SW_SHOW);
}

void CHtmlDlg::OnIntegrityCheck()
{
	//你有一个函数，但不代表你每次都要调用它
	//粘过来是因为我打算改cfg目录
	CString regcmd = svr::GetRegCFGPath() + m_strNameOfIntegrity;
	CFile fi;
	if(fi.Open(regcmd,CFile::readOnly,NULL,NULL))
	{
		fi.Close();
		HINSTANCE hi = ShellExecute(NULL,"open",regcmd,NULL,NULL,SW_NORMAL);
		WaitForSingleObject(hi,INFINITE);
	}
	else
	{
		CString strHint;
		strHint.Format("无法找到或启动程序 %s",regcmd);
		MessageBox(strHint,"错误提示");
	}

}

void CHtmlDlg::OnSwitchToNetVersion()
{
	//Fix me
	OnBnClickedBtnSetLock();
}

void CHtmlDlg::OnContactUs()
{
	ShellExecute(GetSafeHwnd(),"open",m_sOnlineAsk,NULL,NULL,SW_SHOWDEFAULT );
}

void CHtmlDlg::OnRegiser()
{
	CDlgRegRunAsHint dlg;
	dlg.DoModal();
	//Fix me
	CString cfgpa = svr::GetRegCFGPath()+_T("RegPKPMCtrl.exe");
	CString cmdline;
	cmdline.Format("/n,/select,%s",cfgpa);  
	::ShellExecute(0,"open","explorer.exe",cmdline,NULL,SW_SHOWNORMAL);
}

void CHtmlDlg::OnUserManual()
{
	CString helpPath = GetAppPath()+"Help\\UserGuider";
	::ShellExecute(0,"open","explorer.exe",helpPath,NULL,SW_SHOWNORMAL);
}

void CHtmlDlg::OnParameterSettings()
{
	{
		CString strFullName;
		GetAppPathByCFGPATHMarker(get_cfg_path_reg_key(),strFullName);
		toolsvr::FixPathStr(strFullName);
		if(DRIVE_REMOTE == GetDriveType(strFullName))
		{
			CString strHint="您即将更改服务器的配置文件，请您确认是否具有服务器文件的读写权限。";
			//strHint = g_stringMgr.LoadString(IDS_MAYNOT_CONFIG_PKPM_INI);
			if(IDCANCEL ==AfxMessageBox(strHint,MB_OKCANCEL))
				return;
		}
		int nRet = IDOK;
		int nOpen = 0;
		do 
		{
			int nlen;
			bool bHasPwe = false;
			if (fuc_IsProjectWiseVailed)
			{
				bHasPwe = fuc_IsProjectWiseVailed(NULL,nlen);
			}
			CPkpmInfoSheet dialog(_T("PKPM全局参数设置"),NULL,bHasPwe,nOpen);
			dialog.m_otherspage.SetPageType(2);//当前页面类型
			dialog.m_psh.dwFlags &= ~PSH_HASHELP;
			nRet = dialog.DoModal();
		}
		while (false);
	}
}

void CHtmlDlg::OnModelPacking()
{
	//从pkpmv31Dlg复制来的
	OnBnClickedBtnFileMgr();
}

void CHtmlDlg::OnAboutPkpm()
{
	ShellExecute(GetSafeHwnd(),"open",m_introductionUrl,NULL,NULL,SW_SHOWDEFAULT );
}

void CHtmlDlg::OnImprovement()
{
	//run_cmd("改进说明完整版","V5.X版改进说明完整版",""); // 执行命令 wait
	CString updateFilePath = GetAppPath()+"Help\\UpdateGuider";
	::ShellExecute(0,"open","explorer.exe",updateFilePath,NULL,SW_SHOWNORMAL);
}

void CHtmlDlg::OnShortCut(const char* cutName)
{
	if(funcMap.find(cutName)!=funcMap.end())
			(this->*funcMap[cutName])();
	else
		throw std::invalid_argument("invalid event name");
}

/*
单击列表菜单和双击工程都是进入这个函数。
不同的是单击时需要检查工程列表是不是为空。
不合逻辑，但我也一起处理了。
*/
void CHtmlDlg::OnDbClickProject(BSTR prjPath, BSTR pathOfCore, BSTR coreWithPara,BSTR secMenu, BSTR trdMenu)
{
	CComVariant prj(prjPath);
	prj.ChangeType(VT_BSTR);
	USES_CONVERSION;
	std::string str(OLE2T(prj.bstrVal));
	m_WorkPath=CString(str.c_str());
	if(!prjPaths_.size())
	{//不应该再出现这个错误
		MessageBox("工作目录不存在","PKPMV51");
		return ;
	}
	if(false&&CommandFliter(CString(pathOfCore), 
		CString(coreWithPara),
		CString(secMenu),
		CString(trdMenu)))
		return;
	{
		//2019/11/22
		//现在，你再也不是在main里面了
		ShowWindow(SW_HIDE);
		char oldWorkPath[256]={0};
		GetCurrentDirectory(sizeof(oldWorkPath),oldWorkPath);
		auto ret=SetCurrentDirectory(m_WorkPath);
		if(!ret)
		{
			MessageBox("工作目录错误或者没有权限","PKPMV51");
			//return;
		}		
		run_cmd(OLE2T(secMenu),OLE2T(trdMenu),"");
		SetCurrentDirectory(oldWorkPath);
		SaveMenuSelection();
		this->m_pBrowserApp->Refresh();
		ShowWindow(SW_SHOW);
	}
}

void CHtmlDlg::OnListMenu(BSTR pathOfCore, BSTR coreWithPara,BSTR secMenu, BSTR trdMenu)
{
	//坑爹啊
	if(!prjPaths_.size())
	{
		MessageBox("没有选择工作目录","PKPMV51");
		return ;
	}
	auto prjSelected=std::stoi(MenuSelectionOnHtml().back().second);
	OnDbClickProject(_com_util::ConvertStringToBSTR(prjPaths_[prjSelected].c_str()), pathOfCore, coreWithPara,secMenu,trdMenu);
}

bool CHtmlDlg::CommandFliter(CString path,CString core, CString secMenu, CString trdMenu)
{
	auto coreWithParameters=core;
	coreWithParameters.MakeUpper();
	const auto nameOfThisApp="PKPMMAIN.EXE";
	int index=coreWithParameters.Find(nameOfThisApp);
	if(-1==index)
	{
		ShowWindow(SW_HIDE);
		//fix me, check it
		char oldWorkPath[256]={0};
		GetCurrentDirectory(sizeof(oldWorkPath),oldWorkPath);
		auto ret=SetCurrentDirectory(path);
		if(!ret)
		{
			MessageBox("工作目录错误或者没有权限","PKPMV51");
			//return true;
		}		
		run_cmd(secMenu,trdMenu,"");
		SetCurrentDirectory(oldWorkPath);
		SaveMenuSelection();
		this->m_pBrowserApp->Refresh();
		ShowWindow(SW_SHOW);
		return true;
	}
	return false;
}

void CHtmlDlg::SetPkpmPara(BSTR path, BSTR core)
{
	//不要改交互机制!！！
	CComVariant varStr(path);
	varStr.ChangeType(VT_BSTR);
	USES_CONVERSION;
	std::string str(OLE2T(varStr.bstrVal));
	auto vec=string_utility::string_split(str,",");
	//FIX ME
	//我没有办法帮你检查配置文件读到的字符串，
	//只能说，如果是pkpmMain.exe,我就存起来
	//如果不是，那么尝试打开某某进程
	if(vec.empty())
		return;
	//FIX ME
	//前端没有处理
	//所以我需要在这里存一下，等待用户双击
	
	m_coreWithPara=core;
	m_pathOfCore=path;
	
}

//@filename 配置文件路径
//@newPro 新工程路径
bool CHtmlDlg::AddWorkPaths(const std::string &newProj,const std::string& filename)
{
	if(newProj.empty())
		return false;
	auto iter=std::find(prjPaths_.begin(), prjPaths_.end(), newProj); 
	if (iter== prjPaths_.end())
	{
		prjPaths_.put(newProj);
		SaveWorkPaths(prjPaths_, filename);
		return true;
	}			
	else
	{
		auto index=std::distance(prjPaths_.begin(),iter);
		//MessageBox(_T("文件夹已存在"));
		prjPaths_.moveToFront(index);
		SaveWorkPaths(prjPaths_, filename);
		return true;
	}
	return false;
}

/*IE7即不支持拖拽功能*/
int versionOfIE = 0;
void CHtmlDlg::SetIeVersion(BSTR version)
{
	CComVariant varStr(version);
	varStr.ChangeType(VT_BSTR);
	USES_CONVERSION;
	std::string str(OLE2T(varStr.bstrVal));
	versionOfIE = std::stoi(str);
}

void CHtmlDlg::OnClickedMin()
{	
	GetWindowRect(rectForActive);
	::SendMessage(this->m_hWnd,
		WM_SYSCOMMAND, 
		SC_MINIMIZE,
		0);
	
}

void CenterThisWindow(CWnd* wnd)
{
	wnd->CenterWindow();
}

void FullScreen(HWND hwnd)
{
	SendMessage(hwnd,
		WM_SYSCOMMAND,
		SC_MAXIMIZE,
		0);
}

/*
一个取巧的做法
*/
void CHtmlDlg::MaxBrowser()
{
	static bool cached = false;
	GetWindowRect(rectForResume);
	if (!cached)
	{
		FullScreen(this->m_hWnd);
		GetWindowRect(rectCached);
		CRect rectOfWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rectOfWorkArea, 0);    // 获得工作区大小
		int height = ::GetSystemMetrics(SM_CYSCREEN) - rectOfWorkArea.Height();
		rectCached.bottom -= height;
		cached = true;
	}
	FullScreen(this->m_hWnd);
	MoveWindow(rectCached, true);
}

/*
顺便可以测一下alime::thread~~~~
*/
void CHtmlDlg::OnClickedMax()
{
	CRect rc;
	GetWindowRect(rc);
	if(rc==rectCached)
		OnClickedResume();
	else
		MaxBrowser();
}

void CHtmlDlg::OnClickedClose()
{
	OnCancel();
}

//双击标题栏
void CHtmlDlg::OnResume()
{
	CRect rc;
	GetWindowRect(rc);
	if (rc == rectCached)
	{
		OnClickedResume();
		SwitchSrcOfZoomAll(true);
	}
	else
	{
		/*
		为了处理内网机的bug，我重写了标题栏事件
		但这又造成网页的问题。
		为了几台特定的机器，整个代码变成了屎。
		*/
		CPoint point;
		GetCursorPos(&point);
		mouse_event(MOUSEEVENTF_LEFTUP, point.x, point.y, 0, 0);
		MaxBrowser();
		SwitchSrcOfZoomAll(false);
	}		
}

void CHtmlDlg::SwitchSrcOfZoomAll(bool maxFlag)
{
	CComPtr<IHTMLElement> sp=NULL;
	std::string elem="zoomAllTest";
	if(S_OK==GetElementInterface(elem.c_str(),&sp))
	{
		BSTR attribute=::SysAllocString(L"src");
		VARIANT vstr;
		vstr.vt=VT_BSTR;
		if(maxFlag)
			vstr.bstrVal=::SysAllocString(L"./images/zoomAll.png");
		else
			vstr.bstrVal=::SysAllocString(L"./images/zoomAll2.png");
		sp->setAttribute(attribute,vstr);
		::SysFreeString(attribute);
	}
	else
	{
		//这个错误要暴露出来，因为刷新网页片刻会出现问题
		//MessageBox("文档未加载完成，出现本提示，请联系开发人员");
		//LOG_FATAL<<"文档未加载完成";
	}
		
}


//函数名歪了0.0,改的次数太多了
void CHtmlDlg::OnClickedResume()
{
	if (IsRectEmpty(&rectForResume))
		return;
	MoveWindow(rectForResume);
}

/*
这里没有检查重复项，因为
根本就没有机会创建成功
*/
BSTR CHtmlDlg::ReadWorkPaths(BSTR fileName)
{
	prjPaths_.clear();
	CComVariant varStr(fileName);
	varStr.ChangeType(VT_BSTR);
	USES_CONVERSION;
	CString strMsg;
	strMsg.Format(_T("%s"), OLE2T(varStr.bstrVal));
	std::string filename(strMsg.GetBuffer());
	filename = GetAppPath() + filename.c_str();
	//
	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	Value array(kArrayType);//< 创建一个数组对象
	std::vector<std::string> vec;
	char prjPathStr[256] = { 0 };
	for (int i = 0; i < maxPrjNum; ++i)
	{
		auto workPathId = "WorkPath" + std::to_string((ULONGLONG)i);
		memset(prjPathStr, 0, 256);
		auto nRead=GetPrivateProfileString("WorkPath", workPathId.c_str(), "error", prjPathStr, 256, filename.c_str());	
		if (!strcmp("error", prjPathStr))
		{
			//prjPathStr=="error"表明用户手动修改了配置文件!
			continue;//or continue?
		}		
		else if (prjPathStr[nRead - 1] != '\\')
		{
			//用户改了配置文件，但又没改对
			continue;
		}
		std::string timeStamp;
		auto ret = getPrjInfo(prjPathStr, timeStamp);
		rapidjson::Value obj(rapidjson::kObjectType);//每一个数组里面是一个json格式
		if (ret)//ret是啥...我忘了
		{
			vec.emplace_back(prjPathStr);
			Value key(kStringType);
			key.SetString(workPathId.c_str(), allocator);
			Value value(kStringType);
			value.SetString(prjPathStr, allocator);
			obj.AddMember("WorkPath", value, allocator);

			value.SetString(timeStamp.c_str(), allocator);
			obj.AddMember("Date", value, allocator);

			std::string bmpPath = prjPathStr;
			bmpPath += "BuildUp.bmp";
			if (isSnapShotExist(bmpPath))
			{
				bmpPath = "file://" + bmpPath;//网页需要增加这个前缀
				value.SetString(bmpPath.c_str(), allocator);
				obj.AddMember("ImgPath", value, allocator);
			}
			else
			{
				value.SetString(bmpPath.c_str(), allocator);
				obj.AddMember("ImgPath","error", allocator);
			}
			array.PushBack(obj, allocator);
		}
	}
	prjPaths_.put(vec.rbegin(), vec.rend());
	doc.AddMember("Data", array, allocator);
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	doc.Accept(writer);
#ifdef DEBUG
	//MessageBox(s.GetString());
#endif
	return _com_util::ConvertStringToBSTR(s.GetString());
}


bool CHtmlDlg::isSnapShotExist(const std::string& path)
{
	assert(!PathIsDirectory(path.c_str()));
	return static_cast<bool>(PathFileExists(path.c_str()));
}

/*
vs2017编译的程序在
xp下出现api无效0.0
但是我决定保留这个隐患,作为我编写日志系统的理由
*/
bool CHtmlDlg::getPrjInfo(const std::string& pathStr, std::string& timestamp,
	const char* surfix)
{
	auto index=pathStr.find_last_not_of("/\\");
	std::string path=pathStr.substr(0,index+1);
	if (ENOENT == _access(path.c_str(), 0))
		return false;
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) == 0)
	{
		if ((_S_IFMT&statbuf.st_mode) == _S_IFDIR)
		{
			auto seconds = statbuf.st_mtime;
			auto tm_time = *localtime(&seconds);
			char buf[64];
			sprintf(buf, "%4d年%02d月%02d日 %02d:%02d",
				tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
				tm_time.tm_hour, tm_time.tm_min);
			timestamp.assign(buf);
			return true;
		}
	}
	return false;
}

BSTR CHtmlDlg::OnGetConfig(BSTR filename)
{
	CComVariant varStr(filename);
	varStr.ChangeType(VT_BSTR);
	USES_CONVERSION;
	CString strMsg;
	strMsg.Format(_T("%s"), OLE2T(varStr.bstrVal));
	std::string fileNameStr = CT2A(strMsg.GetBuffer());
	if (fileNameStr=="CFG/PKPMAPPMENU\\PKPMAPP-JICHENG.xml")
	{
		fileNameStr="CFG/PKPMAPPMENU/PKPMAPP-JICHENG-part1.xml";
	}
	fileNameStr=GetAppPath()+fileNameStr.c_str();
	string_utility::ReadSmallFile file(fileNameStr);
	std::string xmlStr;
	file.readToString(xmlStr);
	std::string ansi_json = xml2json(xmlStr.c_str());	
	//for debug
	auto content= _com_util::ConvertStringToBSTR(ansi_json.c_str());
	return content;
}

/*
直接丢给dll处理
*/
void CHtmlDlg::DataFormatTransfer(BSTR module_app_name)
{
	if(!prjPaths_.size())
	{
		MessageBox("请先选择工作目录","PKPMV51");
		return;
	}
	CComVariant varStr(module_app_name);
	varStr.ChangeType(VT_BSTR);
	USES_CONVERSION;
	CString nameStr;
	nameStr.Format(_T("%s"), OLE2T(module_app_name));
	std::string str_Ret = CT2A(nameStr.GetBuffer());
	auto vec=string_utility::string_split(str_Ret,",");
	auto iter=vec.begin();
	try
	{
		ShowWindow(SW_HIDE);	
		char oldWorkPath[256]={0};
		GetCurrentDirectory(sizeof(oldWorkPath),oldWorkPath);
		auto prjSelected=std::stoi(MenuSelectionOnHtml().back().second);
		BOOL ret=SetCurrentDirectory(prjPaths_[prjSelected].c_str());
		if(!ret)
		{
			MessageBox("工作目录错误或者没有权限","PKPMV51");
			//return;
		}		
		m_WorkPath=prjPaths_[prjSelected].c_str();
		run_cmd(CString(vec.front().c_str()),CString((*++iter).c_str()),"");
		SetCurrentDirectory(oldWorkPath);
		SaveMenuSelection();
		this->m_pBrowserApp->Refresh();
		ShowWindow(SW_SHOW);		
	}
	catch (CException& e)
	{//FIX ME
		//LOG_FATAL<<""
		IPRETENDTONOTUSEIT(e);
	}
	
}


/*
Set reg value if failed to find hkey ,fix "fix me"
*/
bool SetRegValue(HKEY nKeyType, const std::string& strUrl, const std::string& strKey, DWORD value)
{
	HKEY rootKey=nKeyType;
	DWORD state;
	const int kBuffSize = 256;
	char reBuff[kBuffSize] = { 0 };
	HKEY hKey = NULL;
	auto ret= RegCreateKeyEx(rootKey, strUrl.data(), 0, NULL, 0, 0, NULL, &hKey, &state);
	if (ret == ERROR_SUCCESS)
	{
		if (state == REG_CREATED_NEW_KEY)
			;//LOG_TRACE<<"create new reg";
		RegCloseKey(hKey);
	}
	else//已存在
	{
		//FIX ME 检查值并直接退出
	}
	ret= RegOpenKeyEx(rootKey , strUrl.data(), 0, KEY_WRITE, &hKey);
	if (ret== ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, strKey.c_str(), 0, REG_DWORD, (BYTE *)&value, sizeof(DWORD)/*value.size()*2*/);
		RegCloseKey(hKey);
		return true;
	}
	return false;
}

bool CHtmlDlg::ModifyRegistry()
{
	return SetRegValue(HKEY_CURRENT_USER,
		"SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION",
		"PKPMMain.exe",
		9000) && 
		SetRegValue(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION",
		"PKPMMain.exe",
		9000);
}

void CHtmlDlg::NavigateTo(LPCTSTR pszUrl)
{
	CString strPath, str;
	DWORD dwSize = MAX_PATH;
	::GetModuleFileName(NULL, strPath.GetBuffer(MAX_PATH), dwSize); //AfxGetResourceHandle()
	strPath.ReleaseBuffer(dwSize);
	str = strPath.Left(strPath.ReverseFind(_T('\\'))+1);
	ASSERT(pszUrl != NULL);
	if (pszUrl != NULL)
	{
		CString strUrl=_T("file:///") +str+"Ribbon\\"+pszUrl;
		Navigate(strUrl);
		//Navigate("C:\\_ALL_\\_DEV_\\Out\\Win32\\Release\\nouse\\index.html");
	}
}

//脚本配置文件被删了, 你们是不是闲的?,那就硬编码。
const CString strScriptDrag=_T("document.ondragstart=function(event){return false;};");
const CString strScriptSelect=_T("document.onselectstart=function(){return false;};");

void CHtmlDlg::OnInjectScript()
{
	CString strScript = strScriptDrag+strScriptSelect;
	CString strLanguage(_T("JavaScript"));
	IHTMLDocument2* pIHtmlDoc = NULL;
	GetDHtmlDocument(&pIHtmlDoc);
	if (!pIHtmlDoc) 
		return;
	IHTMLWindow2* pIhtmlwindow2 = NULL;
	pIHtmlDoc->get_parentWindow(&pIhtmlwindow2);
	if (!pIhtmlwindow2) 
		return;
	BSTR bstrScript = strScript.AllocSysString();
	BSTR bstrLanguage = strLanguage.AllocSysString();
	VARIANT pRet;
	// 注入脚本到当前页面
	pIhtmlwindow2->execScript(bstrScript, bstrLanguage, &pRet);
	::SysFreeString(bstrScript);
	::SysFreeString(bstrLanguage);
	pIhtmlwindow2->Release();
}

void CHtmlDlg::OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CDHtmlDialog::OnNavigateComplete(pDisp, szUrl);
	// TODO: 在此添加专用代码和/或调用基类
	CString strScript = strScriptDrag+strScriptSelect;
	CString strLanguage(_T("JavaScript"));
	IHTMLDocument2* pIHtmlDoc = NULL;
	GetDHtmlDocument(&pIHtmlDoc);
	if (!pIHtmlDoc) 
		return;
	IHTMLWindow2* pIhtmlwindow2 = NULL;
	pIHtmlDoc->get_parentWindow(&pIhtmlwindow2);
	if (!pIhtmlwindow2) 
		return;
	BSTR bstrScript = strScript.AllocSysString();
	BSTR bstrLanguage = strLanguage.AllocSysString();
	VARIANT pRet;
	// 注入脚本到当前页面
	pIhtmlwindow2->execScript(bstrScript, bstrLanguage, &pRet);
	::SysFreeString(bstrScript);
	::SysFreeString(bstrLanguage);
	pIhtmlwindow2->Release();
}

BOOL  CHtmlDlg::CanAccessExternal()
{
	return TRUE;
}

HRESULT CHtmlDlg::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	return S_OK;
}

HRESULT CHtmlDlg::OnMoveActive(IHTMLElement *pElement)
{
	SendMessage(WM_NCLBUTTONDOWN, 
		HTCAPTION, 
		0);
	return S_OK;
}

void CHtmlDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UINT count;
	TCHAR filePath[260] = { 0 };
	count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (count!=1)
	{
		MessageBox("仅支持拖拽单个目录");
		return;
	}
	else
	{
		DragQueryFile(hDropInfo, 0, filePath, sizeof(filePath));
		if (PathFileExists(filePath) && PathIsDirectory(filePath))
		{
			filePath[_tcslen(filePath)] = '\\';
			if(AddWorkPaths(filePath, FullPathOfPkpmIni().GetBuffer()))				
			{
				SaveMenuSelection(false);
				this->m_pBrowserApp->Refresh();
			}
		}		
		else
		{
			MessageBox("仅支持拖拽目录");
			return;
		}		
		/*
		we needs fileSystem
		*/
#ifdef DEBUG
		AfxMessageBox(filePath);
#endif		
	}
	DragFinish(hDropInfo);
	CDHtmlDialog::OnDropFiles(hDropInfo);
}

#include "DlgSelectDir.h"
BSTR CHtmlDlg::OnNewProject()
{
	{
		CString path;
		DlgSelectDir dlg;
		auto prjName=OnGetSelectedPrj();
		dlg.m_initPath=prjName.c_str(); 
		if(IDOK==dlg.DoModal())
		{
			//文件夹就是文件，不要以\结束。
			//为了迁就旧程序，我也得跟着乱写
			path=dlg.m_sCurPath;
			std::string dirName=dlg.m_sCurPath.GetBuffer();
			if(!string_utility::endWith(dirName.c_str(),"\\"))
			{
				dirName+="\\";
			}
			const std::string path_of_pkpm_dot_ini(FullPathOfPkpmIni().GetBuffer());
			if(AddWorkPaths(dirName, path_of_pkpm_dot_ini))
			{
					SaveMenuSelection(false);
					this->m_pBrowserApp->Refresh();
			}
		}
		 return _com_util::ConvertStringToBSTR(path.GetBuffer());
	}
	////deprecated///////////////////////////////////////////////////////
	TCHAR  folderPath[MAX_PATH]={ 0 };
	CString path;
	BROWSEINFO  sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择新的工程路径");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX
		| BIF_DONTGOBELOWDOMAIN | BIF_USENEWUI;
	sInfo.lpfn = NULL;

	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		if (::SHGetPathFromIDList(lpidlBrowse, folderPath))
		{
			path =folderPath;
			::CoTaskMemFree(lpidlBrowse);
			std::string dirName(path.GetBuffer());
			dirName+="\\";
			std::string path_of_pkpm_dot_ini=std::string(GetAppPath())+
				"../"+"cfg/pkpm.ini";//这个真的没有办法
			AddWorkPaths(dirName, path_of_pkpm_dot_ini);
			SaveMenuSelection(false);
			this->m_pBrowserApp->Refresh();
		}
	}
	return _com_util::ConvertStringToBSTR(path.GetBuffer());
}


HRESULT IsExternalDispatchSafe(IHTMLElement *pElement)
{
	return true;
}

//旧代码复制
void CHtmlDlg::OnBnClickedBtnSetLock()
{
	CString cfgpath = m_cfg_path;

	CString exePathName = cfgpath + _T("PKPMAuthorize.exe");
	if(-1==_taccess(exePathName,0))
	{
		CString strHint;
		strHint += _T("File belowed is not existed.\n");
		strHint += exePathName;
		MessageBox(strHint,_T("lockconfig"));
		return ;
	}

	TCHAR STRPATH[MAX_PATH];
	ZeroMemory(STRPATH,sizeof(TCHAR)*MAX_PATH);
	_tcscpy_s(STRPATH,MAX_PATH,exePathName.GetBuffer());
	STARTUPINFO info = {0};
	info.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION prinfo;
	CRect rc;
	GetWindowRect(rc);
	ShowWindow(SW_HIDE);
	CreateProcess(NULL,STRPATH,NULL,NULL,FALSE,0,NULL,NULL,&info,&prinfo);
	WaitForSingleObject(prinfo.hProcess,-1);
	//ShowWindow(SW_SHOWNORMAL);
	MoveWindow(rc.left,rc.top,rc.Width(),rc.Height(),0);
	ShowWindow(SW_SHOW);
	//::SetWindowPos(this->m_hWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	CloseHandle(prinfo.hThread);
	CloseHandle(prinfo.hProcess);

}


void CHtmlDlg::LoadPkpmXXXXIniConfig()
{
	CString ini_file=GetPkpmXXXXiniPathName();//pkpm2000.ini
	m_cfg_path=svr::GetRegCFGPath();
	//触目惊心....
	TCHAR help_file[128];
	DWORD nRead;
	nRead= GetPrivateProfileString(_T("CONFIG"), _T("HELP"),	_T("pkpm.hlp"), help_file, sizeof(help_file), ini_file);
	m_help_file=help_file;
	TCHAR ver_exe_name[64];
	nRead= GetPrivateProfileString(_T("CONFIG"), _T("VERSION"),	_T("pkpminfo.exe"), ver_exe_name, sizeof(ver_exe_name), ini_file);
	m_ver_info_exe=ver_exe_name;

	TCHAR web_support[128];
	nRead= GetPrivateProfileString(_T("CONFIG"), _T("ASKONLINE"),	_T("https://pkpm.udesk.cn/im_client/?web_plugin_id=47258"), web_support, sizeof(web_support), ini_file);
	m_sOnlineAsk=web_support;

	TCHAR web_wiki[128];
	nRead= GetPrivateProfileString(_T("CONFIG"), _T("PKPM-WIKI"),	_T("http://help.pkpm.cn"), web_wiki, sizeof(web_wiki), ini_file);
	m_sWebWiki=web_wiki;

	TCHAR filename_configdll[400];
	nRead= GetPrivateProfileString(_T("CONFIG"), _T("CONFIGDLL"),	_T("pkpmconfig.dll"), filename_configdll, sizeof(filename_configdll), ini_file);
	m_configdll_filename=filename_configdll;


	TCHAR filename_regctrl[400];
	nRead= GetPrivateProfileString(_T("CONFIG"), _T("REGCTRL"),	_T("RegPKPMCtrl.exe"), filename_regctrl, sizeof(filename_regctrl), ini_file);
	m_strNameOfRegCtrl=filename_regctrl;
	m_strNameOfRegCtrl.Trim();

	TCHAR filename_Integrity[400];
	nRead= GetPrivateProfileString(_T("CONFIG"), _T("INTEGRITYCHECK"),	_T("PkpmIntegrityCheck.exe"), filename_Integrity, sizeof(filename_Integrity), ini_file);
	m_strNameOfIntegrity=filename_Integrity;
	m_strNameOfIntegrity.Trim();


	TCHAR filename_PManager[400];
	nRead= GetPrivateProfileString(_T("CONFIG"), _T("PMANAGER"),	_T("PMANAGER.exe"), filename_PManager, sizeof(filename_PManager), ini_file);
	m_strNameOfPManager=filename_PManager;
	m_strNameOfPManager.Trim();


	TCHAR DLGCAP[400];
	nRead= GetPrivateProfileString(_T("CONFIG"), _T("CAPTION"),	_T("PKPM V3.2"), DLGCAP, sizeof(DLGCAP), ini_file);
	m_strDlgCap = DLGCAP;
	m_strDlgCap.Trim();


	{
		CString iniFile2= svr::GetRegCFGPath()+_T("PKPM.ini");
		TCHAR langType[400];
		nRead= GetPrivateProfileString(_T("CONFIG"), _T("LANGUAGE"),_T("Chinese"), langType, sizeof(langType), iniFile2);
		m_strLangType = langType;
		m_strLangType.Trim();

		TCHAR CodeType[400];
		nRead= GetPrivateProfileString(_T("CONFIG"), _T("CODE"),_T("Chinese"), CodeType, sizeof(CodeType), iniFile2);
		m_strCodeType = CodeType;
		m_strCodeType.Trim();
	}
}

void CHtmlDlg::ReadPrivateProfile()
{
	CString iniFile= svr::GetRegCFGPath()+_T("PKPM.ini");
	CString cstr_path, keyname;
	TCHAR inBuf[_MAX_PATH+1];
	DWORD nRead;
	m_nMaxWorkPathNum=0;
	m_nMaxWorkPathNum= datasvr::ReadPrivateProfile2000_MaxWorkPath();
	if(m_nMaxWorkPathNum<=0) m_nMaxWorkPathNum=4;

	m_arrWorkPath.clear();
	for(int i=0;i<m_nMaxWorkPathNum;i++)
	{
		keyname.Format(_T("WorkPath%d"),i);
		nRead= GetPrivateProfileString(_T("WorkPath"), keyname,	_T("C"), inBuf, _MAX_PATH, iniFile);
		if( nRead>=2 ) 
		{
			cstr_path.Format(_T("%s"),inBuf);
			toolsvr::FixPathStr(cstr_path);
			bool bHasAlready = false;
			if( -1!=_taccess(cstr_path,0))
			{
				for (size_t j=0;j<m_arrWorkPath.size();j++)
				{
					if (0==cstr_path.CompareNoCase(m_arrWorkPath[j]))
					{
						bHasAlready = true;
						break;
					}
				}
				if (false == bHasAlready)
				{
					m_arrWorkPath.push_back(cstr_path);
				}
			}
		}
	}

	int DestInterrupt= GetPrivateProfileInt(_T("CFG_DEVICE"),_T("DestInterrupt"),0,iniFile);
	if( DestInterrupt==255 )
	{
		m_bNetEdition= TRUE;
	}
	else
	{
		m_bNetEdition= FALSE;

	}

	//是否强制允许更新
	int nForce = GetPrivateProfileInt(_T("CFG_DEVICE"),_T("ForceAllowUpataExe"),0,iniFile);
	m_bForceAllowUpdate = (nForce==0)?false:true;

	//m_nVersion_APM= GetPrivateProfileInt(_T("APM控制参数"),_T("Version"),0,iniFile);

	GetPrivateProfileString(_T("PKPMMENU"),_T("PKPMVersion"),_T("2005.04"),m_sVersion_PKPM,32,iniFile);


	{

		int nShowFullPath=GetPrivateProfileInt("PM控制参数","btnShowFullPath",-1,iniFile);
		if (nShowFullPath>=0)
		{
			//ButtonPrjFile::s_SingleLine = nShowFullPath == 0?true:false;
		}
		else
		{
			//ButtonPrjFile::s_SingleLine = true;
		}
	}
}

void CHtmlDlg::OnInit_gMultLangStrings()
{
	CString strLang;
	TCHAR szBuf[200];
	GetPrivateProfileString(INI_APP_DLGSTYLE, INI_DLGSTYLE_LANG,_T("CHS"),szBuf,sizeof(szBuf),GetPkpmXXXXiniPathName());
	strLang=szBuf;

	CString strApplication = GetAppPath();
	strApplication.Delete(strApplication.GetLength()-1,1);
	int nSep = strApplication.ReverseFind(_T('\\'));
	CString vcxprojPath;
	if (nSep>0)
	{
		vcxprojPath = strApplication.Left(nSep);
		g_stringMgr.SetProjectPathName(vcxprojPath,_T("PKPMMain"));

		g_stringMgr.StringResToXml();
		g_stringMgr.LoadFromFile();
		g_stringMgr.SetCurLang(strLang);
	}
}


void CHtmlDlg::run_cmd( const CString&moduleName,const CString&appName1_,const CString&appName2 )
{
	CString appName1(appName1_);
	ASSERT(!moduleName.IsEmpty());
	ASSERT(!appName1.IsEmpty());
	/*
		lowB代码
	*/
	//PDS,导出Revit
	
	if(moduleName=="数据转换")
	{
			if(appName1=="SP3D/PDS")
				appName1="PDS";
			else if(appName1=="Revit")
			{
				appName1="导出Revit";
			}
	}
	char str[400];
	if(appName2.IsEmpty())
	{
		sprintf(str,"%s|%s",moduleName,appName1);
	}
	else
	{
		sprintf(str,"%s|%s_%s",moduleName,appName1,appName2);
	}

	CWnd *mainWnd = ::AfxGetMainWnd();
	ASSERT(mainWnd);
	//ASSERT(SetCurrentDirectory(m_WorkPath));
	try
	{
		fuc_RunCommand(str); 
	}
	catch (...)
	{
	}

	//恢复工作目录
	//ASSERT(SetCurrentDirectory(m_WorkPath));//?
}

void CHtmlDlg::append_work_path(const CString&path )
{
	datasvr::WritePrivateProfile2000_workPath(path);
	CStringArray strs;
	datasvr::ReadPrivateProfile2000_workPaths(strs);
	m_arrWorkPath.clear();
	for (int i=0;i<strs.GetSize();i++)
	{
		m_arrWorkPath.push_back(strs[i]);
	}
}

void CHtmlDlg::OnMoveWindow(VARIANT  pos)
{
	CComVariant varStr(pos);
	varStr.ChangeType(VT_BSTR);
	USES_CONVERSION;
	CString strMsg;
	strMsg.Format(_T("%s"), OLE2T(varStr.bstrVal));
	std::string stdStr(strMsg.GetBuffer());
	auto index= stdStr.find(",");
	int x = std::stoi(stdStr.substr(0, index));
	int y=  std::stoi(stdStr.substr(index+1));
	CRect rc;
	GetWindowRect(rc);
	MoveWindow(rc.left+x, rc.top +y, rc.Width(), rc.Height());
	//OutputDebugString(stdStr.c_str());
}

void CHtmlDlg::OnBnClickedBtnFileMgr()
{	
	CArray<MODULE_PATH> modPaths;
	CString strPatTDGL;
	if(svr::getModulePath(modPaths))
	{
		for (int i=0;i!=modPaths.GetSize();++i)
		{
			if(!modPaths[i].m_name.CompareNoCase("TDGL"))
			{
				strPatTDGL = modPaths[i].m_path;
				toolsvr::FixPathStr(strPatTDGL);
				break;
			}
		} 
	}

	if (strPatTDGL.IsEmpty())
	{
		CString strHint;
		strHint.Format("无法找到安装目录TDGL");
		MessageBox(strHint,"错误提示");
		return;
	}

	CString strCfgPa;
	GetAppPathByCFGPATHMarker(get_cfg_path_reg_key(),strCfgPa);
	toolsvr::FixPathStr(strCfgPa);

	CString regcmd = strPatTDGL + m_strNameOfPManager;
	CString cmdParm = "-f ";
	cmdParm +="\""+ strCfgPa +"pkpm.ini\"";
	CFile fi;
	if(fi.Open(regcmd,CFile::readOnly,NULL,NULL))
	{
		fi.Close();
		HINSTANCE hi = ShellExecute(NULL,"open",regcmd,cmdParm,NULL,SW_NORMAL);
		WaitForSingleObject(hi,INFINITE);//wtf
	}
	else
	{
		CString strHint;
		strHint.Format("无法找到或启动程序 %s %s",regcmd,cmdParm);
		MessageBox(strHint,"错误提示");
	}
}

bool CHtmlDlg::GetAppPathByCFGPATHMarker(CString strMarker,CString& retPath)
{
	strMarker.Trim();
	retPath.Empty();

	if (strMarker.IsEmpty())
	{
		return false;
	}

	strMarker.Trim();//f**k
	strMarker.MakeUpper();

	CString strP = m_cfg_path;
	strP.Trim();

	if (strMarker == _T("CFG")||strMarker == get_cfg_path_reg_key())
	{
		retPath = strP;
		toolsvr::FixPathStr(retPath);
		return true;
	}
	else
	{ 
		CString CfgPath;
		CfgPath = strP + _T("CFGPATH");
		CStdioFile	file;

		CString rString;
		bool bFind = false;
		if (file.Open(CfgPath,CFile::modeRead ))
		{
			while (file.ReadString(rString))
			{
				rString.MakeUpper();
				rString.Trim();
				if (rString == strMarker)
				{
					file.ReadString(rString);
					rString.Trim();
					bFind = true;
					break;
				}
			}
			file.Close();
		}
		
		if (bFind = true)//张老师，啥时候改?
		{

			toolsvr::FixPathStr(rString);
			retPath = rString;
		}

		return bFind;
	}

	return false;
}

//Alime
using namespace Alime::HttpUtility;
bool CHtmlDlg::VersionPage()
{
	HttpRequest req;
	req.server = serverIp;
	req.query = L"/index.html";
	req.acceptTypes.push_back(L"text/html");
	req.acceptTypes.push_back(L"application/xhtml+xml");
	req.method = L"Get";
	req.port = 80;
	req.secure=false;
	HttpResponse res;
	HttpQuery(req, res);
	if (200 != res.statusCode)
	{
#ifdef _DEBUG
		MessageBox("请检查联网功能");
#endif // _DEBUG
		return false;
	}
	////////////else///////////////////
	auto result = res.GetBodyUtf8();
	std::wstring re = result.c_str();
	auto astring = WStringToString(re);
	std::cout << astring << std::endl;
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::tr1::regex reg("(<(body)>)([\\s\\S]*)(</\\2>)");
		//tr版本的正则表达式无法匹配"空白符"和字符串的组合
		//***这么处理，字面量中不允许出现空格
		if(_MSC_VER<=1600)
			astring.erase(std::remove_if(astring.begin(),astring.end(),[](char c){return c=='\n'|| c=='\r'||c==' ';}),astring.end());
		auto re = std::regex_search(astring, reg);
		std::tr1::smatch match;
		if (std::tr1::regex_search(astring, match, reg))
		{
			PageInfo = match[3];
			return true;
		}
	}
	return false;
}

BSTR CHtmlDlg::TellMeAdvertisement()
{
	if(PageInfo.empty())
	{
		if(!VersionPage())
		{
			//处处要为巨婴考虑
			return _com_util::ConvertStringToBSTR(defaultAdvertise);
		}
	}
	if(PageInfo.empty())
	{
		assert(!PageInfo.empty());
		return _com_util::ConvertStringToBSTR(defaultAdvertise);
	}
	if(!PageInfo.empty())
	{
		if(VersionPage())
		{
			std::vector<std::pair<std::string,std::string>> data;
			//////分成两部分~,方便调试//////////////////////
			Document document;
			document.Parse(PageInfo.c_str());
			auto& arr = document["Advertise"]["NationWide"];
			assert(arr.IsArray());
			for (size_t i = 0; i < arr.Size(); ++i)
			{
				std::string adver_content(arr[i]["Advertisement"].GetString());
				std::string adver_url(arr[i]["Url"].GetString());
				data.push_back(std::make_pair(adver_content,adver_url));
			}
			//////////begin/////////////////////
			Document doc;
			rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

			Value array(kArrayType);//< 创建一个数组对象
			for(int i=0;i!=data.size();++i)
			{
				rapidjson::Value obj(rapidjson::kObjectType);
				//Value key(kStringType);
				//key.SetString(data[i].first.c_str(), allocator);
				//Value value(kStringType);
				//value.SetString(data[i].second.c_str(), allocator);
				//obj.AddMember(key,value,allocator);
				Value content(kStringType);
				content.SetString(data[i].first.c_str(), allocator);
				obj.AddMember("key",content,allocator);
				Value url(kStringType);
				url.SetString(data[i].second.c_str(), allocator);
				obj.AddMember("value",url,allocator);
				array.PushBack(obj,allocator);
			}

			Value root(kObjectType);
			root.AddMember("data",array,allocator);
			rapidjson::StringBuffer s;
			rapidjson::Writer<rapidjson::StringBuffer> writer(s);
			root.Accept(writer);
			std::string toSend = s.GetString();//for debug
			return _com_util::ConvertStringToBSTR(s.GetString());
			///end/////////
		}
	}
	return _com_util::ConvertStringToBSTR(defaultAdvertise);
}


BSTR CHtmlDlg::TellMeNewVersionExistOrNot()
{
	if(PageInfo.empty())
	{
		//注意，本函数是同步的
		if(!VersionPage())//失败
		{
			return _com_util::ConvertStringToBSTR("false");
		}
	}
	//OK!!!
	//提供版本信息
	{
		Document document;
		document.Parse(PageInfo.c_str());
		assert(document.HasMember("UpdateUrl"));
		assert(document.HasMember("Advertise"));
		auto& arr = document["Advertise"]["NationWide"];
		assert(arr.IsArray());
#ifdef ALIME_DEBUG
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			std::string keyName = "Advertisement";
			std::string adver(arr[i][keyName.c_str()].GetString());
			CString str;
			str.Format("advertisement = %s\n", adver.c_str());
			OutputDebugString(str);
			str.Format("url = %s\n", arr[i]["Url"].GetString());
			OutputDebugString(str);
		}
#endif // _DEBUG
		int MainVersionOnServer = std::stoi(document["Version"]["MainVersion"].GetString());
		int ViceVersionOnServer = std::stoi(document["Version"]["ViceVersion"].GetString());
		int SubVersionOnServer = std::stoi(document["Version"]["SubVersion"].GetString());
		int mv=-1, vv=-1, sv=0;
		CString VersionPath=GetAppPath()+"CFG";
		auto vec=FindVersionFiles(VersionPath,"V","ini");
		if(!vec.empty())
		{
			getLatestVersion(vec,mv,vv,sv);
			//偷懒
			if (MainVersionOnServer != mv || ViceVersionOnServer != vv || SubVersionOnServer!=sv)
				return _com_util::ConvertStringToBSTR("true");
			else
				return _com_util::ConvertStringToBSTR("false");
		}
	}
	return _com_util::ConvertStringToBSTR("false");
////////////////////////////////////////////以下为废弃的代码/////////////////////////////////////////
	CString appPath=GetAppPath();
	CString localFileName= appPath+"CFG\\CheckVersion.ini";
	CString SrvAddress = _T("http://111.230.143.87/pkpm.txt");
	if (S_OK==URLDownloadToFile(NULL, SrvAddress, localFileName, 0, NULL))
	{
		int MainVersionOnServer = GetPrivateProfileInt(_T("Version"), _T("MainVer"), 10, localFileName);
		int ViceVersionOnServer = GetPrivateProfileInt(_T("Version"), _T("ViceVer"), 10, localFileName);
		int SubVersionOnServer = GetPrivateProfileInt(_T("Version"), _T("SubVer"), 10, localFileName);
		int mv=-1, vv=-1, sv=0;
		CString VersionPath=GetAppPath()+"CFG";
		auto vec=FindVersionFiles(VersionPath,"V","ini");
		//刘超提供的逻辑
		if(!vec.empty())
		{
			getLatestVersion(vec,mv,vv,sv);
			//偷懒
			if (MainVersionOnServer != mv || ViceVersionOnServer != vv || SubVersionOnServer!=sv)
				return _com_util::ConvertStringToBSTR("true");
		}
		else
			return _com_util::ConvertStringToBSTR("false");
	}
	return _com_util::ConvertStringToBSTR("false");
}

std::vector<std::string> CHtmlDlg::FindVersionFiles(
		const char* path, 
		const char* prefix, 
		const char* suffix)
	{
		std::string toFind(path);
		toFind += "\\*.";//时间再 一次很赶
		toFind +=suffix;
		std::vector<std::string> result;
		long handle;		//用于查找的句柄
		_finddata_t fileinfo;
		handle = _findfirst(toFind.c_str(), &fileinfo);
		if (handle == -1)
			return result;
		do
		{
			if (startWith(fileinfo.name, prefix)/* && endWith(fileinfo.name, ".ini")*/)
			{
				std::string filename(fileinfo.name);
				filename = filename.substr(1, filename.size() - 5);
				result.push_back(filename);
			}				
		} while (!_findnext(handle, &fileinfo));
		_findclose(handle);
		return result;
	}

	void CHtmlDlg::getLatestVersion(std::vector<std::string> &result, 
		int& major_version, int&minor_version, int &sub_version)
	{
		if (result.empty())
			throw std::exception("vector empty");
		std::sort(result.begin(), result.end(), 
			[](const std::string& s1,const std::string &s2)->bool {
				auto num_in_s1  = string_split(s1,".");
				auto num_in_s2 = string_split(s2, ".");
				int size_s1 = num_in_s1.size();
				int size_s2 = num_in_s2.size();
				int n = size_s1 < size_s2 ? size_s1 : size_s2;
				for (int i = 0; i != n; ++i)
				{
					auto num1 = atoi(num_in_s1[i].c_str());
					auto num2 = atoi(num_in_s2[i].c_str());
					if (num1 != num2)
						return num1 - num2<0? true : false;
				}
				return size_s1 < size_s2 ? true :false;
		});
		auto ret=string_split(result.back(),".");
		if(ret.size()<3)
		{
			sub_version=0;
		}		
		else
		{
			sub_version=std::stoi(ret[2]);
		}		
		major_version = std::stoi(ret[0]);
		minor_version = std::stoi(ret[1]);
	}

	bool CHtmlDlg::SetCfgPmEnv()
	{
		TCHAR *szOriEnvPath = new  TCHAR[LENGTH_OF_ENV];//所以这片内存去哪了....?
		DWORD dwRet = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, LENGTH_OF_ENV);
		if(!dwRet)
		{
			AfxMessageBox("Error! Can not find Path");
			return false;
		}
		else if (LENGTH_OF_ENV<dwRet)//需要重新分配内存
		{
			delete[] szOriEnvPath;
			szOriEnvPath = new  TCHAR[dwRet+1];
			DWORD dwRetNewAlloc;
			dwRetNewAlloc = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, dwRet);
			ASSERT(dwRetNewAlloc<=dwRet);
		}

		CString strCFG = svr::GetRegCFGPath();
		CString strPM;
		if(false == svr::getPathByMaker("PM",strPM))
		{
			AfxMessageBox("无法找到PM的路径 ");
			return false;
		}
		CString strPath;
		strPath = strCFG + ";";
		strPath += strPM + ";";
		strPath += szOriEnvPath;
		strPath.Trim();
		int	iv= SetEnvironmentVariable(_T("PATH"),strPath);
		if (iv==0)
		{
			AfxMessageBox("无法设置PATH路径");
			return false;
		}
		else
		{
			return true;
		}
		return false;
	}

	//static
	void CHtmlDlg::SaveWorkPaths(collection_utility::BoundedQueue<std::string>& prjPaths, const std::string& filename)
	{
		for (ULONGLONG i = 0; i < maxPrjNum; ++i)
		{
			std::string workPathId = std::string("WorkPath") + std::to_string(i);
			if (static_cast<size_t>(i) < prjPaths.size())
				WritePrivateProfileString("WorkPath", workPathId.c_str(), prjPaths[i].c_str(), filename.c_str());
			else//de a bug 
				WritePrivateProfileString("WorkPath", workPathId.c_str(), NULL, filename.c_str());
		}
	}

HRESULT STDMETHODCALLTYPE CHtmlDlg::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	static const int accelerators[] = { 'N','F','L','I','P','O','A' };
	if (lpMsg && lpMsg->message == WM_KEYDOWN)
	{
		if (0x80 == (0x80 & GetKeyState(VK_CONTROL)))
		{
			if((accelerators + sizeof(accelerators))!=
				std::find(accelerators,accelerators+sizeof(accelerators),lpMsg->wParam))
				return S_OK;
			if ('V' == lpMsg->wParam || 'v' == lpMsg->wParam)
			{
				FunctionIgnoreError(std::bind(&CHtmlDlg::PathFromClipBoard,this));
			}
		}
		else if (lpMsg->wParam == VK_F5
			|| lpMsg->wParam == VK_ESCAPE || lpMsg->wParam == VK_RETURN)
			return S_OK;
		else if (lpMsg->wParam == VK_TAB)
			return S_OK;
		else if(lpMsg->wParam == VK_DELETE)
		{
			static unsigned long  lastTick=0;
			auto now=GetTickCount();
			auto interval=now-lastTick;
			lastTick=now;
			if(interval<500)	return S_OK;
			try{
			if(prjPaths_.size()>0)
			{
				auto prjSelected=std::stoi(MenuSelectionOnHtml().back().second);
				//没有工程为什么不传-1?
				prjPaths_.deleteAt(prjSelected);
				SaveWorkPaths(prjPaths_,FullPathOfPkpmIni().GetBuffer());
				SaveMenuSelection(false);
				/*Fix me
				Refresh不能在别的线程调用，而mfc又有自己的事件循环，
				所以，只能在documentComplete里做判断。
				我这里先对付过去了。
				你应该判断上一次keyup和这次keydown的间隔
				*/
				this->m_pBrowserApp->Refresh();
				return S_OK;
			}}catch(...){
				//FATAL<<
			}
		}
	}
	return CDHtmlDialog::TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
}

void CHtmlDlg::PathFromClipBoard()
{
	/*
	CRect rc;
	GetWindowRect(rc);
	CPoint cp;
	GetCursorPos(&cp);
	if(!PtInRect(&rc,cp))
		return;
		*/
	std::string filePath;
	if(OpenClipboard())
	{
		if(IsClipboardFormatAvailable(CF_TEXT))
		{
			HANDLE hclip=GetClipboardData(CF_TEXT);
			char* pBuf=static_cast<char*>(GlobalLock(hclip));
			LocalUnlock(hclip);
			USES_CONVERSION;
			filePath=std::move(std::string(pBuf));
		}
		CloseClipboard();
	}
	if(!filePath.empty())
	{
		if(PathFileExists(filePath.c_str())&&PathIsDirectory(filePath.c_str()))
		{
			filePath+="\\";
			if(AddWorkPaths(filePath,FullPathOfPkpmIni().GetBuffer()))
			{
				SaveMenuSelection(false);
				this->m_pBrowserApp->Refresh();
			}
				
		}
		else
		{
			//LOG_ERROR<<"invalid path";
		}
	}
}


void CHtmlDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CDHtmlDialog::OnDocumentComplete(pDisp, szUrl);
	// TODO: 在此添加专用代码和/或调用基类
	if (!strcmp(szUrl, ""))
		return;
	IHTMLDocument2* pIHtmlDoc = NULL;
	GetDHtmlDocument(&pIHtmlDoc);
	FalseIsSometimesCouldBeTolerable(pIHtmlDoc);

	CComPtr<IHTMLElement> spElement;
	pIHtmlDoc->get_body(&spElement);
	FalseIsSometimesCouldBeTolerable(spElement);

	CComPtr<IHTMLBodyElement> spBodyElement;
	spElement->QueryInterface(__uuidof(IHTMLBodyElement), (void**)&spBodyElement);
	FalseIsSometimesCouldBeTolerable(spBodyElement);

	spBodyElement->put_scroll(_bstr_t(_T("auto")));
	//spBodyElement->put_scroll(_bstr_t(_T("auto")));
	pIHtmlDoc->Release();
	documentLoaded=true;
}

std::string DictToJson(const std::vector<std::pair<std::string, std::string>>& dict)
{
	rapidjson::Document doc;
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	Value root(kObjectType);
	for (auto i = 0; i != dict.size(); ++i)
	{
		rapidjson::Value strObject(rapidjson::kStringType);
		strObject.SetString(dict[i].first.c_str(), allocator);
		rapidjson::Value strObjectValue(rapidjson::kStringType);
		strObjectValue.SetString(dict[i].second.c_str() , allocator);
		root.AddMember(strObject, strObjectValue, allocator);
	}
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	root.Accept(writer);
	std::string result = s.GetString();
	//MessageBox(NULL, result.c_str(), "test", 1);
	return s.GetString();
}

BSTR CHtmlDlg::OnGetDefaultMenuSelection()
{
	std::vector<std::pair<std::string, std::string>> dict;
	const int n = sizeof(toRead) / sizeof(*toRead);//不要骗自己了，这个程序没有可能国际化
	char indexRet[32] = { 0 };
	for (size_t i = 0; i!= n; ++i)
	{
		memset(indexRet, 0, sizeof(indexRet));
		GetPrivateProfileStringA("Html", toRead[i], "error", indexRet, sizeof(indexRet), FullPathOfPkpmIni());
		dict.push_back(std::make_pair(toRead[i], indexRet));
	}
	auto json_str = DictToJson(dict);
	return _com_util::ConvertStringToBSTR(json_str.c_str());
}

std::vector<std::pair<std::string, std::string>> CHtmlDlg::MenuSelectionOnHtml()
{
	_variant_t  pVarResult;
	CString args = "uselessArg";
	JavaScriptFuncInvoker("postMenuIndex1", &pVarResult);
	CString selections;
	assert(pVarResult.vt == VT_BSTR);
	selections = pVarResult.bstrVal;
	std::string stdStr(selections.GetBuffer());
	auto vec = string_split(stdStr, ",");
	assert(vec.size() == ArraySize(toRead));
	std::vector<std::pair<std::string, std::string>> dict;
	for (int i = 0; i != ArraySize(toRead); ++i)
	{
		dict.push_back(std::make_pair(toRead[i], vec[i]));
	}
	return dict;
}

std::string CHtmlDlg::OnGetSelectedPrj()
{
	auto vec=MenuSelectionOnHtml();
	auto index=vec.back().second;
	std::string name;
	if(prjPaths_.size()>0)
		name=prjPaths_[std::stoi(index)];
	return name;
}

void CHtmlDlg::OnSetDefaultMenuSelection(BSTR json_str)
{
	CComVariant varStr(json_str);
	varStr.ChangeType(VT_BSTR);
	USES_CONVERSION;
	std::string str(OLE2T(varStr.bstrVal));

	Document d;
	d.Parse(str.c_str());
	size_t i = 0;
	for (; i != ArraySize(toRead)-1; ++i)
	{
		Value&s = d[toRead[i]];
		auto indexSelection = s.GetString();
		WritePrivateProfileString("Html", toRead[i], indexSelection, FullPathOfPkpmIni());
	}
	Value&s = d[toRead[i]];
	int indexSelection=atoi(s.GetString());
	if(!prjPaths_.size() || !indexSelection)
		return;
	prjPaths_.moveToFront(indexSelection);
	SaveWorkPaths(prjPaths_, FullPathOfPkpmIni().GetBuffer());
}

//我不太确定当初网页发生了什么才导致我要重写
//OnSetDefaultMenuSelection，但显然问题提前解决了
//需要检查本函数是否被js调用
void CHtmlDlg::OnSetDefaultMenuSelection2(BSTR json_str)
{
	CComVariant varStr(json_str);
	varStr.ChangeType(VT_BSTR);
	USES_CONVERSION;
	std::string str(OLE2T(varStr.bstrVal));

	Document d;
	d.Parse(str.c_str());
	size_t i = 0;
	for (; i != ArraySize(toRead)-1; ++i)
	{
		Value&s = d[toRead[i]];
		auto indexSelection = s.GetString();
		WritePrivateProfileString("Html", toRead[i], indexSelection, FullPathOfPkpmIni());
	}
	Value&s = d[toRead[i]];
	int indexSelection=atoi(s.GetString());
	prjPaths_.moveToFront(indexSelection);
	SaveWorkPaths(prjPaths_, FullPathOfPkpmIni().GetBuffer());
}

void CHtmlDlg::OnRightClickProject(BSTR prj)
{
	CComVariant varStr(prj);
	varStr.ChangeType(VT_BSTR);
	USES_CONVERSION;
	std::string path(OLE2T(varStr.bstrVal));
	ShellExecute(NULL, "open", "explorer.exe", path.c_str(), NULL, SW_SHOWNORMAL);
}

/*
浏览器先于对话框处理事件，所以部分事件
没有办法捕获。网页需要禁用右键、拖拽，这里才有机会
拿到消息。
*/
BOOL CHtmlDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_LBUTTONDBLCLK)
	{
		CPoint cp;
		GetCursorPos(&cp);
		ScreenToClient(&cp);
		auto elem=GetIdByPos(cp);
		if (elem=="headerTest")//PtInRect(&rc, lpPoint))
		{
			OnResume();
			return TRUE;
		}
		else
		{
			return CDHtmlDialog::PreTranslateMessage(pMsg);
		}
	}
	if (pMsg->message == WM_MOUSEMOVE)
	{
		//神奇的内网机器。标题栏的消息全消失了...
		//if (GetAsyncKeyState(VK_LBUTTON))
		if(1)
		{
			if(!IsFullScreen()&&documentLoaded)
				SwitchSrcOfZoomAll(true);
		}
	}
	
	if (pMsg->message == WM_KEYDOWN)
	{
		if (0x80 == (0x80 & GetKeyState(VK_CONTROL)))
		{
			if ('V' == pMsg->wParam || 'v' == pMsg->wParam)
			{
				FunctionIgnoreError(std::bind(&CHtmlDlg::PathFromClipBoard,this));
				return TRUE;
			}
		}
	}
	
	return CDHtmlDialog::PreTranslateMessage(pMsg);
}

bool CHtmlDlg::IsFullScreen()
{
	if (rectCached.IsRectEmpty())
	{
		return false;
		//你看，逻辑错误都变得合理了
		throw std::logic_error("rectCached not inited");
	}		
	CRect rc;
	GetWindowRect(rc);
	return rc == rectCached;
}

std::string CHtmlDlg::GetIdByPos(const CPoint cp)
{
	CComPtr<IHTMLElement> element;
	m_spHtmlDoc->elementFromPoint(cp.x, cp.y,&element);
	BSTR id;
	element->get_id(&id);
	if(id==NULL)
		return "";
	std::string str=_com_util::ConvertBSTRToString(id);
	return str;
}

BSTR CHtmlDlg::OnOpenProject()
{
	return OnNewProject();
}

void CHtmlDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.x = miniCx_;
	lpMMI->ptMinTrackSize.y = miniCy_;
	CDHtmlDialog::OnGetMinMaxInfo(lpMMI);
}


void CHtmlDlg::OnDestroy()
{
	SaveMenuSelection();
	CDHtmlDialog::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
	
}

void CHtmlDlg::SaveMenuSelection(bool savePrjIndex)
{
	//c++调用js函数出现了bug
	//这里不同机器出现了不同的现象!!!!!
	//我笔记本编译的程序在内网崩溃0.0
	//保留这段代码
	if (0)
	{
		IHTMLDocument2* pDocument;
		IHTMLWindow2* pWindow;
		HRESULT hr = GetDHtmlDocument(&pDocument);
		ASSERT(SUCCEEDED(hr));
		hr = pDocument->get_parentWindow(&pWindow);
		CComBSTR bstrScriptName = TEXT("postMenuIndex1()");
		CComVariant ret;// 这个ret变量其实不能获取javascript的返回值
		//ret.vt = VT_BSTR;
		//ret.ChangeType(VT_EMPTY);
		hr = pWindow->execScript(bstrScriptName, L"Javascript", &ret);
		if (FAILED(hr))
		{
			CComBSTR bstrErrorInfo;
			IErrorInfoPtr errPtr;
			GetErrorInfo(0, &errPtr);
			errPtr->GetDescription(&bstrErrorInfo);
		}
		else
			return;
	}
	std::vector<std::pair<std::string, std::string>> dict=MenuSelectionOnHtml();
	if(savePrjIndex==false)
		dict.back().second="0";
	auto json = DictToJson(dict);
	BSTR bstrText = _com_util::ConvertStringToBSTR(json.c_str());	
	OnSetDefaultMenuSelection(bstrText);
	SysFreeString(bstrText);
}

BOOL CHtmlDlg::JavaScriptFuncInvoker(const CString strFunc, _variant_t* pVarResult)
{
	CStringArray paramArray;
	return InvokeJsFunc(strFunc, paramArray, pVarResult);
}

BOOL CHtmlDlg::JavaScriptFuncInvoker(const CString strFunc, const CString strArg1, _variant_t* pVarResult)
{
	CStringArray paramArray;
	paramArray.Add(strArg1);
	return InvokeJsFunc(strFunc, paramArray, pVarResult);
}

BOOL CHtmlDlg::InvokeJsFunc(const CString strFunc, const CStringArray& paramArray, _variant_t* pVarResult)
{
	CComPtr<IDispatch> spScript;
	if (m_spHtmlDoc == NULL)
		return FALSE;

	HRESULT hr = m_spHtmlDoc->get_Script(&spScript);
	if (!SUCCEEDED(hr))
	{
		MessageBox("函数GetJScrip调用失败！");
		return FALSE;
	}
	CComBSTR bstrFunc(strFunc);
	DISPID dispid = NULL;
	//hr = spScript->GetIDsOfNames(IID_NULL, &bstrFunc, 0, LOCALE_SYSTEM_DEFAULT, &dispid);
	//LOCALE_NAME_INVARIANT
	//hr = spScript->GetIDsOfNames(IID_NULL, &bstrFunc, 0, LOCALE_SYSTEM_DEFAULT, &dispid);
	hr = spScript->GetIDsOfNames(IID_NULL, &bstrFunc, 1, LOCALE_SYSTEM_DEFAULT , &dispid);
	if (FAILED(hr))
	{
		//MessageBox(GetSystemErrorMessage(hr));
		return FALSE;
	}

	INT_PTR arraySize = paramArray.GetSize();

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = (UINT)arraySize;
	dispparams.rgvarg = new VARIANT[dispparams.cArgs];

	for (int i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray.GetAt(arraySize - 1 - i); // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}
	dispparams.cNamedArgs = 0;

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	_variant_t vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg

	hr = spScript->Invoke(dispid, IID_NULL, 0,
		DISPATCH_METHOD, &dispparams, &vaResult, &excepInfo, &nArgErr);

	delete[] dispparams.rgvarg;
	if (FAILED(hr))
	{
		//MessageBox(GetSystemErrorMessage(hr));
		return FALSE;
	}

	if (pVarResult)
	{
		*pVarResult = vaResult;
	}
	return TRUE;
}

void CHtmlDlg::OnOpenDocument(BSTR rootInRibbon, BSTR filePath)
{
	CComVariant root(rootInRibbon);
	root.ChangeType(VT_BSTR);
	CComVariant pathStr(filePath);
	pathStr.ChangeType(VT_BSTR);
	USES_CONVERSION;
	std::string FullPath(GetExePath()+"\\"+"Ribbon\\");
	FullPath+=OLE2T(root.bstrVal);
	FullPath+=OLE2T(pathStr.bstrVal);

	SHELLEXECUTEINFO ShExecInfo;
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS ;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpFile = FullPath.c_str();
	ShExecInfo.lpParameters = _T(""); 
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL; 
	ShellExecuteEx(&ShExecInfo);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//下方的函数未完成     //
//                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int parseTopLevelMenu(const std::string &text)
{
	std::vector<std::string> vec = string_split(text, "\r\n");
	vec.erase(std::remove_if(vec.begin(), vec.end(), [](const std::string &s) {return s.find("LI") == std::string::npos; }), vec.end());
	for (int i = 0; i != vec.size(); ++i)
		if (vec[i].find("active") != std::string::npos)
			return i;
	return -1;
}

std::pair<int,int> parseSecAndTrdLevelMenu(const std::string &text, const std::string &item)
{
	int result_second_menu = -1;
	int result_third_menu = -1;
	std::vector<std::string> vec = string_split(text, "\r\n");
	auto copy = vec;
	auto iter= std::remove_if(vec.begin(), vec.end(),
		[](const std::string &s)
	{
		return s.find("modelLI") == std::string::npos;
	});
	vec.erase(iter, vec.end());
	for (int i = 0; i < vec.size(); ++i)
	{
		if (vec[i].find("menuActive") != std::string::npos)
		{
			result_second_menu = i;
			break;
		}
	}
	assert(result_second_menu != -1);
	int i = 0;
	for (; i != copy.size(); ++i)
	{
		if (copy[i].find("menuActive") != std::string::npos)
			break;
	}
	assert(i != copy.size());
	int j = i;
	for (; j < copy.size(); ++j)
	{
		if (copy[j].find("active") != std::string::npos && copy[j].find("LI") != std::string::npos)
		{
			result_third_menu = j - i - 1;
			break;
		}
	}
	if (copy.size() == j)
	{
		//if(item

	}

	return std::make_pair(result_second_menu, result_third_menu);

}



int CHtmlDlg::parseHtmlText()
{
	//要返回4个整数0.0
	int result = 0;
	{
		std::string elem_navi = "navi";
		auto text_1Level = GetElementHtml(elem_navi.c_str());
		std::string text_string = _com_util::ConvertBSTRToString(text_1Level);
		int index = parseTopLevelMenu(text_string);
		if (index < 0)
			throw("bad index");
		result = index << 24;
	}
	{
		std::string elem_secondLevelMenu = "modelUi";
		auto text_2Level = GetElementHtml(elem_secondLevelMenu.c_str());
		std::string  text_string_second = _com_util::ConvertBSTRToString(text_2Level);

		std::string elem_thirdLevelMenu = "selectText_";
		auto text_3Level = GetElementHtml(elem_thirdLevelMenu.c_str());
		std::string  text_string_third = _com_util::ConvertBSTRToString(text_2Level);

		auto indexs = parseSecAndTrdLevelMenu(text_string_second, text_string_third);
	}
	return 0;
}

void CHtmlDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	//CDHtmlDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 在此处添加消息处理程序代码
	if(WA_ACTIVE==nState||WA_CLICKACTIVE==nState)
	{
		if(1)
		{
			if(!rectForActive.IsRectEmpty())
				MoveWindow(rectForActive);
			if(IsFullScreen())
			{
				SwitchSrcOfZoomAll(false);
			}
		}
		::SetWindowPos(this->m_hWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//上方的函数未完成     //
//                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CHtmlDlg::OnNcHitTest(CPoint point)
{
	CPoint ptCur;
	CRect rect;
	GetCursorPos(&ptCur);
	GetWindowRect(&rect);
	GetWindowRect(rect);
	const int conner = 15;
	const int edge = 5;
	if (CRect(rect.left, rect.top, rect.left + conner, rect.top + conner).PtInRect(ptCur))
		return HTTOPLEFT;
	else if (CRect(rect.right - conner, rect.top, rect.right, rect.top + conner).PtInRect(ptCur))
		return HTTOPRIGHT;
	else if (CRect(rect.left, rect.bottom - conner, rect.left + conner, rect.bottom).PtInRect(ptCur))
		return HTBOTTOMLEFT;
	else if (CRect(rect.right - conner, rect.bottom - conner, rect.right, rect.bottom).PtInRect(ptCur))
		return HTBOTTOMRIGHT;
	else if (CRect(rect.left, rect.top, rect.left + edge, rect.bottom).PtInRect(ptCur))
		return HTLEFT;
	else if (CRect(rect.right - edge, rect.top, rect.right, rect.bottom).PtInRect(ptCur))
		return HTRIGHT;
	else if (CRect(rect.left, rect.top, rect.right - edge, rect.top + edge).PtInRect(ptCur)) // 128 Min,Max,Close
		return HTTOP;
	else if (CRect(rect.left, rect.bottom - edge, rect.right, rect.bottom).PtInRect(ptCur))
		return HTBOTTOM;
	return CDHtmlDialog::OnNcHitTest(point);
}

void CHtmlDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	auto mMouseStates = nHitTest;
	switch (mMouseStates)
	{
	case HTLEFT:
	case HTRIGHT:
		::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		break;
	case HTTOP:
	case HTBOTTOM:
		::SetCursor(LoadCursor(NULL, IDC_SIZENS));
		break;
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		::SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		break;
	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		::SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		break;
	default:
		::SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	}

	if (mMouseStates == HTTOP)
		PostMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, MAKELPARAM(point.x, point.y));
	else if (mMouseStates == HTBOTTOM)
		PostMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, MAKELPARAM(point.x, point.y));
	else if (mMouseStates == HTLEFT)
		PostMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT, MAKELPARAM(point.x, point.y));
	else if (mMouseStates == HTRIGHT)
		PostMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_RIGHT, MAKELPARAM(point.x, point.y));
	else if (mMouseStates == HTTOPLEFT)
		PostMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPLEFT, MAKELPARAM(point.x, point.y));
	else if (mMouseStates == HTBOTTOMRIGHT)
		PostMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMRIGHT, MAKELPARAM(point.x, point.y));
	else if (mMouseStates == HTTOPRIGHT)
		PostMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPRIGHT, MAKELPARAM(point.x, point.y));
	else if (mMouseStates == HTBOTTOMLEFT)
		PostMessage(WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMLEFT, MAKELPARAM(point.x, point.y));
	CDHtmlDialog::OnNcLButtonDown(nHitTest, point);
}

BOOL CHtmlDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCAPTION || nHitTest == HTSYSMENU ||
		nHitTest == HTMENU || nHitTest == HTCLIENT)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
	}
	else if (nHitTest == HTTOP || nHitTest == HTBOTTOM)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
	}
	else if (nHitTest == HTLEFT || nHitTest == HTRIGHT)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
	}
	else if (nHitTest == HTTOPLEFT || nHitTest == HTBOTTOMRIGHT)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));
	}
	else if (nHitTest == HTTOPRIGHT || nHitTest == HTBOTTOMLEFT)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW)));
	}
	else
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
	}
	return 0;
}
