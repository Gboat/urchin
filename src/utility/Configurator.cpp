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
// Created	: 2006/8/3
//
#include "utility/Configurator.h"
#include "utility/XML.h"

namespace firtex
{
	namespace utility
	{
		//////////////////////////////////////////////////////////////////////////
		///Configurator
		Configurator::Configurator(void)
			: m_pSchema(NULL)			
		{					
			collection.identifier = _T("directory");			

			parser.identifier = _T("parser.plain");
			
			analyzer.identifier = _T("analyzer.standard");
			analyzer.parameter = GlobalConfig.Analyzer.parameter;
            
			index.source = _T("");
			index.indexlocation = _T("");
			index.level = 2;
			index.accessmode = _T("w");
			index.maxterms = GlobalConfig.Index.maxIndexTerms;
			index.memory = GlobalConfig.Index.memory;
			index.cachedocs = GlobalConfig.Index.numCachedDocs;

			mergestrategy.strategy = GlobalConfig.MergeStrategy.strategy;
			mergestrategy.param = GlobalConfig.MergeStrategy.param;
			
			add(_T("collection.identifier"),&collection.identifier,collection.identifier);
			add(_T("collection.parameter"),&collection.parameter,collection.parameter);

			add(_T("parser.identifier"),&parser.identifier,parser.identifier);
			add(_T("parser.parameter"),&parser.parameter,parser.parameter);

			add(_T("analyzer.identifier"),&analyzer.identifier,analyzer.identifier);
			add(_T("analyzer.parameter"),&analyzer.parameter,analyzer.parameter);			

			add(_T("index.source"),&index.source,index.source);
			add(_T("index.indexlocation"),&index.indexlocation,index.indexlocation);			
			add(_T("index.level"),&index.level,index.level);
			add(_T("index.accessmode"),&index.accessmode,index.accessmode);

			add(_T("index.maxterms"),&index.maxterms,index.maxterms);
			add(_T("index.memory"),&index.memory,index.memory);			
			add(_T("index.cachedocs"),&index.cachedocs,index.cachedocs);			

			add(_T("mergestrategy.strategy"),&mergestrategy.strategy,mergestrategy.strategy);
			add(_T("mergestrategy.param"),&mergestrategy.param,mergestrategy.param);
			add(_T("mergestrategy.gc_threshold"),&mergestrategy.gc_threshold,mergestrategy.gc_threshold);		
		}

		Configurator::Configurator(const Configurator& src)
			: m_path(src.m_path)
		{
			if(src.m_pSchema)
			{
				m_pSchema = new CDocumentSchema(*(src.m_pSchema));
			}
			else 
			{
				m_pSchema = NULL;
			}

			///collection
			collection.identifier = src.collection.identifier;
			collection.parameter = src.collection.parameter;

			///parser
			parser.identifier = src.parser.identifier;
			parser.parameter = src.parser.parameter;

			///analyzer
			analyzer.identifier = src.analyzer.identifier;
			analyzer.parameter = src.analyzer.parameter;

			///index
			index.source = src.index.source;
			index.indexlocation = src.index.indexlocation;
			index.level = src.index.level;
			index.accessmode = src.index.accessmode;

			index.maxterms = src.index.maxterms;
			index.memory = src.index.memory;
			index.cachedocs = src.index.cachedocs;

			///mergestrategy
			mergestrategy.strategy = src.mergestrategy.strategy;
			mergestrategy.param = src.mergestrategy.param;
			mergestrategy.gc_threshold = src.mergestrategy.gc_threshold;

			add(_T("collection.identifier"),&collection.identifier,collection.identifier);
			add(_T("collection.parameter"),&collection.parameter,collection.parameter);

			add(_T("parser.identifier"),&parser.identifier,parser.identifier);
			add(_T("parser.parameter"),&parser.parameter,parser.parameter);

			add(_T("analyzer.identifier"),&analyzer.identifier,analyzer.identifier);
			add(_T("analyzer.parameter"),&analyzer.parameter,analyzer.parameter);			

			add(_T("index.source"),&index.source,index.source);
			add(_T("index.indexlocation"),&index.indexlocation,index.indexlocation);			
			add(_T("index.level"),&index.level,index.level);
			add(_T("index.accessmode"),&index.accessmode,index.accessmode);

			add(_T("index.maxterms"),&index.maxterms,index.maxterms);
			add(_T("index.memory"),&index.memory,index.memory);			
			add(_T("index.cachedocs"),&index.cachedocs,index.cachedocs);			

			add(_T("mergestrategy.strategy"),&mergestrategy.strategy,mergestrategy.strategy);
			add(_T("mergestrategy.param"),&mergestrategy.param,mergestrategy.param);
			add(_T("mergestrategy.gc_threshold"),&mergestrategy.gc_threshold,mergestrategy.gc_threshold);
		}

		Configurator::~Configurator(void)
		{			
			item_vector::iterator iter = m_items.begin();
			while (iter != m_items.end())
			{
				delete (*iter);
				iter++;
			}
			m_items.clear();
			if(m_pSchema)
			{
				delete m_pSchema;
				m_pSchema = NULL;
			}
		}

		void Configurator::load(const tchar* config)
		{
			FIRTEX_ASSERT((config && *config != 0),_T("Configurator.load()"));			
			
			m_path = config;

			int fileHandle = fileOpen(config, O_BINARY | O_RDONLY | O_RANDOM, _S_IREAD );
			if (fileHandle < 0)
			{
				int err = errno;				
				if ( err == ENOENT )
					FIRTEX_THROW3(FILEIO_ERROR,_T("File does not exist: ") + m_path );
				else if ( err == EACCES )
					FIRTEX_THROW3(FILEIO_ERROR,_T("File Access denied: ") + m_path );
				else if ( err == EMFILE )
					FIRTEX_THROW3(FILEIO_ERROR,_T("Too many open files: ") + m_path);				
			}							

			int32_t len = (int32_t)fileSize(fileHandle);			
			if(len <= 0)
			{				
				FIRTEX_THROW3(FILEIO_ERROR,_T("get the length of argument file failed."));				
			}

			char* buf = new char[len + 1];
			size_t ret = fileRead(fileHandle,buf,(unsigned int)len);
			if(ret == 0)
			{
				delete[] buf;
				FIRTEX_THROW3(FILEIO_ERROR,_T("Configurator::parseArg():read past EOF:") + m_path);								
			}
			else if (ret == -1)
			{
				delete[] buf;
				FIRTEX_THROW3(FILEIO_ERROR,_T("Configurator::parseArg():file IO read error:") + m_path );
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
					FIRTEX_THROW3(FILEIO_ERROR,_T("parse argument file failed:") + m_path );									
				}			

				item_vector::iterator iter = m_items.begin();
				while (iter != m_items.end())
				{
					(*iter)->load(pXML);
					iter++;
				}		

				CXMLElement* pSchema = pXML->getElementByName(_T("document_schema"));
				if(pSchema)
				{
					if(m_pSchema)
					{
						delete m_pSchema;
						m_pSchema = NULL;
					}
					m_pSchema = new CDocumentSchema();
					m_pSchema->loadSchema(pSchema);					
				}

				delete pXML;
				pXML = NULL;
			}
			catch(...)
			{
				if(pXML)
					delete pXML;
				FIRTEX_THROW3(FILEIO_ERROR,_T("parse argument file failed:") + m_path );								
			}
		}

		void Configurator::save(const tchar* config /* = NULL */)
		{
			tstring filename;
			if(!config)
			{
				filename = m_path;
				if(filename.empty())
					FIRTEX_THROW3(FILEIO_ERROR,_T("save argument failed:please specify argument file name."));
			}
			else filename = config;

			CXMLElement* pXML = new CXMLElement(NULL,_T("argument"));			

			item_vector::iterator iter = m_items.begin();
			while (iter != m_items.end())
			{
				(*iter)->save(pXML);
				iter++;
			}
			if(m_pSchema)
			{
				m_pSchema->saveSchema(pXML);
			}

			tstring sXML = pXML->toString(false,true);

			int fileHandle;
			if ( CMisc::dirExists(filename.c_str()) )
				fileHandle = fileOpen( filename.c_str(), O_BINARY | O_RDWR | O_RANDOM | O_TRUNC, _S_IREAD | _S_IWRITE);
			else 
				fileHandle = fileOpen( filename.c_str(), O_BINARY | O_RDWR | O_RANDOM | O_CREAT, _S_IREAD | _S_IWRITE);			
			if ( fileHandle < 0 )
			{
				int err = errno;				
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
				FIRTEX_THROW3(FILEIO_ERROR,_T("Configurator::saveArg():file IO write error:"));			
			delete[] mbsbuf;

#else 
			if(fileWrite(fileHandle,sXML.c_str(),(unsigned int)sXML.length()) != sXML.length())
				FIRTEX_THROW3(FILEIO_ERROR,_T("Configurator::saveArg():file IO write error:"));
#endif
			fileClose(fileHandle);
			delete pXML;			
		}
		//////////////////////////////////////////////////////////////////////
		// Configurator add items

		void Configurator::add(const tstring& sName, int64_t* pInt64, int64_t nDefault)
		{
			m_items.push_back( new ConfiguratorItem( sName, pInt64,NULL, NULL, NULL,NULL ) );
			*pInt64 = nDefault;
		}

		void Configurator::add(const tstring& sName, int32_t* pInt32, int32_t nDefault)
		{			
			m_items.push_back( new ConfiguratorItem( sName, NULL,pInt32, NULL, NULL,NULL ) );
			*pInt32 = nDefault;
		}

		void Configurator::add(const tstring& sName, double* pFloat, double nDefault)
		{
			m_items.push_back( new ConfiguratorItem( sName, NULL, NULL,pFloat, NULL,NULL) );
			*pFloat = nDefault;
		}

		void Configurator::add(const tstring& sName, tstring* pString, const tstring& pszDefault)
		{
			m_items.push_back( new ConfiguratorItem( sName, NULL,NULL, NULL, pString ,NULL) );
			*pString = pszDefault;
		}
		void Configurator::add(const tstring& sName, bool* pBool, bool bDefault)
		{
			m_items.push_back( new ConfiguratorItem( sName, NULL,NULL, NULL, NULL ,pBool) );
			*pBool= bDefault;
		}		
		void Configurator::setSchema(CDocumentSchema* pSchema)
		{
			if(m_pSchema)
				delete m_pSchema;
			m_pSchema = pSchema;
		}

		//////////////////////////////////////////////////////////////////////
		/// Configurator::ConfiguratorItem construction and operations	

		Configurator::ConfiguratorItem::ConfiguratorItem(const tstring& sName)
			: m_sName(sName)
			, m_pInt32(NULL)
			, m_pInt64(NULL)
			, m_pFloat(NULL)
			, m_pString(NULL)
			, m_pBool(NULL)
		{
		}
		Configurator::ConfiguratorItem::ConfiguratorItem(const tstring& sName, int64_t* pInt64,int32_t* pInt32,double* pFloat, tstring* pString,bool* pBool)
		{
			m_sName		= sName;
			m_pInt64	= pInt64;
			m_pInt32	= pInt32;
			m_pFloat	= pFloat;
			m_pString	= pString;
			m_pBool		= pBool;
		}

		void Configurator::ConfiguratorItem::load(CXMLElement* pXML)
		{
			CXMLElement* pSub = pXML;
			tstring sRight = m_sName;
			tstring sLeft;
			firtex::string_utils::trim(sRight);			
			tstring::size_type npos = (size_t)-1;	
			tstring::size_type idx;

			try
			{
				while(sRight != _T(""))
				{
					idx = sRight.find(_T('.'));
					if(idx ==  npos)
					{
						sLeft = sRight;
						sRight = _T("");
					}
					else 
					{
						sLeft = sRight.substr(0,idx);
						sRight = sRight.substr(idx + 1);
					}

					pSub = pSub->getElementByName(sLeft.c_str());
					if(pSub == NULL)
						return;		
				}
				tstring sType = pSub->getAttributeValue(_T("type"),_T(""));

				if ( !_tcsicmp(sType.c_str(),_T("int32")) )
				{
					_stscanf( pSub->getValue().c_str(),_T("%u"), m_pInt32 );		
				}
				else if ( !_tcsicmp(sType.c_str(),_T("int64")) )
				{
					_stscanf( pSub->getValue().c_str(),_T("%u"), m_pInt64 );		
				}
				else if ( !_tcsicmp(sType.c_str(), _T("float")) )
				{
					_stscanf( pSub->getValue().c_str(),_T("%lf"), m_pFloat );		
				}
				else if ( !_tcsicmp(sType.c_str(), _T("string")) )
				{
					*m_pString = pSub->getValue();		
				}
				else if ( !_tcsicmp(sType.c_str(), _T("bool")) )
				{
					if(!_tcsicmp(pSub->getValue().c_str(),_T("true")))
						*m_pBool = true;
					else *m_pBool = false;
				}
				else 
				{
					FIRTEX_CLOG(level::warn) << _T("invalid argument type in argument file:") << m_sName << FIRTEX_ENDL;
				}
			}
			catch(...)
			{
				FIRTEX_CLOG(level::warn) << _T("parse argument failed in argument file:") << m_sName << FIRTEX_ENDL;
			}

		}

		void Configurator::ConfiguratorItem::save(CXMLElement* pXML)
		{	
			CXMLElement* pSub = pXML;
			tstring sRight = m_sName;
			tstring sLeft;
			firtex::string_utils::trimleft(sRight);
			tstring::size_type npos = (size_t)-1;	
			tstring::size_type idx;

			while(sRight != "")
			{
				idx = sRight.find('.');
				if(idx == npos)
				{
					sLeft = sRight;
					sRight = _T("");
				}
				else 
				{
					sLeft = sRight.substr(0,idx);
					sRight = sRight.substr(idx + 1);
				}

				pSub = pSub->getElementByName(sLeft.c_str(),true);
			}


			CXMLAttribute* pAttri = pSub->addAttribute(_T("type"),_T(""));
			tstring str;
			if ( m_pInt64 )
			{
				pAttri->setValue(_T("int64"));
				stringstream ss;
				ss << *m_pInt64;
				ss >> str;				
				pSub->setValue(str.c_str());
			}
			else if ( m_pInt32 )
			{
				pAttri->setValue(_T("int32"));
				stringstream ss;
				ss << *m_pInt32;
				ss >> str;				
				pSub->setValue(str.c_str());
			}
			else if ( m_pFloat )
			{	
				pAttri->setValue(_T("float"));
				stringstream ss;
				ss << *m_pFloat;
				ss >> str;				
				pSub->setValue(str.c_str());
			}
			else if( m_pString )
			{
				pAttri->setValue(_T("string"));		
				pSub->setValue(m_pString->c_str());
			}
			else if( m_pBool)
			{
				pAttri->setValue(_T("bool"));		
				pSub->setValue((*m_pBool)==true?_T("true"):_T("false"));
			}
		}				
	}
}
