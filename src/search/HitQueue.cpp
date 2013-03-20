//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/7/15
//
#include "search/HitQueue.h"
#include "search/SortFieldCache.h"


namespace firtex
{
	namespace search
	{
		//////////////////////////////////////////////////////////////////////////
		//CHitQueue
		CHitQueue::CHitQueue(size_t maxSize)
		{
			initialize(maxSize,true);
		}
		CHitQueue::~CHitQueue()
		{
		}

		//////////////////////////////////////////////////////////////////////////
		//CFieldDocSortedHitQueue
		CFieldDocSortedHitQueue::CFieldDocSortedHitQueue(CIndexReader* pReader,CSort* pSort,int32_t size) 
			:m_pSort(pSort)
			,CHitQueue(size)
		{			
			pSort->getComparators(pReader,CSortFieldCache::DEFAULT);
		}

		CFieldDocSortedHitQueue::~CFieldDocSortedHitQueue(void)
		{
		}
	}
}
