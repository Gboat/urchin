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
// Created	: 2005/12/27
//

#include "index/BarrelsInfo.h"
#include "store/IndexInput.h"
#include "store/IndexOutput.h"
#include "store/Directory.h"
#include "index/IndexBarrelWriter.h"
#include "utility/StringUtils.h"
#include "document/Field.h"
#include "utility/XML.h"
#include <sstream>
#include <fstream>
using namespace std;
using namespace firtex::store;

namespace firtex
{
	namespace index
	{

		//////////////////////////////////////////////////////////////////////////
		//
		void CBarrelInfo::remove(CDirectory* pDirectory)
		{
			try
			{				
				pDirectory->batDeleteFiles(m_sBarrelName);				
			}
			catch(CFirteXException& fe)
			{
				FIRTEX_RETHROW(fe);
			}
			catch (...) 
			{
				FIRTEX_THROW3(FILEIO_ERROR,_T("CBarrelInfo::remove():remove failed."));
			}
		}

		void CBarrelInfo::rename(CDirectory* pDirectory,const string& newName)
		{
			try
			{				
				if(m_sBarrelName == newName)
					return ;
				if(m_pBarrelWriter)///in-memory index barrel
				{
					m_pBarrelWriter->rename(newName.c_str());
				}
				else 
				{
					pDirectory->batRenameFiles(m_sBarrelName,newName);
				}
				m_sBarrelName = newName;
			}
			catch(CFirteXException& fe)
			{
				FIRTEX_RETHROW(fe);
			}
			catch (...) 
			{
				FIRTEX_THROW3(FILEIO_ERROR,_T("CBarrelInfo::rename():rename failed."));
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//
		CBarrelsInfo::CBarrelsInfo()
			: m_pDocSchema(NULL)
			, m_nBarrelCounter(0)			
			, m_sVersion(FIRTEX_VERSION)
		{			
		}

		CBarrelsInfo::~CBarrelsInfo(void)
		{
			clear();
			m_pDocSchema = NULL;
		}
		int32_t CBarrelsInfo::getBarrelCount()
		{
			return (int32_t)m_barrelInfos.size();
		}

		void CBarrelsInfo::setVersion(const tchar* ver)
		{
			m_sVersion = ver;
		}

		void CBarrelsInfo::clear()
		{
			vector<CBarrelInfo*>::iterator iter = m_barrelInfos.begin();
			while (iter != m_barrelInfos.end()) 
			{
				delete *iter;
				iter++;
			}
			m_barrelInfos.clear();
			if(m_pDocSchema)
			{
				delete m_pDocSchema;
				m_pDocSchema = NULL;
			}			
			m_nBarrelCounter = 0;			
		}

		void CBarrelsInfo::read(CDirectory* pDirectory)
		{		
			clear();
			
			if(pDirectory->fileExists(BARRELS_INFONAME))
			{	
				CXMLElement* pDatabase = NULL;
				try
				{
					CIndexInput* pIndexInput = pDirectory->openInput(BARRELS_INFONAME);
					CFXString str;
					str.resize((size_t)pIndexInput->length());
					pIndexInput->read((char*)str.c_str(),str.length());
					delete pIndexInput;

					pDatabase = CXMLElement::fromString(str.c_str(),true);
					if(!pDatabase) FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));

					///get <version></version> element
					CXMLElement* pItem = pDatabase->getElementByName(_T("version"));
					if(!pItem) FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));
					m_sVersion = pItem->getValue();

					///get <barrel_counter></barrel_counter> element
					pItem = pDatabase->getElementByName(_T("barrel_counter"));
					if(!pItem) FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));
					m_nBarrelCounter = _ttoi(pItem->getValue().c_str());

					///get <barrel_count></barrel_count> element
					pItem = pDatabase->getElementByName(_T("barrel_count"));
					if(!pItem) FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));
					int32_t barrelCount = _ttoi(pItem->getValue().c_str());

					CXMLElement* pBarrelsItem = pDatabase->getElementByName(_T("barrels"));
					if(!pBarrelsItem) FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));

					///read barrel information
					CBarrelInfo* pBarrelInfo = NULL;
					ElementIterator  iter = pBarrelsItem->getElementIterator();
					CXMLElement*	pBarrelItem;
					while (pBarrelsItem->hasNextElement(iter))
					{
						pBarrelItem = pBarrelsItem->getNextElement(iter);
						pBarrelInfo = new CBarrelInfo();

						///get <name></name> element
						pItem = pBarrelItem->getElementByName(_T("name"));
						if(!pItem) FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));
						pBarrelInfo->m_sBarrelName = pItem->getValue();

						///get <doc_begin></doc_begin> element
						pItem = pBarrelItem->getElementByName(_T("doc_begin"));
						if(!pItem) FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));
						pBarrelInfo->m_nMinDocID = _ttoi(pItem->getValue().c_str());

						///get <doc_count></doc_count> element
						pItem = pBarrelItem->getElementByName(_T("doc_count"));
						if(!pItem) FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));
						pBarrelInfo->m_nNumDocs = _ttoi(pItem->getValue().c_str());						

						///get <deleteddocs_count></deleteddocs_count> element
						pItem = pBarrelItem->getElementByName(_T("deleteddocs_count"));
						if(!pItem) FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));
						pBarrelInfo->m_nDeletedDocs = _ttoi(pItem->getValue().c_str());

						m_barrelInfos.push_back(pBarrelInfo);
					}
					if(m_pDocSchema)
						delete m_pDocSchema;
					m_pDocSchema = new CDocumentSchema();
					pItem = pDatabase->getElementByName(_T("document_schema"));
					if(!pItem)  FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));
					m_pDocSchema->loadSchema(pItem);
					delete pDatabase;
					pDatabase = NULL;
				}
				catch (CFirteXException& e)
				{		
					if(pDatabase)
						delete pDatabase;
					FIRTEX_RETHROW(e);
				}				
				catch (...) 
				{
					FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("index collapsed."));
				}
			}		
		}
		void CBarrelsInfo::write(CDirectory* pDirectory)
		{			

			tstring str;
			CXMLElement* pDatabase = new CXMLElement(NULL,_T("database"));			
			
			///add version element
			CXMLElement* pItem = pDatabase->addElement(_T("version"));			
			pItem->setValue(m_sVersion);							
						
			///add barrel counter element			
			str = firtex::string_utils::append(str,m_nBarrelCounter);			
			pItem = pDatabase->addElement(_T("barrel_counter"));							
			pItem->setValue(str.c_str());			
			
			///add barrel count element
			str = _T("");
			str = firtex::string_utils::append(str,(int32_t)m_barrelInfos.size());			
			pItem = pDatabase->addElement(_T("barrel_count"));
			pItem->setValue(str.c_str());
			
			///add barrels elements
			CXMLElement* pBarrelsItem = pDatabase->addElement(_T("barrels"));
			CXMLElement* pBarrelItem = NULL;
			CBarrelInfo* pBarrelInfo = NULL;
			vector<CBarrelInfo*>::iterator iter = m_barrelInfos.begin();			
			while(iter != m_barrelInfos.end())
			{
				pBarrelInfo = *iter;
				pBarrelItem = pBarrelsItem->addElement(_T("barrel"));
				///add <name></name> element
				pItem = pBarrelItem->addElement(_T("name"));
				pItem->setValue(pBarrelInfo->m_sBarrelName.c_str());
				///add <doc_begin></doc_begin>
				pItem = pBarrelItem->addElement(_T("doc_begin"));
				str = _T("");
				str = firtex::string_utils::append(str,pBarrelInfo->m_nMinDocID);
				pItem->setValue(str.c_str());
				///add <doc_count></doc_count>
				pItem = pBarrelItem->addElement(_T("doc_count"));
				str = _T("");
				str = firtex::string_utils::append(str,pBarrelInfo->m_nNumDocs);
				pItem->setValue(str.c_str());
				///add <deleteddocs_count></deleteddocs_count>
				pItem = pBarrelItem->addElement(_T("deleteddocs_count"));
				str = _T("");
				str = firtex::string_utils::append(str,pBarrelInfo->m_nDeletedDocs);
				pItem->setValue(str.c_str());

				iter ++;
			}		
			///add schema elements
			m_pDocSchema->saveSchema(pDatabase);

			CIndexOutput* pIndexOutput = pDirectory->createOutput(BARRELS_INFONAME);			
			str = pDatabase->toString(true,true);
			pIndexOutput->write((const char*)str.c_str(),str.length());
			delete pIndexOutput;
			delete pDatabase;			
		}

		void CBarrelsInfo::remove(CDirectory* pDirectory)
		{
			try
			{
				CBarrelInfo* pBInfo = NULL;

				vector<CBarrelInfo*>::iterator iter = m_barrelInfos.begin();
				while (iter != m_barrelInfos.end()) 
				{
					pBInfo = *iter;
					pBInfo->remove(pDirectory);//删除索引文件
					delete pBInfo;
					iter++;
				}
				m_barrelInfos.clear();				
				m_nBarrelCounter = 0;				
				if(pDirectory->fileExists(BARRELS_INFONAME))
				{
					pDirectory->deleteFile(BARRELS_INFONAME,false);					
				}
				
			}
			catch(CFirteXException& fe)
			{
				FIRTEX_RETHROW(fe);
			}
			catch (...) 
			{
				FIRTEX_THROW3(FILEIO_ERROR,_T("CBarrelsInfo::remove():remove failed."));
			}		
		}
		void CBarrelsInfo::removeBarrel(CDirectory* pDirectory,const tstring& barrelname)
		{
			CBarrelInfo* pBInfo = NULL;

			vector<CBarrelInfo*>::iterator iter = m_barrelInfos.begin();
			while (iter != m_barrelInfos.end()) 
			{
				pBInfo = *iter;
				if(pBInfo->getName() == barrelname)
				{
					pBInfo->remove(pDirectory);//删除索引文件
					delete pBInfo;
					m_barrelInfos.erase(iter);					
					break;
				}				
				iter++;
			}			
		}
		const	tstring CBarrelsInfo::newBarrel()
		{						

			tstring sName = _T("_");
			sName = firtex::string_utils::append(sName,m_nBarrelCounter);
			m_nBarrelCounter ++;			
			return sName;
		}
		void CBarrelsInfo::addBarrel(const tchar* name,docid_t minDID,count_t docCount)
		{

			CBarrelInfo* barrelInfo = new CBarrelInfo(name,minDID,docCount);			
			m_barrelInfos.push_back(barrelInfo);						
		}
		void CBarrelsInfo::addBarrel(CBarrelInfo* pBarrelInfo,bool bCopy)
		{
			CBarrelInfo* barrelInfo ;
			if(bCopy)
				barrelInfo = new CBarrelInfo(*pBarrelInfo);			
			else barrelInfo = pBarrelInfo;
			m_barrelInfos.push_back(barrelInfo);			
		}

		int32_t CBarrelsInfo::getDocCount()
		{						
			int32_t count = 0;
			vector<CBarrelInfo*>::iterator iter = m_barrelInfos.begin();
			while (iter != m_barrelInfos.end()) 
			{
				count += (*iter)->getDocCount();
				iter++;
			}			
			return count;
		}
		count_t CBarrelsInfo::numDeletedDocs()
		{
			int32_t count = 0;
			vector<CBarrelInfo*>::iterator iter = m_barrelInfos.begin();
			while (iter != m_barrelInfos.end()) 
			{
				count += (*iter)->numDeletedDocs();
				iter++;
			}
			return count;			
		}
		void CBarrelsInfo::undeleteDocuments()
		{			
			vector<CBarrelInfo*>::iterator iter = m_barrelInfos.begin();
			while (iter != m_barrelInfos.end()) 
			{
				(*iter)->setDeletedDocs(0);
				iter++;
			}		
		}
		/*CBarrelsInfo& CBarrelsInfo::operator=(const CBarrelsInfo& src)
		{
			this->clear();
			m_sVersion = src.m_sVersion;
			m_nBarrelCounter = src.m_nBarrelCounter;
			m_pDocSchema = new CDocumentSchema(*(src.m_pDocSchema));
			vector<CBarrelInfo*>::const_iterator iter = src.m_barrelInfos.begin();
			while (iter != src.m_barrelInfos.end()) 
			{
				m_barrelInfos.push_back(new CBarrelInfo(*(*iter)));
				iter++;
			}		
			return *this;
		}*/
		CBarrelInfo* CBarrelsInfo::getBarrelInfo(const tchar* barrel)
		{
			vector<CBarrelInfo*>::iterator iter = m_barrelInfos.begin();
			while (iter != m_barrelInfos.end())
			{
				if(!_tcsicmp((*iter)->getName().c_str(),barrel))
					return (*iter);
				iter++;
			}
			return NULL;
		}
		CBarrelInfo* CBarrelsInfo::getLastBarrel()
		{
			if(m_barrelInfos.size() <= 0)
				return NULL;
			return m_barrelInfos[m_barrelInfos.size() - 1];
		}
		void CBarrelsInfo::deleteLastBarrel()
		{
			if(m_barrelInfos.size() <= 0)
				return ;
			delete m_barrelInfos[m_barrelInfos.size() - 1];
			m_barrelInfos.pop_back();			
		}
		void CBarrelsInfo::setSchema(CDocumentSchema* pSchema)
		{
			if(m_pDocSchema)
				delete m_pDocSchema;
			m_pDocSchema = new CDocumentSchema(*pSchema);		
			
		}
		void CBarrelsInfo::extractSchema(CDocumentSchema* pSchema)
		{
			FIRTEX_ASSERT((m_pDocSchema != NULL),_T(""));
			*pSchema = *m_pDocSchema;			
		}
		void CBarrelsInfo::sort(CDirectory* pDirectory)
		{			
			docid_t baseDocID = 0;
			CBarrelInfo* pBaInfo;
			CBarrelsInfo newBarrelsInfo;

			vector<CBarrelInfo*>::iterator iter;
			if(m_barrelInfos.size() > 1)
			{
				///sort barrels by number of documents
				stable_sort(m_barrelInfos.begin(),m_barrelInfos.end(),CBarrelInfo::greater);

				///rename to a temp name
				tstring strPrefix = _T("tmp");
				iter = m_barrelInfos.begin();
				while(iter != m_barrelInfos.end())
				{
					pBaInfo = *iter;				
					pBaInfo->rename(pDirectory,strPrefix + pBaInfo->getName());///update barrel name				
					iter++;
				}			
			}

			///rename to final name
			iter = m_barrelInfos.begin();
			while(iter != m_barrelInfos.end())
			{
				pBaInfo = *iter;
				pBaInfo->setMinDocID(baseDocID);	///update base document id
				baseDocID += pBaInfo->getDocCount();
				pBaInfo->rename(pDirectory,newBarrelsInfo.newBarrel());///update barrel name				
				iter++;
			}			
		}
	}
}
