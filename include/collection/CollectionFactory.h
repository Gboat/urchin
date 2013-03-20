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
#ifndef _COLLECTIONFACTORY_H
#define _COLLECTIONFACTORY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "Collection.h"

#include <map>
using namespace std;

namespace firtex
{
	namespace collection
	{	
		typedef map<tstring,CCollection::Creator*> creator_map;
		typedef vector<tstring>	collection_list;
		class CCollectionFactory
		{		
		protected:
			CCollectionFactory(void);
		public:
			virtual ~CCollectionFactory(void);
		public:
			/**
			 * creator a collection by identifier
			 * @param path for collection
			 * @param param parameter for collection. format:param_name=param_value;
			 * @return a new created collection
			 */
			static CCollection*				createCollection(const tchar* identifier,const tchar* path,const tchar* szParam);

			/**
			 * register a creator of collection			 
			 * @param pCreator creator of collection			 
			 */
			static void						registerCreator(CCollection::Creator* pCreator);

			/**
			 * get a registered creator
			 * @param _identifier identifier of collection
			 * @return creator of collection
			 */
			static CCollection::Creator*	getCreator(const tchar* _identifier);

			/**
			 * get creator map			 
			 */
			static creator_map&				getCreatorMap();

			/** 
			 * get registered collections
			 * @return registered collections 
			 */
			static collection_list			getRegisteredCollections();
		};
	}
}

#endif
