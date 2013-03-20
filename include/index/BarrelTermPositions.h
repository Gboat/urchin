//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/1/6
//

#ifndef _BARRELTERMPOSITIONS_H
#define _BARRELTERMPOSITIONS_H

#include "TermPositions.h"
#include "BarrelTermDocFreqs.h"

namespace firtex
{
	namespace index
	{
		class CBarrelTermPositions : public CBarrelTermDocFreqs,public CTermPositions
		{
		public:
			CBarrelTermPositions(void);
			CBarrelTermPositions(CTermReader* pReader,CInputDescriptor* pInputDescriptor,CTermInfo& ti,CBitVector* deletedDocs);
			CBarrelTermPositions(CTermReader* pReader,CPosting* pPosting,CTermInfo& ti,CBitVector* deletedDocs);

			virtual ~CBarrelTermPositions(void);
		public:
			/**
			 * ��λ��ָ��term
			 */
			bool	seek(CTerm* term);

			/**
			* ���ص�ǰ�ĵ���š��ڵ�һ��û�е���next֮ǰ������Ч
			*/
			docid_t doc();

			/**
			* ���ص�ǰtf���ڵ�һ��û�е���next֮ǰ������Ч
			*/
			count_t freq();

			/**
			 * ��ǰ�ƶ�
			 * @return ��������һ��termʱ����ture����false
			 */
			bool	next();

			/**
			 * ��ǰ�ƶ�һ��
			 * @param docs �ĵ��������ָ��
			 * @param freqs tf����ָ�룬��docsһ����С
			 * @param count �ƶ����С
			 * @return ʵ���ƶ�λ�ô�С��ʵ����docs��freqs���鳤��
			 */			
			count_t	next(docid_t*& docs, count_t*& freqs);

			/** 
			 * �ƶ�����һ���ĵ���Ŵ��ڻ����target��λ��
			 * @param target Ŀ���ĵ����
			 * @param nearTarget �������ɹ�ʱnearTarget=target,���򣬷��ش���target������target��ӽ���doc id,
			 *             ���target��������ֵ�򷵻�С��target����target��ӽ���doc id
			 * @return ���������������ĵ����ʱ����true
			 */ 
			bool skipTo(docid_t target,docid_t& nearTarget);

			/** 
			 * �ƶ�����һ��λ�ô��ڻ����target��λ��			 
			 * @param target Ŀ��λ��
			 * @param nearTarget �������ɹ�ʱnearTarget=target,���򣬷��ش���target������target��ӽ���λ��,
			 *             ���target��������ֵ�򷵻�С��target����target��ӽ���λ��
			 * @return ��������targetλ��ʱ����true
			 */ 
			bool skipToPosition(loc_t target,loc_t& nearTarget);

			/**
			 * get document frequency
			 * @return frequency
			 */
			freq_t	docFreq(){return CBarrelTermDocFreqs::docFreq();};

			/**
			 * get collection's total term frequency
			 * @return CTF value
			 */
			int64_t	getCTF(){return CBarrelTermDocFreqs::getCTF();};

			/**
			 * �رգ��ͷ���Դ
			 */
			void	close();

			/**
			 * ��һ��λ��
			 */
			loc_t	nextPosition();
			
			/** 
			 * set document postings buffer
			 * @param buffer buffer for document postings
			 * @param length length of buffer
			 * @return size of buffer used.
			 */
			size_t	setBuffer(int32_t* buffer,size_t length);

			/** 
			 * get size of buffer
			 * @return get size of buffer
			 */
			size_t	getBufferSize();

			/**
			 * ��һ��λ��
			 * @param positions λ������
			 * @return ʵ��λ�ø�����positions�����С
			 */
			count_t	nextPositions(loc_t*& positions);
		protected:
			/** decode positions */
			inline bool	decodePositions();

			/** skip some positions */
			inline void	skipPositions(int32_t nSkips);

			/** create buffer for decoding */
			void	createBuffer();
		protected:			
			int32_t*		m_pPPostingBuffer;				///buffer for position posting	
			size_t			m_nPBufferSize;					///size of buffer
			bool			m_bOwnPPostingBuffer;			///we own the buffer or not

			int32_t*		m_pPPostingBufferWithinDoc;
			int32_t			m_nCurDecodedPCountWithinDoc;	
			int32_t			m_nTotalDecodedPCountWithinDoc;
			int32_t			m_nCurrentPPostingWithinDoc;
			int32_t			m_nPPostingCountWithinDoc;		///total position posting count	

			size_t			m_nTotalDecodedPCount;
			size_t			m_nCurrentPPosting;
			int32_t			m_nLastPosting;
			int32_t			m_nLastUnDecodedPCount;

			static const size_t DEFAULT_BUFFERSIZE = 163840;
		};
		//////////////////////////////////////////////////////////////////////////
		///inline functions
		inline void CBarrelTermPositions::skipPositions(int32_t nSkips)
		{
			if(nSkips <= 0)
				return;

			if((size_t)nSkips <= (m_nTotalDecodedPCount - m_nCurrentPPosting))
				m_nCurrentPPosting += nSkips;
			else 
			{
				m_pPosting->decodeNextPositions(NULL,(int32_t)(nSkips - (m_nTotalDecodedPCount - m_nCurrentPPosting)));
				m_nCurrentPPosting = m_nTotalDecodedPCount;
				m_nLastPosting = -1;
			}
		}
		inline bool CBarrelTermPositions::decodePositions()
		{
			if(m_nTotalDecodedPCountWithinDoc >= m_nPPostingCountWithinDoc)
			{				
				///reach to the end
				return false;
			}
			if(!m_pPPostingBuffer)
			{
				createBuffer();
			}
			if((size_t)m_nCurrentPPosting >= m_nTotalDecodedPCount)///decode position posting to buffer
			{
				int32_t* pPPostingBuffer = m_pPPostingBuffer;
				bool bEnd = false;
				if(m_nLastPosting == -1)
					m_nLastPosting = m_nCurrentPosting;
				m_nTotalDecodedPCount = 0;
				if(m_nLastUnDecodedPCount > 0)
				{
					if((size_t)m_nLastUnDecodedPCount > m_nPBufferSize)
					{
						m_nTotalDecodedPCount = m_nPBufferSize;
						m_nLastUnDecodedPCount -= (int32_t)m_nPBufferSize;
						bEnd = true;
					}
					else 
					{
						m_nTotalDecodedPCount = m_nLastUnDecodedPCount;
						m_nLastUnDecodedPCount = 0;
						m_nLastPosting++;
					}
					m_pPosting->decodeNextPositions(pPPostingBuffer,(int32_t)m_nTotalDecodedPCount);
					pPPostingBuffer += m_nTotalDecodedPCount;
					if(m_nLastUnDecodedPCount == 0)
						m_pPosting->resetPosition();
				}								
				if(!bEnd)
				{
					int32_t nFreqs;
					for (nFreqs = m_nLastPosting; nFreqs < m_nCurDecodedCount;nFreqs++)
					{					
						m_nTotalDecodedPCount += m_pPostingBuffer[m_nFreqStart + nFreqs];
						if(m_nTotalDecodedPCount > m_nPBufferSize)
						{
							m_nTotalDecodedPCount -= m_pPostingBuffer[m_nFreqStart + nFreqs];
							break;
						}
					}
					if((nFreqs - m_nLastPosting) > 0)
					{
						m_pPosting->decodeNextPositions(pPPostingBuffer,&(m_pPostingBuffer[m_nFreqStart + m_nLastPosting]),nFreqs - m_nLastPosting);
						m_nLastPosting = nFreqs;
					}
					else if(m_nTotalDecodedPCount == 0)
					{
						m_nTotalDecodedPCount = m_nPBufferSize;
						m_pPosting->decodeNextPositions(pPPostingBuffer,(int32_t)m_nTotalDecodedPCount);						
						m_nLastUnDecodedPCount = m_pPostingBuffer[m_nFreqStart + m_nLastPosting] - (int32_t)m_nTotalDecodedPCount;
					}
				}
				m_nCurrentPPosting = 0;
				if(m_nLastPosting >= m_nCurDecodedCount)
					m_nLastPosting = -1;
			}

			m_nCurrentPPostingWithinDoc = 0;

			int32_t nNeedDecode = m_nPPostingCountWithinDoc - m_nTotalDecodedPCountWithinDoc;
			if((size_t)nNeedDecode > (m_nTotalDecodedPCount - m_nCurrentPPosting))
				nNeedDecode = (int32_t)(m_nTotalDecodedPCount - m_nCurrentPPosting);			

			m_nCurDecodedPCountWithinDoc = nNeedDecode;
			m_nTotalDecodedPCountWithinDoc += nNeedDecode;
			m_pPPostingBufferWithinDoc = m_pPPostingBuffer + m_nCurrentPPosting;
			m_nCurrentPPosting += nNeedDecode;

			return true;			
		}		
	}
}


#endif
