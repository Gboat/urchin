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
#include "collection/CollectionFactory.h"
#include "collection/DirCollection.h"
#include "collection/FileListCollection.h"
#include "collection/TrecCollection.h"
#include "collection/ZIPTrecCollection.h"
#include "collection/GZTrecCollection.h"
#include "collection/FirteXCollection.h"
#include "collection/TWCollection.h"
#include "collection/GZTWCollection.h"
#include "utility/StringUtils.h"

namespace firtex
{
	namespace collection
	{		
		CCollectionFactory::CCollectionFactory(void)
		{
		}

		CCollectionFactory::~CCollectionFactory(void)
		{
		}

		CCollection* CCollectionFactory::createCollection(const tchar* identifier,const tchar* path,const tchar* szParam)
		{	
			tstring str;
			if(!firtex::string_utils::startsWithNoCase(identifier,_T("collection.")))
				str = _T("collection.");
			str += identifier;

			CCollection::Creator* pCreator = CCollectionFactory::getCreator(str.c_str());
			CCollection* pCol = NULL;
			if(pCreator)
			{
				pCol = pCreator->create(path);
				if(pCol && szParam && *szParam)
					pCol->setParam(szParam);
			}
			return pCol;		
		}
		void CCollectionFactory::registerCreator(CCollection::Creator* pCreator)
		{
			tstring str = pCreator->getIdentifier();
			getCreatorMap().insert(make_pair(str,pCreator));
		}
		CCollection::Creator* CCollectionFactory::getCreator(const tchar* _identifier)
		{
			tstring str = _identifier;
			creator_map& cm = getCreatorMap();
			map<tstring,CCollection::Creator*>::iterator iter = cm.find(str);
			if(iter != cm.end())
				return iter->second;
			return NULL;
		}

		creator_map& CCollectionFactory::getCreatorMap()
		{
			static creator_map	COLLECTION_CREATORS;
			return COLLECTION_CREATORS;
		}
		collection_list CCollectionFactory::getRegisteredCollections()
		{			
			collection_list cl;
			creator_map& am = getCreatorMap();
			creator_map::iterator iter = am.begin();
			while(iter != am.end())
			{
				cl.push_back(iter->first);
				iter++;
			}
			return cl;
		}
	}
}
