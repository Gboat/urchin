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
// Created	: 2006/3/9
//
#ifndef _WEIGHT_H
#define _WEIGHT_H
#include "../utility//StdHeader.h"
#include "Query.h"

namespace firtex
{
	namespace search
	{
		class CWeight
		{
		public:
			CWeight(void) 
				:m_pQuery(NULL)
				,m_bDeleteQuery(false)
			{
			}
			CWeight(CQuery* pQuery,bool bDeleteQuery)
				:m_pQuery(pQuery)
				,m_bDeleteQuery(bDeleteQuery)
			{
			}
			virtual ~CWeight(void)
			{
				if(m_pQuery && m_bDeleteQuery)
					delete m_pQuery;
				m_pQuery = NULL;
			}
		public:						
			virtual weight_t	getValue()=0;

			CQuery*		getQuery(){return m_pQuery;}
		
			void		setQuery(CQuery* pQuery,bool bDeleteQuery){m_pQuery = pQuery;m_bDeleteQuery = bDeleteQuery;}
			void		setDeleteQuery(bool bDeleteQuery = true){m_bDeleteQuery = bDeleteQuery;}
		private:
			CQuery*		m_pQuery;
			bool		m_bDeleteQuery;
		};
	}
}

#endif
