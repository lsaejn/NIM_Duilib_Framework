#pragma once
#include "json/json.h"

class PkpmmainConfigJson
{
public:
	PkpmmainConfigJson(void);
	~PkpmmainConfigJson(void);

protected:
	CString m_fileName;
	bool m_bDebugMode;
	bool m_bDeubugCommon;
	int m_nDebugProfession;
	bool m_bDebugLock;
	bool m_bDebugQBHelp;
	bool m_bDebugQueryAppExit;
	bool m_bDebugReloadSubMain;

	bool m_bAssCapSwith;
	CString m_AssignCap;
	bool   m_bCapHasWorkpath;

	bool m_bStsNewDWGTRANS;
	int  m_nDwgTransNameAppedix;
	CString m_strDwgVersion;

	static PkpmmainConfigJson s_PkpmmainConfigJson;

	CString m_JsonPaName;

	bool readFrom(Json::Value&  jsRoot);
	bool readDebugMode(Json::Value&  deb);
	bool readCaption(Json::Value&  cap);
	//bool readNowToDwg(Json::Value&  N2Dwg);
	
	//bool writeNowToDwg(Json::Value&  N2Dwg);
public:
	void clear();
	//void clearDwg();
	void clearDebug();
	void clearCaption();
	bool read(CString filename);
	bool Load();
	bool save();
	bool IsDebugMode();
	bool IsDebugCommon();
	int  DebugProfession();
	bool IsDebugLock();
	bool IsDebugQBHelp();
	bool IsDebugQueryAppExit();
	bool IsDebugReloadSubMain();

	bool IsAssignCap();
	CString AssignCapStr();
	bool AssignCapHasWorkpath();

	bool IsStsT2DwgNewVersion();
	//bool SetStsT2DwgNewVersion(bool bnew);
	CString DwgVersion();
	//bool SetDwgVersion(CString strVer);


	int DWGTransNameAppendixType();
	

	 

public:
	static PkpmmainConfigJson& Get();
	
};

 