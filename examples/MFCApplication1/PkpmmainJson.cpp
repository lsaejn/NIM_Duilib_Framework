#include "StdAfx.h"
#include "PkpmmainJson.h"

#include "stdlib.h"
#include "stdio.h"
#include <fstream>
#include "json/json.h"
#include <xlocale>
#include "svr.h"

PkpmmainConfigJson::PkpmmainConfigJson(void)
{
}


PkpmmainConfigJson::~PkpmmainConfigJson(void)
{
	clear();
}

void PkpmmainConfigJson::clear()
{
	clearCaption();
	clearDebug();
	//clearDwg();
}
// void PkpmmainConfigJson::clearDwg()
// {
// 	m_bStsNewDWGTRANS = false;
// 	m_nDwgTransNameAppedix = 0;
// 	m_strDwgVersion= "Acad2007";
// }
void PkpmmainConfigJson::clearCaption()
{
	m_bAssCapSwith = false;
	m_AssignCap.Empty();
	m_bCapHasWorkpath=true;
}
void PkpmmainConfigJson::clearDebug()
{
	m_fileName.Empty();
	m_bDebugMode = false;

	m_bDeubugCommon = false;
	m_nDebugProfession = -1;
	m_bDebugLock = false;

	m_bDebugQBHelp = false;
	m_bDebugQueryAppExit = false;
	m_bDebugReloadSubMain = false;
}

bool PkpmmainConfigJson::readFrom( Json::Value&  jsRoot )
{
	const char* MARKER_CONFIG= "CONFIG";
	const char* MARKER_DEBUG_MODE= "DEBUG_MODE";
	const char* MARKER_CAPTION= "CAPTION";
	const char* MARKER_NOWTODWG= "NOWTODWG";
	Json::Value conf = jsRoot[MARKER_CONFIG];
	Json::Value debug = conf[MARKER_DEBUG_MODE];
	Json::Value cap = conf[MARKER_CAPTION];
	Json::Value dwg = conf[MARKER_NOWTODWG];
	readDebugMode(debug);
	readCaption(cap);
	//readNowToDwg(dwg);
	return true;

}
bool PkpmmainConfigJson::readCaption(Json::Value&  cap)
{
	const char* MARKER_SWITCH= "SWITCH"; //总开关
	const char* MARKER_CAPTION=  "CAPTION";  
	const char* MARKER_HAS_WORKPATH = "HAS_WORKPATH"; 

	if (cap[MARKER_SWITCH].isNull())
	{
		m_bAssCapSwith = false;
	}
	else
	{
		m_bAssCapSwith = cap[MARKER_SWITCH].asBool();
	}

	if (false == m_bAssCapSwith)  //调试模式关闭
	{
		clearCaption();
	return true;
	}

	if (cap[MARKER_CAPTION].isNull())
	{
		m_AssignCap.Empty(); // 如果打开了指定开关,又没有给名字,那么就使用默认了
	}
	else
	{
		m_AssignCap = cap[MARKER_CAPTION].asCString();
	}

	if (cap[MARKER_HAS_WORKPATH].isNull())
	{
		m_bCapHasWorkpath = true;
	}
	else
	{
		m_bCapHasWorkpath = cap[MARKER_HAS_WORKPATH].asBool();
	}


}
  
// bool PkpmmainConfigJson::readNowToDwg(Json::Value&  N2Dwg)
// {
// 	const char* MARKER_STS_NEWDWGTRANS= "STS_NEWDWGTRANS"; //总开关
// 	const char* MARKER_DWGTRANSNAME_APPENDIX=  "DWGTRANSNAME_APPENDIX";
// 	const char* MARKER_DWG_VERSION = "DWG_VERSION";
// 
// 
// 	if (N2Dwg[MARKER_STS_NEWDWGTRANS].isNull())
// 	{
// 		m_bStsNewDWGTRANS = false;
// 	}
// 	else
// 	{
// 		m_bStsNewDWGTRANS = N2Dwg[MARKER_STS_NEWDWGTRANS].asBool();
// 	}
// 	if (N2Dwg[MARKER_DWGTRANSNAME_APPENDIX].isNull())
// 	{
// 		m_nDwgTransNameAppedix = 0;//时间
// 	}
// 	else
// 	{
// 		m_nDwgTransNameAppedix = N2Dwg[MARKER_DWGTRANSNAME_APPENDIX].asInt();
// 	}
// 
// 	if (N2Dwg[MARKER_DWG_VERSION].isNull())
// 	{
// 		m_strDwgVersion = "Acad2007"; 
// 	}
// 	else
// 	{
// 		CString str = N2Dwg[MARKER_DWG_VERSION].asCString();
// 		if (0!=str.CompareNoCase("Acad R10")&&
// 			0!=str.CompareNoCase("Acad R12")&&
// 			0!=str.CompareNoCase("Acad R13")&&
// 			0!=str.CompareNoCase("Acad R14")&&
// 			0!=str.CompareNoCase("Acad2000")&&
// 			0!=str.CompareNoCase("Acad2004")&&
// 			0!=str.CompareNoCase("Acad2007")&&
// 			0!=str.CompareNoCase("Acad2010")&&
// 			0!=str.CompareNoCase("Acad2013")
// 			)
// 		{
// 			str = "Acad2007";
// 		}
// 		m_strDwgVersion = str;
// 	}
// 
// 	return true;
// }
//bool PkpmmainConfigJson::writeNowToDwg(Json::Value&  N2Dwg)
//{
//	const char* MARKER_STS_NEWDWGTRANS= "STS_NEWDWGTRANS"; //总开关
//	const char* MARKER_DWGTRANSNAME_APPENDIX=  "DWGTRANSNAME_APPENDIX";
//	const char* MARKER_DWG_VERSION = "DWG_VERSION";
//
//	N2Dwg[MARKER_STS_NEWDWGTRANS] = m_bStsNewDWGTRANS;
//	N2Dwg[MARKER_DWGTRANSNAME_APPENDIX] =  m_nDwgTransNameAppedix;
//	N2Dwg[MARKER_DWG_VERSION] = LPCTSTR(m_strDwgVersion);
//	return true;
//}
bool PkpmmainConfigJson::readDebugMode(Json::Value&  deb)
{
	const char* MARKER_SWITCH= "SWITCH"; //总开关
	const char* MARKER_COMMON=  "COMMON"; // 通用 = true false
	const char* MARKER_PROFESSION = "PROFESSION";  // 专业 = 专业号  -1 为关闭
	const char* MARKER_LOCK = "LOCK";              // 默认没有  = true false
	const char* MARKER_QUICKBAR_HELP = "QUICKBAR_HELP";
	const char* MARKER_QUERYAPPEXIT  = "QUERYAPPEXIT";
	const char* MARKER_RELOADSUBMAIN  = "RELOADSUBMAIN";

	if (deb[MARKER_SWITCH].isNull())
	{
		m_bDebugMode = false;
		 
	}
	else
	{
		m_bDebugMode = deb[MARKER_SWITCH].asBool();
	}

	if (false == m_bDebugMode)  //调试模式关闭
	{
		clearDebug();
		return true;
	}



	if (deb[MARKER_COMMON].isNull())
	{
		m_bDeubugCommon = false; 
	}
	else
	{
		m_bDeubugCommon = deb[MARKER_COMMON].asBool();
	}

	if (deb[MARKER_PROFESSION].isNull())
	{
		m_nDebugProfession = -1;
		 
	}
	else
	{
		m_nDebugProfession = deb[MARKER_PROFESSION].asInt();
	}

	if (deb[MARKER_LOCK].isNull())
	{
		m_bDebugLock = false;
		 
	}
	else
	{
		m_bDebugLock = deb[MARKER_LOCK].asBool();
	}

	if (deb[MARKER_LOCK].isNull())
	{
		m_bDebugLock = false; 
	}
	else
	{
		m_bDebugLock = deb[MARKER_LOCK].asBool();
	}

	if (deb[MARKER_QUICKBAR_HELP].isNull())
	{
		m_bDebugQBHelp = false;
	}
	else
	{
		m_bDebugQBHelp=deb[MARKER_QUICKBAR_HELP].asBool();
	}
	
	if (deb[MARKER_QUERYAPPEXIT].isNull())
	{
		m_bDebugQueryAppExit = false;
	}
	else
	{
		m_bDebugQueryAppExit =deb[MARKER_QUERYAPPEXIT].asBool();
	}
	
	if (deb[MARKER_RELOADSUBMAIN].isNull())
	{
		m_bDebugReloadSubMain = false;
	}
	else
	{
		m_bDebugReloadSubMain =deb[MARKER_RELOADSUBMAIN].asBool();
	}
	
	 return true;

	
}
bool PkpmmainConfigJson::read( CString filename )
{
	clear();

	std::ifstream is;
	is.open(filename.GetBuffer());
	filename.ReleaseBuffer();
	if(is.is_open())
	{

	}
	else
	{
		//ASSERT(FALSE);
		CString str;
		str.Format(_T("未找到配置文件!\n文件:%s"),filename);
		AfxMessageBox(str);

		clear();
		return false;
	}

	Json::Reader reader;
	Json::Value jsRoot;
	 
	if (true == reader.parse(is,jsRoot))
	{
		readFrom(jsRoot);
	}
	else
	{
		CString str;
		str.Format(_T("请检查配置文件格式!\n文件:%s"),filename);
		return false;
	}
	
	return true;

}
bool PkpmmainConfigJson::Load()
{
	clear();

	CString mainPath;
	if(false == svr::getPathByMaker(_T("RIBBON-RELEASE"),mainPath))
	{
		AfxMessageBox(_T("无法找到RIBBON-RELEASE的路径,不能加载PKPMMAIN.JSON。"));
		return false;
	}
	 
	m_JsonPaName = mainPath + _T("PKPMMAIN.JSON");
	return read(m_JsonPaName);

}
PkpmmainConfigJson PkpmmainConfigJson::s_PkpmmainConfigJson;
PkpmmainConfigJson& PkpmmainConfigJson::Get()
{
	return s_PkpmmainConfigJson;
}

bool PkpmmainConfigJson::IsDebugMode()
{
	return m_bDebugMode;
}
bool PkpmmainConfigJson::IsDebugCommon()
{
	return m_bDeubugCommon;
}
int PkpmmainConfigJson::DebugProfession()
{
	return m_nDebugProfession;
}
bool PkpmmainConfigJson::IsDebugLock()
{
	return m_bDebugLock;
}

bool PkpmmainConfigJson::IsDebugQBHelp()
{
	return m_bDebugQBHelp;
}

bool PkpmmainConfigJson::IsDebugQueryAppExit()
{
	return m_bDebugQueryAppExit;
}

bool PkpmmainConfigJson::IsDebugReloadSubMain()
{
	return m_bDebugReloadSubMain;
}
bool PkpmmainConfigJson::IsAssignCap()
{
	return m_bAssCapSwith;
}
CString PkpmmainConfigJson::AssignCapStr()
{
	return m_AssignCap;
}
bool PkpmmainConfigJson::AssignCapHasWorkpath()
{
	return m_bCapHasWorkpath;
}

bool PkpmmainConfigJson::IsStsT2DwgNewVersion()
{
	return m_bStsNewDWGTRANS;
}

int PkpmmainConfigJson::DWGTransNameAppendixType()
{
	return m_nDwgTransNameAppedix;
}

bool PkpmmainConfigJson::save()
{
	const char* MARKER_CONFIG= "CONFIG";
	const char* MARKER_NOWTODWG= "NOWTODWG";

	CString strConfigFile = m_JsonPaName;
	std::ifstream is;
	is.open(strConfigFile.GetBuffer(),std::ios::binary);
	strConfigFile.ReleaseBuffer(-1);
	Json::Value root;
	Json::Reader reader;
	if(false == reader.parse(is,root))
	{
		is.close();
		return  false;
	}
	is.close();

	Json::Value conf;
	Json::Value dwg;
	if (false == root[MARKER_CONFIG].isNull())
	{
		conf = root[MARKER_CONFIG];
		if (false == conf[MARKER_NOWTODWG].isNull())
		{
			dwg = conf[MARKER_NOWTODWG];
		}
	}
	//writeNowToDwg(dwg);

	conf[MARKER_NOWTODWG] =  dwg; 
	root[MARKER_CONFIG] = conf;

	std::ofstream os;
	std::string strFile = root.toStyledString();
	try
	{
		os.open(strConfigFile);
	}
	catch (CException* e)
	{
		UNREFERENCED_PARAMETER(e);
		ASSERT(FALSE);
		return false;
	}
	os << strFile;
	os.close();
	return true;
}

//bool PkpmmainConfigJson::SetStsT2DwgNewVersion(bool bnew)
//{
//	m_bStsNewDWGTRANS = bnew;
//	return true;
//}
CString PkpmmainConfigJson::DwgVersion()
{
	return m_strDwgVersion;
}
//bool PkpmmainConfigJson::SetDwgVersion(CString strVer)
//{
//	m_strDwgVersion = strVer;
//	return true;
//}
 