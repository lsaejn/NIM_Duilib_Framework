#include "InfoItem.h"
#include <afxtempl.h>

using namespace slim;
////////////////////////////////CInfoBase//////////////////////////////////////////
IMPLEMENT_DYNCREATE(CInfoBase,CObject)

CInfoBase::CInfoBase()
{
	m_parent = NULL;
}
CInfoBase::~CInfoBase()
{

}
bool CInfoBase::readString(XmlNode* pNode,const CString& strTag,CString& strResult)
{
	if (NULL==pNode)
	{
		return false;
	}

	XmlNode *pStrNode = pNode->findChild(strTag);

	if (NULL!=pStrNode)
	{
		strResult = pStrNode->getString();
		return true;
	}

	return false;
}
bool CInfoBase::writeString(XmlNode* pNode,const CString& strTag,const CString& strToWrite)
{
	if (NULL==pNode)
	{
		return false;
	}

	XmlNode *pStrNode = pNode->addChild(strTag);

	if (NULL!=pStrNode)
	{
		pStrNode->setString(strToWrite);
		return true;
	}

	return false;
}
bool CInfoBase::readFrom(XmlNode* pNode)
{
	return false;
}
bool CInfoBase::writeTo(XmlNode* pNode)
{
	return false;
}
 bool CInfoBase::readElements(XmlNode* pNodeElements,const CString& elementTag,
 		CArray<CInfoBase*,CInfoBase*>& elements,CRuntimeClass* pRC)
{
 
	if(NULL==pNodeElements)
	{
		return false;
	}
	if(TRUE == elementTag.IsEmpty())
	{
		return false;
	}
	if (pRC==NULL)
	{
		return false;
	}


	NodeIterator itor;
	XmlNode *pItorNode = NULL;
	for (pItorNode = pNodeElements->findFirstChild(elementTag,itor);
		pItorNode!= NULL;
		pItorNode = pNodeElements->findNextChild(elementTag,itor))
	{
		CInfoBase *pInfoBase = (CInfoBase *)pRC->CreateObject();
		if (NULL!=pInfoBase)
		{
			if (true == pInfoBase->readFrom(pItorNode))
			{
				pInfoBase->SetParent(this);
				elements.Add(pInfoBase);
			}
			else
			{
				ASSERT(FALSE);
				continue;
			}

		}
		else
		{
			ASSERT(FALSE);
			continue;
		}
	}

 	return true;
 
}
bool CInfoBase::writeElements(XmlNode* pNodeElements,const CString& elementTag,
	 CArray<CInfoBase*,CInfoBase*>& elements)
{
	if(NULL==pNodeElements)
	{
		return false;
	}
	if (TRUE == elementTag.IsEmpty())
	{
		return false;
	}

	int  i;
	CInfoBase * pInfoBase =NULL;
	XmlNode * pNode = NULL;
	for (i=0;i<elements.GetSize();i++)
	{
		pInfoBase = elements.GetAt(i);
		if (NULL!=pInfoBase)
		{
			pNode = pNodeElements->addChild(elementTag);
			if (NULL!=pNode)
			{
				if(false == pInfoBase->writeTo(pNode))
				{
					ASSERT(FALSE);
				}
			}
			else
			{
				ASSERT(FALSE);
			}
		}
	}

	return true;

}
CInfoBase* CInfoBase::GetParent()
{
	ASSERT(m_parent);
	return m_parent;
}
void CInfoBase::SetParent(CInfoBase* pParent)
{
	ASSERT(pParent);
	m_parent = pParent;
}
