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
// Created	: 2005/12/26
//

#ifndef _BARRELTERMDOCS_H
#define _BARRELTERMDOCS_H

#include "../store/IndexInput.h"
#include "TermDocFreqs.h"
#include "TermInfo.h"
#include "TermReader.h"

using namespace firtex::store;


namespace firtex
{
	namespace index
	{
		class CBarrelTermDocFreqs :	public CTermDocFreqs
		{
		public:
			CBarrelTermDocFreqs();
			CBarrelTermDocFreqs(CTermReader* pReader,CInputDescriptor* pInputDescriptor,CTermInfo& ti,CBitVector* pDeletedDocs);
			CBarrelTermDocFreqs(CTermReader* pReader,CPosting* pPosting,CTermInfo& ti,CBitVector* pDeletedDocs);
			virtual ~CBarrelTermDocFreqs(void);
		public:
			/**
			 * seek to the specific term
			 * @param term term object pointer
			 * @return true if there is such term, otherwise false
			 */
			bool	seek(CTerm* term);	

			/**
			 * get document frequency
			 * @return frequency
			 */
			freq_t	docFreq();

			/**
			 * get collection's total term frequency
			 * @return CTF value
			 */
			int64_t	getCTF(); 

			/**
			 * move to the next documents block
			 * @param docs documents block
			 * @param freqs term frequency block
			 * @return number of documents in the block
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
			 * ��ǰ�ƶ�
			 * @return ��������һ��docʱ����ture����false
			 */
			bool	next();

			/**
			 * ���ص�ǰ�ĵ���š������ڵ���{@link next()}����{@link skipTo(docid_t target,docid_t& nearTarget)}
			 * ����true�����
			 */
			docid_t	doc();

			/**
			 * ���ص�ǰtf�������ڵ���{@link next()}����{@link skipTo(docid_t target,docid_t& nearTarget)}
			 * ����true�����
			 */
			count_t	freq();

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
			 * �رգ��ͷ���Դ
			 */
			void	close();
		protected:			
			/** ��ȡ����ѹ��Postings */
			virtual bool	decode();

			/**
			 * Binary Search
			 * @param docs �ĵ��������
			 * @param start ������ʼλ��
			 * @param end ��������λ��
			 * @param key ������
			 * @param keyFound �������ɹ�ʱkeyFound=key,���򣬷��ش���key������key��ӽ���doc id,
			 *             ���key��������ֵ�򷵻�С��key����key��ӽ���doc id
			 * @param ����keyFound���±�
			 */
			int32_t		bsearch(docid_t docs[],int32_t start,int32_t end,docid_t key,docid_t& keyFound);

			/** create buffer for decoding */
			void		createBuffer();
		protected:
			CTermInfo			m_termInfo;
			CTermReader*		m_pTermReader;
			CInputDescriptor*	m_pInputDescriptor;
			CBitVector*			m_pDeletedDocs;

			CPosting*			m_pPosting;

			int32_t*			m_pPostingBuffer;	///buffer for store decoded postings 
			size_t				m_nBufferSize;		///the size of buffer.
			size_t				m_nFreqStart;
			bool				m_bOwnBuffer;		///who own the buffer
			int32_t				m_nCurDecodedCount;
			int32_t				m_nTotalDecodedCount;	///size of decoded postings
			int32_t				m_nCurrentPosting;	///position of current posting			

			static const size_t	DEFAULT_BUFFERSIZE = 32768;
		};
		////////////////////////////////////////////////////////////////////////////////
		//
		inline int32_t	CBarrelTermDocFreqs::bsearch(docid_t docs[],int32_t start,int32_t end,docid_t key,docid_t& keyFound)
		{
			int32_t k;
			int32_t nk = end;
			keyFound = docs[end];
			while (start<=end)
			{
				k = (start + end)/2;
				if(key == docs[k])//�ҵ�
				{
					keyFound = key;
					return k;
				}
				if(key < docs[k])//��������
				{
					end = k - 1;
					if(k >= start)
					{
						keyFound = docs[k];
						nk =k;
					}
				}
				else //�����Ұ��
				{
					start = k + 1;
					/*if(k <= end)
					{
						keyFound = docs[k];
						nk =k;
					}*/
					if(start <= end)
					{
						if(docs[start] > key)
						{
							keyFound = docs[start];
							nk = start;
						}
					}
				}
			}
			return nk;
		}

	}
}

#endif
