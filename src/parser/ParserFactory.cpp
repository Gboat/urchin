//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈğ½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/11/20
//
#include "parser/ParserFactory.h"
#include "plugin/Plugins.h"
#include "parser/PlainParser.h"
#include "parser/HTMLParser.h"


namespace firtex
{
	namespace parser
	{
		CParserFactory::CParserFactory(void)
		{
		}

		CParserFactory::~CParserFactory(void)
		{
		}
		CParser* CParserFactory::createParser(firtex::document::doctype_t dt,const tchar* param)
		{			
			tstring _identifier = firtex::document::CDocumentType::getInstance()->typeToIdentifier(dt);
			return createParserByIdentifier(_identifier.c_str(),param);						
		}
		CParser* CParserFactory::createParser(const tchar* ext,const tchar* param)
		{			
			tstring _identifier = firtex::document::CDocumentType::getInstance()->nameToIdentifier(ext);
			return createParserByIdentifier(_identifier.c_str(),param);									
		}
		CParser* CParserFactory::createParserByIdentifier(const tchar* identifier,const tchar* param)
		{
			CParser*	pParser = firtex::plugin::GlobalPlugins.ParserPlugin.getParserByIdentifier(identifier);
			if(!pParser)
			{
				CParser::Creator* pCreator = CParserFactory::getCreator(identifier);
				if(pCreator)
				{
					pParser = pCreator->create();
				}
			}
			if(pParser)
			{
				if(param && *param)
					pParser->setParam(param);
			}			
			return pParser;
		}

		void CParserFactory::registerCreator(CParser::Creator* pCreator)
		{
			tstring str = pCreator->getIdentifier();
			getCreatorMap().insert(make_pair(str,pCreator));
		}
		CParser::Creator* CParserFactory::getCreator(const tchar* _identifier)
		{
			tstring str = _identifier;
			creator_map& am = getCreatorMap();
			creator_map::iterator iter = am.find(str);
			if(iter != am.end())
				return iter->second;
			return NULL;
		}		
		creator_map& CParserFactory::getCreatorMap()
		{
			static creator_map INSTANCE;
			return INSTANCE;
		}
		parser_list CParserFactory::getRegisteredParsers()
		{			
			parser_list  pl;
			creator_map& am = getCreatorMap();
			creator_map::iterator iter = am.begin();
			while(iter != am.end())
			{
				pl.push_back(iter->first);
				iter++;
			}
			return pl;
		}
	}
}
