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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2005/12/23
//
#ifndef _TERMINFO_H
#define _TERMINFO_H

#include "../utility/StdHeader.h"

namespace firtex
{
	namespace index
	{
		class CTermInfo
		{
		public:

			CTermInfo(void)
			{
				m_docFreq = 0;
				m_postingPointer = 0;
			}
			CTermInfo(const CTermInfo& ti)
			{
				m_docFreq = ti.m_docFreq;
				m_postingPointer = ti.m_postingPointer;
			}
			CTermInfo(count_t df,fileoffset_t dp)
			{
				m_docFreq = df;
				m_postingPointer = dp;
			}

			~CTermInfo(void)
			{
			}
		public:
			count_t			docFreq()const{return m_docFreq;}
			fileoffset_t	docPointer(){return m_postingPointer;}

			void			set(count_t df,fileoffset_t dp)
			{
				m_docFreq = df;
				m_postingPointer = dp;
			}
		protected:
			count_t			m_docFreq;
			fileoffset_t	m_postingPointer;
		};

	}
}



#endif
