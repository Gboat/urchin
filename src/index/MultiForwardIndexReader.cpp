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
// Created	: 2006/7/2
//
#include "index/MultiForwardIndexReader.h"

namespace firtex
{
	namespace index
	{
		CMultiForwardIndexReader::CMultiForwardIndexReader(int numReaders) : m_numReaders(numReaders)
		{
			m_ppReaders = new CForwardIndexReaderEntry[numReaders];
			memset(m_ppReaders,0,numReaders*sizeof(CForwardIndexReaderEntry));
		}

		CMultiForwardIndexReader::~CMultiForwardIndexReader(void)
		{
			if(m_ppReaders)
			{				
				delete[] m_ppReaders;
				m_ppReaders = NULL;
			}
		}

		forwardindex_t	CMultiForwardIndexReader::getForwardIndexType(const tchar* field)
		{
			CForwardIndexReaderEntry* pEntry = NULL;
			if(m_numReaders <= 0)
				return ForwardIndex::NO;
			return m_ppReaders[0].reader->getForwardIndexType(field);
		}

		CTermFreqVector* CMultiForwardIndexReader::getFreqVector(docid_t did,const tchar* field)
		{
			CForwardIndexReaderEntry* pEntry = NULL;
			for(int i = 0;i<m_numReaders;i++)
			{
				pEntry = &(m_ppReaders[i]);
				if( (did >= pEntry->docStart) && (did < pEntry->docStart + pEntry->numDocs))
					return pEntry->reader->getFreqVector(did - pEntry->docStart,field);
			}
			return NULL;
		}
			
		CForwardIndexArray<CTermFreqVector*>* CMultiForwardIndexReader::getFreqVectors(docid_t did)
		{
			CForwardIndexReaderEntry* pEntry = NULL;
			for(int i = 0;i<m_numReaders;i++)
			{
				pEntry = &(m_ppReaders[i]);
				if( (did >= pEntry->docStart) && (did < pEntry->docStart + pEntry->numDocs))
					return pEntry->reader->getFreqVectors(did - pEntry->docStart);
			}
			return NULL;
		}
		CTermPositionVector* CMultiForwardIndexReader::getPositionVector(docid_t did,const tchar* field)
		{
			CForwardIndexReaderEntry* pEntry = NULL;
			for(int i = 0;i<m_numReaders;i++)
			{
				pEntry = &(m_ppReaders[i]);
				if( (did >= pEntry->docStart) && (did < pEntry->docStart + pEntry->numDocs))
					return pEntry->reader->getPositionVector(did - pEntry->docStart,field);
			}
			return NULL;
		}

		CForwardIndexArray<CTermPositionVector*>* CMultiForwardIndexReader::getPositionVectors(docid_t did)
		{
			CForwardIndexReaderEntry* pEntry = NULL;
			for(int i = 0;i<m_numReaders;i++)
			{
				pEntry = &(m_ppReaders[i]);
				if( (did >= pEntry->docStart) && (did < pEntry->docStart + pEntry->numDocs))
					return pEntry->reader->getPositionVectors(did - pEntry->docStart);
			}
			return NULL;
		}

		CTermSequenceVector* CMultiForwardIndexReader::getSquenceVector(docid_t did,const tchar* field)
		{
			CForwardIndexReaderEntry* pEntry = NULL;
			for(int i = 0;i<m_numReaders;i++)
			{
				pEntry = &(m_ppReaders[i]);
				if( (did >= pEntry->docStart) && (did < pEntry->docStart + pEntry->numDocs))
					return pEntry->reader->getSquenceVector(did - pEntry->docStart,field);
			}
			return NULL;
		}			
		CForwardIndexArray<CTermSequenceVector*>*	CMultiForwardIndexReader::getSquenceVectors(docid_t did)
		{
			CForwardIndexReaderEntry* pEntry = NULL;
			for(int i = 0;i<m_numReaders;i++)
			{
				pEntry = &(m_ppReaders[i]);
				if( (did >= pEntry->docStart) && (did < pEntry->docStart + pEntry->numDocs))
					return pEntry->reader->getSquenceVectors(did - pEntry->docStart);
			}
			return NULL;
		}
		void CMultiForwardIndexReader::addVectorReader(int order,docid_t docStart,count_t numDocs,const CForwardIndexReader* pReader)
		{
			m_ppReaders[order].docStart = docStart;
			m_ppReaders[order].numDocs = numDocs;
			m_ppReaders[order].reader = (CForwardIndexReader*)pReader;
		}
	}
}
