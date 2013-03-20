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
#ifndef _BOOLEANCLAUSE_H
#define _BOOLEANCLAUSE_H


namespace firtex
{
	namespace search
	{
		class CQuery;
		class CBooleanClause
		{
		public:
			CBooleanClause(void):m_pQuery(0),m_bRequired(true),m_bProhibited(false)
			{
			}
			CBooleanClause(CQuery* pQuery,bool req,bool pro):m_bRequired(req),m_bProhibited(pro)
			{
				m_pQuery = pQuery->clone();				
			}
			CBooleanClause(const CBooleanClause& clone)
			{
				if(clone.m_pQuery)
					m_pQuery = clone.m_pQuery->clone();
				else m_pQuery = NULL;
				m_bRequired = clone.m_bRequired;
				m_bProhibited = clone.m_bProhibited;
			}
			~CBooleanClause(void)
			{
				if(m_pQuery)
				{
					delete m_pQuery;					
					m_pQuery = NULL;
				}
			}
		public:
			CBooleanClause* clone(){return new CBooleanClause(*this);}

			CQuery* getQuery(){return m_pQuery;}
			bool	isRequired(){return m_bRequired;}
			bool	isProhibited(){return m_bProhibited;}
			void	setRequired(bool rq=true){m_bRequired = rq;}
			void	setProhibited(bool p=true){m_bProhibited = p;}
		protected:
			CQuery*		m_pQuery;			
			bool		m_bRequired;		//必须出现
			bool		m_bProhibited;		//不能出现
		};
	}
}

#endif
