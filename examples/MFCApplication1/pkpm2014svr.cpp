#include "StdAfx.h"
#include "pkpm2014svr.h"
#include <direct.h>
#include <tchar.h>
#include "io.h"
using namespace slim;

extern CString get_cfg_path_reg_key();

CString cfgpathsvr::GetRegCFGPath()
{
	CString strCFG;
	strCFG.Empty();
	if (GetAssignCFGPath(strCFG,false))
	{
		return strCFG;
	}
	else if(GetAssignCFGPath(strCFG,true))
	{
		return strCFG;
	}
	return strCFG;
}

bool cfgpathsvr::GetAppPathByCFGPATHMarker(CString strMarker,CString& retPath)
{
	if (FALSE == s_mapCfgpath.IsEmpty())
	{
		if(TRUE == s_mapCfgpath.Lookup(strMarker,retPath))
		{
			toolsvr::FixPathStr(retPath);
			return true;
		}
	}

	return false;

}

bool cfgpathsvr::LoadFileCFGPATH()
{
	bool bSuccess = false;
	//设置CFG
	CString strP = GetRegCFGPath();
	if (strP.IsEmpty())
	{
		return false;
	}
	strP.Trim();
	toolsvr::FixPathStr(strP);

	s_mapCfgpath.SetAt(_T("CFG"),strP);
	
	CString CfgPath;
	CfgPath = strP + _T("CFGPATH"); //现在CfgPath 编程了CFGPATH文件本身
	CStdioFile	file;

	CString strItem;
	CString strItemPath;
	strItem.Empty();
	CString rString;//用来遍历cfgpath文件中的每一行
	bool bFind = false;
	if (file.Open(CfgPath,CFile::modeRead|CFile::shareDenyNone ))
	{
		bSuccess = true;
		while (file.ReadString(rString))
		{
			rString.MakeUpper();
			rString.Trim();
			if(rString.IsEmpty())
			{
				continue;
			}

			if (rString.FindOneOf(_T(":\\"))<0)
			{
				strItem = rString;
				file.ReadString(rString);
				if (rString.FindOneOf(_T(":\\"))<0)
				{
					continue;
				}
				strItemPath = rString;
				toolsvr::FixPathStr(strItemPath);
				s_mapCfgpath.SetAt(strItem,strItemPath);

			}

		}
		file.Close();
	}

	return bSuccess;
}



bool cfgpathsvr::GetAssignCFGPath(CString& strCFG,bool bMarchine)
{
	CString path;
	path.Empty();
	HKEY hkey;
	//必需使用KEY_READ权限，windows7 必需用管理员身份运行

	long rt;
	if (bMarchine)
	{
		rt=RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\PKPM\\MAIN\\PATH"),0,KEY_READ,&hkey);
	}
	else
	{
		rt=RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\PKPM\\MAIN\\PATH"),0,KEY_READ,&hkey);
	}

	if(rt==ERROR_SUCCESS)
	{
		DWORD dwType;
		DWORD dwSize = MAX_PATH;
		BYTE  string[MAX_PATH+1];
		CString sKey=get_cfg_path_reg_key();
		ASSERT(!sKey.IsEmpty());
#ifdef _UNICODE
		char ssKey[100];
		int nn=WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK,sKey,sKey.GetLength(),ssKey,sizeof(ssKey),NULL,NULL);
		ssKey[nn]=0;
		rt = RegQueryValueExA (hkey, ssKey, NULL,	&dwType, (BYTE*)string, &dwSize);
#else
		rt = RegQueryValueExA (hkey, sKey, NULL,	&dwType, (BYTE*)string, &dwSize);
#endif
		if(rt==ERROR_SUCCESS)
		{
			path=string;
		}
		RegCloseKey(hkey);
	}
	toolsvr::FixPathStr(path);
	strCFG = path;
	return path.IsEmpty()?false:true;
}

// CString appsvr::GetExeParam( CString strEvn )
// {
// 	CString strParam;
// 	 
// 	if (_T("结构建模")==strEvn)
// 	{
// 		strParam = _T(" -PMCAD");
// 	}
// 	else if (_T("方案设计")==strEvn)
// 	{
// 		strParam = _T(" -PRJDEN");
// 	}
// 	else if (_T("SATWE分析设计")==strEvn)
// 	{
// 		strParam = _T(" -ANA");
// 	}
// 	else if (_T("基础设计")==strEvn)
// 	{
// 		strParam = _T(" -JC");
// 	}
// 	else if (_T("施工图设计")==strEvn)
// 	{
// 		strParam = _T(" -DRAWING");
// 	}
// 	else if (_T("楼板设计")==strEvn)
// 	{
// 		strParam = _T(" -SLAB");
// 	}
// 	else if (_T("EPDA")==strEvn)
// 	{
// 		strParam = _T(" -EPDA");
// 	}
// 	else if (_T("PUSH")==strEvn)
// 	{
// 		strParam = _T(" -PUSH");
// 	}
// 	else if (_T("钢结构施工图")==strEvn)
// 	{
// 		strParam = _T(" -DRAWING");
// 	}
// }

CString cfgpathsvr::GetAppPath()
{
	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL,exeFullPath,MAX_PATH);

	CString strPath;
	strPath = exeFullPath;
	int nSp = strPath.ReverseFind(_T('\\'));
	strPath = strPath.Left(nSp);
	toolsvr::FixPathStr(strPath);

	return strPath;
}

CString cfgpathsvr::GetAppIniFullPathName()
{
	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL,exeFullPath,MAX_PATH);
	CString strIniName;
	strIniName = exeFullPath;
	int nSp = strIniName.ReverseFind(_T('.'));
	strIniName = strIniName.Left(nSp);
	strIniName+=_T(".ini");
	return strIniName;
}


const int cfgpathsvr::ENV_PATH_LENGTH = 2096;
bool cfgpathsvr::s_bHasSetEnv = false;
LPVOID cfgpathsvr::SetCfgvmEn( )
{

	bool bIsDebugMode = appConfig::Get()->IsDebugMode();

	TCHAR *szOriEnvPath = new  TCHAR[2096];

	DWORD dwRet = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, ENV_PATH_LENGTH);
	if(0 == dwRet)
	{
		if (bIsDebugMode)
		{
			AfxMessageBox(_T("无法获得工程路径"));
		}

	}
	else if (ENV_PATH_LENGTH<dwRet)//需要重新分配内存
	{
		delete[] szOriEnvPath;

		szOriEnvPath = new  TCHAR[dwRet+1];

		DWORD dwRetNewAlloc;
		dwRetNewAlloc = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, dwRet);

		ASSERT(dwRetNewAlloc<=dwRet);
	}


	if (s_bHasSetEnv)//如果已经设置过了 直接返回
	{
		if(bIsDebugMode)
		{
			CString strHintFormat;
			strHintFormat = _T("环境路径已经设置过,路径为: %s");
			CString strHint;
			strHint.Format(strHintFormat,szOriEnvPath);
			AfxMessageBox ( strHint, MB_OK|MB_ICONEXCLAMATION );
		}

		LPVOID  env = /*(char *)*/GetEnvironmentStrings();

		delete []szOriEnvPath;
		return env;
	}


	//没有进行设置过则进行设置
	CString pathCFG,pathPM;
	bool bGetCFG,bGetPM;
	bGetCFG = cfgpathsvr::GetAppPathByCFGPATHMarker(_T("CFG"),pathCFG);//得到CFG 路径
	bGetPM = cfgpathsvr::GetAppPathByCFGPATHMarker(_T("PM"),pathPM);//得到PM路径

	ASSERT(true == bGetCFG&&true == bGetPM);

	CString strEnvPathWithCFGPM;
	{
		int i = 0;
		int nHasCFGPos= -1,nHasPmPos=-1;
		CArray<CString> Paths;
		if(SpliteEvnPath(szOriEnvPath,Paths)>0)
		{

			for (i=0;i<Paths.GetSize();i++)
			{
				if (0 == pathCFG.CompareNoCase(Paths.GetAt(i)))
				{
					nHasCFGPos = i;
					break;
				}
			}
			for (i=0;i<Paths.GetSize();i++)
			{
				if (0 == pathPM.CompareNoCase(Paths.GetAt(i)))
				{
					nHasPmPos = i;
					break;
				}
			}
		}

		CArray<CString,CString> PathNew;
		PathNew.Add(pathCFG);
		if (bGetPM)
		{
			PathNew.Add(pathPM);
		}

		for(i=0;i<Paths.GetSize();i++)
		{
			if (nHasCFGPos==i||nHasPmPos==i)
			{
				continue;
			}
			PathNew.Add(Paths.GetAt(i));
		}

		for (i=0;i<PathNew.GetSize()-1;i++)
		{
			strEnvPathWithCFGPM+=(PathNew.GetAt(i)+_T(";"));
		}
		strEnvPathWithCFGPM+=(PathNew.GetAt(i));

		delete []szOriEnvPath;
	}

	int len = strEnvPathWithCFGPM.GetLength();
	TCHAR* szNewEvn = new TCHAR[len+2];
	ZeroMemory(szNewEvn,(len+2)*sizeof(TCHAR));
	_tcscpy_s(szNewEvn,len+1,strEnvPathWithCFGPM);

	int	iv= SetEnvironmentVariable(_T("PATH"),szNewEvn);
	if(iv==0)
	{
		CString strHint;
		strHint = _T("无法设置环境变量：\n");
		strHint+= szNewEvn;
		AfxMessageBox(strHint,MB_OK|MB_ICONEXCLAMATION);
	}
	else
	{
		if (appConfig::Get()->IsDebugMode())
		{
			CString strEVN;
			CString strEVN2;
			strEVN2.Format(_T("CFG = %s \n"),pathCFG.GetBuffer());strEVN+=strEVN2;
			strEVN2.Format(_T("PM = %s \n\n"),pathPM.GetBuffer());strEVN+=strEVN2;
			strEVN2.Format(_T("Enviroment \'PATH\' = %s \n"),strEnvPathWithCFGPM.GetBuffer());strEVN+=strEVN2;

			AfxMessageBox ( strEVN, MB_OK|MB_ICONEXCLAMATION );
		}

		s_bHasSetEnv = true;//已经设置过了
	}
	delete[] szNewEvn;


	LPVOID  env = GetEnvironmentStrings();
	return env;

}


int cfgpathsvr::SpliteEvnPath(CString strEvnPath,CArray<CString>& Paths)
{
	Paths.RemoveAll();

	int nSpl = 0;
	CString strP;
	strEvnPath.TrimLeft();
	strEvnPath.TrimRight();
	strEvnPath.TrimLeft(_T(";"));
	strEvnPath.TrimRight(_T(";"));
	do 
	{
		strEvnPath.TrimLeft();
		strEvnPath.TrimRight();
		strEvnPath.TrimLeft(_T(";"));
		if (strEvnPath.GetLength()<=0)
		{
			break;
		}

		nSpl = strEvnPath.Find(_T(';'));

		if (nSpl>0)
		{
			strP = strEvnPath.Left(nSpl);
			strEvnPath.Delete(0,nSpl+1);
			toolsvr::FixPathStr(strP);
			Paths.Add(strP);

		}
		else if (0 == nSpl)
		{
			strEvnPath.Delete(0,nSpl+1);
		}
		if (nSpl<0)//有可能 1这里仅有一个路径  2可能是最后一个路径
		{
			strP =  strEvnPath;
			toolsvr::FixPathStr(strP);
			Paths.Add(strP);

		}

	} while (nSpl>=0);

	return Paths.GetSize();
}

CString cfgpathsvr::GetCurEnvirom()
{
	TCHAR *szOriEnvPath = new  TCHAR[2096];

	DWORD dwRet = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, ENV_PATH_LENGTH);
	if(0 == dwRet)
	{
		AfxMessageBox(_T("无法获得工程路径"));
	}
	else if (ENV_PATH_LENGTH<dwRet)//需要重新分配内存
	{
		delete[] szOriEnvPath;

		szOriEnvPath = new  TCHAR[dwRet+1];

		DWORD dwRetNewAlloc;
		dwRetNewAlloc = ::GetEnvironmentVariable(_T("PATH"), szOriEnvPath, dwRet);

		ASSERT(dwRetNewAlloc<=dwRet);
	}

	return szOriEnvPath;
}

CMapStringToString cfgpathsvr::s_mapCfgpath;

//////////////////////////////////////////////////////////////////////////
bool toolsvr::FixPathStr(CString &strPath)
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

bool toolsvr::CreateDirtory(const TCHAR *path)
{
	ASSERT(path);
	if(path==NULL)return false;
	int len=_tcslen(path);
	if(len==0)
	{
		return false;
	}
	return 0==_tmkdir(path);
}
bool toolsvr::DirectoryExist(const TCHAR *path)
{
	//return 0==_taccess(path,0);
	return true;
}
int toolsvr::CombMultPaths(const CArray<CString,CString>& paths, CString& str,TCHAR sp  )
{
	if (paths.GetSize()<=0)
	{
		return 0;
	}

	for (int i=0;i<paths.GetSize()-1;i++)
	{
		str += paths[i] + sp;
	}
	str += paths[paths.GetSize()-1];

	return paths.GetSize();
}

int toolsvr::SpliteMultPaths(CString strs,CArray<CString,CString>& paths,TCHAR sp)
{
	paths.RemoveAll();

	int nSpl = 0;
	CString strP;
	strs.TrimLeft();
	strs.TrimRight();
	strs.TrimLeft(sp);
	strs.TrimRight(sp);
	do 
	{
		strs.TrimLeft();
		strs.TrimRight();
		strs.TrimLeft(sp);
		if (strs.GetLength()<=0)
		{
			break;
		}

		nSpl = strs.Find(sp);

		if (nSpl>0)
		{
			strP = strs.Left(nSpl);
			strs.Delete(0,nSpl+1);
			toolsvr::FixPathStr(strP);
			paths.Add(strP);

		}
		else if (0 == nSpl)
		{
			strs.Delete(0,nSpl+1);
		}
		if (nSpl<0)//有可能 1这里仅有一个路径  2可能是最后一个路径
		{
			strP =  strs;
			toolsvr::FixPathStr(strP);
			paths.Add(strP);

		}

	} while (nSpl>=0);

	return paths.GetSize();
}




//////////////////////////////////////////////////////////////////////////
void datasvr::ReadPrivateProfile2000_workPaths(CStringArray& workPaths)
{
 
 	CString iniFile= cfgpathsvr::GetRegCFGPath()+_T("PKPM.ini");
	CString cstr_path, keyname;
	TCHAR inBuf[_MAX_PATH+1];
	DWORD nRead;
	int nMaxWorkPathNum=0;
	nMaxWorkPathNum= ReadPrivateProfile2000_MaxWorkPath();
	if(nMaxWorkPathNum<=0)
	{
		nMaxWorkPathNum=4;
	}

	CStringArray PreworkPaths;
	for(int i=0;i<nMaxWorkPathNum;i++)
	{
		keyname.Format(_T("WorkPath%d"),i);
		nRead= GetPrivateProfileString(_T("WorkPath"), keyname,	_T("C"), inBuf, _MAX_PATH, iniFile);
		if( nRead>=2 ) 
		{
			bool bHasAlready = false;
			cstr_path.Format(_T("%s"),inBuf);
			toolsvr::FixPathStr(cstr_path);
			if( -1!=_taccess(cstr_path,0))
			{
				for (int j=0;j<PreworkPaths.GetSize();j++)
				{
					if (0==cstr_path.CompareNoCase(PreworkPaths[j]))
					{
						bHasAlready = true;
						break;
					}
				}
				if (false == bHasAlready)
				{
					PreworkPaths.Add(cstr_path);
				}
				
			}
		}
	}

	workPaths.Append(PreworkPaths);

	return ;
}
void datasvr::DeletePrivateProfile2000_workPath(CString workPath)
{
	CString iniFile= PrivateProfile2000_IniFileFullPath();
	CString cstr_path, keyname;
	toolsvr::FixPathStr(workPath);

	//查重复
	CStringArray newWorkPaths;
	CStringArray workPaths;
	ReadPrivateProfile2000_workPaths(workPaths);
	
	for (int i=0;i<workPaths.GetSize();i++)
	{
		if(0 == workPaths[i].CompareNoCase(workPath))
		{
			workPaths.RemoveAt(i);
			break;
		}
	}

	int nMax=ReadPrivateProfile2000_MaxWorkPath();

	//首先是全部清理ini中的workpath
	for (int i=0;i<nMax;i++)
	{
		keyname.Format(_T("WorkPath%d"),i);
		WritePrivateProfileString(_T("WorkPath"), keyname, NULL, iniFile);
	}

	for(int i=0;i<nMax&&i<workPaths.GetSize();i++)
	{
		keyname.Format(_T("WorkPath%d"),i);
		WritePrivateProfileString(_T("WorkPath"), keyname, workPaths[i], iniFile);
	}
	if(workPaths.GetSize()>0)
	{
		WritePrivateProfileString(_T("PKPMMENU"), _T("WORK_PATH"), workPaths[0], iniFile);
	}

}
void datasvr::WritePrivateProfile2000_workPath(CString workPath)
{
	CString iniFile= PrivateProfile2000_IniFileFullPath();
	CString cstr_path, keyname;
	toolsvr::FixPathStr(workPath);
	//查重复
	CStringArray newWorkPaths;
	CStringArray workPaths;
	ReadPrivateProfile2000_workPaths(workPaths);
	CArray<int,int> nChong;
	for (int i=0;i<workPaths.GetSize();i++)
	{
		toolsvr::FixPathStr(workPaths[i]);
		
		if (0 == workPaths[i].CompareNoCase(workPath))
		{
			nChong.Add(i);
		}
	}

	for (int i=nChong.GetSize()-1;i>=0;i--)
	{
		workPaths.RemoveAt(nChong[i]);
	}

	newWorkPaths.Add(workPath);
	newWorkPaths.Append(workPaths);

	int nMax=ReadPrivateProfile2000_MaxWorkPath();

	//首先是全部清理ini中的workpath
	for (int i=0;i<nMax;i++)
	{
		keyname.Format(_T("WorkPath%d"),i);
		WritePrivateProfileString(_T("WorkPath"), keyname, NULL, iniFile);
	}

	for(int i=0;i<nMax&&i<newWorkPaths.GetSize();i++)
	{
		keyname.Format(_T("WorkPath%d"),i);
		WritePrivateProfileString(_T("WorkPath"), keyname, newWorkPaths[i], iniFile);
	}
	if(!workPath.IsEmpty())
	{
		WritePrivateProfileString(_T("PKPMMENU"), _T("WORK_PATH"), workPath, iniFile);
	}
}

void datasvr::ReadPrivateProfile2014_PKPMMAIN(CString& strPath, CString& strEXE )
{
	CString iniFile= cfgpathsvr::GetAppIniFullPathName();
	CString tempStr, keyname;
	TCHAR inBuf[_MAX_PATH+1];
	ZeroMemory(inBuf,sizeof(TCHAR)*(MAX_PATH+1));
	DWORD nRead = 0;
	nRead= GetPrivateProfileString(_T("LAUNCH"), _T("RUNEXE"),	_T("PKPMMAIN.exe"), inBuf, _MAX_PATH, iniFile);
	if( nRead>=2 ) 
	{
		tempStr.Format(_T("%s"),inBuf);
		strEXE = tempStr;
	}

	nRead = 0;
	ZeroMemory(inBuf,sizeof(TCHAR)*(MAX_PATH+1));
	nRead= GetPrivateProfileString(_T("LAUNCH"), _T("PATH"),_T("CFG"), inBuf, _MAX_PATH, iniFile);
	if (nRead>=2)
	{
		tempStr.Format(_T("%s"),inBuf);
		strPath = tempStr; 
	}

	return;
}

CString datasvr::PrivateProfile2000_IniFileFullPath()
{
	return   cfgpathsvr::GetRegCFGPath()+_T("PKPM.ini");
}

int datasvr::ReadPrivateProfile2000_MaxWorkPath()
{
	CString iniFile= cfgpathsvr::GetAppIniFullPathName();

	int nMaxWorkPathNum=0;
	nMaxWorkPathNum= GetPrivateProfileInt(_T("WorkPath"),_T("MaxPathName"),10,iniFile);
	if(nMaxWorkPathNum<=0)
	{
		nMaxWorkPathNum=4;
	}

	FixMaxWorkPathV31(nMaxWorkPathNum);

	return nMaxWorkPathNum;
}

void datasvr::FixMaxWorkPathV31(int& nMaxPath)
{
	nMaxPath = 12;
}

// void datasvr::ReadPrivateProfile()
// {
// 	CString iniFile= svr::GetRegCFGPath()+_T("PKPM.ini");
// 	CString cstr_path, keyname;
// 	TCHAR inBuf[_MAX_PATH+1];
// 	DWORD nRead;
// 	m_nMaxWorkPathNum=0;
// 	m_nMaxWorkPathNum= GetPrivateProfileInt(_T("WorkPath"),_T("MaxPathName"),10,iniFile);
// 	if(m_nMaxWorkPathNum<=0) m_nMaxWorkPathNum=4;
// 
// 	m_arrWorkPath.clear();
// 	for(int i=0;i<m_nMaxWorkPathNum;i++)
// 	{
// 		keyname.Format(_T("WorkPath%d"),i);
// 		nRead= GetPrivateProfileString(_T("WorkPath"), keyname,	_T("C"), inBuf, _MAX_PATH, iniFile);
// 		if( nRead>=2 ) 
// 		{
// 			cstr_path.Format(_T("%s"),inBuf);
// 			m_arrWorkPath.push_back(cstr_path);
// 		}
// 	}
// 
// 	int DestInterrupt= GetPrivateProfileInt(_T("CFG_DEVICE"),_T("DestInterrupt"),0,iniFile);
// 	if( DestInterrupt==255 )
// 	{
// 		m_bNetEdition= TRUE;
// 	}
// 	else
// 	{
// 		m_bNetEdition= FALSE;
// 
// 	}
// 
// 	//是否强制允许更新
// 	int nForce = GetPrivateProfileInt(_T("CFG_DEVICE"),_T("ForceAllowUpataExe"),0,iniFile);
// 	m_bForceAllowUpdate = (nForce==0)?false:true;
// 
// 	m_nVersion_APM= GetPrivateProfileInt(_T("APM控制参数"),_T("Version"),0,iniFile);
// 
// 	GetPrivateProfileString(_T("PKPMMENU"),_T("PKPMVersion"),_T("2005.04"),m_sVersion_PKPM,32,iniFile);
// 	//
// 	m_cxScreen= GetPrivateProfileInt(_T("PKPMMENU"),_T("cx_screen"),-1,iniFile);
// 	m_cyScreen= GetPrivateProfileInt(_T("PKPMMENU"),_T("cy_screen"),-1,iniFile);
// 	m_cxDlg= GetPrivateProfileInt(_T("PKPMMENU"),_T("cx_dlg"),-1,iniFile);
// 	m_cyDlg= GetPrivateProfileInt(_T("PKPMMENU"),_T("cy_dlg"),-1,iniFile);
// 	//
// 	GetPrivateProfileString(_T("PKPMMENU"),_T("WORK_PATH"),_T(""),m_sWorkPathLast,_countof(m_sWorkPathLast),iniFile);
// 	GetPrivateProfileString(_T("PKPMMENU"),_T("HEAD_PAGE"),_T(""),m_sHeadPage,_countof(m_sHeadPage),iniFile);
//}
IMPLEMENT_DYNCREATE(appConfig,CInfoBase)
const TCHAR * appConfig::MARKER_LAUNCHER_TOP = _T("LAUNCHER_TOP"); 
const TCHAR * appConfig::MARKER_TARGET = _T("TARGET");
const TCHAR * appConfig::MARKER_PATH= _T("PATH");
const TCHAR * appConfig::MARKER_RUNEXE= _T("RUNEXE");
const TCHAR * appConfig::MARKER_TYPE_BY_RECEIVEED_CMD= _T("TYPE_BY_RECEIVEED_CMD");
const TCHAR * appConfig::MARKER_CORE= _T("CORE");
const TCHAR * appConfig::MARKER_CORE_CATES= _T("CORE_CATES");
const TCHAR * appConfig::MARKER_CORE_CATE= _T("CORE_CATE");
const TCHAR * appConfig::MARKER_CONFIG = _T("CONFIG");
const TCHAR * appConfig::MARKER_DEBUGMODE= _T("DEBUGMODE");

const TCHAR * appConfig::MARKER_REG= _T("REG");

bool appConfig::readFrom( slim::XmlNode* pTopNode )
{
	XmlNode *pNode = NULL;
	pNode = pTopNode->findChild(MARKER_TARGET);
	if (NULL == pNode)
	{
		return false;
	}

	XmlNode * pPathNode = pNode->findChild(MARKER_PATH);
	if (NULL == pPathNode)
	{
		return false;
	}
	m_pathInCFGPATH = pPathNode->getString();


	XmlNode * pRunExeNode = pNode->findChild(MARKER_RUNEXE);
	if (NULL == pRunExeNode)
	{
		return false;
	}
	m_runexe = pRunExeNode->getString();



	XmlNode * pTypeByReceiveCmd = pTopNode->findChild(MARKER_TYPE_BY_RECEIVEED_CMD);
	if (NULL == pTypeByReceiveCmd)
	{
		return false;
	}

	slim::NodeIterator itor;
	for (XmlNode * pOneCore = pTypeByReceiveCmd->findFirstChild(MARKER_CORE,itor);
		pOneCore!=NULL;
		pOneCore = pTypeByReceiveCmd->findNextChild(MARKER_CORE,itor))
	{
		m_Cores.Add( pOneCore->getString());
	}


	XmlNode *pCoreCates = pTopNode->findChild(MARKER_CORE_CATES);
	if (NULL == pCoreCates)
	{
		return false;
	}

	//这里应该有三个
	if(false == readElements(pCoreCates,MARKER_CORE_CATE,m_CoreCates,RUNTIME_CLASS(CoreCate)))
	{
		return false;
	}


	XmlNode * pConfig = pTopNode->findChild(MARKER_CONFIG);
	if (NULL == pConfig)
	{
		return false;
	}



	XmlNode* pDebumode = pConfig->findChild(MARKER_DEBUGMODE);
	if (NULL == pDebumode)
	{
		return false;
	}
	m_isDebugMode = pDebumode->getBool();

	XmlNode* pReg = pConfig->findChild(MARKER_REG);
	if (NULL == pReg)
	{
		return false;
	}
	m_regRoot = pReg->getString();

	return true;
}

bool appConfig::writeTo( slim::XmlNode* pNode )
{
	return false;
}

bool appConfig::readFromFile( CString strPathFile )
{
	XmlDocument doc;
	if(false == doc.loadFromFile(strPathFile))
	{
		return false;
	}

	XmlNode *pTopNode =  doc.findChild(MARKER_LAUNCHER_TOP);
	if (NULL == pTopNode)
	{
		return false;
	}

	if(false == readFrom(pTopNode))
	{
		return false;
	}

	return true;
}

bool appConfig::readFromFile()
{
	CString strPathName = GetLauncherXmlFileFullPathName();
	if(false == readFromFile(strPathName))
	{
		return false;
	}
	return true;
}

CString appConfig::RunExeString()
{
	return m_runexe;
}
CString appConfig::pathInCFGPATH()
{
	return m_pathInCFGPATH;
}
void appConfig::SetCurCoreTypeByCmdLine(CString cmdline)
{
	cmdline.Trim();
	m_CurCoreType = cmdline;//当前类型就是命令行
}
CString appConfig::GetCurCoreType()
{
	return m_CurCoreType;
}
void appConfig::SetCurSelCombo(CString strSel)
{
	m_CurSelCombo = strSel;
	CurSelCombToReg();
}
CString appConfig::GetCurSelCombo()
{
	return m_CurSelCombo;
}
CString appConfig::GetCurSelComboCmdline()
{
	CoreCate* pCate = GetCurCoreCates();
	if (pCate)
	{
		for (int i = 0;i<pCate->GetCount();i++)
		{
			if(0==pCate->GetAt(i)->Caption.CompareNoCase(GetCurSelCombo()))
			{
				return pCate->GetAt(i)->Cmdline;
			}
		}
	}
	return _T("");
}
bool appConfig::IsDebugMode()
{
	return m_isDebugMode;
}
CString appConfig::GetLauncherXmlFileFullPathName()
{
	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL,exeFullPath,MAX_PATH);
	CString strXmlName;
	strXmlName = exeFullPath;
	int nSp = strXmlName.ReverseFind(_T('.'));
	strXmlName = strXmlName.Left(nSp);
	strXmlName+=_T(".xml");
	return strXmlName;
}

appConfig::appConfig()
{
	m_runexe.Empty();
	m_pathInCFGPATH.Empty();

	m_Cores.RemoveAll();
 	m_isDebugMode= false;
	m_CoreCates.RemoveAll();
}

appConfig::~appConfig()
{
	for (int i=0;i<m_CoreCates.GetSize();i++)
	{
		delete m_CoreCates[i];
	}
	m_CoreCates.RemoveAll();
}

CoreCate* appConfig::GetCoreCate( CString strCore )
{
	CoreCate* pOneCate = NULL;
	for(int i=0;i<m_CoreCates.GetSize();i++)
	{
		pOneCate = (CoreCate*)(m_CoreCates.GetAt(i));

		if(0 == pOneCate->Core().CompareNoCase(strCore))
		{
			return pOneCate;
		}
	}

	return NULL;
}

bool appConfig::CurSelCombFromReg()
{
	const TCHAR*key=m_regRoot;

	HKEY hk;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,key,0,KEY_READ,&hk))
	{
		TCHAR vv[200]={0};
		DWORD len=200;
		DWORD type;
		int rt=RegQueryValueEx(hk,m_CurCoreType,0,&type,(BYTE*)vv,&len);
		RegCloseKey(hk);
		m_CurSelCombo = vv;
		return true;
	}
	return false;
}

bool appConfig::CurSelCombToReg()
{
	ASSERT(m_CurSelCombo);

	const TCHAR*key=m_regRoot;
	HKEY hk;
	if(ERROR_SUCCESS==RegCreateKeyEx(HKEY_CURRENT_USER,key,0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hk,NULL))
	{
		TCHAR vv[80]={0};
		LONG len=_countof(vv);
		RegSetValueEx(hk,m_CurCoreType,0,REG_SZ,(BYTE*)(m_CurSelCombo.GetBuffer(80)),(80+1)*sizeof(TCHAR));
		m_CurSelCombo.ReleaseBuffer();
		RegCloseKey(hk);
		return true;
	}
	
	return false;
}

bool appConfig::GetCombStringsByApp( CStringArray& strArr )
{
	CoreCate *pCate=GetCurCoreCates();
	if(NULL == pCate)
	{
		return false;
	}

	for (int i=0;i<pCate->GetCount();i++)
	{
		RUNEXE_CMDLINE *pRun = pCate->GetAt(i);
		strArr.Add(pRun->Caption);
	}
	return true;

}

CoreCate* appConfig::GetCurCoreCates()
{
	if (m_CurCoreType.IsEmpty())
	{
		return NULL;
	}

	CoreCate *pCate = GetCoreCate(m_CurCoreType);

	return pCate;
}


bool appConfig::GetCurMainSubCap(CString& mainCap,CString& subCap)
{
	CoreCate *pCurCate = GetCurCoreCates();
	if (NULL == pCurCate)
	{
		return false;
	}

	mainCap = pCurCate->MainCap();
	subCap = pCurCate->SubCap();

	return true;
}

bool appConfig::LoadDefaultCombSel()
{
	if(false == CurSelCombFromReg())
	{
		CoreCate * pCate =GetCurCoreCates();
		if(NULL!=pCate)
		{
			if(pCate->GetCount()>0)
			{
				RUNEXE_CMDLINE *pCmdline = pCate->GetAt(0);
				SetCurSelCombo(pCmdline->Caption);
				return true;
			}
		}
	}
	return false;
}

appConfig appConfig::s_appConfig;



IMPLEMENT_DYNCREATE(CoreCate,CInfoBase)

const TCHAR * CoreCate::MARKER_CORE = _T("CORE");//SATWE ,PMSAP SPACEPMSAP
const TCHAR * CoreCate::MARKER_PROGCMDS = _T("PROGCMDS");
const TCHAR * CoreCate::MARKER_PROGCMD = _T("PROGCMD");
const TCHAR * CoreCate::MARKER_MAIN_CAPTION = _T("MAIN_CAPTION");
const TCHAR * CoreCate::MARKER_SUB_CAPTION= _T("SUB_CAPTION");
bool CoreCate::readFrom( slim::XmlNode* pThisNode )
{
	
	XmlNode* pNode = pThisNode->findChild(MARKER_CORE);

	if (NULL == pNode)
	{
		return false;
	}
	m_Core = pNode->getString();

	pNode = pThisNode->findChild(MARKER_PROGCMDS);
	if (NULL == pNode)
	{
		return false;
	}


	if(false == readElements(pNode,MARKER_PROGCMD,m_ProCmds,RUNTIME_CLASS(RUNEXE_CMDLINE)))
	{
		return false;
	}

	XmlNode *pMainCap = pThisNode->findChild(MARKER_MAIN_CAPTION);
	if(NULL == pMainCap)
	{
		return false;
	}
	m_MainCap = pMainCap->getString();

	XmlNode *pSubCap = pThisNode->findChild(MARKER_SUB_CAPTION);
	if(NULL == pSubCap)
	{
		return false;
	}
	m_SubCap = pSubCap->getString();

	return true;

}

bool CoreCate::writeTo( slim::XmlNode* pThisNode )
{
	return false;
}

IMPLEMENT_DYNCREATE(RUNEXE_CMDLINE,CInfoBase)
const TCHAR * RUNEXE_CMDLINE::MARKER_COMBBOX_CAPTION = _T("COMBBOX_CAPTION");
const TCHAR * RUNEXE_CMDLINE::MARKER_CMDLINE = _T("CMDLINE");
bool RUNEXE_CMDLINE::readFrom( slim::XmlNode* pThisNode )
{
	XmlNode * pNode = pThisNode->findChild(MARKER_COMBBOX_CAPTION);
	
	if (NULL == pNode)
	{
		return false;
	}
	Caption = pNode->getString();

	pNode = pThisNode->findChild(MARKER_CMDLINE);
	if (NULL == pNode)
	{
		return false;
	}
	Cmdline = pNode->getString();

	return true;
}

bool RUNEXE_CMDLINE::writeTo( slim::XmlNode* pThisNode )
{
	return false;
}


