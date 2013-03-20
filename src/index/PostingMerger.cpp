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
// Created	: 2007/1/15
//
#include "index/PostingMerger.h"
#include "store/IndexOutput.h"
#include "store/IndexInput.h"
#include "utility/BitVector.h"


namespace firtex
{
	namespace index
	{
		//////////////////////////////////////////////////////////////////////////
		///CPostingMerger
		CPostingMerger::CPostingMerger()
			:m_pOutputDescriptor(NULL)
			,m_buffer(NULL)
			,m_bufsize(0)
			,m_bOwnBuffer(false)
			,m_nIDFreed(0)
		{
			reset();
		}
		CPostingMerger::CPostingMerger(COutputDescriptor*	pOutputDescriptor)
			:m_pOutputDescriptor(pOutputDescriptor)
			,m_buffer(NULL)
			,m_bufsize(0)
			,m_bOwnBuffer(false)
			,m_nIDFreed(0)
		{
			reset();
		}
		CPostingMerger::~CPostingMerger()
		{
			if(m_buffer && m_bOwnBuffer)
			{
				delete m_buffer;
				m_buffer = NULL;
			}
			m_bufsize = 0;
		}		
		void CPostingMerger::setBuffer(char* buf,size_t bufSize)
		{
			m_buffer = buf;
			m_bufsize = bufSize;
		}
		void CPostingMerger::createBuffer()
		{
			m_buffer = new char[POSTINGMERGE_BUFFERSIZE];
			m_bufsize = POSTINGMERGE_BUFFERSIZE;
			m_bOwnBuffer = true;
		}

		//////////////////////////////////////////////////////////////////////////
		///CPostingMerger_WL
		CPostingMerger_WL::CPostingMerger_WL()
			:m_nPPostingLength(0)
			,m_bFirstPosting(true)			
		{	

		}
		CPostingMerger_WL::CPostingMerger_WL(COutputDescriptor*	pOutputDescriptor)
			:CPostingMerger(pOutputDescriptor)
			,m_nPPostingLength(0)
			,m_bFirstPosting(true)			
		{						
		}
		CPostingMerger_WL::~CPostingMerger_WL()
		{
		}
		void CPostingMerger_WL::mergeWith(docid_t baseDocID,CInMemoryPosting* pInMemoryPosting)
		{
			CInMemoryPosting_WL* pInMemoryPostingWL = static_cast<CInMemoryPosting_WL*>(pInMemoryPosting);
			FIRTEX_ASSERT(pInMemoryPostingWL,_T("dynamic cast failed."));

			///flush last doc
			pInMemoryPostingWL->flushLastDoc(true);
			
			CBitVector* pFilter = pInMemoryPosting->getFilter();
			if(pFilter && pFilter->hasSmallThan((size_t)pInMemoryPostingWL->m_nLastDocID))///there are deleted documents
			{					
				PostingChunk* pChunk = pInMemoryPostingWL->m_pDocFreqList->m_pHeadChunk;
				if(pChunk)
				{
					uint8_t* bp = &(pChunk->data[0]);
					docid_t firstDocID = CCompressedPostingList::decodePosting32(bp);
					if(!pFilter->hasBiggerThan(firstDocID))///no deleted documents in this posting
					{
						mergeWith_(baseDocID,pInMemoryPostingWL);
					}
					else //may be there are some deleted documents in this posting
					{
						mergeWith_GC(baseDocID,pInMemoryPostingWL,pFilter);
					}
				}
			}
			else 
			{
				mergeWith_(baseDocID,pInMemoryPostingWL);
			}
		}
		void CPostingMerger_WL::mergeWith(docid_t baseDocID,COnDiskPosting* pOnDiskPosting)
		{
			COnDiskPosting_WL* pOnDiskPostingWL = static_cast<COnDiskPosting_WL*>(pOnDiskPosting);
			FIRTEX_ASSERT(pOnDiskPostingWL,_T("static cast failed."));
			
			CBitVector* pFilter = pOnDiskPostingWL->getFilter();
			if(pFilter && pFilter->hasSmallThan((size_t)pOnDiskPostingWL->m_chunkDesc.lastdocid))///there are deleted documents			
			{				
				mergeWith_GC(baseDocID,pOnDiskPostingWL,pFilter);
			}
			else 
			{
				mergeWith_(baseDocID,pOnDiskPostingWL);
			}
		}
		fileoffset_t CPostingMerger_WL::endMerge()
		{
			m_bFirstPosting = true;
			m_nIDFreed = 0;
			if(m_postingDesc.df <= 0)
				return -1;

			CIndexOutput*	pDOutput = m_pOutputDescriptor->getDPostingOutput();
			CIndexOutput*	pPOutput = m_pOutputDescriptor->getPPostingOutput();
			fileoffset_t postingoffset = pDOutput->getFilePointer();

			///write position posting descriptor
			m_nPPostingLength = pPOutput->getFilePointer() - m_postingDesc.poffset;
			m_postingDesc.poffset = pPOutput->getFilePointer();
			pPOutput->writeVLong(m_nPPostingLength);	////<ChunkLength(VInt64)>

			///begin write posting descriptor
			pDOutput->writeVLong(m_postingDesc.length);	///<PostingLength(VInt64)>
			pDOutput->writeVInt(m_postingDesc.df);		///<DF(VInt32)>
			pDOutput->writeVLong(m_postingDesc.ctf);	///<CTF(VInt64)>			
			pDOutput->writeVLong(m_postingDesc.poffset);///<PositionPointer(VInt64)>
			///end write posting descriptor

			pDOutput->writeVInt(1);						///<ChunkCount(VInt32)>
			///begin write chunk descriptor
			pDOutput->writeVLong(m_chunkDesc.length);	///<ChunkLength(VInt64)>
			pDOutput->writeVInt(m_chunkDesc.lastdocid);	///<LastDocID(VInt32)>
			///end write posting descriptor				

			return postingoffset;
		}

	}
}

