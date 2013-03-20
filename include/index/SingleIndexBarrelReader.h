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
// Created	: 2005/11/27
//
#ifndef _SINGLEINDEXBARRELREADER_H
#define _SINGLEINDEXBARRELREADER_H

#include "IndexBarrelReader.h"
#include "NormsReader.h"
#include "../utility/BitVector.h"
using namespace firtex::utility;

namespace firtex
{
	namespace index
	{
		class CIndex;
		class CBarrelInfo;
		class CSingleIndexBarrelReader : public CIndexBarrelReader
		{
		public:
			CSingleIndexBarrelReader(CIndex* pIndex, CBarrelInfo* pBarrelInfo);
			virtual ~CSingleIndexBarrelReader(void);
		public:
			/**
			 * 打开指定的索引桶读
			 * @param pDirectory 存储位置
			 * @param name 索引桶名称
			 */
			void			open(const tchar* name);

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
			bool			deleteDocument(docid_t docID);

			/** 
			 * 恢复先前删除的所有文档，只有在调用{@link #optimize()}前调用才有效
			 */
			void			undeleteAll();

			/** 
			 * 判断文档编号为docID的文档是否已经删除
			 * @param docID 文档编号
			 */
			bool			isDeleted(docid_t docID);

			/** 
			 * 判断是否删除过文档
			 */
			bool			hasDeletions();

			/**
			 * 删除文档总数
			 */
			count_t			numDeletedDocs();

			/**
			 * 获取索引桶的TermReader
			 * @param 如果失败则返回NULL
			 * @memory 内存对象，外部不能删除
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
			 * 获取指定字段的norms Bytes
			 * @param field 字段名
			 */
			CNormsBytes*		norms(const tchar* field);

			/**
			 * 获取指定字段的norms Bytes
			 * @param fid 字段编号
			 */
			CNormsBytes*		norms(fieldid_t fid);

			/**
			 * 获取字段信息
			 */
			CFieldsInfo&		getFieldsInfo(){return *m_pFieldsInfo;}

			/**
			 * 获取词总数
			 */
			uint64_t			numTerms();

			/**
			 * 关闭索引桶，释放资源
			 */
			void				close();
		private:			
			tstring					m_name;
			CFieldsInfo*			m_pFieldsInfo;

			CBarrelInfo*			m_pBarrelInfo;

			CTermReader*			m_pTermReader;
			CStoredFieldsReader*	m_pStoredFieldsReader;
			CNormsReader*			m_pNormsReader;
			CForwardIndexReader*	m_pTermVectorReader;		

			CBitVector*				m_pDeletedDocs;
			bool					m_bDeletedDocsDirty;
		};
	}
}

#endif
