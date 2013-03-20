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
// Created	: 2007/3/8
//

#ifndef _LOWERCASEANTEFILTER_H
#define _LOWERCASEANTEFILTER_H

#include "TokenFilter.h"

namespace firtex
{
	namespace analyzer
	{
		class CLowercaseAnteFilter : public CTokenFilter
		{
		public:
			class Creator : public CTokenFilter::Creator
			{
			protected:
				Creator()					
				{
					m_identifier += _T("ante.lowercase");
				}
			public:
				~Creator(){}
			public:
				/**
				* create a collection object
				* @param param parameter for filter
				* @return create a analyzer
				*/
				virtual CTokenFilter* create(const tchar* /*param*/){return new CLowercaseAnteFilter();}
			public:
				static Creator* getInstance()
				{
					static CLowercaseAnteFilter::Creator INSTANCE;
					return &INSTANCE;
				}
			};
		public:
			CLowercaseAnteFilter(void);
			CLowercaseAnteFilter(CTokenFilter* pPreFilter);			
			virtual ~CLowercaseAnteFilter(void);
		public:
			/**
			 * get identifier of Filter
			 */
			const tchar* getIdentifier(){return CLowercaseAnteFilter::Creator::getInstance()->getIdentifier();};

			/**
			 * filter interface, must implement by sub class
			 * @param pInputTokens input tokens
			 * @return output tokens, may be different from pInputTokens
			 */			
			CTokens*	filterInternal(CTokens* pInputTokens);
		};
	}
}


#endif
