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
// Created	: 2006/7/2
//
#ifndef __MULTIFORWARDINDEXREADER_H
#define __MULTIFORWARDINDEXREADER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ForwardIndexReader.h"

namespace firtex
{
	namespace index
	{
		class CForwardIndexReaderEntry
		{
		public:
			CForwardIndexReaderEntry()
			{
				docStart = numDocs = 0;
				reader = NULL;
			}
			~CForwardIndexReaderEntry(){}
		protected:
			docid_t					docStart;		//文档起始编号
			count_t					numDocs;		//文档总数
			CForwardIndexReader*		reader;			//term vector 对象

			friend class CMultiForwardIndexReader;
		};
		class CMultiForwardIndexReader :	public CForwardIndexReader
		{
		public:
			CMultiForwardIndexReader(int numReaders);
			virtual ~CMultiForwardIndexReader(void);
		public:

			/**
			 * 打开reader,在CMultiForwardIndexReader里此函数不做任何事情
			 * @param barrelName 索引桶名称			 
			 */
			virtual void		open(const tchar* barrelName){}

			/**
			 * 关闭reader,释放资源,在CMultiForwardIndexReader里此函数不做任何事情
			 */
			virtual void		close(){}

			/**
			 * 获取前向索引类型,参见{@link #ForwardIndex}
			 */
			virtual forwardindex_t	getForwardIndexType(const tchar* field);

			/**
			 * 获取Term Freq Vector
			 * @param did 文档编号
			 * @param field 字段名称
			 * @return Term Freq Vector 指针
			 * @memory 非内部指针,需外部删除
			 */
			virtual CTermFreqVector*		getFreqVector(docid_t did,const tchar* field);

			/**
			 * 获取Term Freq Vector
			 * @param did 文档编号			 
			 * @return Term Freq Vector 对象数组
			 * @memory 非内部指针,需外部删除
			 */
			virtual CForwardIndexArray<CTermFreqVector*>*	 getFreqVectors(docid_t did);

			/**
			 * 获取Term Position Vector
			 * @param did 文档编号
			 * @param field 字段名称
			 * @return Term Position Vector 指针
			 * @memory 非内部指针,需外部删除
			 */
			virtual CTermPositionVector*	getPositionVector(docid_t did,const tchar* field);

			/**
			 * 获取Term Position Vector
			 * @param did 文档编号			 
			 * @return Term Position Vector 对象数组
			 * @memory 非内部指针,需外部删除
			 */
			virtual CForwardIndexArray<CTermPositionVector*>*	getPositionVectors(docid_t did);

			/**
			 * 获取Term Sequence Vector
			 * @param did 文档编号
			 * @param field 字段名称
			 * @return Term Sequence Vector 指针
			 * @memory 非内部指针,需外部删除
			 */
			virtual CTermSequenceVector*	getSquenceVector(docid_t did,const tchar* field);

			/**
			 * 获取Term Sequence Vector
			 * @param did 文档编号			 
			 * @return Term Sequence Vector 对象数组
			 * @memory 非内部指针,需外部删除
			 */
			virtual CForwardIndexArray<CTermSequenceVector*>*	getSquenceVectors(docid_t did);

		protected:
			void	addVectorReader(int order,docid_t docStart,count_t numDocs,const CForwardIndexReader* pReader);
		private:
			int							m_numReaders;
			CForwardIndexReaderEntry*		m_ppReaders;

			friend class CMultiIndexBarrelReader;
		};
	}
}

#endif
