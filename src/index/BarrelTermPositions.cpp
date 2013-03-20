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
// Created	: 2006/1/6
//
#include "index/BarrelTermPositions.h"
#include "index/InputDescriptor.h"

namespace firtex
{
	namespace index
	{
		CBarrelTermPositions::CBarrelTermPositions(void)
			:m_pPPostingBuffer(NULL)
			,m_nPBufferSize(0)
			,m_nTotalDecodedPCountWithinDoc(0)
			,m_nCurDecodedPCountWithinDoc(0)
			,m_nCurrentPPostingWithinDoc(0)
			,m_bOwnPPostingBuffer(false)
			,m_nPPostingCountWithinDoc(0)
			,m_nTotalDecodedPCount(0)
			,m_nCurrentPPosting(0)
			,m_nLastUnDecodedPCount(0)
			,m_pPPostingBufferWithinDoc(NULL)
			,m_nLastPosting(-1)
		{			
		}

		CBarrelTermPositions::CBarrelTermPositions(CTermReader* pReader,CInputDescriptor* pInputDescriptor,CTermInfo& ti,CBitVector* deletedDocs)
			:CBarrelTermDocFreqs(pReader,pInputDescriptor,ti,deletedDocs)			
			,m_nTotalDecodedPCountWithinDoc(0)
			,m_nCurDecodedPCountWithinDoc(0)
			,m_nCurrentPPostingWithinDoc(0)
			,m_pPPostingBuffer(NULL)
			,m_nPBufferSize(0)
			,m_bOwnPPostingBuffer(false)
			,m_nPPostingCountWithinDoc(0)
			,m_nTotalDecodedPCount(0)
			,m_nCurrentPPosting(0)
			,m_nLastUnDecodedPCount(0)
			,m_pPPostingBufferWithinDoc(NULL)
			,m_nLastPosting(-1)
		{				
		}
		CBarrelTermPositions::CBarrelTermPositions(CTermReader* pReader,CPosting* pPosting,CTermInfo& ti,CBitVector* deletedDocs)
			:CBarrelTermDocFreqs(pReader,pPosting,ti,deletedDocs)
			,m_nTotalDecodedPCountWithinDoc(0)
			,m_nCurDecodedPCountWithinDoc(0)
			,m_nCurrentPPostingWithinDoc(0)
			,m_pPPostingBuffer(NULL)
			,m_nPBufferSize(0)
			,m_bOwnPPostingBuffer(false)
			,m_nPPostingCountWithinDoc(0)
			,m_nTotalDecodedPCount(0)
			,m_nCurrentPPosting(0)
			,m_nLastUnDecodedPCount(0)
			,m_pPPostingBufferWithinDoc(NULL)
			,m_nLastPosting(-1)
		{			
		}		
		CBarrelTermPositions::~CBarrelTermPositions(void)
		{			
			CBarrelTermDocFreqs::close();
			if(m_bOwnPPostingBuffer)
			{
				delete m_pPPostingBuffer;
				m_pPPostingBuffer = NULL;
				m_nPBufferSize = 0;
			}
			m_pPPostingBufferWithinDoc = NULL;
		}
		
		bool CBarrelTermPositions::seek(CTerm* term)
		{
			return false;
		}		
		
		docid_t CBarrelTermPositions::doc()
		{
			return CBarrelTermDocFreqs::doc();
		}
		count_t CBarrelTermPositions::freq()
		{
			return CBarrelTermDocFreqs::freq();
		}
		bool CBarrelTermPositions::next()
		{
			///skip the pre doc's positions				
			//skipPositions(m_nPPostingCountWithinDoc - m_nCurrentPPostingWithinDoc);
			skipPositions(m_nPPostingCountWithinDoc - m_nTotalDecodedPCountWithinDoc);

			//调用CBarrelTermDocFreqs的next
			if(CBarrelTermDocFreqs::next())
			{
				m_nPPostingCountWithinDoc = CBarrelTermDocFreqs::freq();				
				m_nTotalDecodedPCountWithinDoc = 0;
				m_nCurDecodedPCountWithinDoc = 0;
				m_nCurrentPPostingWithinDoc = 0;
				m_pPosting->resetPosition();
				return true;
			}
			return false;
		}

		count_t	CBarrelTermPositions::next(docid_t*& docs, count_t*& freqs)
		{
			/*不支持一次读取多个文档编号，请使用CBarrelTermDocFreqs*/
			throw CUnsupportedOperationException("don't support CBarrelTermPositions::next(docid_t*& docs, count_t*& freqs),please use CBarrelTermDocFreqs.");
		}

		bool CBarrelTermPositions::skipTo(docid_t target,docid_t& nearTarget)
		{		
			int32_t nStart,nEnd,nSkip;
			int32_t i = 0;
			bool bret = false;
			while (true)
			{
				if((m_nCurrentPosting< 0) || (m_nCurrentPosting >= m_nCurDecodedCount) )
				{
					if(!decode())//解压
						return false;					
				}
				nStart = m_nCurrentPosting;				
				nEnd = nStart + (target - m_pPostingBuffer[m_nCurrentPosting]);
				if(nEnd == nStart)
				{
					nearTarget = target;
					bret = true;
					goto end;
				}
				else if(nEnd < nStart)
				{
					nearTarget = m_pPostingBuffer[m_nCurrentPosting];
					bret = false;					
					goto end;
				}				
				if ( nEnd > (m_nCurDecodedCount - 1) )
					nEnd = m_nCurDecodedCount - 1;				
				m_nCurrentPosting = bsearch(m_pPostingBuffer,nStart,nEnd,target,nearTarget);//二分查找
				//跳过文档位置信息
				nSkip = (m_pPostingBuffer[m_nFreqStart + nStart] - m_nTotalDecodedPCountWithinDoc);
				for (i = nStart + 1;i < m_nCurrentPosting;i++)
				{
					nSkip += m_pPostingBuffer[m_nFreqStart + i];
				}
				/*if(nSkip > 0)
					m_pPosting->decodeNextPositions(NULL,nSkip);*/
				skipPositions(nSkip);

				if(target == nearTarget)//查找成功
				{
					bret = true;
					goto end;
				}
				if(nearTarget > target)//查找失败
				{
					bret = false;
					goto end;
				}
				/*if(nStart != m_nCurrentPosting)
					m_pPosting->decodeNextPositions(NULL,m_pPostingBuffer[m_nFreqStart + m_nCurrentPosting]);*/
				if(nStart != m_nCurrentPosting)
					skipPositions(m_pPostingBuffer[m_nFreqStart + m_nCurrentPosting]);
				m_nTotalDecodedPCountWithinDoc = 0;
				m_nCurDecodedPCountWithinDoc = 0;
				m_nCurrentPPostingWithinDoc = 0;
				m_nCurrentPosting = m_nCurDecodedCount;						
			}			
end:			
			m_nPPostingCountWithinDoc = m_pPostingBuffer[m_nFreqStart + m_nCurrentPosting];
			m_nCurDecodedPCountWithinDoc = 0;
			m_nTotalDecodedPCountWithinDoc = 0;			
			m_nCurrentPPostingWithinDoc = 0;
			m_pPosting->resetPosition();
			return bret;		
		}

		bool CBarrelTermPositions::skipToPosition(loc_t target,loc_t& nearTarget)
		{
			int start,end;	
			int i = 0;
			while (true)
			{
				if(m_nCurrentPPostingWithinDoc >= m_nCurDecodedPCountWithinDoc )
				{
					if(!decodePositions())
					{
						nearTarget = m_pPPostingBufferWithinDoc[m_nCurrentPPostingWithinDoc - 1];//TODO
						return false;
					}
				}				

				start = m_nCurrentPPostingWithinDoc;				
				end = start + (target - m_pPPostingBufferWithinDoc[m_nCurrentPPostingWithinDoc]);
				if(end < start)
				{
					nearTarget = m_pPPostingBufferWithinDoc[m_nCurrentPPostingWithinDoc];
					return false;
				}
				else if(end == start)
				{
					nearTarget = target;
					return true;
				}
				if ( end > (m_nCurDecodedPCountWithinDoc - 1) )
					end = m_nCurDecodedPCountWithinDoc - 1;
				m_nCurrentPPostingWithinDoc = bsearch(m_pPPostingBufferWithinDoc,start,end,target,nearTarget);//二分查找
				if(target == nearTarget)//查找成功
				{					
					return true;
				}
				if(nearTarget > target)//查找失败
					return false;
				m_nCurrentPPostingWithinDoc = m_nCurDecodedPCountWithinDoc;//继续下一页查找
			}			
			return false;						
		}

		void CBarrelTermPositions::close()
		{
			CBarrelTermDocFreqs::close();
		}

		loc_t CBarrelTermPositions::nextPosition()
		{
			if(m_nCurrentPPostingWithinDoc >= m_nCurDecodedPCountWithinDoc)
			{
				if(!decodePositions())
					return -1;
				return m_pPPostingBufferWithinDoc[m_nCurrentPPostingWithinDoc++];
			}

			return m_pPPostingBufferWithinDoc[m_nCurrentPPostingWithinDoc++];
		}

		count_t	CBarrelTermPositions::nextPositions(loc_t*& positions)
		{
			if(m_nCurrentPPostingWithinDoc >= m_nCurDecodedPCountWithinDoc)
			{
				if(!decodePositions())
					return -1;				
			}
			positions = m_pPPostingBufferWithinDoc;
			return m_nCurDecodedPCountWithinDoc;
		}	

		size_t CBarrelTermPositions::setBuffer(int32_t* buffer,size_t length)
		{
			//size_t bufUsed = CBarrelTermDocFreqs::setBuffer(buffer,length/5);
			size_t bufUsed = CBarrelTermDocFreqs::setBuffer(buffer,length);			
			size_t nLeft = length - bufUsed;
			if(m_bOwnPPostingBuffer)
			{
				delete m_pPPostingBuffer;
				m_pPPostingBuffer = NULL;
				m_nPBufferSize = 0;
				m_bOwnPPostingBuffer = false;
			}
			if(!buffer || (nLeft < CBarrelTermPositions::DEFAULT_BUFFERSIZE))
			{
				createBuffer();
				return bufUsed;
			}

			m_pPPostingBuffer = buffer + bufUsed;
			m_nPBufferSize = nLeft;			
			if((int64_t)m_nPBufferSize > getCTF())
				m_nPBufferSize = (size_t)getCTF();			
			return (bufUsed + m_nPBufferSize);
		}
		size_t CBarrelTermPositions::getBufferSize()
		{
			if(m_bOwnPPostingBuffer)
				return CBarrelTermDocFreqs::getBufferSize();
			return CBarrelTermDocFreqs::getBufferSize() + m_nPBufferSize;
		}

		void CBarrelTermPositions::createBuffer()
		{
			size_t bufSize = CBarrelTermPositions::DEFAULT_BUFFERSIZE;
			if((int64_t)bufSize > getCTF())
				bufSize = (size_t)getCTF();
			if(m_pPPostingBuffer == NULL)
			{
				m_nPBufferSize = bufSize;				
				m_pPPostingBuffer = new int32_t[bufSize];
				m_bOwnPPostingBuffer = true;				
			}
		}
	}
}
