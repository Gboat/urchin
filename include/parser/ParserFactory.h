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
#ifndef _PARSERFACTORY_H
#define _PARSERFACTORY_H

#include "../document/doctypedef.h"
#include "Parser.h"

namespace firtex
{
	namespace parser
	{		
		typedef map<tstring,CParser::Creator*> creator_map;
		typedef vector<tstring>	parser_list;
		class CParserFactory
		{
		public:
			CParserFactory(void);
			~CParserFactory(void);
		public:
			/**
			 * create parser by document type
			 * @param dt document type 
			 * @param param parameter of parser
			 * @return parser
			 */
			static CParser*	createParser(firtex::document::doctype_t dt,const tchar* param);

			/**
			 * create parser by file extension 
			 * @param ext file extension
			 * @param param parameter of parser
			 * @return parser
			 */
			static CParser*	createParser(const tchar* ext,const tchar* param);

			/**
			 * create parser by parser identifier
			 * @param identifier identifier of parser
			 * @param param parameter of parser
			 * @return parser
			 */
			static CParser*	createParserByIdentifier(const tchar* identifier,const tchar* param);
		public:			
			/**
			 * register a creator of collection			 
			 * @param pCreator creator of collection			 
			 */
			static void						registerCreator(CParser::Creator* pCreator);

			/**
			 * get a registered creator
			 * @param _identifier identifier of collection
			 * @return creator of collection
			 */
			static CParser::Creator*		getCreator(const tchar* _identifier);

			/**
			 * get creator map			 
			 */
			static creator_map&				getCreatorMap();

			/** 
			 * get registered parsers
			 * @return registered parsers 
			 */
			static parser_list				getRegisteredParsers();
		};
	}
}

#endif
