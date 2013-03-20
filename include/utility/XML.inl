//
// XML.inl
//
// Copyright (c) Shareaza Development Team, 2002-2005.
// This file is part of SHAREAZA (www.shareaza.com)
//
// Shareaza is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Shareaza; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//


//////////////////////////////////////////////////////////////////////
// CXMLNode node type and casting access

int CXMLNode::getType() 
{
	return m_nNode;
}

CXMLNode* CXMLNode::asNode() 
{
	return (CXMLNode*)this;
}

CXMLElement* CXMLNode::asElement() 
{
	return ( m_nNode == xmlElement ) ? (CXMLElement*)this : NULL;
}

CXMLAttribute* CXMLNode::asAttribute() 
{
	return ( m_nNode == xmlAttribute ) ? (CXMLAttribute*)this : NULL;
}

//////////////////////////////////////////////////////////////////////
// CXMLNode parent access and delete

CXMLElement* CXMLNode::getParent() 
{
	return m_pParent;
}

void CXMLNode::Delete()
{
	if ( this == NULL ) return;

	if ( m_pParent != NULL )
	{
		if ( m_nNode == xmlElement ) m_pParent->removeElement( (CXMLElement*)this );
		else if ( m_nNode == xmlAttribute ) m_pParent->removeAttribute( (CXMLAttribute*)this );
	}

	delete this;
}

//////////////////////////////////////////////////////////////////////
// CXMLNode name access

tstring CXMLNode::getName() 
{
	return m_sName;
}

void CXMLNode::setName(const tchar* pszValue)
{
	m_sName = pszValue;
}

bool CXMLNode::isNamed(const tchar* pszName) 
{
	if ( this == NULL ) return false;	
	return _tcsicmp(m_sName.c_str(),pszName ) == 0;
}

//////////////////////////////////////////////////////////////////////
// CXMLNode value access

tstring CXMLNode::getValue() 
{
	return m_sValue;
}

void CXMLNode::setValue(const tchar* pszValue)
{
	m_sValue = pszValue;
}

//////////////////////////////////////////////////////////////////////
// CXMLElement detach

CXMLElement* CXMLElement::detach()
{
	if ( m_pParent ) m_pParent->removeElement( this );
	m_pParent = NULL;
	return this;
}

//////////////////////////////////////////////////////////////////////
// CXMLElement element access

CXMLElement* CXMLElement::addElement(const tchar* pszName)
{
	CXMLElement* pElement = new CXMLElement( this, pszName );
	m_pElements.push_back( pElement );
	return pElement;
}

CXMLElement* CXMLElement::addElement(CXMLElement* pElement)
{
	if ( pElement->m_pParent ) return NULL;
	m_pElements.push_back( pElement );
	pElement->m_pParent = this;
	return pElement;
}

int CXMLElement::getElementCount() 
{
	return (int)m_pElements.size();
}

CXMLElement* CXMLElement::getFirstElement() 
{
	if ( this == NULL ) return NULL;
	return (m_pElements.size() > 0)? m_pElements.front() : NULL;
}

ElementIterator CXMLElement::getElementIterator()
{
	return m_pElements.begin();
}
bool CXMLElement::hasNextElement(ElementIterator& iter)
{
	return (m_pElements.end() != iter);
}

CXMLElement* CXMLElement::getNextElement(ElementIterator& iter) 
{
	return *iter++;	
}

CXMLElement* CXMLElement::getElementByName(const tchar* pszName) 
{
	for ( ElementIterator iter = getElementIterator() ; hasNextElement(iter) ; )
	{
		CXMLElement* pElement = getNextElement( iter );
		if ( _tcsicmp(pElement->getName().c_str(),pszName ) == 0 ) return pElement;
		
	}
	return NULL;
}

CXMLElement* CXMLElement::getElementByName(const tchar* pszName, bool bCreate)
{
	for ( ElementIterator iter = getElementIterator() ; hasNextElement(iter) ; )
	{
		CXMLElement* pElement = getNextElement( iter );
		if ( _tcsicmp(pElement->getName().c_str(),pszName ) == 0 ) return pElement;
	}

	return bCreate ? addElement( pszName ) : NULL;
}

void CXMLElement::removeElement(CXMLElement* pElement)
{
	m_pElements.remove(pElement);

	/*POSITION pos = m_pElements.Find( pElement );
	if ( pos ) m_pElements.RemoveAt( pos );*/
}

//////////////////////////////////////////////////////////////////////
// CXMLElement attribute access

CXMLAttribute* CXMLElement::addAttribute(const tchar* pszName, const tchar* pszValue)
{
	CXMLAttribute* pAttribute = getAttribute( pszName );

	if ( ! pAttribute )
	{
		pAttribute = new CXMLAttribute( this, pszName );
		tstring strName( pszName );
		firtex::string_utils::toLower(strName);
		m_pAttributes.insert(AttributePair(strName, pAttribute ));
	}

	if ( pszValue ) pAttribute->setValue( pszValue );

	return pAttribute;
}

CXMLAttribute* CXMLElement::addAttribute(CXMLAttribute* pAttribute)
{
	if ( pAttribute->m_pParent ) return NULL;
	tstring strName( pAttribute->m_sName );
	firtex::string_utils::toLower(strName);
	m_pAttributes.insert(AttributePair(pAttribute->m_sName, pAttribute) );
	pAttribute->m_pParent = this;
	return pAttribute;
}

int CXMLElement::getAttributeCount() const
{
	return (int)m_pAttributes.size();
}

AttributeIterator CXMLElement::getAttributeIterator() 
{
	return m_pAttributes.begin();
}
bool CXMLElement::hasNextAttribute(AttributeIterator& iter)
{
	return (m_pAttributes.end() != iter);
}

CXMLAttribute* CXMLElement::getNextAttribute(AttributeIterator& iter) 
{
	CXMLAttribute* pAttribute = iter->second;
	iter++;
	return pAttribute;
}

CXMLAttribute* CXMLElement::getAttribute(const tchar* pszName) 
{
	CXMLAttribute* pAttribute = NULL;
	tstring strName( pszName );
	firtex::string_utils::toLower(strName);
	AttributeIterator iter = m_pAttributes.find(strName);
	if(iter != m_pAttributes.end())
		return iter->second;
	return NULL;	
}

tstring CXMLElement::getAttributeValue(const tchar* pszName, const tchar* pszDefault) 
{
	CXMLAttribute* pAttribute = getAttribute( pszName );
	tstring strResult;
	if ( pAttribute ) strResult = pAttribute->m_sValue;
	else if ( pszDefault ) strResult = pszDefault;
	return strResult;
}

void CXMLElement::removeAttribute(CXMLAttribute* pAttribute)
{
	tstring strName( pAttribute->m_sName );
	firtex::string_utils::toLower(strName);
	AttributeIterator iter = m_pAttributes.find (strName);
	if(iter != m_pAttributes.end())
		m_pAttributes.erase(iter);
	//m_pAttributes.removeKey( strName );
}

void CXMLElement::deleteAttribute(const tchar* pszName)
{
	CXMLAttribute* pAttribute = getAttribute( pszName );
	if ( pAttribute ) pAttribute->Delete();
}
