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
			 * 移动到第一个文档编号大于或等于target的位置
			 * @param target 目标文档编号
			 * @param nearTarget 当搜索成功时nearTarget=target,否则，返回大于target并且与target最接近的doc id,
			 *             如果target大于所有值则返回小于target且与target最接近的doc id
			 * @return 仅当存在这样的文档编号时返回true
			 */ 
			bool skipTo(docid_t target,docid_t& nearTarget);
			
			/**
			 * 往前移动
			 * @return 当存在下一个doc时返回ture否则false
			 */
			bool	next();

			/**
			 * 返回当前文档编号。必须在调用{@link next()}或者{@link skipTo(docid_t target,docid_t& nearTarget)}
			 * 返回true后调用
			 */
			docid_t	doc();

			/**
			 * 返回当前tf。必须在调用{@link next()}或者{@link skipTo(docid_t target,docid_t& nearTarget)}
			 * 返回true后调用
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
			 * 关闭，释放资源
			 */
			void	close();
		protected:			
			/** 读取并解压缩Postings */
			virtual bool	decode();

			/**
			 * Binary Search
			 * @param docs 文档编号数组
			 * @param start 搜索起始位置
			 * @param end 搜索结束位置
			 * @param key 搜索键
			 * @param keyFound 当搜索成功时keyFound=key,否则，返回大于key并且与key最接近的doc id,
			 *             如果key大于所有值则返回小于key且与key最接近的doc id
			 * @param 返回keyFound的下标
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
				if(key == docs[k])//找到
				{
					keyFound = key;
					return k;
				}
				if(key < docs[k])//查找左半边
				{
					end = k - 1;
					if(k >= start)
					{
						keyFound = docs[k];
						nk =k;
					}
				}
				else //查找右半边
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
