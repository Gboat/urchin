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
// Created	: 2006/7/15
//
#ifndef _HITQUEUE_H
#define _HITQUEUE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/PriorityQueue.h"
#include "../index/IndexReader.h"
#include "Sort.h"
#include "ScoreDoc.h"

using namespace firtex::utility;
using namespace firtex::index;


namespace firtex
{
	namespace search
	{
		class CHitQueue : public CPriorityQueue<CScoreDoc*>
		{
		public:
			CHitQueue(size_t maxSize);
			virtual~CHitQueue();
		protected:
			bool lessThan(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
			{
				if (pDoc1->m_fScore == pDoc2->m_fScore)
					return pDoc1->m_docID > pDoc2->m_docID; 
				else
					return pDoc1->m_fScore < pDoc2->m_fScore;
			}
		};

		class CFieldDocSortedHitQueue :  public CHitQueue
		{
		public:
			CFieldDocSortedHitQueue(CIndexReader* pReader,CSort* pSort,int32_t size);
			~CFieldDocSortedHitQueue(void);
		public:
			bool lessThan(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
			{
				return !m_pSort->lessThan(pDoc1,pDoc2);
			}
		private:
			CSort*	m_pSort;
		};
	}
}


#endif
