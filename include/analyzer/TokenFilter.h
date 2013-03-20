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
// Created	: 2005/11/24
//
#ifndef __TOKENFILTER_H
#define __TOKENFILTER_H

#include "Tokens.h"

namespace firtex
{
	namespace analyzer
	{
		class CTokenFilter
		{				
		public:
			class Creator
			{
			protected:
				Creator()
					: m_identifier(_T("filter"))
				{}
			public:
				~Creator(){}
			public:
				/**
				 * create a collection object
				 * @param param parameter for filter
				 * @return create a analyzer
				 */
				virtual CTokenFilter* create(const tchar* param) = 0; 

				/** 
				 * get the identifier of collection
				 */
				virtual const tchar* getIdentifier(){return m_identifier.c_str();}			
			protected:
				tstring m_identifier;
			};
		public:
			CTokenFilter(void)
				: m_pPreFilter(NULL)
			{				
			}
			CTokenFilter(CTokenFilter* pPreFilter)
				: m_pPreFilter(pPreFilter)
			{
			}
			virtual ~CTokenFilter(void)
			{
				if(m_pPreFilter)
				{
					delete m_pPreFilter;
					m_pPreFilter = NULL;
				}
			}
		public:
			/**
			 * attach a post-filter which would be called before this filter
			 * @param pPreFilter ante-filter
			 */
			void			attachFilter(CTokenFilter* pPreFilter);

			/**
			 * detach a post-filter
			 * @return detached filter
			 */
			CTokenFilter*	detachFilter(CTokenFilter* pPreFilter);
		public:
			/**
			 * post-filter
			 * @param pInputTokens input tokens
			 * @return output tokens, may be different from pInputTokens
			 */
			inline CTokens* filter(CTokens* pInputTokens);
		public:
			/**
			 * get identifier of Filter
			 */
			virtual const tchar* getIdentifier() = 0;

		protected:
			/**
			 * post-filter interface, must implement by sub class
			 * @param pInputTokens input tokens
			 * @return output tokens, may be different from pInputTokens
			 */
			virtual CTokens* filterInternal(CTokens* pInputTokens) = 0;
		protected:
			CTokenFilter*	m_pPreFilter;
		};
		//////////////////////////////////////////////////////////////////////////
		//
		inline CTokens* CTokenFilter::filter(CTokens* pInputTokens)
		{
			CTokens* pTokens = pInputTokens;
			if(m_pPreFilter)
				pTokens = m_pPreFilter->filter(pInputTokens);
			return filterInternal(pTokens);
		}
		inline void CTokenFilter::attachFilter(CTokenFilter* pPreFilter)
		{
			if(m_pPreFilter)
			{
				m_pPreFilter->attachFilter(pPreFilter);
			}
			else 
			{
				m_pPreFilter = pPreFilter;
			}
		}

		inline CTokenFilter* CTokenFilter::detachFilter(CTokenFilter* pPreFilter)
		{
			if(m_pPreFilter)
			{
				CTokenFilter* pPreFilter = m_pPreFilter;
				m_pPreFilter = NULL;
				return pPreFilter;
			}
			return NULL;
		}
	}
}


#endif
