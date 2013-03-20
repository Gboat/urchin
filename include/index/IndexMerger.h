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
// Created	: 2005/12/09
//

#ifndef _INDEXMERGER_H
#define _INDEXMERGER_H

#include "BarrelsInfo.h"
#include "FieldsInfo.h"
#include "../store/IndexInput.h"
#include "../store/IndexOutput.h"
#include "../store/Directory.h"
#include "../utility/PriorityQueue.h"
#include "../utility/StringUtils.h"
#include "FieldMerger.h"

#include <vector>

using namespace firtex::store;
using namespace std;

namespace firtex{namespace utility{class CSpeedProbe; }}

namespace firtex
{
	namespace index
	{
		class CMergeBarrelEntry
		{
		public:
			CMergeBarrelEntry(CDirectory* pDirectory,CBarrelInfo*	pBarrelInfo);			
			~CMergeBarrelEntry();
		public:
			inline count_t	numDocs(){return m_pBarrelInfo->getDocCount();}
			void			load(bool bGC);			
		public:			
			CDirectory*		m_pDirectory;		///index storage
			CBarrelInfo*	m_pBarrelInfo;		///barrel information
			CFieldsInfo*	m_pFieldsInfo;		///fields information of barrel
			CBitVector*		m_pDocFilter;		///for filtering deleted documents
			bool			m_bDeleteFilter;						

			friend class CMergeBarrel;
			friend class CIndexMerger;
		};

		class CMergeBarrel : public CPriorityQueue<CMergeBarrelEntry*>
		{			
		public:
			CMergeBarrel(size_t maxSize)
			{
				initialize(maxSize,true);
			}			
			CMergeBarrel(const tstring& identifier,size_t maxSize)
				: m_sIdentifier(identifier)
			{
				initialize(maxSize,true);
			}
			~CMergeBarrel()
			{
			}
		public:
			/** get/set identifier */
			void			setIdentifier(const tstring& identifier){m_sIdentifier = identifier;}

			const tstring&	getIdentifier(){return m_sIdentifier;}

			void			load(bool bGC)
			{
				size_t nEntryCount = size();
				for(size_t nEntry = 0;nEntry < nEntryCount;nEntry++)
				{
					getAt(nEntry)->load(bGC);
				}	
			}
		protected:
			bool lessThan(CMergeBarrelEntry* o1, CMergeBarrelEntry* o2) 
			{
				return (o1->numDocs()) > (o2->numDocs());
			}
		protected:			
			tstring			m_sIdentifier;		///identifier of merge barrel
		};			

		class CFactoryFinder;
		class CIndexMerger
		{		
		protected:
			typedef int32_t gc_t;
			typedef struct _gc 
			{
				gc_t  type;		///0 for no GC, 1 for GC by relative deleted documents ,2 for GC by number of deleted documents
				union
				{
					float	fThreshold;
					int32_t nThreshold;
				};
			}gc;
		protected:
			CIndexMerger();
			CIndexMerger(CDirectory* pDirectory);
			CIndexMerger(CDirectory* pDirectory,char* buffer,size_t bufsize);
		public:
			virtual ~CIndexMerger();
		public:
			/**
			 * merge sub indexes
			 * @param pBarrelsInfo barrels of index 
			 */
			void			merge(CBarrelsInfo* pBarrelsInfo);

			/** 
			 * add a index barrel to merge
			 * @param pBarrelsInfo the index 
			 * @param pBarrelInfo info of barrel need to merge
			 */
			void			addToMerge(CBarrelsInfo* pBarrelsInfo,CBarrelInfo* pBarrelInfo);
			
			/**
			 * set buffer for merging 
			 * @param buffer outside buffer for merging
			 * @param length size of buffer
			 */
			void			setBuffer(char* buffer,size_t bufsize);

			/**
			 * set directory of index
			 * @param pDirectory directory of index
			 */
			void			setDirectory(CDirectory* pDirectory){m_pDirectory = pDirectory;}

			/**
			 * get directory of index
			 * @return directory of index
			 */
			CDirectory*		getDirectory(){return m_pDirectory;}

			/**
			 * set force GC flag
			 * @param bForceGC force GC or not
			 */
			void			forceGC(bool bForceGC){m_bForceGC = bForceGC;}

			/** 
			 * transfer in-memory barrel to disk
			 * @param pszBarrelName barrel name
			 */
			void			transferToDisk(const tchar* pszBarrelName);

			/**
			 * set threshold parameter
			 * @param pszThreshold parameter
			 */
			void			setThreshold(const tchar* pszThreshold);
		protected:
			/**
			 * to see whether a merge barrel need to be merge with GC or not
			 * @param pBarrel merge barrel
			 * @return true for GC
			 */
			bool			isGC(CMergeBarrel* pBarrel);
		protected:
			/**
			 * add new index barrel to merge,derived classes implement it,and could apply some merge strategies.
			 * @param pEntry the index barrel,derived classes are responsible for deleting
			 */
			virtual void	addBarrel(CMergeBarrelEntry* pEntry) = 0;

			/**
			 * the merge is over,derived classes implement it,and could clear some resources for merging.			 
			 */
			virtual void	endMerge() = 0;

			/**
			 * set parameter of merger
			 * @param pszParam parameter string, format:name1=value1;param2=value2
			 */
			virtual void	setParam(const tchar* pszParam){}			
		protected:
			/**
			 * update barrel name and its base document id after merging.
			 * @param pBarrelsInfo barrels of index 
			 */
			virtual void	pendingUpdate(CBarrelsInfo* pBarrelsInfo);

			/**
			 * merge a merge barrel which contains more than one index barrels
			 * @param pBarrel merge barrel			 
			 */
			void			mergeBarrel(CMergeBarrel* pBarrel);

			/**
			 * merge stored fields
			 * @param pBarrel index merge barrel,contain more than one index barrels
			 */
			void			mergeStoredFields(CMergeBarrel* pBarrel);

			/**
			 * merge stored fields with garbage collection
			 * @param pBarrel index merge barrel,contain more than one index barrels
			 */
			void			mergeStoredFields_GC(CMergeBarrel* pBarrel);

			/**
			 * merge fields' norms
			 * @param pBarrel index merge barrel,contain more than one index barrels
			 */
			void			mergeNorms(CMergeBarrel* pBarrel);

			/**
			 * merge fields' norms with garbage collection
			 * @param pBarrel index merge barrel,contain more than one index barrels
			 */
			void			mergeNorms_GC(CMergeBarrel* pBarrel);
			
			/**
			 * merge bitmaps of deleted documents
			 * @param pBarrel index merge barrel,contain more than one index barrels
			 */
			void			mergeDeletedDocs(CMergeBarrel* pBarrel,bool bGC);

			/**
			 * merge term vectors
			 * @param pBarrel index merge barrel,contain more than one index barrels
			 */
			void			mergeTermVector(CMergeBarrel* pBarrel);

			/**
			 * merge term vectors with garbage collection
			 * @param pBarrel index merge barrel,contain more than one index barrels
			 */
			void			mergeTermVector_GC(CMergeBarrel* pBarrel);

			/** 
			 * let document ids in a continuous form 
			 * @param pBarrelsInfo barrels information
			 */
			void			continueDocIDs(CBarrelsInfo* pBarrelsInfo);
		protected:
			/** set field factories for create field's object */
			void			setFactoryFinder(CFactoryFinder* pFactories){m_pFieldFactories = pFactories;}
		protected:
			static bool		BarrelGreater(CMergeBarrelEntry* o1, CMergeBarrelEntry* o2) 
			{
				return (o1->numDocs()) > (o2->numDocs());
			}
			/**
			 * remove merged barrels from m_pMergeBarrels
			 * @param pBarrel container of barrels
			 */
			void			removeMergedBarrels(CMergeBarrel* pBarrel);					
		protected:
			CDirectory*			m_pDirectory;			///index data source
			CBarrelsInfo*		m_pBarrelsInfo;			///reference to Index's barrels information
			char*				m_buffer;				///buffer for merging process
			size_t				m_bufsize;				///size of buffer
			bool				m_bBorrowedBuffer;		///is the buffer borrowed from indexer?
			bool				m_bForceGC;				///garbage collection?			
			CIndexMerger::gc	m_gcParam;				///parameter for GC
			CFactoryFinder*	m_pFieldFactories;

			CSpeedProbe*		m_pSpeedProbe;

			vector<CMergeBarrelEntry*>*	m_pMergeBarrels;

			friend class CIndexWriter;
		};
		//////////////////////////////////////////////////////////////////////////
		///inline function			
	}
}

#endif
