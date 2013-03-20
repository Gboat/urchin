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
#include "parser/Parser.h"
#include "utility/ParamParser.h"
using namespace firtex::utility;

namespace firtex
{
	namespace parser
	{
		CParser::CParser(void)
			: m_pDocSchema(NULL)
			, m_sField(_T("TEXT"))
		{			
		}
		CParser::CParser(const tchar* field)
			: m_pDocSchema(NULL)
			, m_sField(field)
		{
		}
		CParser::~CParser(void)
		{
			m_pDocSchema = NULL;			
		}				

		int32_t CParser::parseDocs(document::CDocument** pDocs,int32_t numDocs)
		{
			try
			{
				FIRTEX_CLOG(level::dbg) << _T("Parsing...") << FIRTEX_ENDL;				
				document::CDocumentInput* pDocInput = NULL;
				int32_t numParsed = 0;
				for(int32_t i = 0;i<numDocs;i++)
				{
					pDocInput = pDocs[i]->getInput();
					if(pDocInput->hasDelayedSchema())
					{
						pDocInput->setDelayedSchema(defineSchema(m_pDocSchema));
						pDocs[i]->updateSchema(m_pDocSchema);
					}
					if(parseInternal(pDocInput,pDocs[i]))
					{	
						numParsed++;						
					}					
				}
				return numParsed;
				
			}
			catch (CFileIOException& e) 
			{
				FIRTEX_CLOG(level::err) << _T("Parse doc files error: ") <<  _T("error code:") << e.what() << FIRTEX_ENDL;
			}		
			catch(...)
			{
				FIRTEX_CLOG(level::err) << _T("Parse doc files error: ") << FIRTEX_ENDL;
			}

			return 0;
		}
		
		void CParser::setParam(const tchar* pszParam)
		{
			CParamParser pp;
			if(pp.parse(pszParam))
			{
				tstring name,value;
				for (size_t i = 0;i<pp.getParamCount();i++)
				{
					if(pp.getParam(i,name,value))
					{
						if(!_tcsicmp(name.c_str(),_T("field")))
							m_sField = value;
						else 
							m_params.insert(make_pair(name,value));

					}
				}				
			}
		}
		
		bool CParser::getParam(const tchar* pszParamName,tstring& paramValue)
		{
			param_map::iterator iter = m_params.find(pszParamName);
			if(iter != m_params.end())
			{
				paramValue = iter->second;
				return true;
			}
			return false;
		}

		firtex::document::index_t CParser::getIndex(const tchar* field)
		{
			firtex::document::index_t indexType = firtex::document::Index::ANALYZE;
			tstring str,value;
			if(field && *field)
				str = field;
			else 
				str = m_sField;
			str += _T(".index");
			if(getParam(str.c_str(),value))
			{
				indexType = firtex::document::Index::nameToType(value.c_str());
			}
			return indexType;
		}

		
		firtex::document::store_t CParser::getStore(const tchar* field)
		{
			firtex::document::store_t storeType = firtex::document::Store::NO;
			tstring str,value;
			if(field && *field)
				str = field;
			else 
				str = m_sField;
			str += _T(".store");
			if(getParam(str.c_str(),value))
			{
				storeType = firtex::document::Store::nameToType(value.c_str());
			}
			return storeType;
		}

		firtex::document::forwardindex_t CParser::getForwardIndex(const tchar* field)
		{
			firtex::document::forwardindex_t fiType = firtex::document::ForwardIndex::NO;
			tstring str,value;
			if(field && *field)
				str = field;
			else 
				str = m_sField;
			str += _T(".forwardindex");
			if(getParam(str.c_str(),value))
			{
				fiType = firtex::document::ForwardIndex::nameToType(value.c_str());
			}
			return fiType;
		}

		void CParser::setField(const tchar* field)
		{
			m_sField = field;
		}

		const tchar* CParser::getField()
		{
			return m_sField.c_str();
		}
	}
}
