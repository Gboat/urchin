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
// Created	: 2007/3/6
//

#include "include/SnowballFilter.h"
#include "include/SnowballWrapper.h"
#include "utility/ParamParser.h"

namespace firtex
{
	namespace analyzer
	{
		CSnowballFilter::CSnowballFilter(const tchar* param)
			: m_pStemmedReader(NULL)
			, m_pSnowball(NULL)
		{
			CParamParser pp;
			if(pp.parse(param))
			{
				tstring lang,enc;
				if(pp.getParam("language",lang) == false)
				{
					lang = _T("english");
				}
				pp.getParam(_T("encoding"),enc);
				m_pSnowball = new CSnowballWrapper(lang.c_str(),enc.empty()?NULL:enc.c_str());
			}			
		}
		CSnowballFilter::CSnowballFilter(const tchar* szLanguage,const tchar* szEnc)
			: m_pStemmedReader(NULL)
			, m_pSnowball(new CSnowballWrapper(szLanguage,szEnc))
		{
		}
		CSnowballFilter::CSnowballFilter(const tchar* szLanguage,const tchar* szIrregularWords,const tchar* szEnc,CTokenFilter* pPreFilter)
			: CTokenFilter(pPreFilter)
			, m_pStemmedReader(NULL)
			, m_pSnowball(new CSnowballWrapper(szLanguage,szIrregularWords,szEnc))
		{
		}

		CSnowballFilter::~CSnowballFilter(void)
		{
			if(m_pStemmedReader)
			{
				delete m_pStemmedReader;
				m_pStemmedReader = NULL;
			}
			if(m_pSnowball)
			{
				delete m_pSnowball;
				m_pSnowball = NULL;
			}
		}
	
		CTokens* CSnowballFilter::filterInternal(CTokens* pInputTokens)
		{
			//TODO:
			/*size_t nTextLen;
			char* pInputBuf = pInputReader->readWithoutCopy((size_t&)nTextLen);
			char* pOutputBuf = m_pSnowball->stemmer(pInputBuf,nTextLen);			
			if(m_pStemmedReader == NULL)
				m_pStemmedReader = new CStringReader();
			m_pStemmedReader->open(pOutputBuf,nTextLen);
			return m_pStemmedReader;*/
			return pInputTokens;
		}
	}
}

