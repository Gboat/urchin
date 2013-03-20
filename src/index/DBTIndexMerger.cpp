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
#include "index/DBTIndexMerger.h"
#include "utility/ParamParser.h"

namespace firtex
{
	namespace index
	{
		CDBTIndexMerger::CDBTIndexMerger(CDirectory* pSrcDirectory)
			:CIndexMerger(pSrcDirectory)
			,m_nM(3)
			,m_nS(1)
			,m_nC(3)
			,m_nCurLevelSize(1)
		{
		}
		CDBTIndexMerger::CDBTIndexMerger(CDirectory* pSrcDirectory,char* buffer,size_t bufsize)
			:CIndexMerger(pSrcDirectory,buffer,bufsize)
			,m_nM(3)
			,m_nS(1)
			,m_nC(3)
			,m_nCurLevelSize(1)
		{
		}
		CDBTIndexMerger::~CDBTIndexMerger()
		{
			endMerge();
		}
		void CDBTIndexMerger::addBarrel(CMergeBarrelEntry* pEntry)
		{
			if(m_nS > 0)
			{
				m_nCurLevelSize = (int32_t)( (double)pEntry->numDocs()/(double)m_nS + 0.5 );
			}
			int32_t nLevel = getLevel(m_nCurLevelSize,m_nM);
			CDBTLayer* pLevel = NULL;
			map<int32_t,CDBTLayer*>::iterator iter = m_nodesMap.find(nLevel);
			if(iter != m_nodesMap.end())
			{
				pLevel = iter->second;
				pLevel->m_nLevelSize += m_nCurLevelSize;
				pLevel->add(pEntry);
				if((int32_t)pLevel->m_pMergeBarrel->size() >= m_nC) ///collision,trigger a merge event
				{					
					triggerMerge(pLevel,nLevel); 
				}
			}
			else
			{
				pLevel = new CDBTLayer(nLevel,m_nCurLevelSize,m_nC*MAX_TRIGGERS);
				pLevel->add(pEntry);
				m_nodesMap.insert(make_pair(nLevel,pLevel));
			}
		}
		
		void CDBTIndexMerger::endMerge()
		{
			map<int32_t,CDBTLayer*>::iterator iter = m_nodesMap.begin();
			while (iter != m_nodesMap.end())
			{
				delete iter->second;
				iter++;
			}
			m_nodesMap.clear();
		}
		void CDBTIndexMerger::setParam(const tchar* pszParam)
		{
			firtex::utility::CParamParser parser;
			if(parser.parse(pszParam,_T(";")))
			{
				parser.getParam(_T("m"),m_nM);
				parser.getParam(_T("s"),m_nS);
				parser.getParam(_T("c"),m_nC);
			}
		}
		void CDBTIndexMerger::triggerMerge(CDBTLayer* pLevel,int32_t nLevel)
		{
			CDBTLayer* pLevel1 = pLevel;
			int32_t nL = this->getLevel(pLevel->m_nLevelSize,m_nM);
			int32_t nTriggers = 0;

			int32_t i ;
			for ( i = nLevel + 1;(i <= nL) && (nTriggers <= MAX_TRIGGERS);i++)
			{			
				map<int32_t,CDBTLayer*>::iterator iter2 = m_nodesMap.find(i);
				if(iter2 != m_nodesMap.end())
				{
					CDBTLayer* pLevel2 = iter2->second;
					if((int32_t)pLevel2->m_pMergeBarrel->size() + 1 >= m_nC)	///will trigger a merge event in upper level
					{
						///copy elements to upper level
						while (pLevel1->m_pMergeBarrel->size() >0)
						{
							pLevel2->m_pMergeBarrel->put(pLevel1->m_pMergeBarrel->pop());
						}
						pLevel2->m_nLevelSize += pLevel1->m_nLevelSize;
						pLevel1->m_nLevelSize = 0;
						nL = this->getLevel(pLevel2->m_nLevelSize,m_nM);
						pLevel1 = pLevel2;
						nTriggers++;
					}
					else break;
				}
				else break;	
			}

			if(isGC(NULL))
			{
				map<int32_t,CDBTLayer*>::iterator iter = m_nodesMap.begin();
				while(iter != m_nodesMap.end())
				{
					CDBTLayer* pLevel2 = iter->second;
					if((pLevel1 != pLevel2) && (pLevel2->m_nLevelSize > 0)
						&& isGC(pLevel2->m_pMergeBarrel)  
						&& (pLevel1->m_pMergeBarrel->size() <= (size_t)m_nC*MAX_TRIGGERS - pLevel2->m_pMergeBarrel->size()))
					{
						///copy elements
						while (pLevel2->m_pMergeBarrel->size() > 0)
						{
							pLevel1->m_pMergeBarrel->put(pLevel2->m_pMergeBarrel->pop());
						}
						pLevel1->m_nLevelSize += pLevel2->m_nLevelSize;
						pLevel2->m_nLevelSize = 0;						
					}
					iter++;
				}
			}

			if(pLevel1->m_pMergeBarrel->size() > 0)
			{				
				m_nCurLevelSize = pLevel1->m_nLevelSize;
				pLevel1->m_nLevelSize = 0;
				mergeBarrel(pLevel1->m_pMergeBarrel);				
				pLevel1->increaseMergeTimes();
				m_nCurLevelSize = 1;
			}
		}
		int32_t CDBTIndexMerger::getLevel(int64_t nLevelSize,int32_t m)
		{
			if(nLevelSize <= 0)
				return 0;
			int32_t level = 0;
			///determine its level
			while( ( level < 31) && (nLevelSize >= (int32_t)pow((double)m,(double)(level + 1)) ) )
			{
				level++;
			}
			return level;
		}
	}
}
