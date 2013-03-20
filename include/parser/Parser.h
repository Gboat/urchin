//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/11/20
//

#ifndef _PARSER_H
#define _PARSER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../utility/StdHeader.h"
#include "../document/Document.h"
#include "../document/Field.h"
#include "../utility/FileReader.h"
#include "../utility/StringReader.h"
#include "../utility/Config.h"
#include "../document/DocumentSchema.h"
#include <map>

namespace firtex{namespace index{ class CIndexWriter;}}

namespace firtex
{
	namespace parser
	{
		/**
		 * 分析器抽象基类，用于分析文档内容并生成内部文档对象		 
		 */				
		class CParser
		{
		public:
			class Creator
			{
			protected:
				Creator()
					: m_identifier(_T("parser"))
				{}
			public:
				~Creator(){}
			public:
				/**
				 * create a collection object
				 * @return create a parser
				 */
				virtual CParser* create() = 0;	

				/** 
				 * get the identifier of collection
				 */
				virtual const tchar* getIdentifier(){return m_identifier.c_str();}

				/**
				 * get document type this parser can process
				 * @return document type name
				 */
				virtual const tchar* getDocType(){return m_docType.c_str();}

			protected:
				tstring m_identifier;
				tstring	m_docType;
			};

			typedef std::map<tstring,tstring> param_map;

		public:
			CParser(void);
			CParser(const tchar* field);
			virtual~CParser(void);		
		public:	
			/**
			 * 解析文档
			 * @param pDocInput 文档输入
			 * @param pResultDoc 解析结果文档
			 * @return true:解析成功,false:解析失败
			 */
			bool	parseDoc(firtex::document::CDocumentInput* pDocInput,document::CDocument* pResultDoc);

			/**
			 * 解析文档
			 * @param pDoc 文档对象
			 * @return true:解析成功,false:解析失败
			 */
			bool	parseDoc(firtex::document::CDocument* pDoc);

			/**
			 * 批量解析多个文档			 
			 * @param pDocs 文档对象数组
			 * @param numDocs 数组大小
			 * @return true:解析成功,false:解析失败
			 */
			int32_t	parseDocs(firtex::document::CDocument** pDocs,int32_t numDocs);			

			/**
			 * set field to be parsed
			 * @param field field name
			 */
			void	setField(const tchar* field);

			/**
			 * get field to be parsed 
			 * @return field name
			 */
			const tchar*	getField();			

		public:
			/**
			 * get index type
			 * @param field field name
			 * @return index type
			 */
			firtex::document::index_t			getIndex(const tchar* field = NULL);
			
			/**
			 * get store type
			 * @param field field name
			 * @return store type
			 */
			firtex::document::store_t			getStore(const tchar* field = NULL);

			/**
			 * get forward index type
			 * @param field field name
			 * @return forward index type
			 */
			firtex::document::forwardindex_t	getForwardIndex(const tchar* field = NULL);
		public:
			/**
			 * set parameter of parser
			 * @param pszParam parameter string, format: [field.]param_name=param_value;[field.]param_name2=param_value2
			 */
			virtual void	setParam(const tchar* pszParam);

			/**
			 * get parameter of parser
			 * @param pszParamName parameter name
			 * @param paramValue parameter value
			 */
			virtual bool	getParam(const tchar* pszParamName,tstring& paramValue);			
		public:
			inline void		setSchema(firtex::document::CDocumentSchema* pDocSchema);
		protected:
			/**
			 * parse document and extract content,sub-class MUST implement it
			 * @param pDocInput source input of document
			 * @param pResultDoc parsed document
			 * @return true if success, otherwise false
			 */			
			virtual bool		parseInternal(firtex::document::CDocumentInput* pDocInput,firtex::document::CDocument* pResultDoc) = 0;

			/**
			 * define the schema of document,sub-class MUST implement it 
			 * @param pSchema document schema
			 * @return schema id of main text
			 */			
			virtual schemaid_t	defineSchema(firtex::document::CDocumentSchema* pSchema) = 0;			
		protected:			
			param_map	m_params;
			tstring		m_sField;

			firtex::document::CDocumentSchema*	m_pDocSchema;
			
			friend class CIndexWriter;
		};

		//////////////////////////////////////////////////////////////////////////
		//inline functions		
		inline void CParser::setSchema(firtex::document::CDocumentSchema* pDocSchema)
		{
			m_pDocSchema = pDocSchema;
		}
		inline bool	CParser::parseDoc(firtex::document::CDocumentInput* pDocInput,firtex::document::CDocument* pResultDoc)
		{		
			try
			{
				FIRTEX_CLOG(level::dbg) << _T("Parsing...") << FIRTEX_ENDL;

				if(pDocInput->hasDelayedSchema())
				{
					pDocInput->setDelayedSchema(defineSchema(m_pDocSchema));
					pResultDoc->updateSchema(m_pDocSchema);
				}
				
				if(parseInternal(pDocInput,pResultDoc))
				{			
					pResultDoc->setStatus(firtex::document::doc_status::PARSED);
					return true;
				}
				else return false;
			}
			catch (CFileIOException& e) 
			{
				FIRTEX_CLOG(level::err) << _T("Parse doc file error: ") << ( (pDocInput->getPathInput() == NULL)?_T(""):pDocInput->getPathInput()->docPath.c_str()) << endl << _T("error code:") << e.what() << FIRTEX_ENDL;
			}		
			catch(...)
			{
				FIRTEX_CLOG(level::err) << _T("Parse doc file error: ") << ( (pDocInput->getPathInput() == NULL)?_T(""):pDocInput->getPathInput()->docPath.c_str()) << FIRTEX_ENDL;
			}

			return false;
		}
		inline bool	CParser::parseDoc(firtex::document::CDocument* pDoc)
		{
			FIRTEX_CLOG(level::dbg) << _T("Parsing...") <<FIRTEX_ENDL;
			document::CDocumentInput* pDocInput = pDoc->getInput();
			try
			{
				if(pDocInput->hasDelayedSchema())
				{
					pDocInput->setDelayedSchema(defineSchema(m_pDocSchema));
					pDoc->updateSchema(m_pDocSchema);
				}
				if(parseInternal(pDocInput,pDoc))
				{
					pDoc->setStatus(firtex::document::doc_status::PARSED);
					return true;
				}
				else 
				{
					return false;
				}
			}
			catch (CFileIOException& e) 
			{
				FIRTEX_CLOG(level::err) << _T("Parse doc file error: ") << ( (pDocInput->getPathInput() == NULL)?_T(""):pDocInput->getPathInput()->docPath.c_str()) << endl << _T("error code:") << e.what() << FIRTEX_ENDL;
			}		
			catch(...)
			{
				FIRTEX_CLOG(level::err) << ("Parse doc file error: ") << ( (pDocInput->getPathInput() == NULL)?_T(""):pDocInput->getPathInput()->docPath.c_str()) << FIRTEX_ENDL;
			}

			return false;
		}
	}
}

#endif
