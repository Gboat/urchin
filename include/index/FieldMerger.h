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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/2/23
//

#ifndef _FIELDMERGER_H
#define _FIELDMERGER_H

#include "FieldsInfo.h"
#include "FieldIndexer.h"
#include "BarrelsInfo.h"
#include "TermIterator.h"
#include "TermReader.h"
#include "../store/Directory.h"
#include "../utility/PriorityQueue.h"
#include "PostingMerger.h"
#include <string>
#include <vector>

using namespace std;
using namespace firtex::store;

#define NUM_CACHEDTERMINFO 100

namespace firtex
{
	namespace index
	{
		class CMergeFieldEntry
		{
		public:
			CMergeFieldEntry()
				:m_pFieldInfo(NULL)
				,m_pBarrelInfo(NULL)
				,m_pDocFilter(NULL)
			{				
			}
			CMergeFieldEntry(CBarrelInfo* pBarrelInfo,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
				:m_pBarrelInfo(pBarrelInfo)
				,m_pDocFilter(pDocFilter)
				,m_pFieldInfo(new CFieldInfo(*pFieldInfo))
			{				
				
			}
			~CMergeFieldEntry()
			{
				if(m_pFieldInfo)
				{
					delete m_pFieldInfo;
					m_pFieldInfo = NULL;
				}
				m_pBarrelInfo = NULL;
				m_pDocFilter = NULL;
			}		
		protected:
			CBarrelInfo*	m_pBarrelInfo;
			CFieldInfo*		m_pFieldInfo;
			CBitVector*		m_pDocFilter;

			friend class CFieldMerger;
		};

		class CFieldMergeInfo
		{
		public:
			CFieldMergeInfo(int32_t nOrder,CBarrelInfo* pBarrelInfo,CTermReader* pTermReader,bool bDeleteTermReader)
				:m_pBarrelInfo(pBarrelInfo)
				,m_pTermReader(pTermReader)
				,m_pCurTerm(NULL)
				,m_bDeleteTermReader(bDeleteTermReader)
				,m_nOrder(nOrder)
			{
				m_pIterator = m_pTermReader->termIterator(NULL);
			}
			~CFieldMergeInfo()
			{				
				if(m_bDeleteTermReader)
					delete m_pTermReader;
				m_pTermReader = NULL;
				if(m_pIterator)
					delete m_pIterator;
				m_pIterator = NULL;
			}
		public:
			bool	next()
			{
				if(m_pIterator && m_pIterator->next())
				{
					m_pCurTerm = (CTerm*)m_pIterator->term();
					return true;
				}
				return false;
			}
		protected:
			int32_t			m_nOrder;			///order of barrel
			CBarrelInfo*	m_pBarrelInfo;		///reference to barrel info
			CTerm*			m_pCurTerm;			///current term			
			CTermReader*	m_pTermReader;
			CTermIterator*	m_pIterator;		///term iterator
			bool			m_bDeleteTermReader;///delete m_pTermReader or not

			friend class CFieldMergeQueue;
			friend class CFieldMerger;
		};
        
		class CFieldMergeQueue : public CPriorityQueue<CFieldMergeInfo*>
		{
		public:
			CFieldMergeQueue(size_t maxSize)
			{
				initialize(maxSize,true);
			}
			~CFieldMergeQueue(){}
		protected:
			/**
			 * Determines the ordering of objects in this priority queue. 
			 * Subclasses must define this one method. 
			 */
			bool lessThan(CFieldMergeInfo* a, CFieldMergeInfo* b)
			{
				int32_t ret = a->m_pCurTerm->compare(b->m_pCurTerm);
				if(ret == 0)
					return (a->m_pBarrelInfo->getMinDocID() < b->m_pBarrelInfo->getMinDocID());
				return (ret < 0);
			}
		};

		class CMergeTermInfo
		{
		public:
			CMergeTermInfo()
				:m_pTerm(NULL)
				,m_pTermInfo(NULL)
			{}
			CMergeTermInfo(CTerm* pTerm,CTermInfo* pTermInfo)
				:m_pTerm(pTerm)
				,m_pTermInfo(pTermInfo)
			{}
			~CMergeTermInfo()
			{
				if(m_pTerm)
					delete m_pTerm;
				if(m_pTermInfo)
					delete m_pTermInfo;
			}
		public:
			CTerm*		getTerm(){return m_pTerm;}
			CTermInfo*	getTermInfo(){return m_pTermInfo;}
		protected:
			CTerm*		m_pTerm;
			CTermInfo*	m_pTermInfo;

			friend class CFieldMerger;
		};


		class CPostingMerger;
		class CFieldFactory;
		class CFieldMerger
		{
		public:
			CFieldMerger();
			CFieldMerger(CFieldFactory* pFactory);
			CFieldMerger(CDirectory* pDirectory,CFieldFactory* pFactory);
			virtual ~CFieldMerger(void);
		public:
			/** 
			 * add a field to merger
			 * @param pBarrelInfo about the barrel of the field
			 * @param pFieldInfo about the field
			 * @param pDocFilter filter of deleted docs
			 */
			void		addField(CBarrelInfo* pBarrelInfo,CFieldInfo* pFieldInfo,CBitVector* pDocFilter);

			/** number of fields */
			int32_t		numFields(){return (int32_t)m_fieldEntries.size();}

			/**
			 * merge			 
			 * @param pDesc to store merge result
			 * @return offset of field vocabulary descriptor
			 */
			fileoffset_t	merge(COutputDescriptor* pOutputDescriptor);

			/** 
			 * set buffer for merging
			 * @param buf buffer
			 * @param bufSize size of buffer
			 */
			void		setBuffer(char* buf,size_t bufSize);

			/**
			 * set directory of index database
			 * @param pDirectory directory
			 */
			void		setDirectory(CDirectory* pDirectory){m_pDirectory = pDirectory;}

			/**
			 * get directory of index database
			 * @return internal directory object
			 */
			CDirectory*	getDirectory(){return m_pDirectory;}

			/**
			 * get total number of merged terms
			 * @return number of merged terms
			 */
			int64_t		numMergedTerms(){return m_nMergedTerms;}
		protected:
			/** initialize merge queue */
			bool		initQueue();
		
			/**
			 * merge terms
			 * @param ppMergeInfos merge information array
			 * @param numInfos size of info array
			 */
			inline fileoffset_t	mergeTerms(CFieldMergeInfo** ppMergeInfos,int32_t numInfos);
		protected:
			/**
			 * flush merged term info, Subclasses must define this one method. 			 
			 * @param pOutputDescriptor output descriptor
			 * @param ppTermInfo merged term infos
			 * @param numTermInfos number of term infos.
			 */
			virtual void flushTermInfo(COutputDescriptor* pOutputDescriptor,CMergeTermInfo** ppTermInfo,int32_t numTermInfos) = 0;

			/**
			 * merge is over, Subclasses must define this one method. 			 
			 * @param pOutputDescriptor output descriptor
			 * @return offset of vocabulary descriptor.
			 */
			virtual fileoffset_t endMerge(COutputDescriptor* pOutputDescriptor) = 0;
		protected:			
			CDirectory*					m_pDirectory;
			CFieldMergeQueue*			m_pMergeQueue;
			CFieldFactory*				m_pFactory;
			CPostingMerger*				m_pPostingMerger;		

			int64_t						m_nMergedTerms;

			char*						m_buffer;
			size_t						m_bufsize;
			vector<CMergeFieldEntry*>	m_fieldEntries;

			CMergeFieldEntry**			m_ppFieldInfos;
			size_t						m_nNumInfos;

			CMergeTermInfo*				m_cachedTermInfos[NUM_CACHEDTERMINFO];
			int32_t						m_nNumTermCached;
		};
		//////////////////////////////////////////////////////////////////////////
		//inline
		inline fileoffset_t CFieldMerger::mergeTerms(CFieldMergeInfo** ppMergeInfos,int32_t numInfos)
		{
			int32_t nOrder = 0;
			CPosting* pPosting;
			for(int32_t i = 0;i< numInfos;i++)
			{
				pPosting = ppMergeInfos[i]->m_pIterator->termPosting();
				while (ppMergeInfos[i]->m_nOrder > nOrder)
				{
					m_pPostingMerger->addFreedID(m_ppFieldInfos[nOrder]->m_pDocFilter);
					nOrder++;
				}
				if(ppMergeInfos[i]->m_pBarrelInfo->getWriter())///in-memory posting
					m_pPostingMerger->mergeWith(ppMergeInfos[i]->m_pBarrelInfo->getMinDocID(),(CInMemoryPosting*)pPosting);
				else m_pPostingMerger->mergeWith(ppMergeInfos[i]->m_pBarrelInfo->getMinDocID(),(COnDiskPosting*)pPosting);
				nOrder++;
			}
			///end merge
			return m_pPostingMerger->endMerge();
		}
		
	}
}

#endif
