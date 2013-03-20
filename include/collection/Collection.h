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
#ifndef __COLLECTION_H
#define __COLLECTION_H

#include "../utility/StdHeader.h"
#include "../document/doctypedef.h"
#include "../index/IndexWriter.h"
#include "../thread/FirteXThreads.h"

#include <map>

using namespace firtex::index;
using namespace std;

namespace firtex
{
	namespace collection
	{		
		class CCollection : public firtex::thread::CRunnalble, public LOCK_BASE
		{

		public:
			class Creator
			{
			protected:
				Creator()
					: m_identifier(_T("collection"))
				{}
			public:
				~Creator(){}
			public:
				/**
				 * create a collection object
				 * @param path path of collection
				 */
				virtual CCollection* create(const tchar* path) = 0;		

				/** 
				 * get the identifier of collection
				 */
				virtual const tchar* getIdentifier(){return m_identifier.c_str();}
			protected:
				tstring m_identifier;
			};

		public:
			enum RunMode
			{
				SYNC,	/// run synchronously.
				ASYNC	/// run asynchronously.
			};		
		protected:
			static const int STATUS_CREATED			= 0;
			static const int STATUS_RUNNING			= 1;
			static const int STATUS_STOP_PENDING	= 2;
			static const int STATUS_STOPPED			= 3;
		public:
			CCollection(void);	

			/**
			 * constructor
			 * @param pIndexWriter an index writer, internal object
			 * @param bDestroy destroy index or not
			 */
			CCollection(firtex::index::CIndexWriter* pIndexWriter,bool bDestroy = false);
			virtual ~CCollection(void);						
		public:
			/**
			 * set index writer for collection
			 * @param pWriter index writer object
			 * @param bDestroy destroy writer object or not
			 */
			void				setWriter(CIndexWriter* pWriter,bool bDestroy);

			/**
			 * get the index writer object of collection
			 * @return the internal index writer object
			 */
			CIndexWriter*		getWriter(){return m_pIndexWriter;}

			/**
			 * set parameter for collection
			 * @param pszParam parameter for collection. format:param_name=param_value
			 */
			virtual void		setParam(const tchar* pszParam){}
			
		public:
			/** stop the process */
			virtual void stop();

			/**
			 * stop command has been requested or not 
			 * @return true for yes, false for no
			 */
			virtual bool stopRequested();

			/**
			 * is process stopped?
			 * @return true for yes 
			 */
			virtual bool stopped();

		public:
			
			/**
			 * scan document collection and index it
			 * @param nRunMode scan synchronously or asynchronously
			 */
			virtual void		scan(RunMode nRunMode = SYNC);

			/**
			 * add a single file to the collection
			 * @param filename the file path
			 */
			virtual void		addFile(const tchar* filename);	
		protected:
			/**
			 * scan the collection and extract each document for indexing, MUST be implemented by sub-class.
			 */
			virtual void		scanInternal() = 0;

			/** 
			 * define the schema of collection, MUST be implemented by sub-class.
			 * @param pSchema the schema object where schema is defined
			 */
			virtual void		defineSchema(CDocumentSchema* pSchema) = 0;
		protected:
			/** implement the interface of CRunnalble */
			void				run();
		protected:
			CIndexWriter*		m_pIndexWriter;
			bool				m_bDestroy;			
			bool				m_bTerminate;
		protected:			
			RunMode						m_nRunMode;
			int32_t						m_nStatus;
			firtex::thread::CThread*	m_pThread;
			//firtex::thread::CMutex		LOCK;
		};			
		//////////////////////////////////////////////////////////////////////////
		//					
	}
}


#endif
