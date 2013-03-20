//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/10/11
//
#include "index/Index.h"
#include "store/FSDirectory.h"
#include "index/FieldFactory.h"
#include "index/BarrelsInfo.h"

namespace firtex
{
	namespace index
	{
		CIndex::CIndex(void)
			: m_pDirectory(NULL)
			, m_pReader(NULL)
			, m_pWriter(NULL)
			, m_pBarrelsInfo(NULL)
			, m_accessMode(ACCESS_CREATE)
			, m_pFactoryFinder(NULL)
			, m_pDocSchema(new CDocumentSchema())			
			, m_bDirty(false)
		{				
		}
		CIndex::CIndex(const tchar* path,ACCESS_MODE am)
			: m_pReader(NULL)
			, m_pDirectory(NULL)
			, m_pWriter(NULL)
			, m_pBarrelsInfo(NULL)
			, m_accessMode(am)
			, m_pFactoryFinder(NULL)
			, m_pDocSchema(NULL)
			, m_bDirty(false)
		{		
			open(path,am);
			m_pDocSchema = new CDocumentSchema();
		}
		CIndex::CIndex(CDirectory* pDirectory,ACCESS_MODE am)			
			:m_pReader(NULL)
			,m_pWriter(NULL)
			,m_accessMode(am)
			,m_pFactoryFinder(NULL)
			,m_pDocSchema(new CDocumentSchema())			
			,m_bDirty(false)
		{
			m_pBarrelsInfo = new CBarrelsInfo();

			m_pBarrelsInfo->read(pDirectory);//¶ÁÈ¡BarrelsInfo
			if((am & ACCESS_CREATE) == ACCESS_CREATE)
			{
				m_pBarrelsInfo->remove(getDirectory());
			}
			else
			{			
				if(_tcsicmp(m_pBarrelsInfo->getVersion(),FIRTEX_VERSION))
				{
					delete m_pBarrelsInfo;
					m_pBarrelsInfo = NULL;
					FIRTEX_THROW2(VERSION_ERROR,_T("incompatible version."));
				}	
			}			
		}
		CIndex::~CIndex(void)
		{
			close();
		}

		void CIndex::open(const tchar* path,ACCESS_MODE am)
		{
			close();						

			if ((m_accessMode & ACCESS_APPEND) == ACCESS_APPEND )
			{
				tstring strBarrel = path;
				strBarrel += PATH_DELIMITERA;
				strBarrel += BARRELS_INFONAME;
				if(!firtex::utility::CMisc::dirExists(path) || !firtex::utility::CMisc::dirExists(strBarrel.c_str()))
				{
					m_accessMode = m_accessMode & (~ACCESS_APPEND);
					m_accessMode = m_accessMode|ACCESS_CREATE;
				}				
			}

			m_pDirectory = CFSDirectory::getDirectory(path,((m_accessMode & ACCESS_CREATE) == ACCESS_CREATE));

			m_pBarrelsInfo = new CBarrelsInfo();			

			m_pBarrelsInfo->read(m_pDirectory);//¶ÁÈ¡BarrelsInfo
			if((am & ACCESS_CREATE) == ACCESS_CREATE)
			{
				m_pBarrelsInfo->remove(getDirectory());
			}
			else
			{				
				if(_tcsicmp(m_pBarrelsInfo->getVersion(),FIRTEX_VERSION))
				{
					delete m_pBarrelsInfo;
					m_pBarrelsInfo = NULL;
					FIRTEX_THROW2(VERSION_ERROR,"incompatible version.");
				}	
			}
		}

		void CIndex::open(CDirectory* pDirectory,ACCESS_MODE am)
		{
			close();
			m_pBarrelsInfo = new CBarrelsInfo();

			m_pBarrelsInfo->read(pDirectory);//¶ÁÈ¡BarrelsInfo
			if((am & ACCESS_CREATE) == ACCESS_CREATE)
			{
				m_pBarrelsInfo->remove(getDirectory());
			}
			else
			{			
				if(_tcsicmp(m_pBarrelsInfo->getVersion(),FIRTEX_VERSION))
				{
					delete m_pBarrelsInfo;
					m_pBarrelsInfo = NULL;
					FIRTEX_THROW2(VERSION_ERROR,"incompatible version.");
				}	
			}
		}
		void CIndex::close()
		{			
			if(m_pReader)
			{
				delete m_pReader;
				m_pReader = NULL;
			}
			if(m_pWriter)
			{
				m_pWriter->flushDocuments();
				delete m_pWriter;
				m_pWriter = NULL;
			}
			if(m_pFactoryFinder)
			{
				delete m_pFactoryFinder; 
				m_pFactoryFinder = NULL;
			}
			if(m_pDocSchema)
			{
				delete m_pDocSchema;
				m_pDocSchema = NULL;
			}
			if(m_pBarrelsInfo)
			{
				if(m_bDirty)
					m_pBarrelsInfo->write(getDirectory());
				delete m_pBarrelsInfo;
				m_pBarrelsInfo = NULL;
			}
			if(m_pDirectory)
			{
				m_pDirectory->close();				
				m_pDirectory = NULL;
			}		
			m_bDirty = false;
		}

		void CIndex::setDocSchema(CDocumentSchema* pSchema)
		{
			FIRTEX_ASSERT((m_pWriter==NULL),_T("Call setDocSchema() before calling acquireWriter()"));
			if(m_pDocSchema)
				delete m_pDocSchema;
				m_pDocSchema = NULL;
			m_pDocSchema = new CDocumentSchema(*pSchema);			
		}

		CIndexReader* CIndex::acquireReader()
		{
			if(m_pReader)
				return m_pReader;

			if( (m_accessMode != ACCESS_MODIFY) && (m_accessMode != ACCESS_READONLY) && ((m_accessMode & ACCESS_SEARCH) != ACCESS_SEARCH))
			{
				FIRTEX_CLOG(level::info) << _T("acquireReader(): you are in ACCESS_WRITE or ACCESS_APPEND mode, can't aquire reader of the index") << FIRTEX_ENDL;
				return NULL;
			}

			m_pReader = new CIndexReader(this);
			return m_pReader;
		}

		CIndexWriter* CIndex::acquireWriter(CParser* pParser,CAnalyzer* pAnalyzer)
		{
			if(m_pWriter)
			{
				if(pParser)
					m_pWriter->attachParser(pParser);
				if(pAnalyzer)
					m_pWriter->attachAnalyzer(pAnalyzer);				
				return m_pWriter;
			}
			if( (m_accessMode == ACCESS_READONLY) || ((m_accessMode == ACCESS_SEARCH)))
			{
				FIRTEX_CLOG(level::info) << _T("acquireWriter(): you are in ACCESS_READONLY mode, can't aquire writer of the index") << FIRTEX_ENDL;
				return NULL;
			}
			m_pWriter = new CIndexWriter(pParser,pAnalyzer,this);

			return m_pWriter;
		}
		CIndexWriter* CIndex::acquireWriter()
		{
			if(m_pWriter)
			{					
				return m_pWriter;
			}
			if( (m_accessMode == ACCESS_READONLY) || ((m_accessMode == ACCESS_SEARCH)))
			{
				FIRTEX_CLOG(level::info) << _T("acquireWriter(): you are in ACCESS_READONLY mode, can't aquire writer of the index") << FIRTEX_ENDL;
				return NULL;
			}
			m_pWriter = new CIndexWriter(this);

			return m_pWriter;
		}				

		CFactoryFinder*	CIndex::getFactoryFinder()
		{
			if(m_pFactoryFinder)
				return m_pFactoryFinder;			
			if((m_accessMode & ACCESS_CREATE) != ACCESS_CREATE)
			{
				m_pFactoryFinder = createFactoryFinderFromExistIndex();
			}
			if( (m_pFactoryFinder == NULL) && ( (m_accessMode & ACCESS_APPEND) || (m_accessMode & ACCESS_CREATE)))
			{
				if(m_pWriter == NULL)
					return NULL;
				m_pFactoryFinder = m_pWriter->createFactoryFinder();
			}
			FIRTEX_ASSERT( m_pFactoryFinder,_T("Create field factories failed."));
			return m_pFactoryFinder;
		}
		CFactoryFinder* CIndex::createFactoryFinderFromExistIndex()
		{
			CDocumentSchema* pDocSchema = createSchemaFromExistIndex();
			CFactoryFinder* pFactoryFinder = new CFactoryFinder();		
			pDocSchema->startIterator();
			CSchemaItem* pItem = NULL;
			while (pDocSchema->hasNext())
			{
				pItem = pDocSchema->next();
				if(pItem->getIndex() != Index::NO || pItem->getForwardIndex() != ForwardIndex::NO)
				{
					const tchar* _identifier = pItem->getComponent(_T("indexer"));
					if(_identifier && _tcsicmp(_identifier,_T("")))
					{
						CFieldFactory* pFactory = pFactoryFinder->mapFactory(pItem->getId(),_identifier);						
						if(!pFactory)
						{
							tstring str = _T("create field factory: ");
							str += _identifier;
							str += _T(" failed.");
							FIRTEX_THROW3(ANALYZE_ERROR,str);
						}
					}
				}
			}		
			if(pFactoryFinder->numFactories() <= 0)
			{
				delete pFactoryFinder;
				FIRTEX_THROW3(ANALYZE_ERROR,_T("no field factory created."));
			}
			return pFactoryFinder;
		}
		
		CDocumentSchema* CIndex::createSchemaFromExistIndex()
		{
			m_pDocSchema->clear();
			m_pBarrelsInfo->extractSchema(m_pDocSchema);
			return m_pDocSchema;
		}
		void CIndex::setDirty(bool bDirty)
		{
			if(bDirty && m_pReader)
				m_pReader->close();
			m_bDirty = bDirty;
		}
		void CIndex::setDirectory(CDirectory* pDir)
		{
			if(m_pDirectory)
				m_pDirectory->close();
			m_pDirectory = pDir;
		}
	}
}
