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
// Created	: 2006/11/29
//
#ifndef _INMEMORYINDEXBARRELREADER_H
#define _INMEMORYINDEXBARRELREADER_H

#include "IndexBarrelReader.h"
#include "../utility/BitVector.h"
#include "NormsReader.h"
using namespace firtex::utility;

namespace firtex
{
	namespace index
	{
		class CIndexBarrelWriter;
		class CInMemoryIndexBarrelReader :	public CIndexBarrelReader
		{
		public:
			CInMemoryIndexBarrelReader(CIndexBarrelWriter*	pIndexBarrelWriter);
			virtual ~CInMemoryIndexBarrelReader(void);
		public:
			/**
			 * 返回文档docID的stored fields
			 * @param docID 文档编号
			 * @return 文档对象，需外部维护内存
			 */
			document::CDocument*	document(docid_t docID);

			/**
			 * 获取指定文档编号指定字段编号的文档对象
			 * @param docID 文档编号
			 * @param fid 字段编号
			 * @return 字段对象,当该字段不是存储字段或不存在时返回NULL
			 * @memory 非内部对象,需外部删除
			 */
			document::CField*		field(docid_t docID,fieldid_t fid);

			/**
			 * delete a document
			 * @param docID document ID
			 * @return true if delete success, false if already deleted or some problem
			 */
			bool	deleteDocument(docid_t docID);

			/** 
			 * 恢复先前删除的所有文档，只有在调用{@link #optimize()}前调用才有效
			 */
			void	undeleteAll();

			/** 
			 * 判断文档编号为docID的文档是否已经删除
			 * @param docID 文档编号
			 */
			bool	isDeleted(docid_t docID);

			/** 
			 * 判断是否删除过文档
			 */
			bool	hasDeletions();

			/**
			 * 删除文档总数
			 */
			count_t	numDeletedDocs();

			/**
			 * 获取索引桶的TermReader
			 * @param 如果失败则返回NULL，内存由内部维护，外部不能删除
			 */
			CTermReader*	termReader();

			/**
			 * 获取索引桶指定字段的TermReader
			 * @param 如果失败则返回NULL
			 * @memory 内存对象，外部不能删除
			 */
			CTermReader*	termReader(const tchar* field);

			/**
			 * 获取前向索引阅读器			 
			 * @return 前向索引阅读器对象
			 * @memory 内部对象,外部不能删除
			 */
			CForwardIndexReader*	getForwardIndexReader();

			/**
			 * 获取指定域的norms Bytes
			 * @param field 域名
			 */
			CNormsBytes*		norms(const tchar* field);

			/**
			 * 获取指定域的norms Bytes
			 * @param fid 域编号
			 */
			CNormsBytes*		norms(fieldid_t fid);

			/**
			 * 获取词总数
			 */
			uint64_t		numTerms();

			/**
			 * 获取域信息
			 */
			CFieldsInfo&	getFieldsInfo();

			/**
			 * 关闭索引桶，释放资源
			 */
			void			close();

			/** 设置域工厂 */
			void			setFactoryFinder(CFactoryFinder* pFactories){};
		protected:
			CIndexBarrelWriter*	m_pIndexBarrelWriter;
			CTermReader*		m_pTermReader;
			CNormsReader*		m_pNormsReader;
			CBitVector*			m_pDeletedDocs;
		};
	}
}

#endif
