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
// Created	: 2006/7/14
//
#ifndef _SORTFIELDCACHEIMPL_H
#define _SORTFIELDCACHEIMPL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SortFieldCache.h"
#include <map>
using namespace std;

namespace firtex
{
	namespace search
	{
		class CSortFieldCacheImpl :	public CSortFieldCache
		{
		public:
			CSortFieldCacheImpl(void);
			virtual ~CSortFieldCacheImpl(void);
		public:
			CSortFieldCompArray*	getCompArray(CIndexReader* pReader, const tchar* field);
		protected:
			CSortFieldCompArray*	getI1(CIndexReader* pReader, CFieldInfo* pFieldInfo);
			CSortFieldCompArray*	getUI1(CIndexReader* pReader, CFieldInfo* pFieldInfo);
			CSortFieldCompArray*	getI2(CIndexReader* pReader, CFieldInfo* pFieldInfo);
			CSortFieldCompArray*	getUI2(CIndexReader* pReader, CFieldInfo* pFieldInfo);
			CSortFieldCompArray*	getI4(CIndexReader* pReader, CFieldInfo* pFieldInfo);
			CSortFieldCompArray*	getUI4(CIndexReader* pReader, CFieldInfo* pFieldInfo);
			CSortFieldCompArray*	getI8(CIndexReader* pReader, CFieldInfo* pFieldInfo);
			CSortFieldCompArray*	getUI8(CIndexReader* pReader, CFieldInfo* pFieldInfo);
			CSortFieldCompArray*	getR4(CIndexReader* pReader, CFieldInfo* pFieldInfo);
			CSortFieldCompArray*	getR8(CIndexReader* pReader, CFieldInfo* pFieldInfo);

			CSortFieldCompArray*	getCachedArray(const tstring& field);
			void					storeInCache(const tstring& field,CSortFieldCompArray* pArray);
		private:
			map<tstring,CSortFieldCompArray*>	m_cacheArray;
		};
		//////////////////////////////////////////////////////////////////////////
		//inline functions
		inline CSortFieldCompArray* CSortFieldCacheImpl::getCachedArray(const tstring& field)
		{
			map<tstring,CSortFieldCompArray*>::iterator iter = m_cacheArray.find(field);
			if(iter != m_cacheArray.end())
			{
				return iter->second;
			}
			return NULL;
		}
		inline void CSortFieldCacheImpl::storeInCache(const tstring& field,CSortFieldCompArray* pArray)
		{
			m_cacheArray.insert(make_pair(field,pArray));
		}

	}
}

#endif
