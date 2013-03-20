//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: Kevin Zhang,π˘»Ω‹(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/11/20
//

#ifndef _CHINESEANALYZER_H
#define _CHINESEANALYZER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "Analyzer.h"

#define _CHARSET_SIZE 65536

namespace firtex
{
	namespace analyzer
	{
		class CStandardAnalyzer : public CAnalyzer
		{
		public:
			class Creator : public CAnalyzer::Creator
			{
			protected:
				Creator()					
				{
					m_identifier += _T(".standard");
				}
			public:
				~Creator(){}
			public:
				/**
				 * create a collection object
				 * @return create a analyzer
				 */
				virtual CAnalyzer* create(){return new CStandardAnalyzer();}
			public:
				static Creator* getInstance()
				{
					static CStandardAnalyzer::Creator INSTANCE;
					return &INSTANCE;
				}
			};
		private:
			struct state
			{//state information in double-array trie
				int32_t base;//base value
				int32_t check;//check value
				int32_t handle;//handle for dictionary entry
			};
			typedef struct state STATE,*PSTATE;
		public:
			CStandardAnalyzer(const tchar *sDicName);
			CStandardAnalyzer();
			virtual ~CStandardAnalyzer(void);			
		public:
			/** 
			 * create a analyzer specific CTokens for storing analyzed tokens
			 * @return CTokens object, an internal object
			 */
			CTokens*		newTokenVector();

			/**
			 * get a suitable indexer for the analyzer
			 * @return indexer name
			 */
			const tchar*	getRecommendIndexer(){return _T("indexer.dicthash");};

			/**
			 * acceptable encoding of analyzer, sub class must implement it
			 * @return acceptable encoding name of analyzer
			 */
			const tchar*	acceptableEncoding();		

			/**
			 * get identifier of analyzer
			 * @return the identifier
			 */
			const tchar*	getIdentifier(){return CStandardAnalyzer::Creator::getInstance()->getIdentifier();};
		protected:
			/**
			 * analyze text reader, generate tokens, must implement by sub class
			 * @param pUnAnalyzedTokens unanalyzed tokens
			 * @return analyzed tokens,if pInput is not null, then return pInput,otherwise create a tokens and return it.
			 */
			CTokens*		nextTokensInternal(CTokens* pUnAnalyzedTokens);
		public:
			/** ªÒµ√¥ ”Ôid */
			wordid_t		getWordId(const char *cWord);
		protected:
			bool			load(const tchar *sFilename);
		protected:		
			PSTATE	m_pData;
			int32_t		m_nLength;//Array length
			int32_t		m_nLowerBound;//Lower bound in the array
			int32_t		m_nItemCount;//Item Count
		private:
			int32_t		m_charsetSize;
			int32_t		m_charsetfreq[_CHARSET_SIZE],m_freq[_CHARSET_SIZE];
			int32_t		m_LowerChar,m_UpperChar;
			int32_t		m_charset[_CHARSET_SIZE];
		};

	}
}

#endif
