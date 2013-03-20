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

/**
* Implementation of Geometric Partitioning Merge strategy proposed by Nicholas Lester in 
* "Fast on-line index construction by geometric partitioning", N. Lester, A. Moffat, and J. Zobel, Proceedings of the ACM CIKM Conference on Information and Knowledge Management, A. Chowdhury, N. Fuhr, M. Ronthaler, H.-J. Schek, and W. Teiken (eds), Bremen, Germany, November 2005. pp. 776-783.
*/

#ifndef _GPINDEXMERGER_H
#define _GPINDEXMERGER_H

#include "IndexMerger.h"
#include <map>
using namespace std;

namespace firtex
{
	namespace index
	{
		class CGPIndexMerger :	public CIndexMerger
		{
		public:
			class CPartition
			{
			public:
				CPartition(int32_t p,int32_t nPSize,int32_t nMaxSize)
					:m_nPartition(p)
					,m_nMergeTimes(0)
					,m_nPartitionSize(nPSize)
				{				
					tstring s = _T("_mid_");
					s = firtex::string_utils::append(s,p);
					s += _T("_");
					s = firtex::string_utils::append(s,m_nMergeTimes);
					m_pMergeBarrel = new CMergeBarrel(s.c_str(),nMaxSize);
				}
				~CPartition()
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
					s = firtex::string_utils::append(s,m_nPartition);
					s += _T("_");
					s = firtex::string_utils::append(s,m_nMergeTimes);
					m_pMergeBarrel->setIdentifier(s);
				}
			protected:
				int32_t			m_nPartition;		///partition of this sub-index
				CMergeBarrel*	m_pMergeBarrel;		///index merge barrel
				int32_t			m_nMergeTimes;		///merge times
				int32_t			m_nPartitionSize;	///size of partition

				friend class CGPIndexMerger;
			}; 
		public:
			CGPIndexMerger();
			CGPIndexMerger(CDirectory* pSrcDirectory);
			CGPIndexMerger(CDirectory* pSrcDirectory,char* buffer,size_t bufsize);
			virtual ~CGPIndexMerger(void);				
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
			* determine the partition number
			* @param nPartSize size of partition
			*/
			int32_t getPartition(int32_t nPartSize);

			/**
			* trigger a merge
			* @param pPartition partition need to be merged
			* @param p the partition number of the partition
			*/
			void triggerMerge(CPartition* pPartition,int32_t p);
		protected:
			const static int32_t MAX_TRIGGERS = 5;			
			int32_t		m_nR;			///parameter r, used it to define the capacity of the partitions
			int32_t		m_nP;			///parameter p, number of partitions

			int32_t		m_nCurPartitionSize;

			map<int32_t,CPartition*>	m_partitionMap;
		};
	}
}
#endif
