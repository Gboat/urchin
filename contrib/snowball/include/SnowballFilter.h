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
// Created	: 2007/1/28
//
#ifndef _SNOWBALLFILTER_H
#define _SNOWBALLFILTER_H

#include "utility/StringReader.h"
#include "analyzer/TokenFilter.h"

using namespace firtex::utility;

namespace firtex
{
	namespace analyzer
	{
		class CSnowballWrapper;
		class CSnowballFilter : public CTokenFilter
		{
		public:
			class Creator : public CTokenFilter::Creator
			{
			protected:
				Creator()					
				{
					m_identifier += _T(".ante.snowball");
				}
			public:
				~Creator(){}
			public:
				/**
				 * create a collection object
				 * @param param parameter for filter
				 * @return create a analyzer
				 */
				virtual CTokenFilter* create(const tchar* param){return new CSnowballFilter(param);}
			public:
				static Creator* getInstance()
				{
					static CSnowballFilter::Creator INSTANCE;
					return &INSTANCE;
				}
			};

		public:

			CSnowballFilter(const tchar* param);

			/**
			 * constructor
			 * @param szLanguage language,NULL for English			 
			 * @param szEnc encode, NULL for utf-8
			 */
			CSnowballFilter(const tchar* szLanguage,const tchar* szEnc);

			/**
			 * constructor
			 * @param szLanguage language,NULL for English
			 * @param szIrregularWords irregular words file
			 * @param szEnc encode, NULL for utf-8
			 * @param pPreFilter filter before this filter
			 */
			CSnowballFilter(const tchar* szLanguage,const tchar* szIrregularWords,const tchar* szEnc,CTokenFilter* pPreFilter);			
			virtual ~CSnowballFilter(void);
		public:
			/**
			 * get identifier of Filter
			 */
			const tchar* getIdentifier(){return CSnowballFilter::Creator::getInstance()->getIdentifier();};

			/**
			 * filter interface, must implement by sub class
			 * @param pInputTokens input tokens
			 * @return output tokens, may be different from pInputTokens
			 */			
			CTokens* filterInternal(CTokens* pInputTokens);
		protected:
			CSnowballWrapper*	m_pSnowball;
			CStringReader*		m_pStemmedReader;
		};	
	}
}

#endif

