#pragma once
#include "XmlInfo/SlimXml.h"

class CMultLanguageStringSet;
class CMultLanguageStringMgr
{
protected:
	static const CString MARKER_TOP;
	static const CString MARKER_LANGUAGE;
public:
	CMultLanguageStringMgr(void);
	~CMultLanguageStringMgr(void);

	static const CString  STRING_XML_ID;
protected:
	CString m_CurLang;

	CString m_ProjectName;
	CString m_PathProj;
// 	CString m_Path_Resource_h;
// 	CString m_Path_Project_rc;

//	CString m_Path_string_xml;

	CArray<CMultLanguageStringSet*,CMultLanguageStringSet*> arrSet;

	void clear();
	bool ResourcehToStruct();
	bool SoluMacroValue(CString strMacroValue);

	bool Project_RcToStrcut();
	void SoluLanguage(CStdioFile &file,CString szVer);
	void SoluStringTable(CStdioFile &file,CString szVer);
	void SoluStringTable(CString szItem,CString& szID,CString& szString);
	
	
	
	bool WriteToFile();
	bool readFrom(slim::XmlNode* pNode);
	bool writeTo(slim::XmlNode* pNode);
public:
	bool StringResToXml();//资源向XML转化
	bool IsInCoding();
	bool LoadFromFile();//结构体加载生成


	CString LoadString(CString ID);
	CString LoadString(long ID);
	void SetCurLang(CString strLang){m_CurLang = strLang;}

	CString GetProject_RcFullPathName();
	CString GetResourcehFullPathName();
	CString GetXmlFullPathName();

	void SetProjectPathName(CString path,CString na);
	bool FixPathStr(CString &strPath);
};

class CMultLanguageStringItem;
class CMultLanguageStringSet
{
public:
	CMultLanguageStringSet();
	~CMultLanguageStringSet();
	void clear();
	static const CString MARKER_ATTRI_VER;
	static const CString MARKER_ITEM;

public:
	CString VER;
	bool readFrom(slim::XmlNode* pNode);
	bool writeTo(slim::XmlNode* pNode);

	CArray<CMultLanguageStringItem*,CMultLanguageStringItem*> arrItems;

};

class CMultLanguageStringItem
{

public:
	static const CString MARKER_ATTRI_ID;
	static const CString MARKER_ATTRI_STR_ID;
	static const CString MARKER_ATTRI_String;
public:
	CMultLanguageStringItem();
	~CMultLanguageStringItem();

public:
	long ID;
	CString strID;
	CString String;

	bool readFrom(slim::XmlNode* pNode);
	bool writeTo(slim::XmlNode* pNode);
	
};


extern CMultLanguageStringMgr g_stringMgr;
