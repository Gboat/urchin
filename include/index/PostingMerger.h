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
#ifndef __POSTINGMERGER_H
#define __POSTINGMERGER_H

#include "Posting.h"
#include "index/OutputDescriptor.h"
#include "index/InputDescriptor.h"
#include "utility/BitVector.h"

using namespace firtex::utility;


namespace firtex
{
	namespace index
	{
		#define POSTINGMERGE_BUFFERSIZE	32768		
		///Posting merger
		class CPostingMerger
		{
		public:
			CPostingMerger();
			CPostingMerger(COutputDescriptor*	pOutputDescriptor);
			virtual~CPostingMerger();
		public:
			/** 
			 * set buffer for posting merging
			 * @param buf buffer
			 * @param bufSize size of buffer
			 */
			void		setBuffer(char* buf,size_t bufSize);			

			/**
			 * set output descriptor for merging
			 * @param pOutputDescriptor output descriptor
			 */
			void		setOutputDescriptor(COutputDescriptor*	pOutputDescriptor){m_pOutputDescriptor=pOutputDescriptor;}
		public:
			/**
			 * merge a in-memory posting
			 * @param baseDocID base document id
			 * @param pInMemoryPosting in-memory posting
			 */
			virtual void	mergeWith(docid_t baseDocID,CInMemoryPosting* pInMemoryPosting) = 0;

			/**
			 * merge a on-disk posting
			 * @param baseDocID base document id
			 * @param pOnDiskPosting on-disk posting
			 */
			virtual void	mergeWith(docid_t baseDocID,COnDiskPosting* pOnDiskPosting) = 0;

			/**
			 * end the merge,flush posting descriptor and chunk descriptor to disk. 
			 * @return offset of posting descriptor
			 */
			virtual	fileoffset_t	endMerge() = 0;

		public:
			/**
			 * reset descriptors value
			 */
			inline void	reset();

			/**
			 * there are some freed ids in the begin of posting lists
			 * @param pFilter deleted documents filter
			 */
			inline void addFreedID(CBitVector* pFilter);
		public:
			/**
			 * get posting descriptor
			 * @return posting descriptor,internal object
			 */
			PostingDescriptor&	getPostingDescriptor(){return m_postingDesc;}

			/**
			 * get chunk descriptor
			 * @return chunk descriptor,internal object
			 */
			ChunkDescriptor&	getChunkDescriptor(){return m_chunkDesc;}
		protected:
			/** create buffer for merging */
			void	createBuffer();
		protected:			
			char*				m_buffer;				///buffer for posting merging
			size_t				m_bufsize;				///size of buffer
			bool				m_bOwnBuffer;			///does we own the buffer?
			COutputDescriptor*	m_pOutputDescriptor;	///where merged data store

			docid_t				m_nIDFreed;

			PostingDescriptor	m_postingDesc;
			ChunkDescriptor		m_chunkDesc;
		};

		//////////////////////////////////////////////////////////////////////////
		///
		///WORD_LEVEL posting merger
		class CPostingMerger_WL : public CPostingMerger
		{
		public:
			CPostingMerger_WL();
			CPostingMerger_WL(COutputDescriptor*	pOutputDescriptor);
			virtual~CPostingMerger_WL();
		public:
			/**
			 * merge a in-memory posting
			 * @param baseDocID base document id
			 * @param pInMemoryPosting in-memory posting
			 */
			void	mergeWith(docid_t baseDocID,CInMemoryPosting* pInMemoryPosting);

			/**
			 * merge a on-disk posting
			 * @param baseDocID base document id
			 * @param pOnDiskPosting on-disk posting
			 */
			void	mergeWith(docid_t baseDocID,COnDiskPosting* pOnDiskPosting);

			/**
			 * end the merge,flush posting descriptor and chunk descriptor to disk. 
			 * @return offset of posting descriptor
			 */
			fileoffset_t	endMerge();
		protected:
			/**
			 * merge a in-memory posting
			 * @param baseDocID base document id
			 * @param pInMemoryPosting in-memory posting
			 */
			inline void	mergeWith_(docid_t baseDocID,CInMemoryPosting_WL* pInMemoryPosting);

			/**
			 * merge a on-disk posting
			 * @param baseDocID base document id
			 * @param pOnDiskPosting on-disk posting
			 */
			inline void	mergeWith_(docid_t baseDocID,COnDiskPosting_WL* pOnDiskPosting);

			/**
			 * merge a in-memory posting, filter deleted documents
			 * @param baseDocID base document id
			 * @param pInMemoryPosting in-memory posting
			 * @param pFilter filter of deleted documents
			 */
			inline void	mergeWith_GC(docid_t baseDocID,CInMemoryPosting_WL* pInMemoryPosting,CBitVector* pFilter);

			/**
			 * merge a on-disk posting, filter deleted documents
			 * @param baseDocID base document id
			 * @param pOnDiskPosting on-disk posting
			 * @param pFilter filter of deleted documents
			 */
			inline void	mergeWith_GC(docid_t baseDocID,COnDiskPosting_WL* pOnDiskPosting,CBitVector* pFilter);

			/**
			 * merge a on-disk position posting
			 * @param pPChunk chunk of position posting
			 * @param nPCount count of position 
			 * @param pPOutput output device
			 */
			inline void mergePosition(PostingChunk*& pPChunk,int32_t nPCount,CIndexOutput* pPOutput);
		protected:					
			int64_t		m_nPPostingLength;
			bool		m_bFirstPosting;			
		};

		//////////////////////////////////////////////////////////////////////////
		///inline function
		inline void CPostingMerger::reset()
		{
			///reset posting descriptor
			m_postingDesc.length = 0;
			m_postingDesc.ctf = 0;
			m_postingDesc.df = 0;
			m_postingDesc.poffset = -1;
			m_chunkDesc.lastdocid = 0;
			m_chunkDesc.length = 0;						
		}
		inline void CPostingMerger::addFreedID(CBitVector* pFilter)
		{
			if(pFilter)
				m_nIDFreed += (docid_t)pFilter->firstNotSet();
		}

		inline void	CPostingMerger_WL::mergeWith_(docid_t baseDocID,CInMemoryPosting_WL* pInMemoryPosting)
		{
			CIndexOutput* pDOutput = m_pOutputDescriptor->getDPostingOutput();
			CIndexOutput* pPOutput = m_pOutputDescriptor->getPPostingOutput();			
			fileoffset_t oldDOff = pDOutput->getFilePointer();
			
			if(m_bFirstPosting)///first posting
			{					
				reset();
				m_nPPostingLength = 0;
				m_bFirstPosting = false;
				///save position posting offset
				m_postingDesc.poffset = pPOutput->getFilePointer();
			}
			docid_t nBaseID = baseDocID - m_chunkDesc.lastdocid - m_nIDFreed;
			if(nBaseID == 0)
			{
				///write document posting 
				pInMemoryPosting->writeDPosting(pDOutput);
			}
			else 
			{						
				///write chunk data, update the first doc id
				PostingChunk* pDChunk = pInMemoryPosting->m_pDocFreqList->m_pHeadChunk;
				if(pDChunk)
				{
					uint8_t* bp = &(pDChunk->data[0]);
					docid_t firstDocID = CCompressedPostingList::decodePosting32(bp) + nBaseID;
					pDOutput->writeVInt(firstDocID);///write first doc id
					int32_t writeSize = pDChunk->size - (bp - &(pDChunk->data[0]));
					if(writeSize > 0)
						pDOutput->write((const char*)bp,writeSize);	
					pDChunk = pDChunk->next;
				}					
				///write rest data
				while(pDChunk)
				{				
					pDOutput->write((const char*)pDChunk->data,pDChunk->size);
					pDChunk = pDChunk->next;
				}									
			}						

			m_chunkDesc.length += (pDOutput->getFilePointer() - oldDOff);

			///write position posting				
			PostingChunk* pPChunk = pInMemoryPosting->m_pLocList->m_pHeadChunk;
			while(pPChunk)
			{				
				pPOutput->write((const char*)pPChunk->data,pPChunk->size);
				pPChunk = pPChunk->next;
			}

			///update descriptors
			m_postingDesc.ctf += pInMemoryPosting->m_nCTF;
			m_postingDesc.df += pInMemoryPosting->m_nDF;
			m_postingDesc.length = m_chunkDesc.length;
			m_chunkDesc.lastdocid = pInMemoryPosting->m_nLastDocID + baseDocID - m_nIDFreed;
		}
		inline void	CPostingMerger_WL::mergeWith_(docid_t baseDocID,COnDiskPosting_WL* pOnDiskPosting)
		{
			CIndexOutput*	pDOutput = m_pOutputDescriptor->getDPostingOutput();
			CIndexOutput*	pPOutput = m_pOutputDescriptor->getPPostingOutput();
			CIndexInput*	pDInput = pOnDiskPosting->getInputDescriptor()->getDPostingInput();
			CIndexInput*	pPInput = pOnDiskPosting->getInputDescriptor()->getPPostingInput();			
			
			fileoffset_t oldDOff = pDOutput->getFilePointer();			

			if(m_bFirstPosting)///first posting
			{
				reset();
				m_nPPostingLength = 0;
				m_bFirstPosting = false;
				
				///save position offset
				m_postingDesc.poffset = pPOutput->getFilePointer();
			}

			docid_t nBaseID = baseDocID - m_chunkDesc.lastdocid;				
			nBaseID -= m_nIDFreed;

			if(nBaseID == 0)					
			{
				///write document posting
				pDOutput->write(pDInput,pOnDiskPosting->m_postingDesc.length);
			}
			else 
			{
				docid_t firstDocID = pDInput->readVInt() + nBaseID;
				pDOutput->writeVInt(firstDocID);///write first doc id
				int64_t writeSize = pOnDiskPosting->m_postingDesc.length - pDOutput->getVIntLength(firstDocID - nBaseID);
				if(writeSize > 0)
					pDOutput->write(pDInput,writeSize);
			}				

			m_chunkDesc.length += (pDOutput->getFilePointer() - oldDOff);

			///write position posting
			pPOutput->write(pPInput,pOnDiskPosting->m_nPPostingLength);				

			///update descriptors
			m_postingDesc.ctf += pOnDiskPosting->m_postingDesc.ctf;
			m_postingDesc.df += pOnDiskPosting->m_postingDesc.df;
			m_postingDesc.length = m_chunkDesc.length; ///currently,it's only one chunk				
			m_chunkDesc.lastdocid = pOnDiskPosting->m_chunkDesc.lastdocid + baseDocID - m_nIDFreed;
		}
		inline void	CPostingMerger_WL::mergeWith_GC(docid_t baseDocID,CInMemoryPosting_WL* pInMemoryPosting,CBitVector* pFilter)
		{
#define IS_CHUNKOVER(pChunk,pDataBeg,pDataEnd)\
	if(pDataBeg >= pDataEnd)\
	{\
		pChunk = pChunk->next;\
		if(pChunk)\
		{\
			pDataBeg = &(pChunk->data[0]);\
			pDataEnd = &(pChunk->data[pChunk->size - 1]);\
		}\
    }
			CIndexOutput* pDOutput = m_pOutputDescriptor->getDPostingOutput();
			CIndexOutput* pPOutput = m_pOutputDescriptor->getPPostingOutput();			
			uint8_t* pDChunkData = NULL;
			uint8_t* pDChunkDataEnd = NULL;
			uint8_t* pPChunkData = NULL;
			uint8_t* pPChunkDataEnd = NULL;
			docid_t nDocID = 0;
			docid_t nDocIDPrev = 0;
			docid_t nLastDocID = 0;
			freq_t	nTF = 0;
			count_t	nCTF = 0;
			count_t nDF = 0;
			count_t nPCount = 0;
			fileoffset_t oldDOff = pDOutput->getFilePointer();			

			PostingChunk* pPChunk = pInMemoryPosting->m_pLocList->m_pHeadChunk;
			if(!pPChunk)
				return;
			pPChunkData = &(pPChunk->data[0]);
			pPChunkDataEnd = &(pPChunk->data[pPChunk->size - 1]);
			PostingChunk* pDChunk = pInMemoryPosting->m_pDocFreqList->m_pHeadChunk;
			pDChunkData = &(pDChunk->data[0]);
			pDChunkDataEnd = &(pDChunk->data[pDChunk->size - 1]);
			

			if(m_bFirstPosting)///first posting
			{					
				reset();
				m_nPPostingLength = 0;
				m_bFirstPosting = false;

				///save position posting offset
				m_postingDesc.poffset = pPOutput->getFilePointer();
			}
			docid_t nBaseID = baseDocID - m_chunkDesc.lastdocid;
			m_nIDFreed += (docid_t)pFilter->firstNotSet();
			nBaseID -= m_nIDFreed;
			if(nBaseID != 0)
			{
				nDocID += CCompressedPostingList::decodePosting32(pDChunkData);
				IS_CHUNKOVER(pDChunk,pDChunkData,pDChunkDataEnd);
				///get the first not deleted document
				while(pDChunk && (pFilter->test((size_t)nDocID)))
				{						
					nTF = CCompressedPostingList::decodePosting32(pDChunkData);///skip term freq
					IS_CHUNKOVER(pDChunk,pDChunkData,pDChunkDataEnd);
					///skip positions of deleted documents
					while (nTF > 0)
					{	
						CCompressedPostingList::decodePosting32(pPChunkData);
						IS_CHUNKOVER(pPChunk,pPChunkData,pPChunkDataEnd);
						nTF--;						
					}
					nDocID += CCompressedPostingList::decodePosting32(pDChunkData);
					IS_CHUNKOVER(pDChunk,pDChunkData,pDChunkDataEnd);					
				}
				if(!pDChunk)
					return ;
				nDocIDPrev = nDocID; 

				nTF = CCompressedPostingList::decodePosting32(pDChunkData);
				IS_CHUNKOVER(pDChunk,pDChunkData,pDChunkDataEnd);
				pDOutput->writeVInt(nDocID + nBaseID);///write first doc id								
				pDOutput->writeVInt(nTF);///write term freq of first document
				nPCount += nTF;
				nDF++;
				nLastDocID = nDocID;				
			}

			while (pDChunk)
			{		
				nDocID += CCompressedPostingList::decodePosting32(pDChunkData);
				IS_CHUNKOVER(pDChunk,pDChunkData,pDChunkDataEnd);
				nTF = CCompressedPostingList::decodePosting32(pDChunkData);
				IS_CHUNKOVER(pDChunk,pDChunkData,pDChunkDataEnd);
			
				if(!pFilter->test((size_t)nDocID))///the document has not been deleted
				{
					pDOutput->writeVInt(nDocID - nDocIDPrev);	///compress and write doc id
					pDOutput->writeVInt(nTF);			///compress and write term freq
					nDocIDPrev = nDocID;
					nPCount += nTF;
					nDF++;
					nLastDocID = nDocID;
				}
				else ///this document has been deleted
				{
					uint8_t* pPChunkDataBegin = NULL;
					nCTF += nPCount;
					///write positions of documents
					while (nPCount > 0)
					{	
						IS_CHUNKOVER(pPChunk,pPChunkData,pPChunkDataEnd);
						pPChunkDataBegin = pPChunkData;
						while ((nPCount > 0) && (pPChunkData < pPChunkDataEnd))
						{
							CCompressedPostingList::decodePosting32(pPChunkData);
							nPCount--;
						}
						if( (pPChunkData - pPChunkDataBegin) > 0)
						{
							pPOutput->write((const char*)pPChunkDataBegin,pPChunkDataBegin - pPChunkData);
						}
					}

					///skip positions of deleted documents
					while (nTF > 0)
					{	
						CCompressedPostingList::decodePosting32(pPChunkData);
						IS_CHUNKOVER(pPChunk,pPChunkData,pPChunkDataEnd);
                        nTF--;						
					}					
				}
				//pDChunk = pDChunk->next;///next chunk
				IS_CHUNKOVER(pDChunk,pDChunkData,pDChunkDataEnd);
			}
			if(nPCount > 0)
			{
				uint8_t* pPChunkDataBegin = NULL;
				nCTF += nPCount;
				///write positions of documents
				while (nPCount > 0)
				{	
					IS_CHUNKOVER(pPChunk,pPChunkData,pPChunkDataEnd);
					pPChunkDataBegin = pPChunkData;
					while ((nPCount > 0) && (pPChunkData < pPChunkDataEnd))
					{
						CCompressedPostingList::decodePosting32(pPChunkData);
						nPCount--;
					}
					if( (pPChunkData - pPChunkDataBegin) > 0)
					{
						pPOutput->write((const char*)pPChunkDataBegin,pPChunkDataBegin - pPChunkData);
					}
				}
			}
			
			m_chunkDesc.length += (pDOutput->getFilePointer() - oldDOff);
			

			///update descriptors
			m_postingDesc.ctf += nCTF;
			m_postingDesc.df += nDF;
			m_postingDesc.length = m_chunkDesc.length;
			m_chunkDesc.lastdocid = nLastDocID + baseDocID - m_nIDFreed;
		}
	
		inline void	CPostingMerger_WL::mergeWith_GC(docid_t baseDocID,COnDiskPosting_WL* pOnDiskPosting,CBitVector* pFilter)
		{
			CIndexOutput*	pDOutput = m_pOutputDescriptor->getDPostingOutput();
			CIndexOutput*	pPOutput = m_pOutputDescriptor->getPPostingOutput();
			CIndexInput*	pDInput = pOnDiskPosting->getInputDescriptor()->getDPostingInput();
			CIndexInput*	pPInput = pOnDiskPosting->getInputDescriptor()->getPPostingInput();			

			docid_t nDocID = 0;
			docid_t nDocIDPrev = 0;
			docid_t nLastDocID = 0;
			freq_t	nTF = 0;
			count_t	nCTF = 0;
			count_t nDF = 0;
			count_t nPCount = 0;
			count_t nODDF = pOnDiskPosting->m_postingDesc.df;
			if(nODDF <= 0)
				return;

			fileoffset_t oldDOff = pDOutput->getFilePointer();
			if(m_bFirstPosting)///first posting
			{
				reset();
				m_nPPostingLength = 0;
				m_bFirstPosting = false;

				///save position offset
				m_postingDesc.poffset = pPOutput->getFilePointer();
			}

			docid_t nBaseID = baseDocID - m_chunkDesc.lastdocid;
			m_nIDFreed += (docid_t)pFilter->firstNotSet();
			nBaseID -= m_nIDFreed;

			if(nBaseID != 0)					
			{
				///process first document in the posting
				nDocID += pDInput->readVInt();
				while( (nODDF > 0) && (pFilter->test((size_t)nDocID)))
				{					
					nTF = pDInput->readVInt();///skip term freq
					///skip position
					while (nTF > 0)
					{		
						pPInput->readVInt();
						nTF--;							
					}

					nDocID += pDInput->readVInt();
					nODDF--;
				}
				if(nODDF <= 0)///No documents
				{					
					return;
				}

				nDocIDPrev = nDocID;

				pDOutput->writeVInt(nDocID + nBaseID);
				nTF = pDInput->readVInt();	///read term freq
				pDOutput->writeVInt(nTF);
				nPCount += nTF;

				nDF++;
				nLastDocID = nDocID;
				nODDF--;
			}
			while (nODDF > 0)
			{
				nDocID += pDInput->readVInt();
				nTF = pDInput->readVInt();
				if(!pFilter->test((size_t)nDocID))///the document has not been deleted
				{
					pDOutput->writeVInt(nDocID - nDocIDPrev);
					pDOutput->writeVInt(nTF);
					nDocIDPrev = nDocID;
					nPCount += nTF;
					nDF++;
					nLastDocID = nDocID;
				}
				else ///this document has been deleted
				{					
					nCTF += nPCount;
					///write positions of documents
					while (nPCount > 0)
					{							
						pPOutput->writeVInt(pPInput->readVInt());
						nPCount--;
					}
					///skip positions of deleted documents
					while (nTF > 0)
					{		
						pPInput->readVInt();
						nTF--;							
					}
				}
				nODDF--;
			}	
			if(nPCount > 0)
			{
				nCTF += nPCount;
				while (nPCount > 0)
				{							
					pPOutput->writeVInt(pPInput->readVInt());
					nPCount--;
				}
			}			

			m_chunkDesc.length += (pDOutput->getFilePointer() - oldDOff);

			///update descriptors
			m_postingDesc.ctf += nCTF;
			m_postingDesc.df += nDF;
			m_postingDesc.length = m_chunkDesc.length; ///currently,it's only one chunk				
			//m_chunkDesc.lastdocid = nLastDocID + baseDocID;
			m_chunkDesc.lastdocid = nLastDocID + baseDocID - m_nIDFreed;
		}
	}
}

#endif
