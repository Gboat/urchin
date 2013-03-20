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

/**
 * Implementation of Logarithmic Merge strategy proposed by Stefan Buettcher in 
 * "Stefan B¨¹ttcher and Charles L. A. Clarke. Indexing Time vs. Query Time Trade-offs in Dynamic Information Retrieval Systems. Proceedings of the 14th ACM Conference on Information and Knowledge Management (CIKM 2005). Bremen, Germany, November 2005. "
 */

#ifndef _LOGARITHMICMERGER_H
#define _LOGARITHMICMERGER_H

#include "IndexMerger.h"
#include <map>
using namespace std;

namespace firtex
{
	namespace index
	{
		class CLogarithmicMerger :	public CIndexMerger
		{
		public:
			class CGeneration
			{
			public:
				CGeneration(int32_t g,int32_t nMaxSize)
					:m_nGeneration(g)
					,m_nMergeTimes(0)
				{
					tstring s = _T("_mid_");
					s = firtex::string_utils::append(s,g);
					s += _T("_");
					s = firtex::string_utils::append(s,m_nMergeTimes);
					m_pMergeBarrel = new CMergeBarrel(s.c_str(),nMaxSize);
				}
				~CGeneration()
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
					s = firtex::string_utils::append(s,m_nGeneration);
					s += _T("_");
					s = firtex::string_utils::append(s,m_nMergeTimes);
					m_pMergeBarrel->setIdentifier(s);
				}
			protected:
				int32_t			m_nGeneration;		///generation of this sub-index
				CMergeBarrel*	m_pMergeBarrel;		///index merge barrel
				int32_t			m_nMergeTimes;		///merge times

				friend class CLogarithmicMerger;
			}; 
		public:
			CLogarithmicMerger(void);
			CLogarithmicMerger(CDirectory* pSrcDirectory);
			CLogarithmicMerger(CDirectory* pSrcDirectory,char* buffer,size_t bufsize);
			virtual ~CLogarithmicMerger(void);
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
			/**
			 * trigger a merge
			 * @param pGen the generation need to be merged
			 * @param nGen the generation number
			 */
			void triggerMerge(CGeneration* pGen,int32_t nGen);
		protected:
			const static int32_t MAX_TRIGGERS = 5;
			int32_t			m_nCurGeneration;
			
			map<int32_t,CGeneration*>	m_generationMap;
		};
	}
}
#endif
