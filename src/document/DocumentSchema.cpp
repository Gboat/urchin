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
// Created	: 2006/6/27
//
#include "utility/StdHeader.h"
#include "document/DocumentSchema.h"
#include "utility/Config.h"
#include "utility/StringUtils.h"

namespace firtex
{
	namespace document
	{
		//////////////////////////////////////////////////////////////////////////
		//
		CSchemaItem::CSchemaItem()
			:m_id(-1)
			,m_store( Store::NO)
			,m_index(Index::NO)
			,m_forwardIndex(ForwardIndex::NO)
			,m_fBoost(1.0)			
			,m_eIndexLevel(WORD_LEVEL)
		{
		}

		CSchemaItem::CSchemaItem(fieldid_t id,const tchar* name,store_t store,index_t index,forwardindex_t fi,float fBoost)
			:m_id(id)
			,m_sName(name)
			,m_store(store)
			,m_index(index)
			,m_forwardIndex(fi)
			,m_fBoost(fBoost)			
			,m_eIndexLevel(WORD_LEVEL)
		{				
		}
		CSchemaItem::CSchemaItem(const CSchemaItem& src)
		{
			m_id = src.m_id;
			m_sName = src.m_sName;
			m_store = src.m_store;
			m_index = src.m_index;
			m_forwardIndex = src.m_forwardIndex;
			m_fBoost = src.m_fBoost;

			m_sAliasName = src.m_sAliasName;
			
			m_components.insert(src.m_components.begin(),src.m_components.end());

			m_eIndexLevel = src.m_eIndexLevel;

		}
		CSchemaItem::~CSchemaItem()
		{
			clear();
		}
		
		void CSchemaItem::setComponent(const tchar* szCompName,const tchar* szCompValue)
		{
			if(!szCompName || (*szCompName == 0) || !szCompValue || (*szCompValue == 0) )
				return;
			CompMap::iterator iter = m_components.find(szCompName);
			if(iter != m_components.end())
			{
				m_components.erase(iter);
			}
			m_components.insert(make_pair(szCompName,szCompValue));

		}
		const tchar* CSchemaItem::getComponent(const tchar* szCompName)
		{
			CompMap::iterator iter = m_components.find(szCompName);
			if(iter != m_components.end())
			{
				return iter->second.c_str();
			}
			return NULL;
		}

		void CSchemaItem::setAlias(const tchar* pszAlias)
		{
			m_sAliasName = pszAlias;
		}
		void CSchemaItem::clear()
		{				
			m_components.clear();
		}

		bool CSchemaItem::load(CXMLElement* pXML)
		{
			try
			{
				clear();

				tstring str; 				

				CXMLElement* pItem = pXML->getElementByName(_T("name"));
				if(!pItem)
					return false;
				m_sName = pItem->getValue();
				pItem = pXML->getElementByName(_T("alias"));
				if(pItem)
				{
					m_sAliasName = pItem->getValue();
				}	
				
				pItem = pXML->getElementByName(_T("index_level"));
				if(pItem)
				{
					m_eIndexLevel = (INDEX_LEVEL)_ttoi(pItem->getValue().c_str());
					if(m_eIndexLevel < DOCUMENT_LEVEL_0 || m_eIndexLevel > WORD_LEVEL)
						m_eIndexLevel = WORD_LEVEL;
				}

				///Store
				pItem = pXML->getElementByName(_T("store"));
				if(pItem)
				{
					str = pItem->getValue();
					if(!_tcsicmp(str.c_str(),_T("yes")))
						m_store = Store::YES;
					else if(!_tcsicmp(str.c_str(),_T("no")))
						m_store = Store::NO;
					else if(!_tcsicmp(str.c_str(),_T("compress")))
						m_store = Store::COMPRESS;
				}

				///Index
				pItem = pXML->getElementByName(_T("index"));
				if(pItem)
				{
					str = pItem->getValue();
					if(!_tcsicmp(str.c_str(),_T("no")))
						m_index = Index::NO;
					else if(!_tcsicmp(str.c_str(),_T("analyze")))
						m_index = Index::ANALYZE;
					else if(!_tcsicmp(str.c_str(),_T("unanalyze")))
						m_index = Index::UN_ANALYZE;
					else m_index = Index::NO;
				}					

				///Term Vector
				pItem = pXML->getElementByName(_T("forward_index"));
				if(pItem)
				{
					str = pItem->getValue();
					if(!_tcsicmp(str.c_str(),_T("no")))
						m_forwardIndex = ForwardIndex::NO;
					else if(!_tcsicmp(str.c_str(),_T("sequence")))
						m_forwardIndex = ForwardIndex::SEQUENCE;
					else if(!_tcsicmp(str.c_str(),_T("freq")))
						m_forwardIndex = ForwardIndex::FREQ;
					else if(!_tcsicmp(str.c_str(),_T("position")))
						m_forwardIndex = ForwardIndex::POSITION;
					else m_forwardIndex = ForwardIndex::NO;
				}				

				pItem = pXML->getElementByName(_T("components"));
				if(pItem)
				{
					loadComponents(pItem);
				}

				///Boost
				pItem = pXML->getElementByName(_T("boost"));
				if(pItem)
				{
					str = pItem->getValue();
					if(str.empty())
						m_fBoost = 1.0;
					else m_fBoost = (float)atof(str.c_str());
				}					
			}
			catch (...)
			{
				FIRTEX_CLOG(level::warn) << _T("Load schema item failed.") << FIRTEX_ENDL;
				return false;
			}
			
			if( ((m_index == Index::NO) || (m_index == Index::UN_ANALYZE)) && (m_forwardIndex != ForwardIndex::NO))
			{
				FIRTEX_CLOG(level::warn) << _T("Load schema item failed : specify a field no index or unanalyze but has term vector") << FIRTEX_ENDL;
				return true;
			}
			/*else if( (m_fieldType == FIELD_NONE) && (m_index != Index::NO))
			{
				FIRTEX_CLOG(level::warn) << _T("Load schema item failed : specify a field need index but no field type.") << FIRTEX_ENDL;
				return false;
			}*/
			return true;
		}
		void CSchemaItem::save(CXMLElement* pXML)
		{
			CXMLElement* pRootItem = pXML->addElement(_T("schema_item"));
			CXMLElement* pItem = pRootItem->addElement(_T("name"));
			pItem->setValue(m_sName.c_str());

			if(m_sAliasName.length() > 0)
			{
				pItem = pRootItem->addElement(_T("alias"));
				pItem->setValue(m_sAliasName.c_str());
			}		
			
			tstring str;
			if(m_index != Index::NO)
			{
				pItem = pRootItem->addElement(_T("index_level"));
				 str = firtex::string_utils::append(_T(""),(int32_t)m_eIndexLevel);
				pItem->setValue(str.c_str());
			}			

			pItem = pRootItem->addElement(_T("store"));			
			if(m_store== Store::YES)
				pItem->setValue(_T("yes"));
			else if(m_store== Store::COMPRESS)
				pItem->setValue(_T("compress"));
			else 
                pItem->setValue(_T("no"));

			pItem = pRootItem->addElement(_T("index"));
			if(m_index == Index::ANALYZE)
				pItem->setValue(_T("analyze"));
			else if(m_index == Index::UN_ANALYZE)
				pItem->setValue(_T("unanalyze"));
			else
				pItem->setValue(_T("no"));

			pItem = pRootItem->addElement(_T("forward_index"));
			if(m_forwardIndex == ForwardIndex::SEQUENCE)
				pItem->setValue(_T("sequence"));
			else if(m_forwardIndex == ForwardIndex::FREQ)
				pItem->setValue(_T("freq"));
			else if(m_forwardIndex == ForwardIndex::POSITION)
				pItem->setValue(_T("position"));
			else pItem->setValue(_T("no"));

			if(!m_components.empty())
			{
				pItem = pRootItem->addElement(_T("components"));
				saveComponents(pItem);
			}
			
			str = firtex::string_utils::append(_T(""),m_fBoost);
			pItem = pRootItem->addElement(_T("boost"));
			pItem->setValue(str.c_str());
		}
		void CSchemaItem::loadComponents(CXMLElement* pCompRoot)
		{
			CXMLElement* pItem;
			ElementIterator eIter = pCompRoot->getElementIterator();
			tstring strName,strValue;
			while (pCompRoot->hasNextElement(eIter))
			{
				pItem = pCompRoot->getNextElement(eIter);
				strName = pItem->getAttributeValue(_T("name"));
				strValue = pItem->getValue();
				setComponent(strName.c_str(),strValue.c_str());
			}			
		}
		void CSchemaItem::saveComponents(CXMLElement* pCompRoot)
		{
			CXMLElement* pItem;
			CompMap::iterator iter = m_components.begin();
			while (iter != m_components.end())
			{
				pItem = pCompRoot->addElement(_T("component"));
				pItem->addAttribute(_T("name"),iter->first.c_str());
				pItem->setValue(iter->second.c_str());
				iter++;
			}			
		}

		CSchemaItem& CSchemaItem::operator=(const CSchemaItem& right)
		{
			m_id = right.m_id;
			m_sName = right.m_sName;
			m_store = right.m_store;
			m_index = right.m_index;
			m_forwardIndex = right.m_forwardIndex;
			m_fBoost = right.m_fBoost;
			m_eIndexLevel = right.m_eIndexLevel;

			m_sAliasName = right.m_sAliasName;			

			m_components.insert(right.m_components.begin(),right.m_components.end());
			
			return (*this);
		}
		//////////////////////////////////////////////////////////////////////////
		//
		CDocumentSchema::CDocumentSchema(void)
			:m_schemaCounter(0)
		{
		}

		CDocumentSchema::CDocumentSchema(const CDocumentSchema& src)
		{
			vector<CSchemaItem*>::const_iterator iter = src.m_items.begin();
			while (iter != src.m_items.end())
			{
				m_items.push_back(new CSchemaItem(*(*iter)));
				if( (*iter)->getId() != -1)
					m_schemaCounter++;
				iter++;
			}			
			m_schemaCounter = src.m_schemaCounter;
		}
		CDocumentSchema::~CDocumentSchema(void)
		{
			clear();
		}
		
		void CDocumentSchema::loadSchema(const tchar*	schemaFile)
		{
			clear();

			FIRTEX_ASSERT((schemaFile && *schemaFile != 0),_T("CDocumentSchema.loadSchema()"));			

			m_schemaFile = schemaFile;

			int32_t fileHandle = fileOpen(schemaFile, O_BINARY | O_RDONLY | O_RANDOM, _S_IREAD );
			if (fileHandle < 0)
			{
				int32_t err = errno;				
				if ( err == ENOENT )
					FIRTEX_THROW3(FILEIO_ERROR,_T("File does not exist: ") + m_schemaFile );
				else if ( err == EACCES )
					FIRTEX_THROW3(FILEIO_ERROR,_T("File Access denied: ") + m_schemaFile );
				else if ( err == EMFILE )
					FIRTEX_THROW3(FILEIO_ERROR,_T("Too many open files: ") + m_schemaFile);				
			}				


			size_t len = (size_t)fileSize(fileHandle);
			if(len <= 0)
			{
				FIRTEX_THROW3(FILEIO_ERROR,_T("schema file is empty."));				
			}
			char* buf = new char[len + 1];
			size_t ret = fileRead(fileHandle,buf,(unsigned int)len);
			if(ret == 0)
			{
				delete[] buf;
				FIRTEX_THROW3(FILEIO_ERROR,_T("CDocumentSchema.loadSchema():read past EOF:") + m_schemaFile);								
			}
			else if (ret == -1)
			{
				delete[] buf;
				FIRTEX_THROW3(FILEIO_ERROR,_T("CDocumentSchema.loadSchema():file IO read error:") + m_schemaFile );
			}
			fileClose(fileHandle);

			CXMLElement* pXML = NULL;
			try
			{
#ifdef _UNICODE
				size_t wcslen = CMisc::_mbstowcs(NULL,0,buf,len);
				FIRTEX_ASSERT((wcslen > 0),_T("CMisc._mbstowcs() failed."));
				wchar_t* wcsbuf = new wchar_t[wcslen + 1];
				CMisc::_mbstowcs(wcsbuf,wcslen,buf,len);
				wcsbuf[wcslen] = 0;
				pXML = CXMLElement::fromString(wcsbuf,false);
				delete[] buf;
				delete[] wcsbuf;
#else 
				pXML = CXMLElement::fromString(buf,false);
				delete[] buf;
#endif						

				if(pXML == NULL)
				{					
					FIRTEX_THROW3(FILEIO_ERROR,_T("parse schema file failed:") + m_schemaFile );									
				}			

				ElementIterator  iter = pXML->getElementIterator();
				CXMLElement*	pSub;
				CSchemaItem* pItem;
				while (pXML->hasNextElement(iter))
				{
					pSub = pXML->getNextElement(iter);
					pItem = new CSchemaItem();
					if(pItem->load(pSub) == false)
						delete pItem;
					else 
					{
						if( (pItem->getIndex() != Index::NO) || (pItem->getStore() != Store::NO) || (pItem->getForwardIndex() != ForwardIndex::NO) )
						{
							pItem->m_id = m_schemaCounter++;
						}						
						m_items.push_back(pItem);
					}
				}				

				delete pXML;
				pXML = NULL;
			}
			catch(...)
			{
				if(pXML)
					delete pXML;
				FIRTEX_THROW3(FILEIO_ERROR,_T("parse schema file failed:") + m_schemaFile );								
			}			
		}
		void CDocumentSchema::loadSchema(CXMLElement* pXMLSchema)
		{
			try
			{
				ElementIterator  iter = pXMLSchema->getElementIterator();
				CXMLElement*	pSub;
				CSchemaItem* pItem;
				while (pXMLSchema->hasNextElement(iter))
				{
					pSub = pXMLSchema->getNextElement(iter);
					pItem = new CSchemaItem();
					if(pItem->load(pSub) == false)
						delete pItem;
					else 
					{
						if( (pItem->getIndex() != Index::NO) || (pItem->getStore() != Store::NO) || (pItem->getForwardIndex() != ForwardIndex::NO) )
						{
							pItem->m_id = m_schemaCounter++;
						}
						
						m_items.push_back(pItem);
					}
				}								
			}
			catch (CFirteXException& e)
			{
				FIRTEX_RETHROW(e);
			}
			catch (...)
			{
				FIRTEX_THROW3(FILEIO_ERROR,_T("parse schema file failed:"));
			}

		}
		
		void CDocumentSchema::saveSchema(const tchar*	schemaFile)
		{
			tstring filename;
			if(!schemaFile)
			{
				filename = m_schemaFile;
				if(filename.empty())
					FIRTEX_THROW3(FILEIO_ERROR,_T("save schema failed:please specify schema file name."));
			}
			else filename = schemaFile;

			CXMLElement* pXML = new CXMLElement(NULL,_T("document_schema"));

			vector<CSchemaItem*>::iterator iter = m_items.begin();
			while (iter != m_items.end())
			{
				(*iter)->save(pXML);
				iter++;
			}

			tstring sXML = pXML->toString(false,true);

			int32_t fileHandle;
			if ( CMisc::dirExists(filename.c_str()) )
				fileHandle = fileOpen( filename.c_str(), O_BINARY | O_RDWR | O_RANDOM | O_TRUNC, _S_IREAD | _S_IWRITE);
			else 
				fileHandle = fileOpen( filename.c_str(), O_BINARY | O_RDWR | O_RANDOM | O_CREAT, _S_IREAD | _S_IWRITE);			
			if ( fileHandle < 0 )
			{
				int32_t err = errno;				
				if ( err == EACCES )
					FIRTEX_THROW3(FILEIO_ERROR,_T("File Access denied:") + filename);
				else if ( err == EMFILE )
					FIRTEX_THROW3(FILEIO_ERROR,_T("Too many open files:") + filename );
			}				

#ifdef _UNICODE

			size_t mbslen = CMisc::_wcstombs(NULL,0,sXML,sXML.length());
			FIRTEX_ASSERT((mbslen > 0),_T("CMisc._wcstombs() failed."));
			char* mbsbuf = new char[mbslen + 1];
			CMisc::_wcstombs(mbsbuf,mbslen,sXML.c_str(),sXML.length());			
			mbsbuf[mbslen] = 0;
			if(fileWrite(fileHandle,mbsbuf,(unsigned int)mbslen) != mbslen)
				FIRTEX_THROW3(FILEIO_ERROR,_T("CArgument::saveArg():file IO write error:"));			
			delete[] mbsbuf;

#else 
			if(fileWrite(fileHandle,sXML.c_str(),(unsigned int)sXML.length()) != sXML.length())
				FIRTEX_THROW3(FILEIO_ERROR,_T("CArgument::saveArg():file IO write error:"));
#endif
			fileClose(fileHandle);
			delete pXML;			
		}
		void CDocumentSchema::saveSchema(CXMLElement* pXMLSchema)
		{
			CXMLElement* pXML = pXMLSchema->addElement(_T("document_schema"));

			vector<CSchemaItem*>::iterator iter = m_items.begin();
			while (iter != m_items.end())
			{
				(*iter)->save(pXML);
				iter++;
			}
		}
		fieldid_t CDocumentSchema::addKeywordItem(const tchar* name)
		{
			if(!name || *name == 0)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("CDocumentSchema::addKeywordItem()"));
			CSchemaItem* pItem = findSchema(name);
			if(!pItem)				 
			{
				pItem = new CSchemaItem((fieldid_t)m_schemaCounter++,name,Store::YES,Index::UN_ANALYZE,ForwardIndex::NO);
				m_items.push_back(pItem);
			}			
			return pItem->m_id;
		}
		fieldid_t CDocumentSchema::addUnIndexedItem(const tchar* name)
		{
			if(!name || *name == 0)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("CDocumentSchema::addUnIndexedItem()"));
			CSchemaItem* pItem = findSchema(name);
			if(!pItem)
			{
				pItem = new CSchemaItem((fieldid_t)m_schemaCounter++,name,Store::YES,Index::NO,ForwardIndex::NO);
				m_items.push_back(pItem);
			}
			return pItem->m_id;
		}
		fieldid_t CDocumentSchema::addTextItem(const tchar* name,store_t store,forwardindex_t termVector)
		{
			if(!name || *name == 0)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("CDocumentSchema::addTextItem()"));
			CSchemaItem* pItem = findSchema(name);
			if(!pItem)
			{
				pItem = new CSchemaItem((fieldid_t)m_schemaCounter++,name,store,Index::ANALYZE,termVector);
				m_items.push_back(pItem);
			}
			return pItem->m_id;
		}
		fieldid_t CDocumentSchema::addUnStoredItem(const tchar* name,forwardindex_t termVector)
		{
			if(!name || *name == 0)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("CDocumentSchema::addUnStoredItem()"));
			CSchemaItem* pItem = findSchema(name);
			if(!pItem)
			{
				pItem = new CSchemaItem((fieldid_t)m_schemaCounter++,name,Store::NO,Index::ANALYZE,termVector);
				m_items.push_back(pItem);
			}
			return pItem->m_id;
		}
		fieldid_t CDocumentSchema::addBinaryItem(const tchar* name,bool bCompress)
		{
			if(!name || *name == 0)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("CDocumentSchema::addBinaryItem()"));
			CSchemaItem* pItem = findSchema(name);
			if(!pItem)
			{
				pItem = new CSchemaItem((fieldid_t)m_schemaCounter++,name,Store::COMPRESS,Index::NO,ForwardIndex::NO);
				m_items.push_back(pItem);
			}
			return pItem->m_id;
		}
		fieldid_t CDocumentSchema::addItem(const tchar* name,index_t index,store_t store,forwardindex_t termVector)
		{
			if(!name || *name == 0)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("CDocumentSchema::addItem()"));
			
			CSchemaItem* pItem = findSchema(name);
			if(!pItem)
			{
				if( (index != Index::NO) || (store !=Store::NO) || (termVector != ForwardIndex::NO) )
					pItem = new CSchemaItem((fieldid_t)m_schemaCounter++,name,store,index,termVector);
				else 
					pItem = new CSchemaItem((fieldid_t)-1,name,store,index,termVector);
				m_items.push_back(pItem);
			}
			return pItem->m_id;
		}
		fieldid_t CDocumentSchema::addItem(const CSchemaItem& item)
		{
			CSchemaItem* pItem = new CSchemaItem(item);
			if( (pItem->getIndex() != Index::NO) || (pItem->getStore() != Store::NO) || (pItem->getForwardIndex() != ForwardIndex::NO) )
				pItem->m_id = (fieldid_t)m_schemaCounter++;
			else pItem->m_id = -1;
			m_items.push_back(pItem);
			return pItem->m_id;
		}		
		void CDocumentSchema::deleteItem(const tchar* name)
		{
			vector<CSchemaItem*>::iterator iter = m_items.begin();
			while (iter!=m_items.end())
			{
				if(_tcsicmp((*iter)->m_sName,name)==0)
				{
					m_items.erase(iter);
					break;
				}
				iter++;
			}		
			updateID();			
		}
		void CDocumentSchema::updateID()
		{
			m_schemaCounter = 0;
			CSchemaItem* pItem;
			vector<CSchemaItem*>::iterator iter = m_items.begin();
			while (iter!=m_items.end())
			{
				pItem = *iter;
				if( (pItem->getIndex() != Index::NO) || (pItem->getStore() != Store::NO) || (pItem->getForwardIndex() != ForwardIndex::NO) )
					pItem->m_id = (fieldid_t)m_schemaCounter++;

				iter++;
			}		
		}
		CSchemaItem* CDocumentSchema::findSchema(const tchar* name)
		{
			vector<CSchemaItem*>::iterator iter = m_items.begin();
			while (iter!=m_items.end())
			{
				if(_tcsicmp((*iter)->m_sName,name)==0)
					return (*iter);
				iter++;
			}		
			return NULL;
		}
		CSchemaItem* CDocumentSchema::getSchema(schemaid_t sid)
		{
			vector<CSchemaItem*>::iterator iter = m_items.begin();
			while (iter!=m_items.end())
			{
				if((*iter)->m_id == sid)
					return (*iter);
				iter++;
			}		
			return NULL;
		}

		void CDocumentSchema::clear()
		{
			vector<CSchemaItem*>::iterator iter = m_items.begin();
			while (iter!=m_items.end())
			{
				delete (*iter);
				iter++;
			}
			m_items.clear();
			m_schemaCounter = 0;
		}		
	}
}
