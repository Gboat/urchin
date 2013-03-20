//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
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
			 * constructor��must be called by CIndex
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
			 * ����Ĭ�ϵ��ĵ����ݷ�����
			 * @param pAnalyzer �ĵ����ݷ�����,���������ڲ�ά�����ⲿ����ɾ��
			 */
			void		attachAnalyzer(CAnalyzer* pAnalyzer);

			/**
			 * ����ָ���ֶε��ĵ����ݷ�����
			 * @param field �ֶ�����
			 * @param pAnalyzer �ĵ����ݷ�����,���������ڲ�ά�����ⲿ����ɾ��
			 */
			void		attachAnalyzer(const tchar* field,CAnalyzer* pAnalyzer);

			/**
			 * ����ָ���ֶε��ĵ����ݷ�����
			 * @param field �ֶ�����
			 * @param identifier �ĵ����ݷ�������ʾ			 
			 * @param szParam parameter for analyzer
			 * @return attached analyzer
			 */
			CAnalyzer*	attachAnalyzer(const tchar* field,const tchar* identifier,const tchar* szParam);

			/**
			 * ����Ĭ�ϵ��ĵ����ݷ��������������ⲿ����ά����������CIndexWriter���ٳ��и÷�����
			 * @return �ĵ����ݷ��������ⲿά��
			 */
			CAnalyzer*	detachAnalyzer();

			/**
			 * ����ָ���ֶε��ĵ����ݷ��������������ⲿ����ά����������CIndexWriter���ٳ��и÷�����
			 * @param field �ֶ�����
			 * @return �ĵ����ݷ���������������ڶ�Ӧ�ķ����䣬�򷵻�NULL
			 */
			CAnalyzer*	detachAnalyzer(const tchar* field);

			/**
			 * ����ָ���ֶε��ĵ����ݷ��������������ⲿ����ά����������CIndexWriter���ٳ��и÷�����
			 * @param fieldid �ֶα��
			 * @return �ĵ����ݷ���������������ڶ�Ӧ�ķ����䣬�򷵻�NULL
			 */
			CAnalyzer*	detachAnalyzer(fieldid_t fieldid);

			/**
			 * ��ȡ�ڲ��ĵ����ݷ���������
			 * @return �ĵ����ݷ����������ڲ������ⲿ����ɾ��
			 */
			CAnalyzer*	getAnalyzer(){return m_pAnalyzer;}

			/**
			 * ����Ĭ�ϵ��ĵ���ʽ�����������������ڲ�ά�������������ڲ�ά�����ⲿ����ɾ��
			 * @param pParser �ĵ���ʽ������
			 */
			void		attachParser(CParser* pParser);

			/**
			 * ����ָ����ʽ�ĵ��ĵ���ʽ�����������������ڲ�ά�������������ڲ�ά�����ⲿ����ɾ��
			 * @param docType �ĵ�����
			 * @param pParser �ĵ���ʽ������
			 */
			void		attachParser(doctype_t docType,CParser* pParser);

			/**
			 * ����Ĭ�ϵ��ĵ���ʽ���������������ⲿ����ά����������CIndexWriter���ٳ��и÷�����
			 * @return �ĵ����ݽ�����
			 */
			CParser*	detachParser();

			/**
			 * ����ָ����ʽ���ĵ���ʽ���������������ⲿ����ά����������CIndexWriter���ٳ��и÷�����
			 * @param docType �ĵ�����
			 * @return �ĵ����ݽ�����
			 */
			CParser*	detachParser(doctype_t docType);

			/**
			 * ��ȡ�ڲ��ĵ����ݽ���������
			 * @return �ĵ����ݽ����������ڲ������ⲿ����ɾ��
			 */
			CParser*	getParser(){return m_ppParsers[0];}		
		
			/**
			 * ��ȡ�ڲ�������
			 * @return �ڲ�������
			 * @memory �ڲ�����,�ⲿ����ɾ��
			 */
			CIndex*		getIndex(){return m_pIndex;}	

			/**
			 * ��ȡCache�е���һ�����ĵ�,���صĿ��ĵ��Զ����Ϊ�ǿ�
			 * @return Cache�е��ĵ������ڲ�ά�����ⲿ����ɾ��
			 */
			document::CDocument*	getDoc();

			/** 
			 * remove the last document in cache
			 */
			void		ungetDoc();
			
			/**
			 * ��ȡ�ڴ�����������Ͱ��ȡ�����ڴ��е���������Ҳ������һ������Ͱ��
			 * �ö������ڶ�ȡ�ڴ��е���������
			 * @return ��������Ͱ��ȡ�������ڲ��������ⲿɾ��
			 */
			CIndexBarrelReader*	inMemoryReader();
		public:
			/**
			 * ���ݲ����ļ��еĲ������ý�������
			 * @param pszCOnfigFile �����ļ�			 
			 */
			static void	buildIndex(const tchar* pszCOnfigFile);

			/**
			 * ���ݲ��������ṩ�Ĳ�����������
			 * @param pArgument ��������,���ڲ������ⲿά��
			 */
			static void buildIndex(firtex::utility::Configurator* pConfigurator);

			/**
			 * ���ݲ����ļ��еĲ����������ɽ�������Ҫ�����ж���
			 * @param argFile �����ļ�
			 * @return Collection����,���Ե���<code>CCollection::scan()</code>��ʼ���н���������
			 * @memory ���ڲ�����,��Ҫ�ⲿɾ��,
			 * @see CArgument
			 */
			static firtex::collection::CCollection*	prepareBuildIndex(const tchar* pszConfigFile);

			/**
			 * ���ݲ����ļ��еĲ����������ɽ�������Ҫ�����ж���
			 * @param pConfigurator ��������,���ڲ������ⲿά��
			 * @return Collection����,���Ե���<code>CCollection::scan()</code>��ʼ���н���������
			 * @memory ���ڲ�����,��Ҫ�ⲿɾ��,
			 * @see CArgument
			 */
			static firtex::collection::CCollection*	prepareBuildIndex(firtex::utility::Configurator* pConfigurator);
		protected:
			/**
			 * ���ĵ��������ݷ���
			 */
			inline void analyzeDoc(document::CDocument* pParsedDoc);

			/** �����ĵ� */
			inline void	indexDoc(document::CDocument* pAnalyzedDoc);

		protected:
			/** �����������ĵ������� */
			void	analyzeCachedDocs();

			/** �����ĵ���schema */
			void	defineSchema();
			
			/**
			 * ��Analyzer��������飬�Լӿ�Analyzer�Ķ�λ
			 */
			void	prepareAnalyzer();
			
			/** 
			 * ����Document Schema���� 
			 * @return �򹤳����󣬷��ڲ������ⲿɾ��
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
			document::CDocument**	m_ppCachedDocs;			///�������ڴ��е��ĵ����飬Ŀ���Ǽ��ٺ������ÿ���
			int32_t					m_nNumCachedDocs;		///�����ĵ�����
			int32_t					m_nNumCacheUsed;		///����ʹ�����

			count_t*				m_pCurDocCount;			///�ĵ�������
			docid_t*				m_pCurMinDocID;			///��ǰ����Ͱ����С�ĵ����
			CBarrelInfo*			m_pCurBarrelInfo;

			map<tstring,CAnalyzer*>	m_analyzers;			///ÿ���ֶζ�Ӧ���ĵ����ݷ�����

			CAnalyzer**				m_ppAnalyzers;
			int32_t					m_nNumAnalyzers;

			///ÿ���ĵ����Ͷ�Ӧ�ĸ�ʽ����������һ����Ĭ�ϵĽ�����������û��ָ���ĵ���ʽʱʹ��
			///Ĭ�ϵĽ����������ĵ���ʽ�����ָ�����ĵ���ʽ����Ӧ����NULLҲʹ��Ĭ�Ͻ�����
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

				if(pField->isAnalyzed())//��ҪAnalyze
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
