#pragma once

#include "XmlInfo//InfoItem.h"

class cfgpathsvr
{
private:
	static const int ENV_PATH_LENGTH;
	static bool s_bHasSetEnv;
private:
	static bool GetAssignCFGPath(CString& strCFG,bool bMarchine);
public:
	cfgpathsvr(void);
	~cfgpathsvr(void);
	
	static CString GetRegCFGPath();

	static CString GetAppPath();
	static CString GetAppIniFullPathName();
	//static bool IsDebugMode();

protected:
	static CMapStringToString s_mapCfgpath;
public:
	static bool LoadFileCFGPATH();
	static bool GetAppPathByCFGPATHMarker(CString strMarker,CString& retPath);
	static LPVOID SetCfgvmEn(/*BOOL bSetCfgPm*/);
	static CString GetCurEnvirom();
protected:
	static int SpliteEvnPath(CString strEvnPath,CArray<CString>& Paths);
	
};

class RUNEXE_CMDLINE:public CInfoBase
{
	friend class CoreCate;
	friend class appConfig;
	DECLARE_DYNCREATE(RUNEXE_CMDLINE)
public:
	static const TCHAR * MARKER_COMBBOX_CAPTION;
	static const TCHAR * MARKER_CMDLINE;
public:
	CString Caption;//阶段
	CString Cmdline;//命令行
public:
	virtual bool readFrom(slim::XmlNode* pNode);
	virtual bool writeTo(slim::XmlNode* pNode);
};

class CoreCate:public CInfoBase
{	
	friend class appConfig;
	DECLARE_DYNCREATE(CoreCate)
public:
	static const TCHAR * MARKER_CORE;
	static const TCHAR * MARKER_PROGCMDS;
	static const TCHAR * MARKER_PROGCMD;
	static const TCHAR * MARKER_MAIN_CAPTION;
	static const TCHAR * MARKER_SUB_CAPTION;
protected:
	CString m_Core;//SATWE ,PMSAP SPACEPMSAP
	
	CString m_MainCap;
	CString m_SubCap;
public:
	CArray<CInfoBase*,CInfoBase*> m_ProCmds;

	int GetCount(){return m_ProCmds.GetCount();};
	RUNEXE_CMDLINE * GetAt( int  idx){return (RUNEXE_CMDLINE*) m_ProCmds.ElementAt(idx);}
	virtual bool readFrom(slim::XmlNode* pNode);
	virtual bool writeTo(slim::XmlNode* pNode);
	CString Core(){return m_Core;}
	CString MainCap(){return m_MainCap;}
	CString SubCap(){return m_SubCap;}

};

enum
{
	CORE_NULL = 0,
	CORE_SATWE = 1,
	CORE_PMSAP = 2,
	CORE_SPACEPMSAP =3
};

class appConfig: public CInfoBase
{
protected:
	static appConfig s_appConfig;
public:
	static appConfig* Get(){return &s_appConfig;}
public:
	appConfig();
	~appConfig();
	DECLARE_DYNCREATE(appConfig)
public:
	static const TCHAR * MARKER_LAUNCHER_TOP;

	static const TCHAR * MARKER_TARGET;
	static const TCHAR * MARKER_PATH;
	static const TCHAR * MARKER_RUNEXE;
	static const TCHAR * MARKER_TYPE_BY_RECEIVEED_CMD;
	static const TCHAR * MARKER_CORE;
	static const TCHAR * MARKER_CORE_CATES;
	static const TCHAR * MARKER_CORE_CATE;
	static const TCHAR * MARKER_CONFIG;
	static const TCHAR * MARKER_DEBUGMODE;
	static const TCHAR * MARKER_REG;
protected:
	CString m_regRoot;
	CString m_runexe;
	CString m_pathInCFGPATH;//CFGPATH文件中路径

	CStringArray m_Cores;
	bool m_isDebugMode;
	CArray<CInfoBase*,CInfoBase*> m_CoreCates;

public:
	virtual bool readFrom(slim::XmlNode* pNode);
	virtual bool writeTo(slim::XmlNode* pNode);

protected:
	bool readFromFile(CString strPathFile);
	CString GetLauncherXmlFileFullPathName();
public:
	bool readFromFile();

	CString RunExeString();
	CString pathInCFGPATH();
	CoreCate* GetCoreCate(CString strCore);

protected://外部设置参数
	CString m_CurCoreType;//命令行参数
	CString m_CurSelCombo;//注册表读取
public:
	void SetCurCoreTypeByCmdLine(CString cmdline);
	CString GetCurCoreType();

	void SetCurSelCombo(CString strSel);
	CString GetCurSelCombo();
	CString GetCurSelComboCmdline();

	bool GetCombStringsByApp(CStringArray& strArr);


	bool LoadDefaultCombSel();
	bool CurSelCombFromReg();
	bool CurSelCombToReg();

	bool GetCurMainSubCap(CString& mainCap,CString& subCap);


	CoreCate* GetCurCoreCates();
public:
	bool IsDebugMode();
	
}; 



class toolsvr
{
public:
	static bool FixPathStr(CString &strPath);
	static bool CreateDirtory(const TCHAR*path);
	static bool DirectoryExist(const TCHAR *path);
	static int SpliteMultPaths(CString str,CArray<CString,CString>& paths,TCHAR sp  );
	static int CombMultPaths(const CArray<CString,CString>& paths, CString& str,TCHAR sp  );
};

class datasvr
{
public:
	static void ReadPrivateProfile2000_workPaths(CStringArray& workPaths);
	static void WritePrivateProfile2000_workPath(CString workPath);
	static void DeletePrivateProfile2000_workPath(CString workPath);
	static int ReadPrivateProfile2000_MaxWorkPath();
	static void FixMaxWorkPathV31(int& nMaxPath);
	static CString PrivateProfile2000_IniFileFullPath();
	static void ReadPrivateProfile2014_PKPMMAIN(CString& strPath,CString& strEXE);

};

 