#include "stdafx.h"
#include "MultLanguageStringMgr.h"
const CString CMultLanguageStringMgr::STRING_XML_ID = _T("MULTLANGSTRINGS");

const CString STRING_XML_NAME = _T("res\\MultLangStrings.xml");
const CString RESOURCE_H = _T("resource.h");

const CString CMultLanguageStringMgr::MARKER_TOP=_T("TOP");
const CString CMultLanguageStringMgr::MARKER_LANGUAGE = _T("LANGUAGE");

const CString RC_LANGUAGE = _T("defined(AFX_TARG_");
const CString RC_STRINGTABLE = _T("STRINGTABLE");
const CString RC_BEGIN = _T("BEGIN");
const CString RC_END= _T("END");
const CString RES_DEFINE= _T("#define");

CMultLanguageStringMgr::CMultLanguageStringMgr(void)
{

}


CMultLanguageStringMgr::~CMultLanguageStringMgr(void)
{
	clear();
}
bool CMultLanguageStringMgr::FixPathStr(CString &strPath)
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
void CMultLanguageStringMgr::clear()
{
	for (int i=0;i<arrSet.GetSize();i++)
	{
		delete arrSet.GetAt(i);
	}
	arrSet.RemoveAll();
}
bool CMultLanguageStringMgr::StringResToXml()
{
	if (false == IsInCoding())
	{
		return true;
	}

	if (Project_RcToStrcut()&&ResourcehToStruct())
	{
		if (false == WriteToFile())
		{
			AfxMessageBox(_T("Can not Write String.xml\nplease check it out."));
			return false;
		}
		return true;
		
	}
	return false;
}
bool CMultLanguageStringMgr::IsInCoding()
{
	if (true == m_PathProj.IsEmpty())
	{
		ASSERT(FALSE);
	}

	FixPathStr(m_PathProj);

	CFileFind Finder;
	CString strForFind;
	strForFind = m_PathProj + _T("*.vcxproj");
	bool bWorking = Finder.FindFile( strForFind ) ;
	bool bFindSln = false;
	while( bWorking ) 
	{
		bWorking = Finder.FindNextFile() ;
		CString DataFile = Finder.GetFilePath() ; //GetFilePath：得到全路径名
		if(Finder.IsDirectory() && !Finder.IsDots())    //如果是目录 
		{ 
			continue;
		}
		else if( !Finder.IsDirectory() && !Finder.IsDots() )//不为目录
		{
			bFindSln = true;
			break;
		}
	}

	return bFindSln;
}
bool CMultLanguageStringMgr::Project_RcToStrcut()
{
	CString rcPathName = GetProject_RcFullPathName();
	CStdioFile file;
	if(FALSE == file.Open(rcPathName,CFile::modeRead))
	{
		return false;
	}
	CString strLine;
	int nAFX_TARGE;
	CString szfind = RC_LANGUAGE;
	while (file.ReadString(strLine))
	{
		nAFX_TARGE = strLine.Find(szfind);
		if (nAFX_TARGE>=0)
		{
			CString szVer ;
			strLine.Delete(0,nAFX_TARGE+szfind.GetLength());
			int nS = strLine.Find(_T(")"));
			szVer = strLine.Left(nS);

			SoluLanguage(file,szVer);
		}
	}
	return true;
}
bool CMultLanguageStringMgr::ResourcehToStruct()
{
	CString resPathName = GetResourcehFullPathName();
	CStdioFile file;
	if(FALSE == file.Open(resPathName,CFile::modeRead))
	{
		return false;
	}

	CString strLine;
	CString szfind = RES_DEFINE;
	int nDefine = -1;
	while (file.ReadString(strLine))
	{
		nDefine = strLine.Find(szfind);
		if (nDefine>=0)
		{
			CString szVer ;
			strLine.Delete(nDefine,nDefine+szfind.GetLength());
			strLine.Trim();

			
			SoluMacroValue(strLine);
		

		}
	}
	return true;
}
bool CMultLanguageStringMgr::SoluMacroValue(CString strMacroValue)
{
	CString strMacro;
	CString strVaule;
	int nSep  = strMacroValue.FindOneOf(_T(" \t"));
	strMacro = strMacroValue.Left(nSep);
	strMacro.Trim();
	strVaule = strMacroValue.Mid(nSep+1);
	strVaule.Trim();

	CMultLanguageStringSet *pSet;
	CMultLanguageStringItem *pItem;
	int i = 0;
	int j=0;
	for (i=0;i<arrSet.GetCount();i++)
	{
		pSet = arrSet.GetAt(i);

		for (j=0;j<pSet->arrItems.GetCount();j++)
		{
			pItem = pSet->arrItems.GetAt(j);
			if (0 == pItem->strID.Compare(strMacro))
			{
				long a =  _tstol(strVaule);
				pItem->ID = a;
				break;
			}
		}
	}

	return true;
}

void CMultLanguageStringMgr::SoluLanguage(CStdioFile &file,CString szVer)
{
	CString strLine;
	int nAFX_TARGE;
	CString szfind = RC_LANGUAGE;

	ULONGLONG curPos = file.GetPosition();
	while (file.ReadString(strLine))
	{
		nAFX_TARGE = strLine.Find(szfind);
		if (nAFX_TARGE>=0)
		{
			file.Seek(curPos,CFile::begin);
			return;
		}
		else if(0 == strLine.Compare(RC_STRINGTABLE))
		{
			SoluStringTable(file,szVer);//读过了End行
		}
		
	}

	return;
}
void CMultLanguageStringMgr::SoluStringTable(CStdioFile &file,CString szVer)
{
	CString strTmp;
	CString szID;CString szString;

	bool bFind = false;
	CMultLanguageStringSet *pSet = NULL;
	int i =0;
	for (i=0;i<arrSet.GetSize();i++)
	{
		if(0 ==  arrSet.GetAt(i)->VER.Compare(szVer))
		{
			pSet = arrSet.GetAt(i);
			bFind = true;
		}
	}

	if (false == bFind)
	{
		pSet = new  CMultLanguageStringSet;
		pSet->VER = szVer;
		arrSet.Add(pSet);
	}


	CMultLanguageStringItem* pItem;
	if(file.ReadString(strTmp))
	{
		if(0 == strTmp.Compare(RC_BEGIN))
		{
			file.ReadString(strTmp);

			if(strTmp.Find(_T("\""))<0)
			{
				//尝试读取下一行
				CString strLine2;
				file.ReadString(strLine2);
				strTmp += (_T(" ") + strLine2);
			}

			while(strTmp!=RC_END)
			{
				SoluStringTable(strTmp,szID,szString);
				pItem = new CMultLanguageStringItem;
				pItem->strID = szID;
				pItem->String = szString;
				pSet->arrItems.Add(pItem);

				file.ReadString(strTmp);

				if(strTmp!=RC_END&&strTmp.Find(_T("\""))<0)
				{
					//尝试读取下一行
					CString strLine2;
					file.ReadString(strLine2);
					strTmp += (_T(" ") + strLine2);
				}
			}
			

		}
	}

	return ;
}

void CMultLanguageStringMgr::SoluStringTable(CString szItem,CString& szID,CString& szString)
{
	int ndo = szItem.Find(_T("\""));
	szID = szItem.Left(ndo);
	szID.Trim();

	szString = szItem.Mid(ndo);
	szString.Trim();
	szString.Replace(_T("\\n"),_T("\n"));
	return;
}

void CMultLanguageStringMgr::SetProjectPathName(CString path,CString na)
{
	m_ProjectName = na;
	m_PathProj = path;
}

CString CMultLanguageStringMgr:: GetXmlFullPathName()
{
	CString strFileNameXml; 
	FixPathStr(m_PathProj);
	strFileNameXml = m_PathProj + STRING_XML_NAME ;
	return strFileNameXml;
}
CString CMultLanguageStringMgr::GetProject_RcFullPathName()
{
	CString strFileNameRc;
	FixPathStr(m_PathProj);
	strFileNameRc = m_PathProj+m_ProjectName+_T(".rc");
	return strFileNameRc;
}
CString CMultLanguageStringMgr::GetResourcehFullPathName()
{
	CString strFileNameResourceh;
	FixPathStr(m_PathProj);
	strFileNameResourceh = m_PathProj+RESOURCE_H;
	return strFileNameResourceh;
}
bool CMultLanguageStringMgr::LoadFromFile()
{
	slim::XmlDocument doc;
	if (IsInCoding())
	{
		return doc.loadFromFile(GetXmlFullPathName());
	}
	else
	{
		HRSRC hRsrc  = ::FindResource(AfxGetApp()->m_hInstance,STRING_XML_ID,_T("XML"));
		bool bResXmlLoad =true;
		if (NULL == hRsrc)
		{
			bResXmlLoad = false;
		}
		DWORD dwSize = SizeofResource(AfxGetApp()->m_hInstance, hRsrc);//获取资源的大小
		if (0 == dwSize)
		{	
			bResXmlLoad = false;
		}
		HGLOBAL hGlobal = LoadResource(AfxGetApp()->m_hInstance, hRsrc);//加载资源
		if (NULL == hGlobal)
		{	
			bResXmlLoad = false;
		}
		LPVOID pBuffer = LockResource(hGlobal);//锁定资源
		if (NULL == pBuffer)
		{	
			bResXmlLoad = false;
		}

		if(false == bResXmlLoad)
		{
			return false;
		}

		doc.loadFromMemory((char*)pBuffer,dwSize);
	}
	
	slim::XmlNode *pNodeTop = doc.findChild(MARKER_TOP);

	if (NULL == pNodeTop)
	{
		return false;
	}
	readFrom(pNodeTop);

	return true;
}
bool CMultLanguageStringMgr::readFrom(slim::XmlNode* pNode)
{
	clear();

	if (NULL==pNode)
	{
		return false;
	}

	slim::NodeIterator itor;
	slim::XmlNode* pSets;
	for (pSets = pNode->findFirstChild(MARKER_LANGUAGE,itor);
		pSets!=NULL;
		pSets = pNode->findNextChild(MARKER_LANGUAGE,itor))
	{
		CMultLanguageStringSet *pItemSet = new CMultLanguageStringSet;
		if(pItemSet->readFrom(pSets))
		{
			arrSet.Add(pItemSet);
		}
	}

	return true;
}
bool CMultLanguageStringMgr::WriteToFile()
{
	slim::XmlDocument doc;
	slim::XmlNode *pNodeTop = doc.addChild(MARKER_TOP);
	writeTo(pNodeTop);
	doc.save(GetXmlFullPathName());
	return true;
	
}
bool CMultLanguageStringMgr::writeTo(slim::XmlNode* pNode)
{
	if (NULL==pNode)
	{
		return false;
	}

	slim::XmlNode *pItem;
	int i=0;
	for (i=0;i<arrSet.GetSize();i++)
	{
		pItem = pNode->addChild(MARKER_LANGUAGE);
		arrSet.GetAt(i)->writeTo(pItem);
	}

	return true;
}

CString CMultLanguageStringMgr::LoadString(long ID)
{
	CMultLanguageStringSet *pCurSet = NULL;
	int i;
	for (i=0;i<arrSet.GetSize();i++)
	{
		if(0 == arrSet.GetAt(i)->VER.Compare(m_CurLang))
		{
			pCurSet = arrSet.GetAt(i);
		}
	}

	if (NULL == pCurSet)
	{
		ASSERT(FALSE);
		return _T("");
	}
	for (i=0;i<pCurSet->arrItems.GetCount();i++)
	{
		AfxTrace(L"%d = %d\n",pCurSet->arrItems.GetAt(i)->ID,ID);
		if( pCurSet->arrItems.GetAt(i)->ID==ID)
		{
			
			return pCurSet->arrItems.GetAt(i)->String;
		}
	}

	ASSERT(FALSE);
	return _T("");
}
CString CMultLanguageStringMgr::LoadString(CString ID)
{
	CMultLanguageStringSet *pCurSet = NULL;
	int i;
	for (i=0;i<arrSet.GetSize();i++)
	{
		if(0 == arrSet.GetAt(i)->VER.Compare(m_CurLang))
		{
			pCurSet = arrSet.GetAt(i);
		}
	}

	if (NULL == pCurSet)
	{
		ASSERT(FALSE);
		return _T("");
	}
	for (i=0;i<pCurSet->arrItems.GetCount();i++)
	{
		if(0 == pCurSet->arrItems.GetAt(i)->strID.Compare(ID))
		{
			return pCurSet->arrItems.GetAt(i)->String;
		}
	}

	ASSERT(FALSE);
	return _T("");
}
//////////////////////////////////////////////////////////////////////////

const CString CMultLanguageStringSet::MARKER_ATTRI_VER= _T("VER");
const CString CMultLanguageStringSet::MARKER_ITEM= _T("ITEM");
CMultLanguageStringSet::CMultLanguageStringSet()
{

}
CMultLanguageStringSet::~CMultLanguageStringSet()
{
	clear();
}
void CMultLanguageStringSet::clear()
{
	int i=0;
	for (i=0;i<arrItems.GetSize();i++)
	{
		delete arrItems.GetAt(i);
	}

	arrItems.RemoveAll();
}
bool CMultLanguageStringSet::readFrom(slim::XmlNode* pNode)
{
	clear();

	if (NULL==pNode)
	{
		return false;
	}

	VER =pNode->readAttributeAsString(MARKER_ATTRI_VER,_T(""));

	slim::NodeIterator itor;
	slim::XmlNode* pItem;
	for (pItem = pNode->findFirstChild(MARKER_ITEM,itor);
		pItem!=NULL;
		pItem = pNode->findNextChild(MARKER_ITEM,itor))
	{
		CMultLanguageStringItem *pStrItem = new CMultLanguageStringItem;
		if(pStrItem->readFrom(pItem))
		{
			arrItems.Add(pStrItem);
		}
	}
	return true;
}
bool CMultLanguageStringSet::writeTo(slim::XmlNode* pNode)
{
	if (NULL==pNode)
	{
		return false;
	}

	pNode->addAttribute(MARKER_ATTRI_VER,VER);
	slim::XmlNode *pItem;
	int i=0;
	for (i=0;i<arrItems.GetSize();i++)
	{
		pItem = pNode->addChild(MARKER_ITEM);
		arrItems.GetAt(i)->writeTo(pItem);
	}

	return true;
	
}



//////////////////////////////////////////////////////////////////////////
const CString CMultLanguageStringItem::MARKER_ATTRI_ID = _T("ID");
const CString CMultLanguageStringItem::MARKER_ATTRI_STR_ID = _T("STR_ID");
const CString CMultLanguageStringItem::MARKER_ATTRI_String = _T("String");
CMultLanguageStringItem::CMultLanguageStringItem()
{

}
CMultLanguageStringItem::~CMultLanguageStringItem()
{

}
bool CMultLanguageStringItem::readFrom(slim::XmlNode* pNode)
{
	if (NULL==pNode)
	{
		return false;
	}

	strID = pNode->readAttributeAsString(MARKER_ATTRI_STR_ID);
	String = pNode->readAttributeAsString(MARKER_ATTRI_String);
	ID = pNode->readAttributeAsInt(MARKER_ATTRI_ID);
	return true;
}
bool CMultLanguageStringItem::writeTo(slim::XmlNode* pNode)
{
	if (NULL==pNode)
	{
		return false;
	}

	String.Trim(_T("\""));

	
	pNode->addAttribute(MARKER_ATTRI_STR_ID,strID);
	pNode->addAttribute(MARKER_ATTRI_ID,ID);

	pNode->addAttribute(MARKER_ATTRI_String,String);
	
	return true;
}





CMultLanguageStringMgr g_stringMgr;