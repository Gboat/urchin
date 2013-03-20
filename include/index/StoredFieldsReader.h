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
// Created	: 2006/4/12
//
#ifndef _STOREDFIELDREADER_H
#define _STOREDFIELDREADER_H

#include "../utility/StdHeader.h"
#include "../document/Document.h"
#include "../document/Field.h"
#include "../store/Directory.h"
#include "../store/IndexInput.h"
#include <string>

#define STOREDFIELD_READERBUFFSIZE	4096		//4K

using namespace std;
using namespace firtex::store;
using namespace firtex;

namespace firtex
{
	namespace index
	{
		class CFieldsInfo;
		class CStoredFieldsReader
		{
		public:
			CStoredFieldsReader(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo);
			~CStoredFieldsReader(void);
		public:
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

			void			open(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo);
			void			close();
		protected:
			CIndexInput*	m_pFieldValueReader;
			CIndexInput*	m_pFieldIndexReader;			

			CFieldsInfo*	m_pFieldsInfo;
		};
	}
}


#endif
