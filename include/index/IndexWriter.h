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
// Created	: 2005/11/20
// Modified	: 2006/10/11 changed to build index directly,not by CIndex object,and must instantiate by CIndex object.
//						 add documents cache.-by GuoRuijie
//

#ifndef __INDEXWRITER_H
#define __INDEXWRITER_H

#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "../parser/Parser.h"
#include "../analyzer/Analyzer.h"
#include "../utility/Reader.h"
#include "../document/Document.h"
#include "../store/Directory.h"
#include "BarrelsInfo.h"
#include "IndexBarrelWriter.h"
#include "IndexMerger.h"
#include "utility/Configurator.h"


using namespace std;
using namespace firtex::parser;
using namespace firtex::analyzer;
using namespace firtex::utility;
using namespace firtex;
using namespace firtex::store;

namespace firtex
{
	namespace collection
	{
		class CCollection;
	}
}

namespace firtex
{
	namespace index
	{		
		class CIndex;
		class CFactoryFinder;
		class CIndexBarrelReader;
		class CIndexWriter
		{		
		protected:	
			/**
			 * constructor,must be called by CIndex
			 * @param pParser parser of document
			 * @param pAnalyzer default analyzer of document
			 * @param pIndex the index object
			 * @param ms index merge strategy
			 */
			CIndexWriter(CParser* pParser,CAnalyzer* pAnalyzer,CIndex* pIndex);

			/**
			 * constructor，must be called by CIndex
			 * @param pIndex the index object
			 * @param ms index merge strategy		 
			 */
			CIndexWriter(CIndex* pIndex);
		public:
			virtual~CIndexWriter();			
		public:			
			/**
			 * add document to index			 
			 * @param filename file path			 
			 */
			void		addDocument(const tchar* filename);			

			/**
			 * add and merge index
			 * @param pDirectory the place where index stored,must compatible
			 */
			bool		addExistIndex(CDirectory* pDirectory);

			/**        
			 * merge index in an explicit form, the writer will create index merge according to the configure parameter by itself
			 */
			void		mergeIndex();		

			/**        
			 * merge index use specific merge strategy
			 * @param pIndexMerger index merger			 
			 */
			void		mergeIndex(CIndexMerger* pIndexMerger);

			/**
			 * optimize index,it will merge all sub indexes to one index.
			 * @param bGC perform garbage collection or not
			 */
			void		optimizeIndex(bool bGC = false);			

			/**
			 * flush all cached documents to index database
			 */
			void		flushDocuments();

			/**
			 * flush index to disk
			 */
			void		flushToDisk();

			/**
			 * close index writer,release resources
			 */
			void		close();		

			/**
			 * acquire index merger,internal object, {@link setMergeStrategy()} may change this object,
			 * so after calling it you must acquire the merger again.
			 * @return index merger,internal object
			 */
			CIndexMerger*	acquireMerger();
			/**
			 * get configurator of writer
			 * @return configurator of writer
			 */
			firtex::utility::Configurator*	getConfigurator() {return m_pConfigurator;}

			/**
			 * set configurator of writer
			 * @param configurator configurator of writer
			 */
			void setConfigurator(const firtex::utility::Configurator& configurator);
		public:	
			/**
			 * get index document schema
			 */
			CDocumentSchema*	getDocSchema();
		public:
			/**
			 * 关联默认的文档内容分析器
			 * @param pAnalyzer 文档内容分析器,关联后由内部维护，外部不能删除
			 */
			void		attachAnalyzer(CAnalyzer* pAnalyzer);

			/**
			 * 关联指定字段的文档内容分析器
			 * @param field 字段名称
			 * @param pAnalyzer 文档内容分析器,关联后由内部维护，外部不能删除
			 */
			void		attachAnalyzer(const tchar* field,CAnalyzer* pAnalyzer);

			/**
			 * 关联指定字段的文档内容分析器
			 * @param field 字段名称
			 * @param identifier 文档内容分析器标示			 
			 * @param szParam parameter for analyzer
			 * @return attached analyzer
			 */
			CAnalyzer*	attachAnalyzer(const tchar* field,const tchar* identifier,const tchar* szParam);

			/**
			 * 撤销默认的文档内容分析器，撤销后外部负责维护分析器，CIndexWriter不再持有该分析器
			 * @return 文档内容分析器，外部维护
			 */
			CAnalyzer*	detachAnalyzer();

			/**
			 * 撤销指定字段的文档内容分析器，撤销后外部负责维护分析器，CIndexWriter不再持有该分析器
			 * @param field 字段名称
			 * @return 文档内容分析器，如果不存在对应的分析其，则返回NULL
			 */
			CAnalyzer*	detachAnalyzer(const tchar* field);

			/**
			 * 撤销指定字段的文档内容分析器，撤销后外部负责维护分析器，CIndexWriter不再持有该分析器
			 * @param fieldid 字段编号
			 * @return 文档内容分析器，如果不存在对应的分析其，则返回NULL
			 */
			CAnalyzer*	detachAnalyzer(fieldid_t fieldid);

			/**
			 * 获取内部文档内容分析器对象
			 * @return 文档内容分析器对象，内部对象，外部不能删除
			 */
			CAnalyzer*	getAnalyzer(){return m_pAnalyzer;}

			/**
			 * 关联默认的文档格式解析器，关联后由内部维护，关联后由内部维护，外部不能删除
			 * @param pParser 文档格式解析器
			 */
			void		attachParser(CParser* pParser);

			/**
			 * 关联指定格式的的文档格式解析器，关联后由内部维护，关联后由内部维护，外部不能删除
			 * @param docType 文档类型
			 * @param pParser 文档格式解析器
			 */
			void		attachParser(doctype_t docType,CParser* pParser);

			/**
			 * 撤销默认的文档格式解析器，撤销后外部负责维护分析器，CIndexWriter不再持有该分析器
			 * @return 文档内容解析器
			 */
			CParser*	detachParser();

			/**
			 * 撤销指定格式的文档格式解析器，撤销后外部负责维护分析器，CIndexWriter不再持有该分析器
			 * @param docType 文档类型
			 * @return 文档内容解析器
			 */
			CParser*	detachParser(doctype_t docType);

			/**
			 * 获取内部文档内容解析器对象
			 * @return 文档内容解析器对象，内部对象，外部不能删除
			 */
			CParser*	getParser(){return m_ppParsers[0];}		
		
			/**
			 * 获取内部索引器
			 * @return 内部索引器
			 * @memory 内部对象,外部不能删除
			 */
			CIndex*		getIndex(){return m_pIndex;}	

			/**
			 * 获取Cache中的下一个空文档,返回的空文档自动标记为非空
			 * @return Cache中的文档对象，内部维护，外部不能删除
			 */
			document::CDocument*	getDoc();

			/** 
			 * remove the last document in cache
			 */
			void		ungetDoc();
			
			/**
			 * 获取内存索引的索引桶读取器，内存中的索引数据也看作是一个索引桶，
			 * 该对象用于读取内存中的索引数据
			 * @return 返回索引桶读取器，非内部对象，需外部删除
			 */
			CIndexBarrelReader*	inMemoryReader();
		public:
			/**
			 * 根据参数文件中的参数设置建立索引
			 * @param pszCOnfigFile 参数文件			 
			 */
			static void	buildIndex(const tchar* pszCOnfigFile);

			/**
			 * 根据参数对象提供的参数建立索引
			 * @param pArgument 参数对象,非内部对象，外部维护
			 */
			static void buildIndex(firtex::utility::Configurator* pConfigurator);

			/**
			 * 根据参数文件中的参数设置生成建索引需要的所有对象
			 * @param argFile 参数文件
			 * @return Collection对象,可以调用<code>CCollection::scan()</code>开始进行建索引操作
			 * @memory 非内部对象,需要外部删除,
			 * @see CArgument
			 */
			static firtex::collection::CCollection*	prepareBuildIndex(const tchar* pszConfigFile);

			/**
			 * 根据参数文件中的参数设置生成建索引需要的所有对象
			 * @param pConfigurator 参数对象,非内部对象，外部维护
			 * @return Collection对象,可以调用<code>CCollection::scan()</code>开始进行建索引操作
			 * @memory 非内部对象,需要外部删除,
			 * @see CArgument
			 */
			static firtex::collection::CCollection*	prepareBuildIndex(firtex::utility::Configurator* pConfigurator);
		protected:
			/**
			 * 对文档进行内容分析
			 */
			inline void analyzeDoc(document::CDocument* pParsedDoc);

			/** 索引文档 */
			inline void	indexDoc(document::CDocument* pAnalyzedDoc);

		protected:
			/** 分析缓存中文档的内容 */
			void	analyzeCachedDocs();

			/** 定义文档的schema */
			void	defineSchema();
			
			/**
			 * 将Analyzer重组成数组，以加快Analyzer的定位
			 */
			void	prepareAnalyzer();
			
			/** 
			 * 根据Document Schema创建 
			 * @return 域工厂对象，非内部对象，外部删除
			 */
			CFactoryFinder*	createFactoryFinder();

			/** destroy cache for indexing */
			void	destroyCache();

			/** write cached index to database */
			void	writeCachedIndex();

			/** merge and write cached index to database */
			void	mergeAndWriteCachedIndex();

			/** merge and write cached index to database */
			void	mergeAndWriteCachedIndex2();

			/**create index barrel writer */
			void	createBarrelWriter();

			/**
			 * create index merger 
			 */
			void	createMerger();			
		protected:						
			CAnalyzer*				m_pAnalyzer;			///default analyzer
			CIndex*					m_pIndex;				///the index object related to the writer
			CIndexBarrelWriter*		m_pIndexBarrelWriter;	///for building in-memory index
			CBarrelsInfo*			m_pBarrelsInfo;			///reference to CIndex's m_pBarrelsInfo

			CIndexMerger*			m_pIndexMerger;			///for index merge			
			bool					m_bMerged;				

			CMemCache*				m_pMemCache;			///memory cache for indexing or merging
			document::CDocument**	m_ppCachedDocs;			///缓存在内存中的文档数组，目的是减少函数调用开销
			int32_t					m_nNumCachedDocs;		///缓存文档总数
			int32_t					m_nNumCacheUsed;		///缓冲使用情况

			count_t*				m_pCurDocCount;			///文档计数器
			docid_t*				m_pCurMinDocID;			///当前索引桶的最小文档编号
			CBarrelInfo*			m_pCurBarrelInfo;

			map<tstring,CAnalyzer*>	m_analyzers;			///每个字段对应的文档内容分析器

			CAnalyzer**				m_ppAnalyzers;
			int32_t					m_nNumAnalyzers;

			///每种文档类型对应的格式解析器，第一个是默认的解析器，即当没有指定文档格式时使用
			///默认的解析器解析文档格式，如果指定了文档格式但对应项是NULL也使用默认解析器
			CParser**				m_ppParsers;
			int32_t					m_nNumParsers;

			firtex::utility::Configurator*		m_pConfigurator;


			friend class CIndex;
			friend class CIndexReader;
			friend class firtex::collection::CCollection;
		};
		//////////////////////////////////////////////////////////////////////////
		//inline functions	
		inline document::CDocument* CIndexWriter::getDoc()
		{
			if(m_nNumCacheUsed >= m_nNumCachedDocs)
				flushDocuments();
			if(m_ppCachedDocs == NULL)
				return NULL;
			return m_ppCachedDocs[m_nNumCacheUsed++];
		}		

		inline void CIndexWriter::ungetDoc()
		{
			if(m_nNumCacheUsed > 0 )
			{
				m_ppCachedDocs[m_nNumCacheUsed]->setStatus(doc_status::UNPARSED);
				m_nNumCacheUsed--;
			}
		}

		inline void CIndexWriter::analyzeDoc(document::CDocument* pParsedDoc)
		{
			FIRTEX_CLOG(level::dbg) << _T("Analyzing...") << FIRTEX_ENDL;
			if(m_ppAnalyzers == NULL)
			{
				prepareAnalyzer();
			}
			document::CField* pField = NULL;
			document::CField* pTokenField = NULL;
			CTokens* pTokens = NULL;
			CReader* pReader;

			int32_t nNumField = 0;
			pParsedDoc->startIterator();	
			while (pParsedDoc->hasNext()) 
			{
				pField = pParsedDoc->next();	                

				if(pField->isAnalyzed())//需要Analyze
				{
					switch(pField->getType())
					{
					case CField::READER:
						{							
							pReader = pField->readerValue();
							pTokens = m_ppAnalyzers[pField->getID()]->nextTokens(pReader);
						}						
						break;					
					case CField::TEXT:
						{							
							CFXStringA*  pText = pField->textAValue();
							if(pText->length() > 0)
							{													
								pTokens = m_ppAnalyzers[pField->getID()]->nextTokens( (*pText),pParsedDoc->getEncoding());
							}
							else 
							{
								//FIRTEX_CLOG(level::warn) << _T("The length of field:") << pField->getName() << _T(" is zero.") << FIRTEX_ENDL;
							}

						}
						break;					
					case CField::TEXTW:
						{							
							CFXStringW*  pText = pField->textWValue();
							if(pText->length() > 0)
							{													
								pTokens = m_ppAnalyzers[pField->getID()]->nextTokens( (*pText));
							}
							else 
							{
								//FIRTEX_CLOG(level::warn) << _T("The length of field:") << pField->getName() << _T(" is zero.") << FIRTEX_ENDL;
							}

						}
						break;					
					}
					
					if(pField->tokensValue() == NULL)
					{						
						pField->tokensValue(pTokens,false);
					}
				}								
				nNumField++;
			}			
			pParsedDoc->setStatus(document::doc_status::ANALYZED);			
		}		
	}
}
#endif
