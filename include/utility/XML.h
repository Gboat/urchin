#ifndef _XML_H
#define _XML_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typedefs.h"
#include "StringUtils.h"
#include "Misc.h"

#include <map>
#include <list>
using namespace std;




namespace firtex
{
	namespace utility
	{
		class CXMLNode;
		class CXMLElement;
		class CXMLAttribute;
		class CXMLNode
		{
			// Construction
		public:
			CXMLNode(CXMLElement* pParent = NULL, const tchar* pszName = NULL);
			virtual ~CXMLNode();

			// Attributes
		protected:
			int				m_nNode;
			CXMLElement*	m_pParent;
			tstring			m_sName;
			tstring			m_sValue;

			enum { xmlNode, xmlElement, xmlAttribute };

			// Operations
		public:
			inline int				getType() ;
			inline CXMLNode*		asNode() ;
			inline CXMLElement*		asElement() ;
			inline CXMLAttribute*	asAttribute() ;
		public:
			inline CXMLElement*	getParent() ;
			inline void			Delete();
		public:
			inline tstring		getName() ;
			inline void			setName(const tchar* pszValue);
			inline bool			isNamed(const tchar* pszName) ;
			inline tstring		getValue() ;
			inline void			setValue(const tchar* pszValue);
		protected:
			static bool		parseMatch(const tchar*& pszXML, const tchar* pszToken);
			static bool		parseIdentifier(const tchar*& pszXML, tstring& strIdentifier);
		public:
			static tstring	stringToValue(const tchar*& pszXML, int nLength);
			static void		valueToString(const tchar* pszValue, tstring& strXML);
			static void		uniformString(tstring& str);

			friend class CXMLElement;			
		};

		typedef list<CXMLElement*> ElementList;
		typedef ElementList::iterator ElementIterator;
		typedef map<tstring,CXMLAttribute*> AttributeMap;
		typedef AttributeMap::iterator AttributeIterator;
		typedef pair<tstring,CXMLAttribute*> AttributePair;
		class CXMLElement : public CXMLNode
		{
			// Construction
		public:
			CXMLElement(CXMLElement* pParent = NULL, const tchar* pszName = NULL);
			virtual ~CXMLElement();

			// Attributes
		protected:
			ElementList			m_pElements;
			AttributeMap		m_pAttributes;

			// Operations
		public:
			CXMLElement*			clone(CXMLElement* pParent = NULL);
			inline CXMLElement*		detach();
		public:
			inline CXMLElement*		addElement(const tchar* pszName);
			inline CXMLElement*		addElement(CXMLElement* pElement);
			inline int				getElementCount() ;
			inline CXMLElement*		getFirstElement() ;
			inline ElementIterator	getElementIterator() ;
			inline CXMLElement*		getNextElement(ElementIterator& iter) ;
			inline bool				hasNextElement(ElementIterator& iter) ;
			inline CXMLElement*		getElementByName(const tchar* pszName) ;
			inline CXMLElement*		getElementByName(const tchar* pszName, bool bCreate);
			inline void				removeElement(CXMLElement* pElement);
			void					deleteAllElements();
		public:
			inline CXMLAttribute*	addAttribute(const tchar* pszName, const tchar* pszValue = NULL);
			inline CXMLAttribute*	addAttribute(CXMLAttribute* pAttribute);
			inline int				getAttributeCount() const;

			inline AttributeIterator	getAttributeIterator() ;
			inline CXMLAttribute*		getNextAttribute(AttributeIterator& iter) ;
			inline bool					hasNextAttribute(AttributeIterator& iter) ;
			inline CXMLAttribute*	getAttribute(const tchar* pszName) ;
			inline tstring			getAttributeValue(const tchar* pszName, const tchar* pszDefault = NULL) ;
			inline void				removeAttribute(CXMLAttribute* pAttribute);
			inline void				deleteAttribute(const tchar* pszName);
			void					deleteAllAttributes();
		public:
			tstring					toString(bool bHeader = false, bool bNewline = false);
			void					toString(tstring& strXML, bool bNewline = false,int nDepth = 0);
			void					toFile(const tstring& sPath, bool bHeader = false);
			bool					parseString(const tchar*& strXML);
			bool					equals(CXMLElement* pXML) ;
			//tstring				getRecursiveWords();
			//void					addRecursiveWords(tstring& strWords);

			static CXMLElement*		fromString(const tchar* pszXML, bool bHeader = false);
			static CXMLElement*		fromBytes(byte* pByte, size_t nByte, bool bHeader = false);
			static CXMLElement*		fromFile(const tstring& sPath, bool bHeader = false);

			
		};


		class CXMLAttribute : public CXMLNode
		{
			// Construction
		public:
			CXMLAttribute(CXMLElement* pParent, const tchar* pszName = NULL);
			virtual ~CXMLAttribute();

			// Attributes
		public:
			static const tchar*	xmlnsSchema;
			static const tchar*	xmlnsInstance;
			static const tchar*	schemaName;

			// Operations
		public:
			CXMLAttribute*	clone(CXMLElement* pParent = NULL);
			void			toString(tstring& strXML);
			bool			parseString(const tchar*& strXML);
			bool			equals(CXMLAttribute* pXML) ;
		};

#include "XML.inl"

	}
}

#endif
