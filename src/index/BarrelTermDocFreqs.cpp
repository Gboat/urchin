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
// Created	: 2005/12/26
//

#include "index/BarrelTermDocFreqs.h"
#include "index/InputDescriptor.h"
#include "index/InMemoryTermReader.h"

namespace firtex
{
	namespace index
	{
		CBarrelTermDocFreqs::CBarrelTermDocFreqs(void)
			:m_pPosting(NULL)
            ,m_pPostingBuffer(0)
			,m_nBufferSize(0)
			,m_nFreqStart(0)
			,m_nTotalDecodedCount(0)
			,m_nCurDecodedCount(0)
			,m_nCurrentPosting(-1)
			,m_bOwnBuffer(false)
			,m_pTermReader(NULL)
			,m_pDeletedDocs(NULL)
		{		
		}

		CBarrelTermDocFreqs::CBarrelTermDocFreqs(CTermReader* pReader,CInputDescriptor* pInputDescriptor,CTermInfo& ti,CBitVector* deletedDocs)
			:m_nTotalDecodedCount(0)
			,m_nCurDecodedCount(0)
			,m_nCurrentPosting(-1)			
			,m_pPostingBuffer(NULL)
			,m_nBufferSize(0)
			,m_nFreqStart(0)
			,m_bOwnBuffer(false)
			,m_pTermReader(pReader)
			,m_pDeletedDocs(deletedDocs)
		{
			m_termInfo.set(ti.docFreq(),ti.docPointer());
			CIndexInput* pDInput = pInputDescriptor->getDPostingInput();
			CIndexInput* pPInput = pInputDescriptor->getPPostingInput();
			m_pInputDescriptor = new CInputDescriptor(NULL,pDInput->clone(),pPInput?pPInput->clone():NULL,true);
			m_pInputDescriptor->getDPostingInput()->seek(ti.docPointer());
			if(pReader->getFieldInfo()->getIndexLevel() == WORD_LEVEL)
				m_pPosting = new COnDiskPosting_WL(m_pInputDescriptor,ti.docPointer());
			else if(pReader->getFieldInfo()->getIndexLevel() == DOCUMENT_LEVEL)
				m_pPosting = new COnDiskPosting_DL(m_pInputDescriptor);
			//else 
			//	m_pPosting = new COnDiskPosting_DL0(m_pInputDescriptor);
			m_pPosting->setFilter(pReader->getFilter());			
		}
		CBarrelTermDocFreqs::CBarrelTermDocFreqs(CTermReader* pReader,CPosting* pPosting,CTermInfo& ti,CBitVector* deletedDocs)
			:m_nTotalDecodedCount(0)
			,m_nCurDecodedCount(0)
			,m_nCurrentPosting(-1)
			,m_pPosting(pPosting->clone())			
			,m_pInputDescriptor(NULL)
			,m_pPostingBuffer(NULL)
			,m_nBufferSize(0)
			,m_nFreqStart(0)
			,m_bOwnBuffer(false)
			,m_pTermReader(pReader)
			,m_pDeletedDocs(deletedDocs)
		{			
			m_termInfo.set(ti.docFreq(),ti.docPointer());
			m_pPosting->setFilter(pReader->getFilter());
		}		

		CBarrelTermDocFreqs::~CBarrelTermDocFreqs(void)
		{
			close();
			if(m_bOwnBuffer)
			{
				if(m_pPostingBuffer)
				{
					delete[] m_pPostingBuffer;
					m_pPostingBuffer = NULL;
				}
				m_nBufferSize = 0;
			}
		}
		
		bool  CBarrelTermDocFreqs::seek(CTerm* term)
		{
			if(m_pTermReader == NULL)
				return false;
			///重置
			m_nTotalDecodedCount = 0;
			m_nCurDecodedCount = 0;
			m_nCurrentPosting = -1;	
			m_nFreqStart = 0;			

			CTermInfo* pTI = m_pTermReader->termInfo(term);
			if(pTI)
			{
				m_termInfo.set(pTI->docFreq(),pTI->docPointer());
				if(m_pInputDescriptor)
				{
					if(m_pTermReader->getFieldInfo()->getIndexLevel() == WORD_LEVEL)
						((COnDiskPosting_WL*)m_pPosting)->reset(pTI->docPointer());
					else if(m_pTermReader->getFieldInfo()->getIndexLevel() == DOCUMENT_LEVEL)
						((COnDiskPosting_DL*)m_pPosting)->reset(pTI->docPointer());
					//else 
					//  ((COnDiskPosting_DL0*)m_pPosting)->reset(pTI->docPointer());
				}
				else
				{
					if(m_pPosting)
						delete m_pPosting;
					m_pPosting = ((CInMemoryTermReader*)m_pTermReader)->inMemoryPosting()->clone();
				}
				return true;
			}
			else m_termInfo.set(0,0);

			return false;
		}
		freq_t	CBarrelTermDocFreqs::docFreq()
		{
			return m_termInfo.docFreq();
		}

		int64_t CBarrelTermDocFreqs::getCTF()
		{
			return m_pPosting->getCTF();
		}

		docid_t CBarrelTermDocFreqs::doc()
		{
			return m_pPostingBuffer[m_nCurrentPosting];
		}

		count_t CBarrelTermDocFreqs::freq()
		{
			return m_pPostingBuffer[m_nFreqStart + m_nCurrentPosting];
		}

		size_t CBarrelTermDocFreqs::setBuffer(int32_t* buffer,size_t length)
		{
			if(m_bOwnBuffer && m_pPostingBuffer)
			{
				delete[] m_pPostingBuffer;				
				m_pPostingBuffer = NULL;
			}
			if(buffer == NULL)
			{
				createBuffer();
				return 0;
			}	
			m_pPostingBuffer = buffer;

			size_t bufUsed = length;
			if( (size_t)(m_termInfo.docFreq()*2) < bufUsed)
				bufUsed = m_termInfo.docFreq()*2;
			else 
				bufUsed = bufUsed - bufUsed%2;
			m_nBufferSize = bufUsed;
			bufUsed += m_pPosting->setBuffer(buffer + bufUsed,length - bufUsed);
			m_nFreqStart = m_nBufferSize/2;
			m_bOwnBuffer = false;
			return bufUsed;
		}
		size_t CBarrelTermDocFreqs::getBufferSize()
		{
			if(m_bOwnBuffer)
				return m_pPosting->getBufferSize();
			return m_nBufferSize + m_pPosting->getBufferSize();
		}

		bool CBarrelTermDocFreqs::next()
		{
			m_nCurrentPosting ++;
			if(m_nCurrentPosting >= m_nCurDecodedCount)
			{
				if(!decode())
					return false;
				return true;
			}			
			return true;			
		}
		count_t CBarrelTermDocFreqs::next(docid_t*& docs, count_t*& freqs)
		{
			if((m_nCurrentPosting < 0) || (m_nCurrentPosting >= m_nCurDecodedCount) )
			{
				if(!decode())
					return false;
			}
			docs = &m_pPostingBuffer[m_nCurrentPosting];
			freqs = &m_pPostingBuffer[m_nFreqStart + m_nCurrentPosting];
			int32_t c = m_nCurDecodedCount - m_nCurrentPosting;
			m_nCurDecodedCount = m_nCurrentPosting = 0;
			return c;
		}

		bool CBarrelTermDocFreqs::skipTo(docid_t target,docid_t& nearTarget)
		{				
			int32_t start,end;				
			while (true)
			{
				if((m_nCurrentPosting < 0) || (m_nCurrentPosting >= m_nCurDecodedCount) )
				{
					if(!decode())
						return false;
				}
				start = m_nCurrentPosting;
				end = start + (target - m_pPostingBuffer[m_nCurrentPosting]);
				if(end == start)
				{
					nearTarget = target;
					return true;
				}
				else if(end < start)
				{
					nearTarget = m_pPostingBuffer[m_nCurrentPosting];
					return false;					
				}				
				if ( end > (m_nCurDecodedCount - 1) )
					end = m_nCurDecodedCount - 1;				
				m_nCurrentPosting = bsearch(m_pPostingBuffer,start,end,target,nearTarget);//二分查找
				if(target == nearTarget)//查找成功
				{					
					return true;
				}
				if(nearTarget > target)//查找失败
					return false;
				m_nCurrentPosting = m_nCurDecodedCount;//继续下一页查找
			}			
			return false;
		}	

		void CBarrelTermDocFreqs::close()
		{
			if(m_pInputDescriptor)
			{				
				delete m_pInputDescriptor;
				m_pInputDescriptor = NULL;
				m_pTermReader = NULL;
			}
			if(m_pPosting)
			{
				delete m_pPosting;
				m_pPosting = NULL;
			}		
		}
		bool CBarrelTermDocFreqs::decode()
		{
			count_t df = m_termInfo.docFreq();
			if(m_nTotalDecodedCount == df)
			{				
				return false;
			}			

			if(!m_pPostingBuffer)
				createBuffer();

			m_nCurrentPosting = 0;
			m_nCurDecodedCount = m_pPosting->decodeNext(m_pPostingBuffer,m_nBufferSize);
			if(m_nCurDecodedCount <= 0)
				return false;
			m_nTotalDecodedCount += m_nCurDecodedCount;
			
			return true;
		}
		void CBarrelTermDocFreqs::createBuffer()
		{
			size_t bufSize = CBarrelTermDocFreqs::DEFAULT_BUFFERSIZE;
			/*if( (size_t)(m_termInfo.docFreq()*2) < bufSize)
				bufSize = m_termInfo.docFreq()*2;*/
			if(m_pPostingBuffer == NULL)
			{
				m_nBufferSize = bufSize;
				m_nFreqStart = m_nBufferSize/2;
				m_pPostingBuffer = new int32_t[m_nBufferSize];
				m_bOwnBuffer = true;				
			}
		}
	}
}


