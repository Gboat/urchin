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
// Created	: 2005/12/07
//

#ifndef _INDEX_H
#define _INDEX_H

#include "utility/StdHeader.h"
#include "../document/Document.h"
#include "../store/Directory.h"
#include "IndexWriter.h"
#include "IndexReader.h"

using namespace firtex;
using namespace firtex::store;

namespace firtex
{
	namespace index
	{		
		class CNormBytes;
		class CBarrelsInfo;
		class CIndex
		{
		public:
			typedef int32_t ACCESS_MODE;

			///索引访问模式定义			
			const static int32_t ACCESS_READONLY	= 0x00;		/// 只读，可以读，搜索索引数据库，但是不能修改（添加和删除文件）
																	/// read only,can read,search it but can't modify it,i.e.add files or delete files.
			const static int32_t ACCESS_CREATE		= 0x10;		/// 创建，可以添加，删除文档，能读取索引的部分数据（例如文档总数等统计信息），但是不能进行搜索操作，如果索引数据库已经存在则自动删除
																	/// write only,can add or delete files but can't search over it,will delete existed index database				
			const static int32_t ACCESS_APPEND		= 0x20;		/// 增量索引，但是不同于ACCESS_WRITE，如果索引数据库已经存在，则追加新索引数据而不删除已有索引数据
																	/// write only,but unlike ACCESS_WRITE,will append index data do existed index database.
			const static int32_t ACCESS_MODIFY		= 0x40;		/// 修改，打开已存在的索引数据，可以做相应的修改，例如索引合并，删除文档等
																	/// modify, open exist database, can modify database,etc.merge index,delete documents
			const static int32_t ACCESS_SEARCH		= 0x01;		/// 搜索，打开已存在的索引数据，可以读取索引信息和搜索，不能修改
																	/// search, open exist database, can read/search database, but can't modify it.
						
		public:
			CIndex(const tchar* indName,ACCESS_MODE am);
			CIndex(CDirectory* pDirectory,ACCESS_MODE am);
			CIndex(void);
			virtual ~CIndex(void);
		public:		
			/**
			 * 获取索引的版本号
			 * @return 版本号字符串，格式：名称.版本号
			 */
			tstring	getVersion(){return FIRTEX_VERSION;}

			/**
			 * 打开索引
			 * @param name 索引路径
			 * @param am 索引的访问方式
			 */
			void	open(const tchar* path,ACCESS_MODE am);

			/**
			 * 打开索引
			 * @param pDirectory 索引存储设备
			 * @param am 索引的访问方式
			 */
			void	open(CDirectory* pDirectory,ACCESS_MODE am);

			/**
			 * 关闭索引释放资源
			 */
			void	close();										

			/**
			 * 设置文档的schema,必须在调用acquireWriter()前调用,调用后旧的schema信息将清除
			 * @param pSchema schema对象指针,不作为内部对象
			 */
			void				setDocSchema(CDocumentSchema* pSchema);

			/** 获取索引文档Schema */
			CDocumentSchema*	getSchema(){return m_pDocSchema;}

			/**
			 * 获取索引的directory对象
			 */
			CDirectory*		getDirectory(){return m_pDirectory;}

			/** 
			 * 设置索引名称
			 */
			void			setDirectory(CDirectory* pDir);

			/**
			 * 获取索引桶信息
			 */
			CBarrelsInfo*	getBarrelsInfo(){return m_pBarrelsInfo;}

			/** get access mode */
			ACCESS_MODE		getAccessMode(){return m_accessMode;}

			/**
			 * is index modified?
			 * @return true if modified, otherwise false
			 */
			bool			isDirty(){return m_bDirty;}

			/**
			 * set or clear index modification state
			 * @param bDirty modification state
			 */
			void			setDirty(bool bDirty = true);

			/**
			 * 获取索引的reader对象，仅当ACCESS_MODE = ACCESS_READONLY 或
			 * ACCESS_READWRITE 或 ACCESS_READAPPEND时返回有效对象
			 * @return reader对象的引用
			 * @memory 内部对象，外部不能删除
			 */
			CIndexReader*	acquireReader();

			/**
			 * 获取索引的writer对象，仅当ACCESS_MODE = ACCESS_WRITE 或
			 * ACCESS_READWRITE 或 ACCESS_APPEND或ACCEACCESS_READAPPEND时返回有效对象
			 * @param pParser 文本格式分析器,由内部维护
			 * @param pAnalyzer 文本内容分析器，由内部维护
			 * @param ms 索引合并策略
			 * @return writer对象的引用，内部对象，外部不能删除			 
			 */
			CIndexWriter*	acquireWriter(CParser* pParser,CAnalyzer* pAnalyzer);

			/**
			 * 获取索引的writer对象，仅当ACCESS_MODE = ACCESS_WRITE 或
			 * ACCESS_READWRITE 或 ACCESS_APPEND或ACCEACCESS_READAPPEND时返回有效对象			 
			 * @param ms 索引合并策略
			 * @return writer对象的引用，内部对象，外部不能删除			 
			 */
			CIndexWriter*	acquireWriter();
		protected:
			/** 从索引创建域工厂 */
			CFactoryFinder*		createFactoryFinderFromExistIndex();

			/** 从索引创建文档模式 */
			CDocumentSchema*	createSchemaFromExistIndex();

			/** 获取域工厂*/
			CFactoryFinder*		getFactoryFinder();	
		protected:
			CDirectory*			m_pDirectory;		///索引存储位置
            CIndexReader*		m_pReader;			///索引读访问
			CIndexWriter*		m_pWriter;			///索引写访问
			CBarrelsInfo*		m_pBarrelsInfo;		///索引全局信息
			ACCESS_MODE			m_accessMode;		///索引存取模式
			bool				m_bDirty;			///索引是否进行过修改

			CDocumentSchema*	m_pDocSchema;		///文档的schema信息

			CFactoryFinder*		m_pFactoryFinder;	///field component factory finder		

			friend class CIndexWriter;
			friend class CIndexReader;
			friend class CIndexBarrelReader;
		};
	}
}

#endif
