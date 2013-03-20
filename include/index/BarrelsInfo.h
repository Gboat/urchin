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
// Created	: 2005/12/27
//

#ifndef _SEGMENTINFO_H
#define _SEGMENTINFO_H

#include "../utility/StdHeader.h"
#include "../store/Directory.h"
#include "../utility/FXString.h"
#include "FieldsInfo.h"

#include <vector>
using namespace std;
using namespace firtex::store;
using namespace firtex::utility;

#define BARRELS_INFONAME	_T("barrels")

namespace firtex
{
	namespace index
	{
		class CIndexBarrelWriter;
		///barrel information, description of index barrel
		class CBarrelInfo
		{
		public:
			CBarrelInfo()
				: m_nMinDocID(0)	
				, m_nNumDocs(0)
				, m_pBarrelWriter(NULL)
				, m_nDeletedDocs(0)
			{
			}
			CBarrelInfo(const tstring& name,docid_t minDID,count_t count)
				: m_nMinDocID(minDID)
				, m_sBarrelName(name)
				, m_nNumDocs(count)
				, m_pBarrelWriter(NULL)
				, m_nDeletedDocs(0)
			{				
			}
			CBarrelInfo(const CBarrelInfo& bi)
			{
				m_nMinDocID = bi.m_nMinDocID;
				m_sBarrelName = bi.m_sBarrelName;
				m_nNumDocs = bi.m_nNumDocs;
				m_pBarrelWriter = bi.m_pBarrelWriter;
				m_nDeletedDocs = bi.m_nDeletedDocs;
			}
			~CBarrelInfo()
			{
			}
		public:
			const tstring&		getName(){return m_sBarrelName;}
			void				setName(const tstring& name){m_sBarrelName = name;}

			docid_t				getMinDocID(){return m_nMinDocID;}
			void				setMinDocID(docid_t did){m_nMinDocID = did;}

			count_t				getDocCount(){return m_nNumDocs;}
			void				setDocCount(count_t count){m_nNumDocs = count;}

			count_t				numDeletedDocs(){return m_nDeletedDocs;}
			void				setDeletedDocs(count_t count){m_nDeletedDocs = count;}
			void				deleteDocument()
			{
				if(m_nDeletedDocs < m_nNumDocs)
					m_nDeletedDocs++;				
			}

			CIndexBarrelWriter*	getWriter(){return m_pBarrelWriter;}
			void				setWriter(CIndexBarrelWriter* pWriter){m_pBarrelWriter = pWriter;}

			/**
			 * delete index files of this barrel
			 */
			void				remove(CDirectory* pDirectory);
			void				rename(CDirectory* pDirectory,const tstring& newName);
		public:
			static bool greater (CBarrelInfo* pElem1, CBarrelInfo* pElem2 )
			{
				return pElem1->getDocCount() > pElem2->getDocCount();
			}
		protected:
			tstring				m_sBarrelName;		///name of barrel	
			docid_t				m_nMinDocID;		///minimal document id in this barrel
			count_t				m_nNumDocs;			///number of documents in this barrel
			count_t				m_nDeletedDocs;		///number of deleted documents

			CIndexBarrelWriter*	m_pBarrelWriter;	///only valid when this barrel is a in-memory barrel,otherwise NULL.

			friend class CBarrelsInfo;
			friend class CIndexWriter;
		};
		
		///barrels information, description of all barrels in the index database
		class CBarrelsInfo
		{
		public:
			CBarrelsInfo();
			~CBarrelsInfo(void);
		public:
			/**
			 * create a new name of barrel
			 * @return new name of barrel
			 */
			const	tstring newBarrel();

			/**
			 * add a barrel
			 * @param name barrel name
			 * @param minDID base id of document in this barrel
			 * @param docCount number of documents contains in this barrel
			 */
			void	addBarrel(const tchar* name,docid_t minDID,count_t docCount);

			/**
			 * add a barrel
			 * @param pBarrelInfo barrel information
			 * @param bCopy copy pBarrelInfo or not
			 */
			void	addBarrel(CBarrelInfo* pBarrelInfo,bool bCopy = true);

			/**
			 * read barrels information from directory
			 * @param pDirectory directory 
			 */
			void	read(CDirectory* pDirectory);

			/**
			 * write barrels information to directory
			 * @param pDirectory directory 
			 */
			void	write(CDirectory* pDirectory);

			/**
			 * remove all barrels
			 */
			void	remove(CDirectory* pDirectory);

			/**
			 * clear a specific barrel
			 * @param pDirectory directory 
			 * @param barrelname barrel name to be removed
			 */
			void	removeBarrel(CDirectory* pDirectory,const tstring& barrelname);
			
			/**
			 * set schema of index
			 * @param pSchema schema of index
			 */
			void	setSchema(CDocumentSchema* pSchema);

			/**
			 * get schema of the database
			 */
			void	extractSchema(CDocumentSchema* pSchema);

			/**
			 * get number of barrels			 
			 */
			int32_t	getBarrelCount();

			/**
			 * get number of documents in all barrels
			 */
			int32_t	getDocCount();

			/**
			 * get number of deleted documents
			 * @return number of deleted documents
			 */
			count_t	numDeletedDocs();

			/**
			 * undelete all documents;
			 */
			void	undeleteDocuments();
			
			/**
			 * get the version of index database, it will be "firtex.version.", where "version" is the version number of FirteX
			 * @return version string
			 */
			const tchar*	getVersion(){return (const tchar*)m_sVersion;}

			/**
			 * set the version of index database, it will be "firtex.version.", where "version" is the version number of FirteX
			 * @param ver version string
			 */
			void			setVersion(const tchar* ver);

			/** 
			 * get barrel info by barrel name
			 * @param barrel name of barrel
			 * @return info of barrel
			 */
			CBarrelInfo*	getBarrelInfo(const tchar* barrel);

			/**
			 * get the last barrel
			 * @return the  last barrel
			 */
			CBarrelInfo*	getLastBarrel();

			/**
			 * delete last barrel
			 */
			void			deleteLastBarrel();

			/**
			 * sort barrels by number of documents in barrel
			 * @param pDirectory directory where barrels stored
			 */
			void			sort(CDirectory* pDirectory);

			/**
			 * clear barrels and release resources
			 */
			void			clear();
		public:
			CBarrelInfo* operator [](int32_t i){return m_barrelInfos[i];}			
		public:
			void			startIterator();
			bool			hasNext();
			CBarrelInfo*	next();
		protected:
			CFXString				m_sVersion;
			int32_t					m_nBarrelCounter;	///barrel counter									
			CDocumentSchema*		m_pDocSchema;
			
			vector<CBarrelInfo*>	m_barrelInfos;			
			vector<CBarrelInfo*>::iterator m_barrelsiterator;
		};
		
		//////////////////////////////////////////////////////////////////////////
		//Inline functions

		inline void CBarrelsInfo::startIterator()
		{
			m_barrelsiterator = m_barrelInfos.begin();
		}
		inline  bool CBarrelsInfo::hasNext()
		{
			return (m_barrelsiterator != m_barrelInfos.end());
		}
		inline  CBarrelInfo* CBarrelsInfo::next()
		{
			return *m_barrelsiterator++;
		}		

	}
}

#endif
