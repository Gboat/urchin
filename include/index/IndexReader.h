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
// Created	: 2006/4/27
// Modified	: 2006/10/11 changed to access index data directly,not by CIndex object.-by GuoRuijie
//

#ifndef _INDEXREADER_H
#define _INDEXREADER_H

#include "../utility/StdHeader.h"
#include "../utility/DateTime.h"
#include "../store/Directory.h"
#include "../document/Document.h"
#include "IndexBarrelReader.h"
#include "TermInfo.h"
#include "BarrelsInfo.h"

using namespace firtex::document;
using namespace firtex::store;
using namespace std;

namespace firtex
{
	namespace index
	{
		class CNormsBytes;
		class CIndex;
		class CIndexReader
		{
		protected:
			CIndexReader();
			CIndexReader(CIndex* pIndex);
		public:
			virtual ~CIndexReader(void);
		public:			
			/**
			 * 获取索引的版本号
			 * @return 版本号字符串，格式：名称.版本号
			 */
			tstring	getVersion();

			/** 
			 * 判断文档编号为docID的文档是否
			 * @param docID 文档编号
			 */
			bool	isDeleted(docid_t docID);

			/** 
			 * 判断是否有文档删除过
			 */
			bool	hasDeletions();

			/**
			 * 删除文档总数
			 */
			count_t numDeletedDocs();

			/**
			 * 删除索引文档
			 * @param docID 文档编号
			 */
			bool	deleteDocument(docid_t docID);

			/** 
			 * 恢复先前删除的所有文档，只有在调用{@link #optimize()}前调用才有效
			 */
			void	undeleteAll();

			/**
			 * 关闭索引释放资源
			 */
			void	close();
			
			/**
			 * 索引中包含的文档总数
			 */
			count_t	numDocs();

			/**
			 * 返回可能最大的文档编号，一般情况下如果没有删除文档则maxDoc()=numDocs()
			 * 可以用来决定分配文档数组的大小
			 */
			count_t	maxDoc();			

			/**
			 * get total number of terms have been indexed
			 * @return number of terms
			 */
			uint64_t	getNumTerms();
			/**
			 * 返回文档docID的stored fields
			 * @param docID 文档编号
			 * @memory 非内部对象,由外部删除
			 */
			document::CDocument* document(docid_t docID);

			/**
			 * 获取指定文档编号指定字段编号的文档对象
			 * @param docID 文档编号
			 * @param fid 字段编号
			 * @return 字段对象,当该字段不是存储字段或不存在时返回NULL
			 * @memory 非内部对象,需外部删除
			 */
			document::CField*		field(docid_t docID,fieldid_t fid);

			/**
			 * 获取指定Term的文档频率
			 * @return 返回文档频率
			 */
			freq_t			docFreq(CTerm* term);

			/**
			 * 获取指定Term的Term Info
			 * @return 返回Term Info
			 */
			CTermInfo*		termInfo(CTerm* term);		

			/**
			 * 获取指定域的norm bytes
			 * @param fieldID 域编号
			 * @return norms bytes
			 * @memory 内部对象,外部不能删除
			 */
			CNormsBytes*		norms(fieldid_t fieldID);

			/**
			 * 获取指定域的norm bytes
			 * @param field 域名
			 * @return norms bytes
			 * @memory 内部对象,外部不能删除
			 */
			CNormsBytes*		norms(const tchar* field);

			/**
			 * 获取索引桶信息
			 */
			CBarrelsInfo*	getBarrelsInfo();			

			/**
			 * 获取字段信息
			 * @return 字段信息,内部对象
			 */
			CFieldsInfo&	getFieldsInfo();
			
			/**
			 * 获取内部索引对象
			 * @return 内部索引器对象
			 * @memory 内部对象,外部不能删除
			 */
			CIndex*			getIndex(){return m_pIndex;}

			/**
			 * get term reader			 
			 * @return term reader, MUST be deleted by caller.
			 */
			CTermReader*	termReader();

			/**
			 * 获取Term Vector 阅读器
			 * @return 阅读器对象指针
			 * @memory 内部对象,外部不能删除
			 */
			CForwardIndexReader*	getForwardIndexReader();

			/**
			 * 获取索引的最后修改时间
			 * @return 64位整数,可以用localtime()获取 struct tm形式表示的时间
			 */
			int64_t			lastModified();

			/**
			 * 获取索引的最后修改时间
			 * @param dt CDateTime对象,用于返回值
			 */
			void			lastModified(CDateTime& dt);

		public:
			/**
			 * 获取索引的最后修改时间
			 * @param pDirectory 索引位置
			 */
			static int64_t	lastModified(CDirectory* pDirectory);

			/**
			 * 获取索引的最后修改时间
			 * @param dt CDateTime对象,用于返回值
			 */
			static void		lastModified(CDirectory* pDirectory,CDateTime& dt);
		protected:
			/**
			 * create barrel reader
			 */			
			void	createBarrelReader();

			/**
			 * get index barrel reader 
			 * @return reference to index barrel reader
			 */
			CIndexBarrelReader* getBarrelReader();

			/**
			 * get internal term reader
			 * @return term reader, internal object
			 */
			CTermReader*		getTermReader();
		protected:
			CIndex*				m_pIndex;				///reference to index object
			CBarrelsInfo*		m_pBarrelsInfo;			///reference to CIndex's m_pBarrelsInfo
			CTermReader*		m_pTermReader;			///term reader
			CIndexBarrelReader*	m_pBarrelReader;		///barrel reader

			friend class CIndex;
		};	
		//////////////////////////////////////////////////////////////////////////
		//inline functions
		inline document::CDocument* CIndexReader::document(docid_t docID)
		{
			if(m_pBarrelReader == NULL)
			{
				createBarrelReader();
			}
			return m_pBarrelReader->document(docID);
		}
		inline document::CField* CIndexReader::field(docid_t docID,fieldid_t fid)
		{
			if(m_pBarrelReader == NULL)
			{
				createBarrelReader();
			}
			return m_pBarrelReader->field(docID,fid);
		}
		inline CBarrelsInfo* CIndexReader::getBarrelsInfo()
		{
			return m_pBarrelsInfo;
		}
	}
}

#endif
