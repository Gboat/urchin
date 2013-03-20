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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2007/6/09
//


#include "index/DateTimeFactory.h"
#include "search/Similarity.h"
#include "search/SortFieldCache.h"
#include "collection/CollectionFactory.h"
#include "collection/FirteXCollection.h"
#include "collection/DirCollection.h"
#include "collection/TrecCollection.h"
#include "collection/GZTrecCollection.h"
#include "collection/ZIPTrecCollection.h"
#include "collection/FileListCollection.h"
#include "collection/TWCollection.h"
#include "collection/GZTWCollection.h"
//#include "collection/FilteredDirCollection.h"

#include "index/DictHashFactory.h"

#include "analyzer/AnalyzerFactory.h"
#include "analyzer/StandardAnalyzer.h"
#include "analyzer/DateTimeAnalyzer.h"
#include "analyzer/NumericAnalyzer.h"

#include "parser/ParserFactory.h"
#include "parser/PlainParser.h"
#include "parser/HTMLParser.h"

#include "utility/Starter.h"
#include <map>
using namespace std;

namespace firtex
{
	namespace utility
	{
		Starter::Starter(void)
		{
		}

		Starter::~Starter(void)
		{
		}

		void Starter::initialize()
		{						
			CFieldFactory::registerFactory(CDictHashFactory::getInstance());
			CFieldFactory::registerFactory(CDateTimeFactory::getInstance());

			firtex::collection::CCollectionFactory::registerCreator(firtex::collection::CFirteXCollection::Creator::getInstance());
			firtex::collection::CCollectionFactory::registerCreator(firtex::collection::CDirCollection::Creator::getInstance());
			firtex::collection::CCollectionFactory::registerCreator(firtex::collection::CFileListCollection::Creator::getInstance());
			firtex::collection::CCollectionFactory::registerCreator(firtex::collection::CTrecCollection::Creator::getInstance());
			firtex::collection::CCollectionFactory::registerCreator(firtex::collection::CGZTrecCollection::Creator::getInstance());
			firtex::collection::CCollectionFactory::registerCreator(firtex::collection::CZIPTrecCollection::Creator::getInstance());
			firtex::collection::CCollectionFactory::registerCreator(firtex::collection::CTWCollection::Creator::getInstance());
			firtex::collection::CCollectionFactory::registerCreator(firtex::collection::CGZTWCollection::Creator::getInstance());
//			firtex::collection::CCollectionFactory::registerCreator(firtex::collection::CFilteredDirCollection::Creator::getInstance());


			firtex::analyzer::CAnalyzerFactory::registerCreator(firtex::analyzer::CStandardAnalyzer::Creator::getInstance());
			firtex::analyzer::CAnalyzerFactory::registerCreator(firtex::analyzer::CDateTimeAnalyzer::Creator::getInstance());
			firtex::analyzer::CAnalyzerFactory::registerCreator(firtex::analyzer::CNumericAnalyzer::Creator::getInstance());

			firtex::parser::CParserFactory::registerCreator(firtex::parser::CPlainParser::Creator::getInstance());
			firtex::parser::CParserFactory::registerCreator(firtex::parser::CHTMLParser::Creator::getInstance());

			firtex::document::CDocumentType::getInstance()->registerType(firtex::parser::CPlainParser::Creator::getInstance()->getDocType(),firtex::parser::CPlainParser::Creator::getInstance()->getIdentifier());
			firtex::document::CDocumentType::getInstance()->registerType(firtex::parser::CHTMLParser::Creator::getInstance()->getDocType(),firtex::parser::CHTMLParser::Creator::getInstance()->getIdentifier());
		
		}
		
		void Starter::terminate()
		{
			if(firtex::search::CSimilarity::getDefault())
				delete firtex::search::CSimilarity::getDefault();
			delete firtex::search::CSortFieldCache::DEFAULT;
			firtex::search::CSortFieldCache::DEFAULT = NULL;
		}
	}
}

