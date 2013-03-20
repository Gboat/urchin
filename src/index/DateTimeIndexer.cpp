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
// Created	: 2006/6/10
//

#include "index/DateTimeIndexer.h"
#include "index/DateTimeTermReader.h"

namespace firtex
{
	namespace index
	{
		CDateTimeIndexer::CDateTimeIndexer(CMemCache* pMemCache)
			:m_pMemCache(pMemCache)
			,m_tdiFilePointer(0)	
		{					
		}

		CDateTimeIndexer::~CDateTimeIndexer(void)
		{
			DateTimeMap::iterator iter = m_mapDateTime.begin();			
			while(iter != m_mapDateTime.end())
			{
				delete iter->second;				
				iter++;
			}
			m_pMemCache = NULL;
		}

		void CDateTimeIndexer::addField(docid_t did,CField* pField)
		{
			register count_t posInc = 0;			
			CTokens* pTokens = pField->tokensValue();
			int64_t dt;
			if(pTokens)
			{
				dt = *((int64_t*)pTokens->getTokenBuffer());				
			}
			else 
			{
				numbervar nv = pField->numberValue();
				dt = nv.value.i64val;
			}
			docLevel0Indexing(did,dt);
		}
		void CDateTimeIndexer::addFields(docid_t* pdid,CField** ppField,int32_t numFields)
		{
		}
		fileoffset_t CDateTimeIndexer::write(COutputDescriptor* pWriterDesc)
		{
			m_tdiFilePointer = pWriterDesc->getVocOutput()->getFilePointer();

			CIndexOutput* pVocWriter = pWriterDesc->getVocOutput();
			CIndexOutput* pDWriter = pWriterDesc->getDPostingOutput();			

			fileoffset_t	poffset;			
			fileoffset_t	lastPOffset = 0;
			int64_t			tid;
			int64_t			lastID = 0;
			int32_t			termCount = 0;
			CInMemoryPosting_DL0* pPosting;
						
			fileoffset_t vocOffset = pVocWriter->getFilePointer();
			DateTimeMap::iterator iter = m_mapDateTime.begin();			
			while (iter != m_mapDateTime.end())
			{				
				pPosting = iter->second;
				if(!pPosting->hasNoChunk())
				{
					tid = iter->first;
					pVocWriter->writeVLong(tid - lastID);				///write termid
					pVocWriter->writeVInt(pPosting->docFreq());			///write df					

					poffset = pPosting->write(pWriterDesc);				///write posting data
					pVocWriter->writeVLong(poffset - lastPOffset);		///write offset of posting descriptor 
					pPosting->reset();									///clear posting data

					lastID = tid;
					lastPOffset = poffset;

					termCount++;				
				}				
				iter ++;
			}								
			

			//table_array* lex = new table_array[m_mapDateTime.size()];
			//size_t len = 0;			
			//DateTimeMap::iterator iter = m_mapDateTime.begin();			
			//while (iter != m_mapDateTime.end())
			//{				
			//	pPosting = iter->second;
			//	if(!pPosting->hasNoChunk())
			//	{
			//		lex[len].dt = iter->first;
			//		lex[len].posting = pPosting;
			//		len ++;					
			//	}				
			//	iter ++;
			//}					

			//if(len == 0)
			//	return 0;
			//quickSort(lex,0,len -1);///quick sort		
			//fileoffset_t vocOffset = pVocWriter->getFilePointer();

			//for (size_t i = 0;i < len;i++)
			//{
			//	pVocWriter->writeVLong(lex[i].dt - lastID);			///write termid
			//	pVocWriter->writeVInt(pPosting->docFreq());			///write df					

			//	poffset = pPosting->write(pWriterDesc);				///write posting data
			//	pVocWriter->writeVLong(poffset - lastPOffset);		///write offset of posting descriptor 
			//	pPosting->reset();									///clear posting data

			//	lastID = lex[i].dt;
			//	lastPOffset = poffset;

			//	termCount++;				
			//}

			fileoffset_t vocDescOffset = pVocWriter->getFilePointer();
			int64_t vocLength = vocDescOffset - vocOffset;
			///begin write vocabulary descriptor			
			pVocWriter->writeVLong(vocLength);	///<VocLength(VInt64)>			
			pVocWriter->writeVLong(termCount);	///<TermCount(VInt64)>
			///end write vocabulary descriptor

			return vocDescOffset;		
		}		

		void CDateTimeIndexer::reset()
		{
			DateTimeMap::iterator iter = m_mapDateTime.begin();
			CInMemoryPosting_DL0* pPosting;
			while (iter != m_mapDateTime.end())
			{
				pPosting = iter->second;
				if(!pPosting->hasNoChunk())
				{
					pPosting->reset();///clear posting data
				}
				iter++;
			}			
		}

		uint64_t CDateTimeIndexer::distinctNumTerms()
		{
			size_t len = 0;			
			DateTimeMap::iterator iter = m_mapDateTime.begin();			
			while (iter != m_mapDateTime.end())
			{				
				if(!iter->second->hasNoChunk())
				{					
					len ++;					
				}				
				iter ++;
			}				

			return len;			
		}
		CTermReader* CDateTimeIndexer::termReader()
		{			
			return new CDateTimeInMemoryTermReader(getField(),&m_mapDateTime,NULL);				
		}
	}
}

