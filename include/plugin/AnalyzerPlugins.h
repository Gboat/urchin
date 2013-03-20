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
// Created	: 2006/5/9
//
#ifndef _ANALYZERPLUGINS_H
#define _ANALYZERPLUGINS_H

#include "../analyzer/Analyzer.h"
#include <map>
#include <string>
#include "FirteXCOM.h"

using namespace firtex::analyzer;
using namespace std;

namespace firtex
{
	namespace plugin
	{
		class CPlugin;
		class CAnalyzerPlugin : public CAnalyzer
		{
		public:
			CAnalyzerPlugin(IAnalyzerPlugin* pAnalyzerInterface);
			CAnalyzerPlugin(IAnalyzerPlugin* pAnalyzerInterface,CParser* pParser);
			virtual ~CAnalyzerPlugin();
		public:			
			/** 
			 * create a analyzer specific CTokens for storing analyzed tokens
			 * @return CTokens object
			 */
			CTokens*		newTokenVector();

			/**
			 * 获取Analyzer推荐使用的Indexer
			 * @return 推荐使用的Indexer
			 */
			const tchar*	getRecommendIndexer();

			/**
			 * acceptable encoding of analyzer, sub class must implement it
			 * @return acceptable encoding name of analyzer
			 */
			const tchar*	acceptableEncoding();

			/**
			 * get identifier of analyzer
			 * @return the identifier
			 */
			const tchar*	getIdentifier();
		protected:
			/**
			 * analyze text reader, generate tokens, must implement by sub class
			 * @param pUnAnalyzedTokens unanalyzed tokens
			 * @return analyzed tokens,an internal object
			 */
			CTokens*		nextTokensInternal(CTokens* pUnAnalyzedTokens);			
		protected:		
			
			IAnalyzerPlugin*	m_pAnalyzerInterface;
			ITokens*			m_pAnalyzedTokenVectorInterface;
			CTokens*			m_pAnalyzedTokenVector;
			
			ITokens*			m_pUnAnalyzedTokenVectorInterface;			
			CTokens*			m_pLastUnAnalyzedTokenVector;
		};

		class CAnalyzerPlugins
		{
		public:
			CAnalyzerPlugins(void);
			~CAnalyzerPlugins(void);
		public:
			CAnalyzer*	getAnalyzer(const tchar* analyzerName);			

			/**
			 * load a analyzer plugin
			 * @param pPlugin the plugin 
			 * @return the plugin is loaded or not
			 */
			bool			loadPlugin(CPlugin* pPlugin);
		protected:
			/**
			 * clear all plugins
			 */
			void			clear();
		protected:
			map<tstring,IAnalyzerPlugin*>m_pluginMap;
			bool				m_bCOMInit;
		};
	}
}
#endif
