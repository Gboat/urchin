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
#ifndef _MULTIINDEXBARRELREADER_H
#define _MULTIINDEXBARRELREADER_H

#include "IndexBarrelReader.h"
#include "BarrelsInfo.h"
#include "SingleIndexBarrelReader.h"
#include "../store/Directory.h"
#include "IndexBarrelWriter.h"
using namespace firtex::store;

namespace firtex
{
	namespace index
	{		
		class CBarrelReaderEntry
		{
		public:
			CBarrelReaderEntry(CIndex* pIndex,CBarrelInfo* pBarrelInfo)
			{
				m_pBarrelInfo = pBarrelInfo;
				if(pBarrelInfo->getWriter())
					m_pBarrel = pBarrelInfo->getWriter()->inMemoryReader();
				else 
					m_pBarrel = new CSingleIndexBarrelReader(pIndex,pBarrelInfo);
			}
			CBarrelReaderEntry(CBarrelInfo* pBarrelInfo,CIndexBarrelReader* pIndexBarrelReader)
			{
				m_pBarrelInfo = pBarrelInfo;
				m_pBarrel = pIndexBarrelReader;
			}
			~CBarrelReaderEntry()
			{
				delete m_pBarrel;								
				m_pBarrel = NULL;
				m_pBarrelInfo = NULL;
			}
		public:
			CBarrelInfo*			m_pBarrelInfo;
			CIndexBarrelReader*		m_pBarrel;

			friend class CMultiIndexBarrelReader;
		};
		

		class CMultiForwardIndexReader;
		class CMultiIndexBarrelReader : public CIndexBarrelReader
		{
		public:
			CMultiIndexBarrelReader(CIndex* pIndex,CBarrelsInfo* pBarrelsInfo);
			virtual ~CMultiIndexBarrelReader(void);
		public:
			/**
			 * 打开指定的索引桶读
			 * @param pDirectory 存储位置
			 * @param name 索引桶名称
			 */
			void				open(const tchar* name);

			/**
			 * 返回文档docID的stored fields
			 */
			document::CDocument*	document(docid_t docID);

			/**
			 * 获取指定文档编号指定字段编号的文档对象
			 * @param docID 文档编号
			 * @param fid 字段编号
			 * @return 字段对象,当该字段不是存储字段或不存在时返回NULL
			 * @memory 非内部对象,需外部删除
			 */
			document::CField*	field(docid_t docID,fieldid_t fid);

			/** 
			 * 恢复先前删除的所有文档，只有在调用{@link #optimize()}前调用才有效
			 */
			void				undeleteAll();

			/** 
			 * 判断文档编号为docID的文档是否已经删除
			 * @param docID 文档编号
			 */
			bool				isDeleted(docid_t docID);

			/** 
			 * 判断是否删除过文档
			 */
			bool				hasDeletions();

			/**
			 * 删除文档总数
			 */
			count_t				numDeletedDocs();

			/**
			 * delete a document
			 * @param docID document ID
			 * @return true if delete success, false if already deleted or some problem
			 */
			bool				deleteDocument(docid_t docID);

			/**
			 * get term reader			 
			 * @return term reader, MUST be deleted by caller.
			 */
			CTermReader*		termReader();

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
			uint64_t			numTerms();
            
			/**
			 * 获取域信息
			 */
			CFieldsInfo&		getFieldsInfo();

			/**
			 * 关闭索引桶，释放资源
			 */
			void				close();		

			/** 设置域工厂 */
			/*void				setFactoryFinder(CFactoryFinder* pFactories);*/

			/** 
			 * 添加索引桶读取器
			 * @param pBarrelInfo 索引桶信息
			 * @param pBarrelReader 读取器对象，由内部删除
			 */
			void				addBarrelReader(CBarrelInfo* pBarrelInfo,CIndexBarrelReader* pBarrelReader);
		public:
			/**
			 * 开始进行迭代
			 */
			void				startIterator(){m_readersIterator = m_readers.begin();}
			/**
			 * 判断迭代是否完毕
			 * @reture true 迭代未结束，可以调用{@link nextEntry(),nextReader()或nextBarrel()}
			 */
			bool				hasNext(){return (m_readersIterator != m_readers.end());}
			CBarrelReaderEntry* nextEntry();
			CIndexBarrelReader* nextReader();
			CBarrelInfo*		nextBarrel();		
		protected:
			void			addReader(CBarrelInfo* pBarrelInfo);
			bool			loadNorms(const tchar* field);
			bool			loadNorms(fieldid_t fid);
		private:			
			CBarrelsInfo*				m_pBarrelsInfo;
			CTermReader*				m_pTermReader;
			CMultiForwardIndexReader*	m_pTermVectorReader;
			vector<CBarrelReaderEntry*> m_readers;
			vector<CBarrelReaderEntry*>::iterator m_readersIterator;

			map<fieldid_t,CNormsBytes*>	m_normsByID;
			map<tstring,CNormsBytes*>	m_normsByName;
		};
		//////////////////////////////////////////////////////////////////////////
		//Inline Functions
		inline CBarrelReaderEntry* CMultiIndexBarrelReader::nextEntry()
		{
			return *m_readersIterator++;
		}
		inline CIndexBarrelReader* CMultiIndexBarrelReader::nextReader()
		{
			CIndexBarrelReader* pReader = (*m_readersIterator)->m_pBarrel;
			m_readersIterator++;
			return pReader;
		}		
		inline CBarrelInfo* CMultiIndexBarrelReader::nextBarrel()
		{
			CBarrelInfo* pBarrelInfo = (*m_readersIterator)->m_pBarrelInfo;
			m_readersIterator++;
			return pBarrelInfo;
		}
	}
}

#endif
