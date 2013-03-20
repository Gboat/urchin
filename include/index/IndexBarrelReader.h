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
// Created	: 2005/12/27
//

#ifndef _INDEXBARRELREADER_H
#define _INDEXBARRELREADER_H

#include "../utility/StdHeader.h"
#include "FieldsInfo.h"
#include "StoredFieldsReader.h"
#include "Term.h"
#include "TermDocFreqs.h"
#include "TermPositions.h"
#include "FieldIndexer.h"
#include "FieldsInfo.h"
#include "../store/Directory.h"
#include "../document/Document.h"
#include "../document/Field.h"
#include "ForwardIndexReader.h"

#include <map>

using namespace std;
using namespace firtex;
using namespace firtex::store;


namespace firtex
{
	namespace index
	{
		class CTermReader;
		class CNormsBytes;
		class CIndex;
		class CIndexBarrelReader
		{
		public:
			CIndexBarrelReader(CIndex* pIndex);
			CIndexBarrelReader();
			virtual ~CIndexBarrelReader(void);
		public:
			/**
			 * 返回文档docID的stored fields
			 * @param docID 文档编号
			 * @return 文档对象，需外部维护内存
			 */
			virtual document::CDocument*	document(docid_t docID) = 0;

			/**
			 * 获取指定文档编号指定字段编号的文档对象
			 * @param docID 文档编号
			 * @param fid 字段编号
			 * @return 字段对象,当该字段不是存储字段或不存在时返回NULL
			 * @memory 非内部对象,需外部删除
			 */
			virtual document::CField*		field(docid_t docID,fieldid_t fid) = 0;

			/**
			 * delete a document
			 * @param docID document ID
			 * @return true if delete success, false if already deleted or some problem
			 */
			virtual bool	deleteDocument(docid_t docID) = 0;

			/** 
			 * 恢复先前删除的所有文档，只有在调用{@link #optimize()}前调用才有效
			 */
			virtual void	undeleteAll() = 0;

			/** 
			 * 判断文档编号为docID的文档是否已经删除
			 * @param docID 文档编号
			 */
			virtual bool	isDeleted(docid_t docID) = 0;

			/** 
			 * 判断是否删除过文档
			 */
			virtual bool	hasDeletions() = 0;

			/**
			 * 删除文档总数
			 */
			virtual count_t	numDeletedDocs() = 0;

			/**
			 * 获取索引桶的TermReader
			 * @param 如果失败则返回NULL，内存由内部维护，外部不能删除
			 */
			virtual CTermReader*	termReader() = 0;

			/**
			 * 获取索引桶指定字段的TermReader
			 * @param 如果失败则返回NULL
			 * @memory 内存对象，外部不能删除
			 */
			virtual CTermReader*	termReader(const tchar* field){return NULL;}

			/**
			 * 获取前向索引阅读器			 
			 * @return 前向索引阅读器对象
			 * @memory 内部对象,外部不能删除
			 */
			virtual CForwardIndexReader*	getForwardIndexReader() = 0;

			/**
			 * 获取指定域的norms Bytes
			 * @param field 域名
			 */
			virtual CNormsBytes*		norms(const tchar* field) = 0;

			/**
			 * 获取指定域的norms Bytes
			 * @param fid 域编号
			 */
			virtual CNormsBytes*		norms(fieldid_t fid) = 0;

			/**
			 * 获取词总数
			 */
			virtual uint64_t		numTerms() = 0;

			/**
			 * 获取域信息
			 */
			virtual CFieldsInfo&	getFieldsInfo() = 0;

			/**
			 * 关闭索引桶，释放资源
			 */
			virtual void			close() = 0;		

			/**
			 * 获取域工厂
			 */
			CFactoryFinder*		getFactoryFinder();
		protected:			
			CIndex*					m_pIndex;
		};
	}
}

#endif
