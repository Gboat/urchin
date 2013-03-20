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
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/7/3
//
#include "index/DictHashIndexer.h"
#include "index/DictHashTermReader.h"

namespace firtex
{
	namespace index
	{
		CDictHashIndexer::CDictHashIndexer(CMemCache* pMemCache)
			:m_pMemCache(pMemCache)
			,m_tdiFilePointer(0)	
		{					
		}

		CDictHashIndexer::~CDictHashIndexer(void)
		{
			DynPostingArray::array_iterator aiter = m_array.elements();				
			CInMemoryPosting* pPosting;
			while(aiter.next())
			{
				pPosting = aiter.element();
				delete pPosting;		///clear posting data									
			}		

			m_pMemCache = NULL;				
		}
		
		void CDictHashIndexer::addField(docid_t did,CField* pField)
		{
			register count_t posInc = 0;			
			CTokens* pTokens = pField->tokensValue();
			termid_t* tids = (termid_t*)pTokens->getTokenBuffer();			
			count_t tcnt = (count_t)pTokens->getNumTokens();	
			if(getIndexLevel() == WORD_LEVEL)
			{
				wordLevelIndexing(did,tids,tcnt);
			}
			else if(getIndexLevel() == DOCUMENT_LEVEL) 
			{
				docLevelIndexing(did,tids,tcnt);
			}
			else 
			{
				docLevel0Indexing(did,tids,tcnt);
			}
			
		}
		void CDictHashIndexer::addFields(docid_t* pdid,CField** ppField,int32_t numFields)
		{
		}
		fileoffset_t CDictHashIndexer::write(COutputDescriptor* pWriterDesc)
		{
			m_tdiFilePointer = pWriterDesc->getVocOutput()->getFilePointer();

			CIndexOutput* pVocWriter = pWriterDesc->getVocOutput();
			CIndexOutput* pDWriter = pWriterDesc->getDPostingOutput();
			CIndexOutput* pPWriter = pWriterDesc->getPPostingOutput();

			fileoffset_t	poffset;
			termid_t		tid;
			fileoffset_t	lastPOffset = 0;
			termid_t		lastDocID = 0;
			int32_t termCount = 0;
			CInMemoryPosting* pPosting;
			fileoffset_t vocOffset = pVocWriter->getFilePointer();
			DynPostingArray::array_iterator aiter = m_array.elements();				
			while(aiter.next())
			{
				pPosting = aiter.element();
				if(!pPosting->hasNoChunk())
				{
					tid = (termid_t)aiter.position();					
					pVocWriter->writeVInt(tid - lastDocID);			///write term id					

					pVocWriter->writeVInt(pPosting->docFreq());		///write df					

					poffset = pPosting->write(pWriterDesc);			///write posting data
					pVocWriter->writeVLong(poffset - lastPOffset);	///write offset of posting descriptor 
					pPosting->reset();								///clear posting data

					lastDocID = tid;
					lastPOffset = poffset;

					termCount++;
				}
			}				
			fileoffset_t vocDescOffset = pVocWriter->getFilePointer();
			int64_t vocLength = vocDescOffset - vocOffset;
			///begin write vocabulary descriptor			
			pVocWriter->writeVLong(vocLength);	///<VocLength(VInt64)>			
			pVocWriter->writeVLong(termCount);	///<TermCount(VInt64)>
			///end write vocabulary descriptor

			return vocDescOffset;
		}		
		void CDictHashIndexer::reset()
		{
			DynPostingArray::array_iterator aiter = m_array.elements();				
			CInMemoryPosting* pPosting;
			while(aiter.next())
			{
				pPosting = aiter.element();
				if(!pPosting->hasNoChunk())
				{
					pPosting->reset();		///clear posting data					
				}
			}				
		}

		uint64_t CDictHashIndexer::distinctNumTerms()
		{
			return (uint64_t)m_array.length();
		}
		CTermReader* CDictHashIndexer::termReader()
		{			
			return new CDictHashInMemoryTermReader(getField(),this,NULL);	
		}
	}
}
