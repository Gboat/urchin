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
#include "analyzer/Analyzer.h"
#include "parser/Parser.h"
#include "document/Field.h"
#include "parser/ParserFactory.h"
#include "analyzer/AnalyzerFactory.h"
#include "utility/ParamParser.h"

#ifdef HAS_ICONV_FILTER
	#include "iconvfilter/include/IconvFilterInclude.h"
#endif

using namespace firtex::utility;

namespace firtex
{
	namespace analyzer
	{	
		int32_t CAnalyzer::DEFAULT_MAX_BYTES_TO_ANALYZE = 10*1024*1024;///10M

		CAnalyzer::CAnalyzer()
			: m_pPostTokenFilter(NULL)
			, m_pAnteTokenFilter(NULL)
			, m_bConvertFilterAttached(false)
			, m_lastEncoding(charset::UNKNOWN)
			, m_pAnalyzedTokens(NULL)
			, m_pUnAnalyzedTokens(NULL)
			, m_pReaderBuffer(NULL)
			, m_nBufSize(0)
			, m_nTextLength(0)
			, m_nMaxTokensToAnalyze(GlobalConfig.Index.maxIndexTerms)
		{		
		}

		CAnalyzer::~CAnalyzer(void)
		{				
			if(m_pUnAnalyzedTokens)
			{
				delete m_pUnAnalyzedTokens;
				m_pUnAnalyzedTokens = NULL;
			}	
			
			if(m_pAnalyzedTokens)
			{
				delete m_pAnalyzedTokens;
				m_pAnalyzedTokens = NULL;
			}

			if(m_pPostTokenFilter)
			{
				delete m_pPostTokenFilter;
				m_pPostTokenFilter = NULL;
			}
			if(m_pAnteTokenFilter)
			{
				delete m_pAnteTokenFilter;
				m_pAnteTokenFilter = NULL;
			}			
			
			if(m_pReaderBuffer)
			{
				delete[] m_pReaderBuffer;
				m_pReaderBuffer = NULL;
			}
		}					
		void CAnalyzer::attachPostFilter(CTokenFilter* pPostTokenFilter)
		{
			if(m_pPostTokenFilter)
				m_pPostTokenFilter->attachFilter(pPostTokenFilter);
			else 
				m_pPostTokenFilter = pPostTokenFilter;
		}

		CTokenFilter* CAnalyzer::detachPostFilter()
		{
			CTokenFilter* pPostFilter = m_pPostTokenFilter;
			m_pPostTokenFilter = NULL;
			return pPostFilter;
		}					
		void CAnalyzer::attachAnteFilter(CTokenFilter* pAnteTokenFilter)
		{
			if(m_pAnteTokenFilter)
				m_pAnteTokenFilter->attachFilter(pAnteTokenFilter);
			else 
				m_pAnteTokenFilter = pAnteTokenFilter;
		}

		CTokenFilter* CAnalyzer::detachAnteFilter()
		{
			CTokenFilter* pAnteFilter = m_pAnteTokenFilter;
			m_pAnteTokenFilter = NULL;
			return pAnteFilter;
		}					
		void CAnalyzer::attachEncodingFilter()
		{
#ifdef HAS_ICONV_FILTER
			attachAnteFilter(new CIconvFilter(acceptableEncoding()));
			m_bConvertFilterAttached = true;
#endif 
		}
		void CAnalyzer::setParam(const tchar* pszParam)
		{
			if(!pszParam || !(*pszParam))
				return;
			CParamParser pp;
			pp.parse(pszParam,_T(";"));
			tstring str;
			if(pp.getParam(_T("stopword"),str) && !str.empty())
			{	
				CTokenFilter* pFilter = CAnalyzerFactory::createStopFilter(str.c_str());
				if(pFilter)
				{
					if(firtex::string_utils::startsWithNoCase(pFilter->getIdentifier(),_T("filter.ante")))
						attachAnteFilter(pFilter);
					else if(firtex::string_utils::startsWithNoCase(pFilter->getIdentifier(),_T("filter.post")))
						attachPostFilter(pFilter);
					else 
						delete pFilter;
				}					
			}

			if(pp.getParam(_T("stemmer"),str) && !str.empty())
			{		
				CTokenFilter* pFilter = CAnalyzerFactory::createStemmerFilter(str.c_str());
				if(pFilter)
				{
					if(firtex::string_utils::startsWithNoCase(pFilter->getIdentifier(),_T("filter.ante")))
						attachAnteFilter(pFilter);
					else if(firtex::string_utils::startsWithNoCase(pFilter->getIdentifier(),_T("filter.post")))
						attachPostFilter(pFilter);
				}
			}
			else if(pp.getParam(_T("lowercase"),str) && !str.empty())
			{	
				/*if(firtex::string_utils::endsWithNoCase(_T("filter.ante.lowercase"),str.c_str()))
				{
					CAnteTokenFilter* pFilter = new CLowercaseAnteFilter();
					attachAnteFilter(pFilter);
				}		*/			
			}
		}
	}
}
