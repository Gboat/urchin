//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/11/22
//
#include "index/Posting.h"
#include <math.h>
#include "utility/StdHeader.h"
#include "store/IndexOutput.h"
#include "store/IndexInput.h"
#include "index/OutputDescriptor.h"
#include "index/InputDescriptor.h"
#include "utility/BitVector.h"
#include "store/FSIndexInput.h"
using namespace firtex::store;
using namespace firtex::utility;

namespace firtex
{
	namespace index
	{
		//////////////////////////////////////////////////////////////////////////
		///CPosting
		CPosting::CPosting()
			:m_pDocFilter(NULL)
		{

		}
		CPosting::CPosting(CBitVector* pDocFilter)
			:m_pDocFilter(pDocFilter)
		{

		}
		CPosting::~CPosting()
		{
		}

		//////////////////////////////////////////////////////////////////////////
		///CInMemoryPosting
		CInMemoryPosting::AllocStrategy CInMemoryPosting::WL_ALLOCSTRATEGY ;
		CInMemoryPosting::AllocStrategy CInMemoryPosting::DL_ALLOCSTRATEGY ;
		CInMemoryPosting::AllocStrategy CInMemoryPosting::DL0_ALLOCSTRATEGY;


		CInMemoryPosting::CInMemoryPosting(CMemCache* pMemCache)
			:m_pMemCache(pMemCache)
		{

		}
		CInMemoryPosting::CInMemoryPosting()
			:m_pMemCache(NULL)
		{

		}
		CInMemoryPosting::~CInMemoryPosting()
		{
			m_pMemCache = NULL;
		}

		PostingChunk* CInMemoryPosting::newChunk(int32_t chunkSize)
		{
			uint8_t* begin = m_pMemCache->getMem(chunkSize);
			///allocate memory failed,decrease chunk size
			if(!begin)
			{
				///into UPTIGHT state
				begin = m_pMemCache->getMem(UPTIGHT_ALLOC_CHUNKSIZE);				
				///allocation failed again, grow memory cache.
				if(!begin)
				{
					begin  = m_pMemCache->grow(UPTIGHT_ALLOC_MEMSIZE)->getMem(UPTIGHT_ALLOC_CHUNKSIZE);						
					if(!begin)
					{
						FIRTEX_THROW2(OUTOFMEM_ERROR,_T("CInMemoryPosting:newChunk() : Allocate memory failed."));
					}					
				}
				chunkSize = UPTIGHT_ALLOC_CHUNKSIZE;
			}

			PostingChunk* pChunk = (PostingChunk*)begin;
			pChunk->size = (int32_t)(firtex::utility::POW_TABLE[chunkSize] - sizeof(PostingChunk*) - sizeof(int32_t));
			pChunk->next = NULL;							
			return pChunk;
		}

		//////////////////////////////////////////////////////////////////////////
		///CInMemoryPosting_WL
		CInMemoryPosting_WL::CInMemoryPosting_WL(CMemCache* pMemCache)
			:CInMemoryPosting(pMemCache)
			,m_nDF(0)
			,m_nLastDocID(-1)
			,m_nLastLoc(0)
			,m_nCurTermFreq(0)
			,m_nCTF(0)
			,m_pDS(NULL)
		{			
			m_pDocFreqList = new CCompressedPostingList();
			m_pLocList  = new CCompressedPostingList();			

			int32_t newSize = getNextChunkSize(m_pDocFreqList->m_nTotalSize,CInMemoryPosting::DL_ALLOCSTRATEGY);
			m_pDocFreqList->addChunk(newChunk(newSize));
			newSize = getNextChunkSize(m_pLocList->m_nTotalSize,CInMemoryPosting::WL_ALLOCSTRATEGY);
			m_pLocList->addChunk(newChunk(newSize));			
		}
		CInMemoryPosting_WL::CInMemoryPosting_WL()
			:m_nDF(0)
			,m_nLastDocID(-1)
			,m_nLastLoc(0)
			,m_nCurTermFreq(0)
			,m_nCTF(0)
			,m_pDS(NULL)
			,m_pDocFreqList(NULL)
			,m_pLocList(NULL)
		{			
		}
		CInMemoryPosting_WL::~CInMemoryPosting_WL()
		{
			if(m_pDocFreqList)
			{
				delete m_pDocFreqList;
				m_pDocFreqList = NULL;
			}
			if(m_pLocList)
			{
				delete m_pLocList;
				m_pLocList = NULL;
			}
			m_pMemCache = NULL;

			if(m_pDS)
			{
				delete m_pDS;
				m_pDS = NULL;
			}
		}	

		fileoffset_t CInMemoryPosting_WL::write(COutputDescriptor* pOutputDescriptor)
		{
			///flush last document
			flushLastDoc(true);

			CIndexOutput* pDOutput = pOutputDescriptor->getDPostingOutput();
			
			///write chunk data
			writeDPosting(pDOutput);

			///save the offset of posting descriptor
			fileoffset_t poffset = pDOutput->getFilePointer();

			CIndexOutput* ptiOutput = pOutputDescriptor->getPPostingOutput();
			///write position posting data
			fileoffset_t positionPointer = writePPosting(ptiOutput);			
			///write descriptors
			writeDescriptor(pDOutput,positionPointer);			

			return poffset;
		}

		void CInMemoryPosting_WL::reset()
		{
			m_pDocFreqList->reset();
			m_pLocList->reset();

			m_nCTF = 0;
			m_nLastDocID = -1;
			m_nLastLoc = 0;
			m_nDF = 0;
			m_nCurTermFreq = 0;		

			if(m_pDS)
			{				
				delete m_pDS;
				m_pDS = NULL;
			}
		}
		
		CPosting* CInMemoryPosting_WL::clone()
		{
			CInMemoryPosting_WL* pClone = new CInMemoryPosting_WL();
			if(m_pDocFreqList)
			{
				pClone->m_pDocFreqList = new CCompressedPostingList(*m_pDocFreqList);
			}			
			if(m_pLocList)
			{
				pClone->m_pLocList = new CCompressedPostingList(*m_pLocList);
			}			
			pClone->m_nCTF = m_nCTF;
			pClone->m_nDF = m_nDF;
			pClone->m_nCurTermFreq = m_nCurTermFreq;
			pClone->m_nLastDocID = m_nLastDocID;
			pClone->m_nLastLoc = m_nLastLoc;
			return pClone;
		}

		int32_t CInMemoryPosting_WL::decodeNext(int32_t* pPosting,int32_t length)
		{

#define ISCHUNKOVER_D()\
	if(pDChunk > pDChunkEnd)\
	{\
		m_pDS->decodingDChunk = m_pDS->decodingDChunk->next;\
		if(!m_pDS->decodingDChunk)\
			break;\
		m_pDS->decodingDChunkPos = 0;\
		pDChunk = &(m_pDS->decodingDChunk->data[m_pDS->decodingDChunkPos]);\
		pDChunkEnd = &(m_pDS->decodingDChunk->data[m_pDS->decodingDChunk->size-1]);\
	}

			///flush last document
			//flushLastDoc(false);
			if(!m_pDS)
			{				
				m_pDS = new CInMemoryPosting_WL::DecodeState;
				m_pDS->decodingDChunk = m_pDocFreqList->m_pHeadChunk;
				m_pDS->decodingDChunkPos = 0;
				m_pDS->lastDecodedDocID = 0;
				m_pDS->decodedDocCount = 0;
				m_pDS->decodingPChunk = m_pLocList->m_pHeadChunk;
				m_pDS->decodingPChunkPos = 0;
				m_pDS->lastDecodedPos = 0;
				m_pDS->decodedPosCount = 0;							
			}

			int32_t* pDoc = pPosting;
			int32_t* pFreq = pPosting + (length >> 1);

			int32_t left = m_nDF - m_pDS->decodedDocCount;
			if(left <= 0)
				return -1;
			if(length > left*2)
				length = left*2;			
			left = (length >> 1);
		
			uint8_t* pDChunk = &(m_pDS->decodingDChunk->data[m_pDS->decodingDChunkPos]);
			uint8_t* pDChunkEnd = &(m_pDS->decodingDChunk->data[m_pDS->decodingDChunk->size-1]);
			
			int32_t count = 0;
			docid_t did = m_pDS->lastDecodedDocID;
			while (count < left)
			{
				ISCHUNKOVER_D();
				did += CCompressedPostingList::decodePosting32(pDChunk);				
				if(!getFilter() || !getFilter()->test((size_t)did))///Is the document deleted?
				{
					*pDoc++ = did;
					
					ISCHUNKOVER_D();

					*pFreq++ = CCompressedPostingList::decodePosting32(pDChunk);					
				}
				else
				{
					ISCHUNKOVER_D();
					CCompressedPostingList::decodePosting32(pDChunk); /// skip the freq
				}
				count++;
			}
			///update state
			m_pDS->decodedDocCount += count; 
			m_pDS->lastDecodedDocID = did;
			m_pDS->decodingDChunkPos = (int32_t)(pDChunk - m_pDS->decodingDChunk->data);

			return (int32_t)(pDoc - pPosting);
		}

		void CInMemoryPosting_WL::decodeNextPositions(int32_t* pPosting,int32_t length)
		{
			FIRTEX_ASSERT((m_pDS!=NULL),_T("Call CInMemoryPosting_WL::decodeNext() first."));			
			
			uint8_t* pPChunk = &(m_pDS->decodingPChunk->data[m_pDS->decodingPChunkPos]);
			uint8_t* pPChunkEnd = &(m_pDS->decodingPChunk->data[m_pDS->decodingPChunk->size-1]);
			
			int32_t* pPos = pPosting;
			loc_t loc = m_pDS->lastDecodedPos;
			int32_t  nDecoded = 0;
			while (nDecoded < length)
			{
				if(pPChunk > pPChunkEnd)
				{
					m_pDS->decodingPChunk = m_pDS->decodingPChunk->next;
					if(!m_pDS->decodingPChunk)
						break;
					m_pDS->decodingPChunkPos = 0;
					pPChunk = &(m_pDS->decodingPChunk->data[m_pDS->decodingPChunkPos]);
					pPChunkEnd = &(m_pDS->decodingPChunk->data[m_pDS->decodingPChunk->size-1]);
				}

				loc += CCompressedPostingList::decodePosting32(pPChunk);
				if(pPos)
				{
					*pPos = loc;					
					pPos++;	
				}	
				nDecoded++;
			}			
			m_pDS->decodedPosCount += nDecoded;
			m_pDS->lastDecodedPos = loc;			
		}		
		
		void CInMemoryPosting_WL::decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs)
		{
			FIRTEX_ASSERT((m_pDS!=NULL),_T("Call CInMemoryPosting_WL::decodeNext() first."));			

			uint8_t* pPChunk = &(m_pDS->decodingPChunk->data[m_pDS->decodingPChunkPos]);
			uint8_t* pPChunkEnd = &(m_pDS->decodingPChunk->data[m_pDS->decodingPChunk->size-1]);

			int32_t* pPos = pPosting;
			loc_t loc = m_pDS->lastDecodedPos;
			int32_t  nTotalDecoded = 0;
			int32_t  nCurDecoded;
			for (int32_t nF = 0; nF < nFreqs;nF++)
			{
				nCurDecoded = 0;
				while (nCurDecoded < pFreqs[nF])
				{
					if(pPChunk > pPChunkEnd)
					{
						m_pDS->decodingPChunk = m_pDS->decodingPChunk->next;
						if(!m_pDS->decodingPChunk)
							break;
						m_pDS->decodingPChunkPos = 0;
						pPChunk = &(m_pDS->decodingPChunk->data[m_pDS->decodingPChunkPos]);
						pPChunkEnd = &(m_pDS->decodingPChunk->data[m_pDS->decodingPChunk->size-1]);
					}

					loc += CCompressedPostingList::decodePosting32(pPChunk);
					if(pPos)
					{
						*pPos = loc;					
						pPos++;	
					}	
					nCurDecoded++;
				}
				m_pDS->lastDecodedPos = loc = 0;
				nTotalDecoded += nCurDecoded;
			}
			
			m_pDS->decodedPosCount += nTotalDecoded;
			m_pDS->lastDecodedPos = loc;		
		}

		void CInMemoryPosting_WL::resetPosition()
		{
			m_pDS->lastDecodedPos = 0;
		}
		//////////////////////////////////////////////////////////////////////////
		///COnDiskPosting_WL
		COnDiskPosting_WL::COnDiskPosting_WL(CInputDescriptor* pInputDescriptor,fileoffset_t poffset)
			:COnDiskPosting(pInputDescriptor)			
			,m_nBufSize(0)
		{
			m_postingOffset = poffset;
			reset(m_postingOffset);
		}
		COnDiskPosting_WL::COnDiskPosting_WL()
		{			
			m_nBufSize = 0;
			m_postingOffset = 0;
			m_postingDesc.length = 0;
			m_postingDesc.df = 0;
			m_postingDesc.ctf = 0;
			m_postingDesc.poffset = 0;
			
			m_chunkDesc.length = 0;
			m_chunkDesc.lastdocid = 0;

			m_nPPostingLength = 0;					

			m_ds.decodedDocCount = 0;
			m_ds.lastDecodedDocID = 0;
			m_ds.decodedPosCount= 0;
			m_ds.lastDecodedPos = 0;
		}
		COnDiskPosting_WL::~COnDiskPosting_WL()
		{
		}
		void COnDiskPosting_WL::reset(fileoffset_t newOffset)
		{
			m_postingOffset = newOffset;
			CIndexInput* pDPInput = m_pInputDescriptor->getDPostingInput();
			pDPInput->seekInternal(newOffset);///not seek(), because seek() may trigger a large data read event.

			///read descriptor of posting list <PostingDescriptor>
			uint8_t buf[512];
			uint8_t* u = buf;
			pDPInput->readInternal((char*)buf,512,false);
			m_postingDesc.length = CCompressedPostingList::decodePosting64(u);	///<PostingLength(VInt64)>
			m_postingDesc.df = CCompressedPostingList::decodePosting32(u);		///<DF(VInt32)>
			m_postingDesc.ctf = CCompressedPostingList::decodePosting64(u);		///<CTF(VInt64)>
			m_postingDesc.poffset = CCompressedPostingList::decodePosting64(u);	///PositionPointer(VInt64)

			CCompressedPostingList::decodePosting32(u);///<ChunkCount(VInt32)>			
			///read first chunk descriptor of posting list <ChunkDescriptor>
			m_chunkDesc.length = CCompressedPostingList::decodePosting64(u);	///<ChunkLength(VInt64)>
			m_chunkDesc.lastdocid = CCompressedPostingList::decodePosting32(u);	///<LastDocID(VInt32)>						

			CIndexInput* pPPInput = m_pInputDescriptor->getPPostingInput();
			if(pPPInput)
			{
				pPPInput->seekInternal(m_postingDesc.poffset);///not seek(), because seek() may trigger a large data read event.
				pPPInput->readInternal((char*)buf,8,false);
				u = buf;
				m_nPPostingLength = CCompressedPostingList::decodePosting64(u); ///<ChunkLength(VInt64)>				
				pPPInput->seek(m_postingDesc.poffset - m_nPPostingLength);///seek to the begin of position posting data							
			}
			else 
			{
				m_nPPostingLength = 0;
			}

			pDPInput->seek(newOffset - m_postingDesc.length);	///seek to the begin of posting data			

			m_ds.decodedDocCount = 0;
			m_ds.lastDecodedDocID = 0;
			m_ds.decodedPosCount= 0;
			m_ds.lastDecodedPos = 0;
		}
		int32_t COnDiskPosting_WL::decodeNext(int32_t* pPosting,int32_t length)
		{
			int32_t left = m_postingDesc.df - m_ds.decodedDocCount;
			if(left <= 0)
				return -1;
			int32_t* pDoc = pPosting;
			int32_t* pFreq = pPosting + (length>>1);

			if(length > left*2)
				length = left*2;			
			left = (length>>1);

			CIndexInput*	pDPostingInput = getInputDescriptor()->getDPostingInput();			

			count_t	df = m_postingDesc.df;
			int32_t count = 0;
			docid_t did = m_ds.lastDecodedDocID;			
			while (count < left)
			{				
				did += pDPostingInput->readVInt();				
				if(!getFilter() || !getFilter()->test((size_t)did))///Is the document deleted?
				{
					*pDoc++ = did;
					*pFreq++ = pDPostingInput->readVInt();
				}
				else
				{
					pDPostingInput->readVInt(); /// skip the freq
				}				
				count++;
			}			
			FIRTEX_ASSERT((did<=m_chunkDesc.lastdocid),_T("document id is out of range."));

			///update state
			m_ds.decodedDocCount += count; 
			m_ds.lastDecodedDocID = did;

			return (int32_t)(pDoc - pPosting);
		}
		void COnDiskPosting_WL::decodeNextPositions(int32_t* pPosting,int32_t length)
		{
			if(length <= 0)
				return;
			CIndexInput*	pPPostingInput = getInputDescriptor()->getPPostingInput();								

			int32_t nDecoded = 0;
			loc_t loc = m_ds.lastDecodedPos;			
			int32_t* pPos = pPosting;
			while (nDecoded < length)
			{
				loc += pPPostingInput->readVInt();
				if(pPos)
				{
					*pPos = loc;					
					pPos++;			
				}
				nDecoded++;
			}			
						
			m_ds.decodedPosCount += nDecoded;
			m_ds.lastDecodedPos = loc;				
		}			
		void COnDiskPosting_WL::decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs)
		{			
			CIndexInput*	pPPostingInput = getInputDescriptor()->getPPostingInput();								

			int32_t nTotalDecoded = 0;
			int32_t nCurDecoded = 0;
			loc_t loc = m_ds.lastDecodedPos;			
			int32_t* pPos = pPosting;
			for (int32_t nF = 0;nF < nFreqs;nF++)
			{
				nCurDecoded = 0;
				while (nCurDecoded < pFreqs[nF])
				{
					loc += pPPostingInput->readVInt();
					if(pPos)
					{
						*pPos = loc;					
						pPos++;			
					}
					nCurDecoded++;
				}
				nTotalDecoded += nCurDecoded;
				loc = 0;
			}			

			m_ds.decodedPosCount += nTotalDecoded;
			m_ds.lastDecodedPos = loc;				
		}
		void COnDiskPosting_WL::resetPosition()
		{
			m_ds.lastDecodedPos = 0;
		}
		        
		void COnDiskPosting_WL::reset()
		{
			reset(m_postingOffset);
		}
		
		CPosting* COnDiskPosting_WL::clone()
		{			
			return NULL;
		}
		size_t COnDiskPosting_WL::setBuffer(int32_t* buffer,size_t nBufSize)
		{
			size_t nBufUsed = nBufSize*sizeof(int32_t);
			if(nBufUsed <= 2*INDEXINPUT_BUFFSIZE)
			{
				m_nBufSize = 0;
				return 0;
			}

			CIndexInput* pDInput = getInputDescriptor()->getDPostingInput();
			CIndexInput* pPInput = getInputDescriptor()->getPPostingInput();
			if((int64_t)nBufUsed > (m_postingDesc.length + m_nPPostingLength))
			{
				nBufUsed = (size_t)(m_postingDesc.length + m_nPPostingLength);
				pDInput->setBuffer((char*)buffer,(size_t)m_postingDesc.length);
				if(pPInput)
					pPInput->setBuffer((char*)buffer + m_postingDesc.length,(size_t)m_nPPostingLength);
			}
			else
			{
				size_t nDSize = nBufUsed/2;
				if((int64_t)nDSize > m_postingDesc.length)
					nDSize = (size_t)m_postingDesc.length;
				pDInput->setBuffer((char*)buffer,nDSize);
				if(pPInput)
					pPInput->setBuffer((char*)buffer + nDSize,nBufUsed - nDSize);
			}			
			m_nBufSize = nBufUsed;			
			return (nBufUsed + sizeof(int32_t) - 1)/sizeof(int32_t);
		}
		//////////////////////////////////////////////////////////////////////////
		///CInMemoryPosting_DL
		CInMemoryPosting_DL::CInMemoryPosting_DL(CMemCache* pMemCache)
			:CInMemoryPosting(pMemCache)
			,m_nDF(0)
			,m_nLastDocID(-1)			
			,m_nCurTermFreq(0)
			,m_nCTF(0)
		{
			m_pDocFreqList = new CCompressedPostingList();
			
			int32_t newSize = getNextChunkSize(m_pDocFreqList->m_nTotalSize,CInMemoryPosting::DL_ALLOCSTRATEGY);
			m_pDocFreqList->addChunk(newChunk(newSize));			
		}
		CInMemoryPosting_DL::~CInMemoryPosting_DL()
		{
			if(m_pDocFreqList)
			{
				delete m_pDocFreqList;
				m_pDocFreqList = NULL;
			}
			m_pMemCache = NULL;
		}
		fileoffset_t CInMemoryPosting_DL::write(COutputDescriptor* pOutputDescriptor)
		{
			//添加最后一个
			if(m_nCurTermFreq > 0)
			{
				if(!m_pDocFreqList->addPosting(m_nCurTermFreq))
				{
					//没内存了					
					int32_t newSize = getNextChunkSize(m_pDocFreqList->m_nTotalSize,CInMemoryPosting::DL_ALLOCSTRATEGY);
					m_pDocFreqList->addChunk(newChunk(newSize));
					m_pDocFreqList->addPosting(m_nCurTermFreq);
				}
				m_pDocFreqList->truncTailChunk();///update real size				
				m_nCurTermFreq = 0;
			}				

			CIndexOutput* pDOutput = pOutputDescriptor->getDPostingOutput();

			///begin write posting descriptor
			pDOutput->writeVLong(m_pDocFreqList->getRealSize());	///<PostingLength(VInt64)>
			pDOutput->writeVLong(m_nCTF);							///<CTF(VInt64)>			
			///end write posting descriptor

			///begin write chunk descriptor
			///just one chunk on disk
			pDOutput->writeVInt(m_pDocFreqList->getRealSize());	///<ChunkLength(VInt32)>
			pDOutput->writeVInt(m_nLastDocID);					///<LastDocID(VInt32)>
			///end write chunk descriptor

			PostingChunk* pChunk = m_pDocFreqList->m_pHeadChunk;
			while(pChunk)
			{				
				pDOutput->write((const char*)pChunk->data,pChunk->size);
				pChunk = pChunk->next;
			}
			return 0;
		}

		void CInMemoryPosting_DL::reset()
		{
			m_pDocFreqList->reset();		

			m_nLastDocID = -1;			
			m_nDF = 0;
			m_nCurTermFreq = 0;		
			m_nCTF = 0;
		}
		int32_t CInMemoryPosting_DL::decodeNext(int32_t* pPosting,int32_t length)
		{
			return -1;
		}

		void CInMemoryPosting_DL::decodeNextPositions(int32_t* pPosting,int32_t length)
		{			
		}
		void CInMemoryPosting_DL::decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs)
		{
		}
		
		void CInMemoryPosting_DL::resetPosition()
		{

		}

		//////////////////////////////////////////////////////////////////////////
		///COnDiskPosting_DL
		COnDiskPosting_DL::COnDiskPosting_DL(CInputDescriptor* pInputDescriptor)
			:COnDiskPosting(pInputDescriptor)
		{
			CIndexInput* pDPInput = pInputDescriptor->getDPostingInput();

			///read descriptor of posting list <PostingDescriptor>
			m_postingDesc.length = pDPInput->readVLong();	///<PostingLength(VInt64)>
			m_postingDesc.df = pDPInput->readVInt();		///<DF(VInt32)>
			m_postingDesc.ctf = pDPInput->readVLong();		///<CTF(VInt64)>
			m_postingDesc.poffset = pDPInput->readVLong();	///PositionPointer(VInt64)

			///read first chunk descriptor of posting list <ChunkDescriptor>
			m_chunkDesc.length = pDPInput->readVInt(); ///<ChunkLength(VInt32)>
			m_chunkDesc.lastdocid = pDPInput->readVInt();///<LastDocID(VInt32)>			
		
			m_ds.decodedDocCount = 0;
			m_ds.lastDecodedDocID = 0;
		}
		COnDiskPosting_DL::~COnDiskPosting_DL()
		{
		}

		int32_t COnDiskPosting_DL::decodeNext(int32_t* pPosting,int32_t length)
		{
			int32_t left = m_postingDesc.df - m_ds.decodedDocCount;
			if(left <= 0)
				return -1;
			if(length > left*2)
				length = left*2;
			else if((length % 2) != 0)
				length--;
			left = length/2;

			CIndexInput*	pDPostingInput = getInputDescriptor()->getDPostingInput();
			int32_t* pDoc = pPosting;
			int32_t* pFreq = pPosting + left;

			count_t	df = m_postingDesc.df;
			int32_t count = m_ds.decodedDocCount;
			docid_t did = m_ds.lastDecodedDocID;
			while (count < left)
			{
				did += pDPostingInput->readVInt();
				if(!getFilter() || !getFilter()->test((size_t)did))///Is the document deleted?
				{
					*pDoc = did;
					*pFreq = pDPostingInput->readVInt();
					pDoc++;
					pFreq++;
				}
				else pDPostingInput->readVInt(); /// skip the freq
				count++;
			}
			///update state
			m_ds.decodedDocCount = count; 
			m_ds.lastDecodedDocID = did;

			return length;
		}
		void COnDiskPosting_DL::decodeNextPositions(int32_t* pPosting,int32_t length)
		{		
			
		}
		void COnDiskPosting_DL::decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs)
		{
		}
		
		void COnDiskPosting_DL::resetPosition()
		{

		}

		void COnDiskPosting_DL::reset()
		{
			CInputDescriptor* pInputDesc = getInputDescriptor();
			pInputDesc->getDPostingInput()->seek(0);			

			m_ds.decodedDocCount = 0;
			m_ds.lastDecodedDocID = 0;			
		}
		void COnDiskPosting_DL::reset(fileoffset_t newOffset)
		{
		}

		//////////////////////////////////////////////////////////////////////////
		///CInMemoryPosting_DL0
		CInMemoryPosting_DL0::CInMemoryPosting_DL0(CMemCache* pMemCache)
			:CInMemoryPosting(pMemCache)
			,m_nDF(0)
			,m_nLastDocID(-1)						
			,m_pDS(NULL)
		{			
			m_pDocList = new CCompressedPostingList();			

			int32_t newSize = getNextChunkSize(m_pDocList->m_nTotalSize,CInMemoryPosting::DL0_ALLOCSTRATEGY);
			m_pDocList->addChunk(newChunk(newSize));
			
		}
		CInMemoryPosting_DL0::CInMemoryPosting_DL0()
			:m_nDF(0)
			,m_nLastDocID(-1)									
			,m_pDS(NULL)
			,m_pDocList(NULL)			
		{			
		}
		CInMemoryPosting_DL0::~CInMemoryPosting_DL0()
		{
			if(m_pDocList)
			{
				delete m_pDocList;
				m_pDocList = NULL;
			}
			
			m_pMemCache = NULL;

			if(m_pDS)
			{
				delete m_pDS;
				m_pDS = NULL;
			}
		}	

		fileoffset_t CInMemoryPosting_DL0::write(COutputDescriptor* pOutputDescriptor)
		{
			CIndexOutput* pDOutput = pOutputDescriptor->getDPostingOutput();

			///write chunk data
			writeDPosting(pDOutput);

			///save the offset of posting descriptor
			fileoffset_t poffset = pDOutput->getFilePointer();
			
			///write descriptors
			writeDescriptor(pDOutput,0);

			return poffset;
		}

		void CInMemoryPosting_DL0::reset()
		{
			m_pDocList->reset();			
			
			m_nLastDocID = -1;
			m_nDF = 0;			

			if(m_pDS)
			{				
				delete m_pDS;
				m_pDS = NULL;
			}
		}

		CPosting* CInMemoryPosting_DL0::clone()
		{
			CInMemoryPosting_DL0* pClone = new CInMemoryPosting_DL0();
			if(m_pDocList)
			{
				pClone->m_pDocList = new CCompressedPostingList(*m_pDocList);
			}						
			
			pClone->m_nDF = m_nDF;			
			pClone->m_nLastDocID = m_nLastDocID;			
			return pClone;
		}

		int32_t CInMemoryPosting_DL0::decodeNext(int32_t* pPosting,int32_t length)
		{

#define ISCHUNKOVER_D()\
	if(pDChunk > pDChunkEnd)\
			{\
			m_pDS->decodingDChunk = m_pDS->decodingDChunk->next;\
			if(!m_pDS->decodingDChunk)\
			break;\
			m_pDS->decodingDChunkPos = 0;\
			pDChunk = &(m_pDS->decodingDChunk->data[m_pDS->decodingDChunkPos]);\
			pDChunkEnd = &(m_pDS->decodingDChunk->data[m_pDS->decodingDChunk->size-1]);\
			}

			///flush last document
			//flushLastDoc(false);
			if(!m_pDS)
			{				
				m_pDS = new CInMemoryPosting_DL0::DecodeState;
				m_pDS->decodingDChunk = m_pDocList->m_pHeadChunk;
				m_pDS->decodingDChunkPos = 0;
				m_pDS->lastDecodedDocID = 0;
				m_pDS->decodedDocCount = 0;					
			}

			int32_t* pDoc = pPosting;
			int32_t* pFreq = pPosting + (length >> 1);

			int32_t left = m_nDF - m_pDS->decodedDocCount;
			if(left <= 0)
				return -1;
			if(length > left*2)
				length = left*2;			
			left = (length >> 1);

			uint8_t* pDChunk = &(m_pDS->decodingDChunk->data[m_pDS->decodingDChunkPos]);
			uint8_t* pDChunkEnd = &(m_pDS->decodingDChunk->data[m_pDS->decodingDChunk->size-1]);

			int32_t count = 0;
			docid_t did = m_pDS->lastDecodedDocID;
			while (count < left)
			{
				ISCHUNKOVER_D();
				did += CCompressedPostingList::decodePosting32(pDChunk);
				if(!getFilter() || !getFilter()->test((size_t)did))///Is the document deleted?
				{
					*pDoc++ = did;

					ISCHUNKOVER_D();
				}
				
				count++;
			}
			///update state
			m_pDS->decodedDocCount += count; 
			m_pDS->lastDecodedDocID = did;
			m_pDS->decodingDChunkPos = (int32_t)(pDChunk - m_pDS->decodingDChunk->data);

			return (int32_t)(pDoc - pPosting);
		}

		void CInMemoryPosting_DL0::decodeNextPositions(int32_t* pPosting,int32_t length)
		{
			FIRTEX_THROW3(UNSUPPORTED_ERROR,_T(""));
		}		

		void CInMemoryPosting_DL0::decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs)
		{
			FIRTEX_THROW3(UNSUPPORTED_ERROR,_T(""));
		}

		void CInMemoryPosting_DL0::resetPosition()
		{			
		}
		//////////////////////////////////////////////////////////////////////////
		///COnDiskPosting_DL0
		COnDiskPosting_DL0::COnDiskPosting_DL0(CInputDescriptor* pInputDescriptor,fileoffset_t poffset)
			:COnDiskPosting(pInputDescriptor)			
			,m_nBufSize(0)
		{
			m_postingOffset = poffset;
			reset(m_postingOffset);
		}
		COnDiskPosting_DL0::COnDiskPosting_DL0()
		{			
			m_nBufSize = 0;
			m_postingOffset = 0;
			m_postingDesc.length = 0;
			m_postingDesc.df = 0;
			m_postingDesc.ctf = 0;
			m_postingDesc.poffset = 0;

			m_chunkDesc.length = 0;
			m_chunkDesc.lastdocid = 0;


			m_ds.decodedDocCount = 0;
			m_ds.lastDecodedDocID = 0;
		}
		COnDiskPosting_DL0::~COnDiskPosting_DL0()
		{
		}
		void COnDiskPosting_DL0::reset(fileoffset_t newOffset)
		{
			m_postingOffset = newOffset;
			CIndexInput* pDPInput = m_pInputDescriptor->getDPostingInput();
			pDPInput->seekInternal(newOffset);///not seek(), because seek() may trigger a large data read event.

			///read descriptor of posting list <PostingDescriptor>
			uint8_t buf[512];
			uint8_t* u = buf;
			pDPInput->readInternal((char*)buf,512,false);
			m_postingDesc.length = CCompressedPostingList::decodePosting64(u);	///<PostingLength(VInt64)>
			m_postingDesc.df = CCompressedPostingList::decodePosting32(u);		///<DF(VInt32)>
			m_postingDesc.ctf = CCompressedPostingList::decodePosting64(u);		///<CTF(VInt64)>
			m_postingDesc.poffset = CCompressedPostingList::decodePosting64(u);	///PositionPointer(VInt64)

			CCompressedPostingList::decodePosting32(u);///<ChunkCount(VInt32)>			
			///read first chunk descriptor of posting list <ChunkDescriptor>
			m_chunkDesc.length = CCompressedPostingList::decodePosting64(u);	///<ChunkLength(VInt64)>
			m_chunkDesc.lastdocid = CCompressedPostingList::decodePosting32(u);	///<LastDocID(VInt32)>						


			pDPInput->seek(newOffset - m_postingDesc.length);	///seek to the begin of posting data			

			m_ds.decodedDocCount = 0;
			m_ds.lastDecodedDocID = 0;
		}
		int32_t COnDiskPosting_DL0::decodeNext(int32_t* pPosting,int32_t length)
		{
			int32_t left = m_postingDesc.df - m_ds.decodedDocCount;
			if(left <= 0)
				return -1;
			int32_t* pDoc = pPosting;
			int32_t* pFreq = pPosting + (length>>1);

			if(length > left*2)
				length = left*2;			
			left = (length>>1);

			CIndexInput*	pDPostingInput = getInputDescriptor()->getDPostingInput();			

			count_t	df = m_postingDesc.df;
			int32_t count = 0;
			docid_t did = m_ds.lastDecodedDocID;			
			while (count < left)
			{				
				did += pDPostingInput->readVInt();
				if(!getFilter() || !getFilter()->test((size_t)did))///Is the document deleted?
				{
					*pDoc++ = did;
					*pFreq++ = 1;
				}				
				count++;
			}			
			FIRTEX_ASSERT((did<=m_chunkDesc.lastdocid),_T("document id is out of range."));

			///update state
			m_ds.decodedDocCount += count; 
			m_ds.lastDecodedDocID = did;

			return (int32_t)(pDoc - pPosting);
		}
		void COnDiskPosting_DL0::decodeNextPositions(int32_t* pPosting,int32_t length)
		{	
			FIRTEX_THROW3(UNSUPPORTED_ERROR,_T(""));
		}			
		void COnDiskPosting_DL0::decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs)
		{			
			FIRTEX_THROW3(UNSUPPORTED_ERROR,_T(""));
		}		

		void COnDiskPosting_DL0::reset()
		{
			reset(m_postingOffset);
		}

		CPosting* COnDiskPosting_DL0::clone()
		{			
			return NULL;
		}
		size_t COnDiskPosting_DL0::setBuffer(int32_t* buffer,size_t nBufSize)
		{
			size_t nBufUsed = nBufSize*sizeof(int32_t);
			if(nBufUsed <= 2*INDEXINPUT_BUFFSIZE)
			{
				m_nBufSize = 0;
				return 0;
			}

			CIndexInput* pDInput = getInputDescriptor()->getDPostingInput();			
			if((int64_t)nBufUsed > m_postingDesc.length)
			{
				nBufUsed = (size_t)m_postingDesc.length;
				pDInput->setBuffer((char*)buffer,(size_t)m_postingDesc.length);				
			}
			else
			{				
				pDInput->setBuffer((char*)buffer,nBufUsed);
			}			
			m_nBufSize = nBufUsed;			
			return (nBufUsed + sizeof(int32_t) - 1)/sizeof(int32_t);
		}
	}
}

