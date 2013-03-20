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
// Created	: 2005/12/10
//
#ifndef _FIELDINDEXER_H
#define _FIELDINDEXER_H

#include "../utility/StdHeader.h"
#include "../document/Field.h"
#include "OutputDescriptor.h"
#include "Posting.h"
#include "thread/FirteXThreads.h"
using namespace firtex;
using namespace firtex::document;

namespace firtex
{
	namespace index
	{
		class CTermReader;
		class CFieldIndexer : public LOCK_BASE
		{
		public:

			CFieldIndexer(void)
			{
			}

			virtual ~CFieldIndexer(void)
			{
			}
		public:		
			/**
			 * 添加索引字段
			 * @param did 文档内部编号
			 * @param pField 待索引字段对象指针
			 */
			virtual void			addField(docid_t did,CField* pField) = 0;

			/**
			 * 批量添加索引字段，以减少函数调用开销，pdid和ppField必须一一对应，即ppField[i]的文档编号为pdid[i]
			 * @param pdid 文档内部编号数组，大小为numFields
			 * @param ppField 待索引字段数组，大小为numFields
			 * @param numFields pdid和ppFields数组大小
			 */
			virtual void			addFields(docid_t* pdid,CField** ppField,int32_t numFields) = 0;
			
			/**
			 * write field index data
			 * @param pWriterDesc out descriptor
			 * @return offset of field vocabulary descriptor
			 */
			virtual fileoffset_t	write(COutputDescriptor* pWriterDesc) = 0;
			
			/**
			 * 设置这个域在tdi文件中的偏移位置
			 * @param off tdi文件偏移地址
			 */
			virtual	void			setFilePointer(fileoffset_t off) = 0;

			/**
			 * 获取这个域在tdi文件中的偏移位置
			 */
			virtual	fileoffset_t	getFilePointer() = 0;

			/**
			 * 获取非重复的索引词个数
			 */
			virtual uint64_t		distinctNumTerms() = 0;

			/** 
			 * 创建用于搜索内存词典的Term Reader
			 * @return Term Reader 对象，需外部删除
			 */
			virtual CTermReader*	termReader() = 0;

			/**
			 * reset postings of the indexer
			 */
			virtual void			reset() = 0;
		public:
			/** 获取这个域的索引级别 */
			INDEX_LEVEL				getIndexLevel(){return m_eIndexLevel;}
			
			/** 设置这个域的索引级别 */
			void					setIndexLevel(INDEX_LEVEL eLevel){m_eIndexLevel = eLevel;}

			/**
			 * set field name of the indexer
			 * @param field field name
			 */
			void					setField(const tchar* field){m_sField = field;}

			/**
			 * get field name of the indexer
			 * @return field name
			 */
			const tchar*			getField(){return m_sField.c_str();}
		private:
			INDEX_LEVEL		m_eIndexLevel;
			tstring			m_sField;
		};
	}
}

#endif
