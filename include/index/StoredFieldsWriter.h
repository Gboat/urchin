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
// Created	: 2005/12/9
//
#ifndef __STOREDFIELDSWRITER_H
#define __STOREDFIELDSWRITER_H

#include "../utility/StdHeader.h"
#include "../document/Document.h"
#include "../document/Field.h"
#include "../store/Directory.h"
#include "../store/IndexOutput.h"
#include <string>

#define STOREDFIELD_WRITEBUFFSIZE	4096		//4K

using namespace std;
using namespace firtex::store;
using namespace firtex;


namespace firtex
{
	namespace index
	{		
		class CFieldsInfo;
		class CStoredFieldsReader;
		class CStoredFieldsWriter
		{
		public:
			CStoredFieldsWriter(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo);
			CStoredFieldsWriter();
			~CStoredFieldsWriter(void);
		public:
			/**
			 * 添加文档，存储存储字段(Store Field)
			 * @param pDoc 文档对象			 
			 */
			void			addDocument(document::CDocument* pDoc);	

			/**
			 * 获取指定文档编号的文档对象，此文档对象仅包含建索引时有存储标志的字段
			 * @param docID 文档编号
			 * @return 文档对象
			 * @memory 非内部对象,需外部删除
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
			 * 打开新的文档域信息文件
			 * @param pDirectory 存储位置
			 * @param barrels索引桶名称
			 * @param fieldsInfo 索引的域信息
			 */
			void			open(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo);

			/** 关闭文件 */
			void			close();			
		protected:			
			CIndexOutput*	m_pFieldValueWriter;
			CIndexOutput*	m_pFieldIndexWriter;			

			CDirectory*		m_pDirectory;
			CFieldsInfo*	m_pFieldsInfo;
			tstring			m_sBarrel;
			bool			m_bDirty;

			CStoredFieldsReader*	m_pSFReader;
		};

	}
}

#endif
