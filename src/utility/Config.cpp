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
// Created	: 2005/10/3
//
#include "utility/Config.h"

namespace firtex
{
	namespace utility
	{

		CConfig GlobalConfig;		
	}
}


#include "utility/XML.h"
#include "utility/StringUtils.h"
#include "index/Posting.h"
#include "index/FieldFactory.h"
#include <fstream>

using namespace std;
using namespace firtex::index;


#ifdef WIN32
	#ifndef STRICT
		#define STRICT
	#endif	
	#include <windows.h>
#endif

//#include "../plugin/Plugins.h"


namespace firtex
{
	namespace index
	{

		int32_t	CInMemoryPosting::UPTIGHT_ALLOC_CHUNKSIZE = GlobalConfig.Advance.UptightAlloc.chunkSize;
		int32_t	CInMemoryPosting::UPTIGHT_ALLOC_MEMSIZE = GlobalConfig.Advance.UptightAlloc.memSize;
	}
}


namespace firtex
{
	namespace utility
	{
		//////////////////////////////////////////////////////////////////////////
		//CConfig

		CConfig::CConfig(void)
		{
			MergeStrategy.strategy = _T("hlm");
			MergeStrategy.param = _T("m=3");

			Index.numCachedDocs = 100;
			Index.maxIndexTerms = 100000;
			Index.memory = 128000000;	

			Search.memory = 100000000;

			Advance.MMS.WordLevel.strategy = _T("exp:32:2");
			Advance.MMS.DocumentLevel.strategy = _T("exp:32:2");
			Advance.MMS.DocumentLevel0.strategy = _T("exp:32:2");

			m_sLoglevel = "default_level";	//disable_all,enable_all,default_level(info),fatal,err,warn,info,dbg
			tlogger::enableLogs(m_sLoglevel);

			tchar szPath[512];
			GetModuleFileName( NULL, szPath, 512 );
			
			General.Path = szPath;
#ifdef WIN32
			if ( General.Path.rfind( '\\' ) >= 0 )
				General.Path = General.Path.substr(0,General.Path.rfind( '\\' ) );
			
			m_sPath = General.Path + _T("\\config.xml");
			General.PluginPath = General.Path + _T("\\plugins");	
			FIRTEX_CLOG(level::dbg) << _T("Configure file:") << m_sPath <<FIRTEX_ENDL;
#else
			if ( General.Path.rfind( '/' ) >= 0 )
				General.Path = General.Path.substr(0,General.Path.rfind( '/' ) );

			m_sPath = General.Path + _T("/config.xml");
			General.PluginPath = General.Path + _T("/plugins");			
			setenv("FIRTEX_COMDB_PATH",General.PluginPath.c_str(),1);
#endif
			setup();			
		}

		CConfig::~CConfig(void)
		{
			save();

			list<CParamItem*>::iterator iter = m_items.begin();
			while (iter != m_items.end())
			{
				delete (*iter);
				iter++;
			}
			m_items.clear();

			terminate();
		}

		void CConfig::setLogLevel(const tchar* l)
		{
			m_sLoglevel = l;
			tlogger::enableLogs(m_sLoglevel);
		}
		void CConfig::setup()
		{
			add(_T("analyzer.parameter"),&Analyzer.parameter,Analyzer.parameter);			

			add(_T("index.memory"),&Index.memory,Index.memory);
			add(_T("index.maxIndexTerms"),&Index.maxIndexTerms,Index.maxIndexTerms);

			add(_T("search.memory"),&Search.memory,Search.memory);

			add(_T("mergestrategy.strategy"),&MergeStrategy.strategy,MergeStrategy.strategy);
			add(_T("mergestrategy.param"),&MergeStrategy.param,MergeStrategy.param);

			add(_T("advance.mms.word_level.strategy"),&Advance.MMS.WordLevel.strategy,Advance.MMS.WordLevel.strategy);
			add(_T("advance.mms.document_level.strategy"),&Advance.MMS.DocumentLevel.strategy,Advance.MMS.DocumentLevel.strategy);
			add(_T("advance.mms.document_level_0.strategy"),&Advance.MMS.DocumentLevel0.strategy,Advance.MMS.DocumentLevel0.strategy);

			add(_T("log.level"),&m_sLoglevel,m_sLoglevel);
			
			load(m_sPath);			

			Advance.UptightAlloc.chunkSize = 8;
			Advance.UptightAlloc.memSize = (int32_t)(Index.maxIndexTerms*Advance.UptightAlloc.chunkSize/2);					

			initMMS();
			m_starter.initialize();
		}
		void CConfig::terminate()
		{
			 m_starter.terminate();
		}

		void CConfig::initMMS()
		{
			CInMemoryPosting::MEMALLOC_STRATEGY mas = CInMemoryPosting::ALLOC_CONST;
			
			int32_t strategy = 1,n = 32,k = 0,l = 0;
			if(getMMSConfig(Advance.MMS.WordLevel.strategy,strategy,n,k,l))			
			{

				CInMemoryPosting::WL_ALLOCSTRATEGY.strategy = (CInMemoryPosting::MEMALLOC_STRATEGY)strategy;
				CInMemoryPosting::WL_ALLOCSTRATEGY.n = n;
				CInMemoryPosting::WL_ALLOCSTRATEGY.k = k;
				CInMemoryPosting::WL_ALLOCSTRATEGY.l = l;	
			}
			else
			{
				FIRTEX_CLOG(level::warn) << _T("Load WORD_LEVEL memory managament strategy configure error.") << FIRTEX_ENDL;
			}			
			if(getMMSConfig(Advance.MMS.DocumentLevel.strategy,strategy,n,k,l))
			{

				CInMemoryPosting::DL_ALLOCSTRATEGY.strategy = (CInMemoryPosting::MEMALLOC_STRATEGY)strategy;
				CInMemoryPosting::DL_ALLOCSTRATEGY.n = n;
				CInMemoryPosting::DL_ALLOCSTRATEGY.k = k;
				CInMemoryPosting::DL_ALLOCSTRATEGY.l = l;	
			}
			else
			{
				FIRTEX_CLOG(level::warn) << _T("Load DOCUMENT_LEVEL memory managament strategy configure error.") << FIRTEX_ENDL;
			}			

			if(getMMSConfig(Advance.MMS.DocumentLevel0.strategy,strategy,n,k,l))
			{

				CInMemoryPosting::DL_ALLOCSTRATEGY.strategy = (CInMemoryPosting::MEMALLOC_STRATEGY)strategy;
				CInMemoryPosting::DL_ALLOCSTRATEGY.n = n;
				CInMemoryPosting::DL_ALLOCSTRATEGY.k = k;
				CInMemoryPosting::DL_ALLOCSTRATEGY.l = l;	
			}
			else
			{
				FIRTEX_CLOG(level::warn) << _T("Load DOCUMENT_LEVEL_0 memory managament strategy configure error.") << FIRTEX_ENDL;
			}			
		}
		bool CConfig::getMMSConfig(tstring strConfig,int32_t& strategy,int32_t& n,int32_t& k,int32_t l)
		{
			try
			{
				vector<tstring>ps = firtex::string_utils::split(strConfig,_T(":"));				
				if(ps.size() >= 2)
				{					
					if(!_tcsicmp(ps[0].c_str(),_T("exp")))
					{						
						strategy = CInMemoryPosting::ALLOC_EXP;
						if(ps.size() >= 3)
						{
							n = _ttoi(ps[1].c_str());
							k = _ttoi(ps[2].c_str());
						}
						else 
						{
							n = 32;
							k = 2;
						}
					}
					else if(!_tcsicmp(ps[0].c_str(),_T("explimit")))
					{
						strategy = CInMemoryPosting::ALLOC_EXPLIMIT;
						if(ps.size() >= 4)
						{
							n = _ttoi(ps[1].c_str());
							k = _ttoi(ps[2].c_str());
							l = _ttoi(ps[3].c_str());
						}
						else 
						{
							n = 32;
							k = 2;
							l = 256;
						}
					}	
					else
					{			
						strategy = CInMemoryPosting::ALLOC_CONST;
						n = _ttoi(ps[1].c_str());
					}
				}				
				else return false;
			}
			catch (...)
			{
				return false;
			}			
			return true;
		}

		//////////////////////////////////////////////////////////////////////
		// CConfig load

		void CConfig::load(const tstring& sFile)
		{
			if(!sFile.empty())
			{
				m_sPath = sFile;
			}	

			CXMLElement* pXML = CXMLElement::fromFile(m_sPath);
			if(pXML == NULL)
				return;

			list<CParamItem*>::iterator iter = m_items.begin();
			while (iter != m_items.end())
			{
				(*iter)->load(pXML);
				iter++;
			}			

			delete pXML;
		}

		void CConfig::save(const tstring& sFile)
		{	
			if(!sFile.empty())
			{
				m_sPath = sFile;
			}

			CXMLElement* pXML = new CXMLElement(NULL,_T("config"));			

			list<CParamItem*>::iterator iter = m_items.begin();
			while (iter != m_items.end())
			{
				(*iter)->save(pXML);
				iter++;
			}			
					
			pXML->toFile(m_sPath,true);
			delete pXML;			
		}		

		//////////////////////////////////////////////////////////////////////
		// CConfig add items

		void CConfig::add(const tstring& sName, int64_t* pDword, int64_t nDefault)
		{
			m_items.push_back( new CParamItem( sName, pDword,NULL, NULL, NULL ) );
			*pDword = nDefault;
		}

		void CConfig::add(const tstring& sName, int32_t* pDword, int32_t nDefault)
		{			
			m_items.push_back( new CParamItem( sName, NULL,pDword, NULL, NULL ) );
			*pDword = nDefault;
		}

		void CConfig::add(const tstring& sName, double* pFloat, double nDefault)
		{
			m_items.push_back( new CParamItem( sName, NULL, NULL,pFloat, NULL ) );
			*pFloat = nDefault;
		}

		void CConfig::add(const tstring& sName, tstring* pString, const tstring& pszDefault)
		{
			m_items.push_back( new CParamItem( sName, NULL,NULL, NULL, pString ) );
			*pString = pszDefault;
		}

		//////////////////////////////////////////////////////////////////////
		// CConfig::CParamItem construction and operations

		CConfig::CParamItem::CParamItem(const tstring& sName, int64_t* pInt64,int32_t* pInt32,double* pFloat, tstring* pString)
		{
			m_sName		= sName;
			m_pInt64	= pInt64;
			m_pInt32	= pInt32;
			m_pFloat	= pFloat;
			m_pString	= pString;
		}

		void CConfig::CParamItem::load(CXMLElement* pXML)
		{
			CXMLElement* pSub = pXML;
			tstring sRight = m_sName;
			tstring sLeft;
			firtex::string_utils::trim(sRight);			
			tstring::size_type npos = (size_t)-1;	
			tstring::size_type idx;

			while(sRight != "")
			{
				idx = sRight.find('.');
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
		}

		void CConfig::CParamItem::save(CXMLElement* pXML)
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
		}

	}
}
