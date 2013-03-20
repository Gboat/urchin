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
// Created	: 2005/7/24
//
#ifndef __DATETIMEANALYZER_H
#define __DATETIMEANALYZER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Analyzer.h"

namespace firtex
{
	namespace analyzer
	{

		struct DateTime
		{
			int year, 
				month,
				day,
				hour,
				minute,
				second;
		};

		class CDateTimeAnalyzer :	public CAnalyzer
		{		
		public:
			class Creator : public CAnalyzer::Creator
			{
			protected:
				Creator()					
				{
					m_identifier += _T(".datetime");
				}
			public:
				~Creator(){}
			public:
				/**
				 * create a collection object
				 * @return create a analyzer
				 */
				virtual CAnalyzer* create(){return new CDateTimeAnalyzer();}

			public:
				static Creator* getInstance()
				{
					static CDateTimeAnalyzer::Creator INSTANCE;
					return &INSTANCE;
				}
			};

		public:
			CDateTimeAnalyzer(void);
			virtual ~CDateTimeAnalyzer(void);
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
			const tchar*	getRecommendIndexer(){return _T("indexer.datetime");};

			/**
			 * acceptable encoding of analyzer, sub class must implement it
			 * @return acceptable encoding name of analyzer
			 */
			const tchar*	acceptableEncoding(){return _T("");};

			/**
			 * get identifier of analyzer
			 * @return the identifier
			 */
			const tchar*	getIdentifier(){return CDateTimeAnalyzer::Creator::getInstance()->getIdentifier();};
		protected:
			/**
			 * analyze text reader, generate tokens, must implement by sub class
			 * @param pUnAnalyzedTokens unanalyzed tokens
			 * @return analyzed tokens,an internal object
			 */
			CTokens*		nextTokensInternal(CTokens* pUnAnalyzedTokens);	
		public:
			/**
			 * 解析"1999-3-3 4:43:44"， "1999/3/3 4:43:44"，"1999年3月3日 4点43分44秒"，
			 * "1999年3月3日 4点43分"，"1999年3月3日 4点"，"1999年3月3日"，...等形式的
			 * 时间日期字符串，并转换成time64_t
			 * @param str 时间日期字符串
			 * @return 转换成time64_t的时间结果,如果<code>str</code>则返回-1
			 */
			static int64_t		parse(const tchar* str);
		protected:
			static bool	checkDate(const char* pszDate, DateTime &dt);					
		};
	}
}

#endif
