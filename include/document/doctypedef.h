//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/11/20
//
#ifndef _DOCTYPEDEF_H
#define _DOCTYPEDEF_H

#include "utility/StdHeader.h"
#include "utility/StringUtils.h"
#include <string>
#include <map>
using namespace std;

namespace firtex
{
	namespace document
	{
		typedef int32_t doctype_t;
		
		namespace doc_type
		{
			//////////////////////////////////////////////////////////////////////////
			///document type define			
			const static doctype_t UNKNOWN = 0;
		}			

		class CDocumentType
		{
		protected:
			struct DOC_TYPE 
			{
				doctype_t	id;
				tstring		name;
				tstring		identifier;
			};	

			typedef map<tstring,CDocumentType::DOC_TYPE*> doctype_map;
			typedef map<doctype_t,CDocumentType::DOC_TYPE*> typeid_map;
		protected:
			CDocumentType()
			{					
			}
		public:
			virtual ~CDocumentType()
			{
				doctype_map::iterator iter = m_nameMap.begin();
				while (iter != m_nameMap.end())
				{
					delete iter->second;
					iter++;
				}
				m_nameMap.clear();
				m_identifierMap.clear();
				m_idMap.clear();
			}
		public:
			void	registerType(const tchar* _name,const tchar* _identifier)
			{
				DOC_TYPE* pType = new DOC_TYPE();
				pType->id = (doctype_t)m_nameMap.size() + 1;
				pType->name = _name;
				pType->identifier = _identifier;
				pType->name = firtex::string_utils::toLower(pType->name);
				if(m_nameMap.find(pType->name) != m_nameMap.end())
				{
					delete pType;
				}
				else 
				{
					m_nameMap.insert(make_pair(pType->name,pType));
					m_identifierMap.insert(make_pair(pType->identifier,pType));
					m_idMap.insert(make_pair(pType->id,pType));
				}					
			}

			tstring typeToName(doctype_t _doctype)
			{
				typeid_map::iterator iter = m_idMap.find(_doctype);
				if(iter != m_idMap.end())
				{
					return iter->second->name;
				}
				return _T("");
			}	

			doctype_t nameToType(const tchar* _name)
			{
				tstring str = _name;
				str = firtex::string_utils::toLower(str);
				doctype_map::iterator iter = m_nameMap.find(str);
				if(iter != m_nameMap.end())
				{
					return iter->second->id;
				}
				return doc_type::UNKNOWN;
			}

			tstring typeToIdentifier(doctype_t _doctype)
			{
				typeid_map::iterator iter = m_idMap.find(_doctype);
				if(iter != m_idMap.end())
				{
					return iter->second->identifier;
				}
				return _T("");
			}

			tstring nameToIdentifier(const tchar* _name)
			{
				tstring str = _name;
				str = firtex::string_utils::toLower(str);
				doctype_map::iterator iter = m_nameMap.find(str);
				if(iter != m_nameMap.end())
				{
					return iter->second->identifier;
				}
				return _T("");
			}

			int32_t	numTypes(){return (int32_t)m_nameMap.size();}
		public:
			static CDocumentType* getInstance()
			{
				static CDocumentType INSTANCE;
				return &INSTANCE;
			}
		protected:
			doctype_map		m_nameMap;
			doctype_map		m_identifierMap;
			typeid_map		m_idMap;
		};

		//////////////////////////////////////////////////////////////////////////
		///document status define

		typedef int32_t docstatus_t;
		namespace doc_status
		{
			const static docstatus_t UNPARSED	= 1;		/// unparsed
			const static docstatus_t PARSED		= 2;		/// parsed but not analyzed
			const static docstatus_t ANALYZED	= 3;		/// analyzed but not indexed
			const static docstatus_t INDEXED	= 4;		/// indexed			
		}		
	}
}


#endif
