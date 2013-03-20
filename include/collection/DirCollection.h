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
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/7/3
//
#ifndef _DIRCOLLECTION_H
#define _DIRCOLLECTION_H

#include "Collection.h"

#include <string>
using namespace std;

namespace firtex
{
	namespace collection
	{
		class CDirCollection :	public CCollection
		{
		public:		
			class Creator : public CCollection::Creator
			{	
			protected:
				Creator()
				{
					m_identifier += _T(".directory");
				}
			public:
				~Creator(){}
			public:
				CCollection* create(const tchar* path){return new CDirCollection(path);}
			public:
				static Creator* getInstance()
				{
					static CDirCollection::Creator INSTANCE;
					return &INSTANCE;
				}
			};
		public:
			/**
			 * constructor
			 * @param dir directory of document collection
			 */
			CDirCollection(const tchar* dir);

			/**
			 * constructor
			 * @param dir directory of document collection
			 * @param pIndexWriter index writer object
			 */
			CDirCollection(const tchar* dir,CIndexWriter* pIndexWriter);

			/**
			 * constructor
			 * @param dir directory of document collection
			 * @param bAutoDetectDocType detect document type automatically or not
			 */
			CDirCollection(const tchar* dir,bool bAutoDetectDocType);

			/**
			 * constructor
			 * @param dir directory of document collection
			 * @param pIndexWriter index writer object
			 * @param bAutoDetectDocType detect document type automatically or not
			 */
			CDirCollection(const tchar* dir,CIndexWriter* pIndexWriter,bool bAutoDetectDocType);

			virtual ~CDirCollection(void);
		public:
			void		addFile(const tchar* filename);

			/**
			 * set parameter for collection
			 * @param pszParam parameter for collection. format:param_name=param_value
			 */
			void		setParam(const tchar* pszParam);
		protected:		
			/**
			 * 扫描处理文档集，子类实现，一般从文档集中抽取出单个文档，交给CIndexWriter对象处理
			 */
			void		scanInternal();		

			/** 
			 * define collection's schema
			 * @param pSchema schema
			 */
			void		defineSchema(CDocumentSchema* pSchema);

		protected:
			/**
			 * enumerate directory and index files in it
			 * @param dir directory path
			 */
			virtual void		indexDocs(const tchar* dir);

		protected:
			/**
			 * get doc type 
			 * @param path file path
			 * @return doc type
			 */
			inline doctype_t	getDocType(const tchar* path);
		protected:			
			tstring		m_sDirectory;		
			schemaid_t	m_schemas[2];
			bool		m_bAutoDetectDocType;
		};
		//////////////////////////////////////////////////////////////////////////
		//inline function
		inline doctype_t CDirCollection::getDocType(const tchar* path)
		{
			tchar* pszExt = (tchar*)path + _tcslen(path) - 1;
			while ( (*pszExt != _T('.')) && (pszExt > path))
			{
				pszExt--;
			}
			if(pszExt > path)
			{
				pszExt++;
				return firtex::document::CDocumentType::getInstance()->nameToType(pszExt);
			}
			return firtex::document::doc_type::UNKNOWN;
		}
	}
}


#endif
