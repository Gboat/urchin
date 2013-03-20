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
// Created	: 2006/6/10
//
#include "index/FieldFactory.h"
#include "index/DictHashFactory.h"
#include "utility/StringUtils.h"

namespace firtex
{
	namespace index
	{
		CFieldFactory::CFieldFactory(void)
			: m_identifier(_T("indexer"))
		{		
		}

		CFieldFactory::~CFieldFactory(void)
		{
		}
		factory_map& CFieldFactory::getFactoryMap()
		{
			static factory_map FACTORIES;
			return FACTORIES;
		}
		void CFieldFactory::registerFactory(CFieldFactory* pFactory)
		{			
			tstring str = pFactory->getIdentifier();
			factory_map& fmap = getFactoryMap();
			if(fmap.find(str) != fmap.end())
			{
				FIRTEX_CLOG(level::warn) << _T("Field factory: ") << str << _T("is already registered.") << FIRTEX_ENDL;
				return ;
			}
			getFactoryMap().insert(make_pair(str,pFactory));
		}
		
		CFieldFactory* CFieldFactory::getFactory(const tchar* _identifier)
		{
			tstring str = _identifier;
			factory_map& fmap = getFactoryMap();
			factory_map::const_iterator iter = fmap.find(_identifier);
			if(iter != fmap.end())
				return iter->second;
			return NULL;
		}

		//////////////////////////////////////////////////////////////////////////
		//CFactoryFinder
		CFactoryFinder::CFactoryFinder()
		{

		}
		CFactoryFinder::~CFactoryFinder()
		{
			clear();
		}
	
		CFieldFactory* CFactoryFinder::mapFactory(fieldid_t fid,const tchar* _identifier)
		{
			CFieldFactory* pFactory = CFieldFactory::getFactory(_identifier);
			if(pFactory)
			{

				map<fieldid_t,CFieldFactory*>::iterator iter = m_factoryById.find(fid);
				if(iter != m_factoryById.end())
				{
					FIRTEX_CLOG(level::warn) << _T("The field factory of field:") << fid << _T("already exist,over write it.")<< FIRTEX_ENDL;
					m_factoryById.erase(iter);
				}
				m_factoryById.insert(make_pair(fid,pFactory));
				
			}
			return pFactory;
		}
		
		CFieldFactory*	CFactoryFinder::getFactory(fieldid_t fid)
		{
			map<fieldid_t,CFieldFactory*>::iterator iter = m_factoryById.find(fid);
			if(iter != m_factoryById.end())
				return iter->second;
			return NULL;
		}
		
		CFieldFactory* CFactoryFinder::getFactoryByIdentifier(const tchar* _identifier)
		{
			map<fieldid_t,CFieldFactory*>::iterator iter = m_factoryById.begin();
			while(iter != m_factoryById.end())
			{
				if(!_tcsicmp(iter->second->getIdentifier(),_identifier))
				{
					return iter->second;
				}
				iter++;
			}
			return NULL;
		}
		void CFactoryFinder::clear()
		{			
			m_factoryById.clear();
		}		
	}
}
