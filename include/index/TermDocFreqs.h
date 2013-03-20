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
			 * 移动到第一个文档编号大于或等于target的位置
			 * @param target 目标文档编号
			 * @param nearTarget 当搜索成功时nearTarget=target,否则，返回大于target并且与target最接近的doc id,
			 *             如果target大于所有值则返回小于target且与target最接近的doc id
			 * @return 仅当存在这样的文档编号时返回true
			 */ 
			virtual bool	skipTo(docid_t target,docid_t& nearTarget)=0;
			
			/**
			 * move to the next document
			 * @return tr当存在下一个term时返回ture否则false
			 */
			virtual bool	next() = 0;
			
			/**
			 * 返回当前文档编号。必须在调用{@link next()}或者{@link skipTo(docid_t target,docid_t& nearTarget)}
			 * 返回true后调用
			 */
			virtual docid_t doc() = 0;

			/**
			 * 返回当前tf。必须在调用{@link next()}或者{@link skipTo(docid_t target,docid_t& nearTarget)}
			 * 返回true后调用
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
			 * 关闭，释放资源
			 */
			virtual void	close()=0;
		};
	}
}


#endif
