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
#ifndef __SIMPLEPARSER_H
#define __SIMPLEPARSER_H

#include "Parser.h"
#include "../document/Document.h"
#include "../utility/FileReader.h"

using namespace firtex::document;
using namespace firtex::utility;

namespace firtex
{
	namespace parser
	{
		class CPlainParser : public CParser
		{		
		public:
			class Creator : public CParser::Creator
			{
			protected:
				Creator()
				{
					m_identifier += _T(".plain");
					m_docType = _T("txt");
				}
			public:
				~Creator(){}
			public:
				/**
				 * create a collection object
				 * @return create a parser
				 */
				CParser* create(){return new CPlainParser();};					
			public:
				static Creator* getInstance()
				{
					static CPlainParser::Creator INSTANCE;
					return &INSTANCE;
				}
			};
		public:
			CPlainParser();
			CPlainParser(const tchar* field);
			virtual~CPlainParser();		
		protected:
			/**
			 * parse document and extract content,sub class MUST implement it
			 * @param pDocInput source input of document
			 * @param pResultDoc parsed document
			 * @return true if success, otherwise false
			 */			
			bool		parseInternal(document::CDocumentInput* pDocInput,document::CDocument* pResultDoc);
			
			/**
			 * define the schema of document, MUST implement by sub-class
			 * @param pSchema document schema 
			 * @return schema id of main text
			 */			
			schemaid_t	defineSchema(CDocumentSchema* pSchema);
		protected:
			CFileReader*	m_pReader;
			schemaid_t		m_nTextId;			
		};		
	}
}


#endif
