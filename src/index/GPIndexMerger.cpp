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
// Created	: 2007/01/7
//
#include "index/GPIndexMerger.h"
#include "utility/ParamParser.h"

namespace firtex
{
	namespace index
	{	
		CGPIndexMerger::CGPIndexMerger()
			:m_nCurPartitionSize(1)
			,m_nR(3)
			,m_nP(0)
		{
		}
		CGPIndexMerger::CGPIndexMerger(CDirectory* pSrcDirectory)
			:CIndexMerger(pSrcDirectory)
			,m_nCurPartitionSize(1)
			,m_nR(3)
			,m_nP(0)
		{
		}
		CGPIndexMerger::CGPIndexMerger(CDirectory* pSrcDirectory,char* buffer,size_t bufsize)
			:CIndexMerger(pSrcDirectory,buffer,bufsize)
			,m_nCurPartitionSize(1)
			,m_nR(3)
			,m_nP(0)
		{
		}

		CGPIndexMerger::~CGPIndexMerger(void)
		{
			endMerge();
		}
		void CGPIndexMerger::addBarrel(CMergeBarrelEntry* pEntry)
		{
			CPartition* pPartition = NULL;
			int32_t nPartition = getPartition(m_nCurPartitionSize);
			map<int32_t,CPartition*>::iterator iter = m_partitionMap.find(nPartition);
			if(iter != m_partitionMap.end())
			{
				pPartition = iter->second;
				pPartition->m_nPartitionSize += m_nCurPartitionSize;///update partition size
				pPartition->add(pEntry);
				if((int32_t)pPartition->m_pMergeBarrel->size() >= 2) ///collision,trigger a merge event
				{					
					triggerMerge(pPartition,nPartition); 
				}
			}
			else
			{
				pPartition = new CPartition(nPartition,m_nCurPartitionSize,2*MAX_TRIGGERS);
				pPartition->add(pEntry);
				m_partitionMap.insert(make_pair(nPartition,pPartition));
			}
		}
		void CGPIndexMerger::endMerge()
		{
			map<int32_t,CPartition*>::iterator iter = m_partitionMap.begin();
			while (iter != m_partitionMap.end())
			{
				delete iter->second;
				iter++;
			}
			m_partitionMap.clear();
		}

		void CGPIndexMerger::setParam(const tchar* pszParam)
		{
			firtex::utility::CParamParser parser;
			if(parser.parse(pszParam,_T(";")))
			{
				parser.getParam(_T("r"),m_nR);
				parser.getParam(_T("p"),m_nP);
			}
		}

		void CGPIndexMerger::triggerMerge(CPartition* pPartition,int32_t p)
		{			
			CPartition* pPartition1 = pPartition;						
			int32_t nP = this->getPartition(pPartition->m_nPartitionSize);
			int32_t nTriggers = 0;
			for (int32_t i = p + 1;(i <= nP) && (nTriggers <= MAX_TRIGGERS);i++)
			{			
				map<int32_t,CPartition*>::iterator iter = m_partitionMap.find(i);
				if(iter != m_partitionMap.end())
				{
					CPartition* pPartition2 = iter->second;
					if((int32_t)pPartition2->m_pMergeBarrel->size() + 1 >= 2)	///will trigger a merge event in upper partition
					{
						///copy elements to upper partition
						while (pPartition1->m_pMergeBarrel->size() >0)
						{
							pPartition2->m_pMergeBarrel->put(pPartition1->m_pMergeBarrel->pop());
						}
						pPartition2->m_nPartitionSize += pPartition1->m_nPartitionSize;
						pPartition1->m_nPartitionSize = 0;
						nP = this->getPartition(pPartition2->m_nPartitionSize);
						pPartition1 = pPartition2;
						nTriggers++;
					}
					else break;
				}
				else break;	
			}

			if(isGC(NULL))
			{
				map<int32_t,CPartition*>::iterator iter = m_partitionMap.begin();
				while(iter != m_partitionMap.end())
				{
					CPartition* pPartition2 = iter->second;
					if((pPartition1 != pPartition2) && (pPartition2->m_nPartitionSize > 0) && isGC(pPartition2->m_pMergeBarrel))
					{
						///copy elements
						while ( (pPartition2->m_pMergeBarrel->size() > 0) && (pPartition1->m_pMergeBarrel->size() < (size_t)2*MAX_TRIGGERS))
						{
							pPartition1->m_pMergeBarrel->put(pPartition2->m_pMergeBarrel->pop());
							pPartition1->m_nPartitionSize++;
							pPartition2->m_nPartitionSize--;
						}												
					}
					iter++;
				}
			}

			if(pPartition1->m_pMergeBarrel->size() > 0)
			{
				m_nCurPartitionSize = pPartition1->m_nPartitionSize;
				pPartition1->m_nPartitionSize = 0;
				mergeBarrel(pPartition1->m_pMergeBarrel);				
				pPartition1->increaseMergeTimes();
				m_nCurPartitionSize = 1;
			}
		}
		int32_t CGPIndexMerger::getPartition(int32_t nPartSize)
		{
			if(nPartSize <= 0)
				return 0;
			int32_t nPartition = 0;
			///determine its level
			while(nPartSize > (m_nR - 1)*((int32_t)pow((double)m_nR,(double)nPartition)) )
			{
				nPartition++;
			}
			return nPartition;
		}
	}
}
