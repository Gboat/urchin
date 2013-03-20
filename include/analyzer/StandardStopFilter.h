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
// Created	: 2005/11/24
//
#ifndef _STANDARDSTOPFILTER_H
#define _STANDARDSTOPFILTER_H
#include "../utility/StdHeader.h"

#include "TokenFilter.h"

using namespace std;

#define STOPWORD_ID	-1

namespace firtex
{
	namespace analyzer
	{
		class CStandardStopFilter :	public CTokenFilter
		{
		public:
			class Creator : public CTokenFilter::Creator
			{
			protected:
				Creator()					
				{
					m_identifier += _T("post.stopword");
				}
			public:
				~Creator(){}
			public:
				/**
				* create a collection object
				* @param param parameter for filter
				* @return create a analyzer
				*/
				virtual CTokenFilter* create(const tchar* param){return new CStandardStopFilter(param);}
			public:
				static Creator* getInstance()
				{
					static CStandardStopFilter::Creator INSTANCE;
					return &INSTANCE;
				}
			};
		public:
			CStandardStopFilter(const tchar* stopfile);
			CStandardStopFilter();
			virtual ~CStandardStopFilter(void);
	
		public:
			void	load(const tchar* filename);	
			void	add(const termid_t tid);		
			void	clear();			
			
			bool	stopWord(const termid_t tid) const;
		
		protected:
			/**
			 * get identifier of Filter
			 */
			const tchar* getIdentifier(){return CStandardStopFilter::Creator::getInstance()->getIdentifier();};

			/**
			 * filter interface, must implement by sub class
			 * @param pInputTokens input tokens
			 * @return output tokens, may be different from pInputTokens
			 */			
			CTokens*	filterInternal(CTokens* pInputTokens);
		protected:
			tstring		m_sStopwordFile;

			bool*		m_pStopList; ///停用词数组
			termid_t	m_minTerm;	 ///最小停用词
			termid_t	m_maxTerm;	 ///最大停用词
		};
		//////////////////////////////////////////////////////////////////////////
		//
		inline bool CStandardStopFilter::stopWord(const termid_t tid) const
		{

			if( (tid >= m_minTerm) && (tid <= m_maxTerm) && (m_pStopList[tid-m_minTerm]) )
				return true;
			else return false;	
		}
	}
}
#endif
