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
#ifndef _HLINDEXMERGER_H
#define _HLINDEXMERGER_H

#include "IndexMerger.h"

namespace firtex
{
	namespace index
	{
		class CDBTIndexMerger :	public CIndexMerger
		{
		public:
			class CDBTLayer
			{
			public:
				CDBTLayer(int32_t l,int32_t nLevelSize,int32_t nMaxSize)
					:m_nLevel(l)					
					,m_nMergeTimes(0)
					,m_nLevelSize(nLevelSize)
				{				
					tstring s = _T("_mid_");
					s = firtex::string_utils::append(s,l);
					s += _T("_");
					s = firtex::string_utils::append(s,m_nMergeTimes);
					m_pMergeBarrel = new CMergeBarrel(s.c_str(),nMaxSize);
				}
				~CDBTLayer()
				{
					delete m_pMergeBarrel;
					m_pMergeBarrel = NULL;
				}
			public:
				void add(CMergeBarrelEntry* pEntry)
				{
					m_pMergeBarrel->put(pEntry);
				}
				void	increaseMergeTimes()
				{
					m_nMergeTimes++;
					tstring s = _T("_mid_");
					s = firtex::string_utils::append(s,m_nLevel);
					s += _T("_");
					s = firtex::string_utils::append(s,m_nMergeTimes);
					m_pMergeBarrel->setIdentifier(s);
				}
			protected:
				int32_t			m_nLevel;			///level of this node				
				CMergeBarrel*	m_pMergeBarrel;		///index merge barrel
				int32_t			m_nMergeTimes;		///merge times
				int32_t			m_nLevelSize;		///size of level
				friend class CDBTIndexMerger;
			}; 
			
		public:
			CDBTIndexMerger(CDirectory* pSrcDirectory);
			CDBTIndexMerger(CDirectory* pSrcDirectory,char* buffer,size_t bufsize);
			virtual ~CDBTIndexMerger(void);
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

			/**
			 * set parameter of merger
			 * @param pszParam parameter string, format:name1=value1;param2=value2
			 */
			void	setParam(const tchar* pszParam);

		protected:
			/** 
			 * determine the level of index barrel.
			 * @param nLevelSize size of level
			 * @param m M parameter
			 */
			int32_t getLevel(int64_t nLevelSize,int32_t m);

			/**
			 * trigger a merge
			 * @param pLevel the level need to be merged
			 * @param nLevel the level number
			 */
			void triggerMerge(CDBTLayer* pLevel,int32_t nLevel);
		protected:					
			const static int32_t MAX_TRIGGERS = 5;

			int32_t			m_nM;				///
			int32_t			m_nS;				///scale factor 
			int32_t			m_nC;				///collision factor, when there are m_nC barrels in a same level, a merge will be trigged.
			int32_t			m_nCurLevelSize;	///size of level

			map<int32_t,CDBTLayer*>	m_nodesMap;				
		};
	}
}

#endif
