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
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/11/9
//
#include "collection/Collection.h"
#include "parser/ParserFactory.h"
#include "analyzer/AnalyzerFactory.h"
#include "index/Index.h"
#include "index/IndexWriter.h"

namespace firtex
{
	namespace collection
	{		

		CCollection::CCollection(void)
			: m_pIndexWriter(NULL)
			, m_bTerminate(false)
			, m_bDestroy(false)
			, m_nStatus(STATUS_CREATED)
			, m_pThread(NULL)			
			, m_nRunMode(SYNC)			
		{			
		}		
		CCollection::CCollection(firtex::index::CIndexWriter* pIndexWriter,bool bDestroy)
			: m_pIndexWriter(pIndexWriter)
			, m_bDestroy(bDestroy)
			, m_bTerminate(false)
			, m_nStatus(STATUS_CREATED)
			, m_pThread(NULL)
			, m_nRunMode(SYNC)			
		{				
		}			
		CCollection::~CCollection(void)
		{
			if(m_bDestroy && m_pIndexWriter)
			{
				CIndex* pIndex = m_pIndexWriter->getIndex();
				delete pIndex;
				m_pIndexWriter = NULL;
			}			

			if(m_nRunMode == CCollection::ASYNC)
			{
				if (!stopped())
					stop();
				while (!stopped()) 
				{
					m_pThread->sleep(20);
				}
				if(m_pThread)
				{
					delete m_pThread;
					m_pThread = NULL;
				}
			}
		}

		void CCollection::setWriter(CIndexWriter* pWriter,bool bDestroy)
		{
			if(pWriter == m_pIndexWriter)
				m_bDestroy = bDestroy;
			else 
			{
				if(m_bDestroy && m_pIndexWriter)
				{
					CIndex* pIndex = m_pIndexWriter->getIndex();
					delete pIndex;				
				}
				m_pIndexWriter = pWriter;
				m_bDestroy = bDestroy;
			}
		}
		void CCollection::scan(RunMode nRunMode)
		{
			///first,define collection's schema
			defineSchema(m_pIndexWriter->getDocSchema());
			///then,define index writer's schema
			m_pIndexWriter->defineSchema();
			
			if(nRunMode == SYNC)
			{
				m_nRunMode = nRunMode;
				m_nStatus = STATUS_RUNNING;
				scanInternal();
				getWriter()->close();
			}
			else 
			{
				SCOPED_LOCK(LOCK);
				m_nRunMode = nRunMode;				
				m_nStatus = STATUS_RUNNING;
				if(!m_pThread)
					m_pThread = new firtex::thread::CThread();
				m_pThread->spawn(this,true);
			}
		}
		void CCollection::run()
		{
			scanInternal();
			getWriter()->close();

			SCOPED_LOCK(LOCK);

			m_nStatus = STATUS_STOPPED;
		}
		/*void CCollection::close()
		{
			if(m_pIndexWriter)
				m_pIndexWriter->close();
		}*/		

		void CCollection::addFile(const tchar* filename)
		{
			throw CUnsupportedOperationException("CCollection.addFile");			
		}

		void CCollection::stop()
		{
			SCOPED_LOCK(LOCK);
			if (m_nStatus != STATUS_STOPPED)
				m_nStatus = STATUS_STOP_PENDING;
		}
		
		bool CCollection::stopRequested()
		{
			int status;
			SCOPED_LOCK(LOCK);
			status = this->m_nStatus;			
			if (status == STATUS_STOP_PENDING)
				return true;
			else
				return false;
		}

		bool CCollection::stopped()
		{
			int status;
			SCOPED_LOCK(LOCK);
			status = this->m_nStatus;			
			if (status == STATUS_CREATED)
				return false;
			else if (status == STATUS_STOPPED)
				return true;
			else
				return false;
		}
	}
}
