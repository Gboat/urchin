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
#include "index/LogarithmicMerger.h"

namespace firtex
{
	namespace index
	{
		CLogarithmicMerger::CLogarithmicMerger(void)
			:m_nCurGeneration(0)
		{
		}

		CLogarithmicMerger::CLogarithmicMerger(CDirectory* pSrcDirectory)
			:CIndexMerger(pSrcDirectory)
			,m_nCurGeneration(0)
		{
		}
		CLogarithmicMerger::CLogarithmicMerger(CDirectory* pSrcDirectory,char* buffer,size_t bufsize)
			:CIndexMerger(pSrcDirectory,buffer,bufsize)			
			,m_nCurGeneration(0)
		{
		}
		CLogarithmicMerger::~CLogarithmicMerger(void)
		{
			endMerge();
		}
		void CLogarithmicMerger::addBarrel(CMergeBarrelEntry* pEntry)
		{
			CGeneration* pGen = NULL;
			map<int32_t,CGeneration*>::iterator iter = m_generationMap.find(m_nCurGeneration);
			if(iter != m_generationMap.end())
			{
				pGen = iter->second;
				pGen->add(pEntry);
				if((int32_t)pGen->m_pMergeBarrel->size() >= 2) ///collision,trigger a merge event
				{					
					triggerMerge(pGen,m_nCurGeneration); 
				}
			}
			else
			{
				pGen = new CGeneration(m_nCurGeneration,2*MAX_TRIGGERS);
				pGen->add(pEntry);
				m_generationMap.insert(make_pair(m_nCurGeneration,pGen));
			}
		}
		void CLogarithmicMerger::endMerge()
		{
			map<int32_t,CGeneration*>::iterator iter = m_generationMap.begin();
			while (iter != m_generationMap.end())
			{
				delete iter->second;
				iter++;
			}
			m_generationMap.clear();
		}

		void CLogarithmicMerger::triggerMerge(CGeneration* pGen,int32_t nGen)
		{			
			CGeneration* pGen1 = pGen;			
			int32_t nCurGen = nGen;
			for (int32_t i = nGen + 1;i <= (nGen + MAX_TRIGGERS);i++)
			{			
				map<int32_t,CGeneration*>::iterator iter2 = m_generationMap.find(i);
				if(iter2 != m_generationMap.end())
				{
					CGeneration* pGen2 = iter2->second;
					if((int32_t)pGen2->m_pMergeBarrel->size() + 1 >= 2)	///will trigger a merge event in upper generation
					{
						///copy elements to upper generation
						while (pGen1->m_pMergeBarrel->size() >0)
						{
							pGen2->m_pMergeBarrel->put(pGen1->m_pMergeBarrel->pop());
						}
						pGen1 = pGen2;
						nCurGen = i;
					}
					else break;
				}
				else break;	
			}
			if(isGC(NULL))
			{
				map<int32_t,CGeneration*>::iterator iter = m_generationMap.begin();
				while(iter != m_generationMap.end())
				{
					CGeneration* pGen2 = iter->second;
					if((pGen1 != pGen2) && (pGen2->m_pMergeBarrel->size() > 0) && isGC(pGen2->m_pMergeBarrel))
					{
						///copy elements
						while ( (pGen2->m_pMergeBarrel->size() > 0) && (pGen1->m_pMergeBarrel->size() < (size_t)2*MAX_TRIGGERS))
						{
							pGen1->m_pMergeBarrel->put(pGen2->m_pMergeBarrel->pop());
						}
						if(iter->first > nCurGen)
							nCurGen = iter->first;
					}
					iter++;
				}
			}


			if(pGen1->m_pMergeBarrel->size() > 0)
			{
				m_nCurGeneration = nCurGen + 1;
				mergeBarrel(pGen1->m_pMergeBarrel);	
				pGen1->increaseMergeTimes();
				m_nCurGeneration = 0;
			}
		}		
	}
}
