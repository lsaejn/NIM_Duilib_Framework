#include "afx.h"
#include "SlimXml.h"

#ifndef INFO_DLLEXPORT
#define INFO_DLLEXPORT 
#endif //INFO_DLLEXPORT

#ifndef INFOITEM_H
#define INFOITEM_H


class  INFO_DLLEXPORT CInfoBase:public CObject
{
	DECLARE_DYNCREATE(CInfoBase)
public:
	CInfoBase();
	virtual ~CInfoBase();
public:
	virtual bool readFrom(slim::XmlNode* pNode);
	virtual bool writeTo(slim::XmlNode* pNode);
public:
	static bool readString(slim::XmlNode* pNode,const CString& strTag,CString& strResult);
	static bool writeString(slim::XmlNode* pNode,const CString& strTag,const CString& strToWrite);

	bool readElements(slim::XmlNode* pNode,const CString& elementTag,CArray<CInfoBase*,CInfoBase*>& elements,CRuntimeClass* pRC);
	bool writeElements(slim::XmlNode* pNode,const CString& elementTag,CArray<CInfoBase*,CInfoBase*>& elements);

	CInfoBase* GetParent();
	void SetParent(CInfoBase* pParent);

protected:
	CInfoBase* m_parent;
};


#endif //INFOITEM_H


