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
// Email	: ruijieguo@software.ict.ac.cn ruijieguo@gmail.com
// Created	: 2007/01/5
//

#ifndef _IMMEDIATEMERGER_H
#define _IMMEDIATEMERGER_H

#include "IndexMerger.h"

namespace firtex
{
	namespace index
	{
		class CImmediateMerger : public CIndexMerger
		{
		public:
			CImmediateMerger(void);
			CImmediateMerger(CDirectory* pSrcDirectory);
			CImmediateMerger(CDirectory* pSrcDirectory,char* buffer,size_t bufsize);
			virtual ~CImmediateMerger(void);
		public:
			/**
			 * add new index barrel to merge,derived classes implement it,and could apply some merge strategies.
			 * @param pEntry the index barrel,derived classes are responsible for deleting
			 */
			void	addBarrel(CMergeBarrelEntry* pEntry);

			/**
			 * the merge is over,derived classes implement it,and could clear some resources for merging.			 
			 */
			void	endMerge();
		protected:
			CMergeBarrel*	m_pMergeBarrel;
			int32_t			m_nMergeTimes;
		};
	}
}


#endif
