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
// Created	: 2005/12/20
//
#ifndef __TERMDOCS_H
#define __TERMDOCS_H

#include "Term.h"
#include "../utility/StdHeader.h"

namespace firtex
{
	namespace index
	{
		class CTermDocFreqs
		{
		public:

			CTermDocFreqs(void)
			{
			}
 
			virtual ~CTermDocFreqs(void)
			{
			}

			/**
			 * seek to the specific term
			 * @param term term object pointer
			 * @return true if there is such term, otherwise false
			 */
			virtual bool	seek(CTerm* term) = 0;

			/**
			 * get document frequency
			 * @return frequency
			 */
			virtual freq_t	docFreq() = 0;

			/**
			 * get collection's total term frequency
			 * @return CTF value
			 */
			virtual int64_t	getCTF() = 0; 

			/**
			 * move to the next documents block
			 * @param docs documents block
			 * @param freqs term frequency block
			 * @return number of documents in the block
			 */			
			virtual count_t next(docid_t*& docs, count_t*& freqs)=0;

			/** 
			 * �ƶ�����һ���ĵ���Ŵ��ڻ����target��λ��
			 * @param target Ŀ���ĵ����
			 * @param nearTarget �������ɹ�ʱnearTarget=target,���򣬷��ش���target������target��ӽ���doc id,
			 *             ���target��������ֵ�򷵻�С��target����target��ӽ���doc id
			 * @return ���������������ĵ����ʱ����true
			 */ 
			virtual bool	skipTo(docid_t target,docid_t& nearTarget)=0;
			
			/**
			 * move to the next document
			 * @return tr��������һ��termʱ����ture����false
			 */
			virtual bool	next() = 0;
			
			/**
			 * ���ص�ǰ�ĵ���š������ڵ���{@link next()}����{@link skipTo(docid_t target,docid_t& nearTarget)}
			 * ����true�����
			 */
			virtual docid_t doc() = 0;

			/**
			 * ���ص�ǰtf�������ڵ���{@link next()}����{@link skipTo(docid_t target,docid_t& nearTarget)}
			 * ����true�����
			 */
			virtual count_t freq()=0;

			/** 
			 * set document postings buffer
			 * @param buffer buffer for document postings
			 * @param length length of buffer
			 * @return size of buffer used.
			 */
			virtual size_t	setBuffer(int32_t* buffer,size_t length) = 0;

			/** 
			 * get size of buffer
			 * @return get size of buffer
			 */
			virtual size_t	getBufferSize() = 0;

			/**
			 * �رգ��ͷ���Դ
			 */
			virtual void	close()=0;
		};
	}
}


#endif
