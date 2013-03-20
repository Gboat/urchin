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
#ifndef _ANALYZERFACTORY_H
#define _ANALYZERFACTORY_H

#include "../utility/StdHeader.h"
#include "Token.h"
#include "TokenFilter.h"
#include "Analyzer.h"

#include <map>

using namespace std;

namespace firtex
{
	namespace analyzer
	{		
		typedef map<tstring,CAnalyzer::Creator*> creator_map;
		typedef vector<tstring>	analyzer_list;
		class CAnalyzerFactory
		{
		public:
			CAnalyzerFactory(void);
			~CAnalyzerFactory(void);
		public:			
			/**
			 * create analyzer by identifier
			 * @param szIdentifier identifier of analyzer
			 * @param szParam parameter of analyzer
			 * @return analyzer
			 */
			static CAnalyzer*		createAnalyzer(const tchar* szIdentifier,const tchar* szParam);		

			/**
			 * create standard analyzer
			 * @param szParam parameter for analyzer
			 * @param szLanguage language
			 * @return analyzer
			 */
			static CAnalyzer*		standardAnalyzer(const tchar* szParam,const tchar* szLanguage = NULL);


			/**
			 * create stemmer
			 * @param szIdentifier identifier of stemmer,format:identifier:param1:param2...
			 * @return stemmer filter
			 */
			static CTokenFilter*	createStemmerFilter(const tchar* szIdendifier);

			/**
			 * create stop filter
			 * @param szIdentifier identifier of stop filter,format:identifier:param1:param2...			 
			 * @return stop filter
			 */
			static CTokenFilter*	createStopFilter(const tchar* szIdendifier);
		public:
			/**
			 * register a creator of collection			 
			 * @param pCreator creator of collection			 
			 */
			static void						registerCreator(CAnalyzer::Creator* pCreator);

			/**
			 * get a registered creator
			 * @param _identifier identifier of collection
			 * @return creator of collection
			 */
			static CAnalyzer::Creator*		getCreator(const tchar* _identifier);

			/**
			 * get creator map			 
			 */
			static creator_map&				getCreatorMap();

			/** 
			 * get registered analyzers
			 * @return registered analyzers 
			 */
			static analyzer_list			getRegisteredAnalyzers();
		};
	}
}

#endif
