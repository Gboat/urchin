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
// Created	: 2006/12/21
//

#include "index/OptimizeMerger.h"

namespace firtex
{
	namespace index
	{
		COptimizeMerger::COptimizeMerger(CDirectory* pSrcDirectory)
			:CIndexMerger(pSrcDirectory)
			,m_nC(5)
			,m_nScale(0)
			,m_nCurLevelSize(1)
		{
		}
		COptimizeMerger::COptimizeMerger(CDirectory* pSrcDirectory,char* buffer,size_t bufsize)
			:CIndexMerger(pSrcDirectory,buffer,bufsize)
			,m_nC(5)
			,m_nScale(0)
			,m_nCurLevelSize(1)
		{
		}

		COptimizeMerger::~COptimizeMerger(void)
		{
			map<int32_t,COptimizeMergeTreeLevel*>::iterator iter = m_levelsMap.begin();
			while (iter != m_levelsMap.end())
			{
				delete iter->second;
				iter++;
			}
			m_levelsMap.clear();
		}
		void COptimizeMerger::addBarrel(CMergeBarrelEntry* pEntry)
		{
			if(m_nScale > 0)
			{
				m_nCurLevelSize = (int32_t)( (double)pEntry->numDocs()/(double)m_nScale + 0.5 );
			}
			int32_t nLevel = getLevel(m_nCurLevelSize);

			COptimizeMergeTreeLevel* pLevel = NULL;
			map<int32_t,COptimizeMergeTreeLevel*>::iterator iter = m_levelsMap.find(nLevel);
			if(iter != m_levelsMap.end())
			{
				pLevel = iter->second;
				pLevel->m_nLevelSize += m_nCurLevelSize;
				pLevel->add(pEntry);
				if((int32_t)pLevel->m_pMergeBarrel->size() >= m_nC) ///collision,trigger a merge event
				{					
					m_nCurLevelSize = pLevel->m_nLevelSize;
					pLevel->m_nLevelSize = 0;
					mergeBarrel(pLevel->m_pMergeBarrel);
					pLevel->increaseMergeTimes();
					m_nCurLevelSize = 1;
				}
			}
			else
			{
				pLevel = new COptimizeMergeTreeLevel(nLevel,m_nCurLevelSize,m_nC);
				pLevel->add(pEntry);
				m_levelsMap.insert(make_pair(nLevel,pLevel));
			}
		}
		void COptimizeMerger::endMerge()
		{	
			COptimizeMergeTreeLevel* pCurLevel = NULL;
			COptimizeMergeTreeLevel* pLevel = NULL;
			if(m_levelsMap.size() == 1)
			{
				pLevel =	m_levelsMap.begin()->second;
				if(pLevel->m_pMergeBarrel->size() > 1)
				{
					m_nCurLevelSize = pLevel->m_nLevelSize;
					pLevel->m_nLevelSize = 0;
					mergeBarrel(pLevel->m_pMergeBarrel);
					pLevel->increaseMergeTimes();	
					m_nCurLevelSize = 1;	
				}
			}
			else 
			{
				bool bMerged;
				do
				{
					bMerged = false;
					pCurLevel = NULL;					
					map<int32_t,COptimizeMergeTreeLevel*>::iterator iter = m_levelsMap.begin();
					while (iter != m_levelsMap.end())
					{
						pLevel = iter->second;
						if(pCurLevel)
						{
							///copy elements to upper level
							while (pCurLevel->m_pMergeBarrel->size() > 0)
							{								
								pLevel->m_pMergeBarrel->put(pCurLevel->m_pMergeBarrel->pop());
								pLevel->m_nLevelSize = pCurLevel->m_nLevelSize;
								pCurLevel->m_nLevelSize = 0;
								if((int32_t)pLevel->m_pMergeBarrel->size() >= m_nC)
								{
									m_nCurLevelSize = pLevel->m_nLevelSize;
									pLevel->m_nLevelSize = 0;
									mergeBarrel(pLevel->m_pMergeBarrel);
									pLevel->increaseMergeTimes();
									m_nCurLevelSize = 1;
									bMerged = true;
								}
							}
						}
						iter++;
						
						pCurLevel = pLevel;					
					}
				}while (bMerged);			

				if(pLevel->m_pMergeBarrel->size() > 0)
				{
					m_nCurLevelSize = pLevel->m_nLevelSize;
					pLevel->m_nLevelSize = 0;
					mergeBarrel(pLevel->m_pMergeBarrel);
					pLevel->increaseMergeTimes();				
					m_nCurLevelSize = 1;
				}
			}		

			map<int32_t,COptimizeMergeTreeLevel*>::iterator iter = m_levelsMap.begin();
			while (iter != m_levelsMap.end())
			{
				delete iter->second;
				iter++;
			}
			m_levelsMap.clear();
		}

		int32_t COptimizeMerger::getLevel(int64_t nLevelSize)
		{
			if(nLevelSize <= 0)
				return 0;
			int32_t level = 0;
			///determine its level
			while( ( level < 31) && (nLevelSize >= (int32_t)pow((double)m_nC,(double)(level + 1)) ) )
			{
				level++;
			}
			return level;
		}
	}
}
