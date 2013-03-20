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
// Created	: 2006/7/13
//
#include "search/SortFieldCache.h"
#include "search/SortFieldCacheImpl.h"

 namespace firtex
 {
	 namespace search
	 {
		 CSortFieldCache* CSortFieldCache::DEFAULT = new CSortFieldCacheImpl();
		 CSortFieldCache::CSortFieldCache(void)
		 {
		 }

		 CSortFieldCache::~CSortFieldCache(void)
		 {
		 }

		 CSortFieldComparator* CSortFieldCache::getComparator(CIndexReader* pReader, const tchar* field)
		 {
			 CSortFieldCompArray* pArray = getCompArray(pReader,field);
			 if(!pArray)
				 return NULL;
			 switch(pArray->arrayType)
			 {
			 case CSortFieldCompArray::I1_ARRAY:
				 return new CSortFieldComparators::I1(pArray->i8array,pArray->arrayLen);
				 break;
			 case CSortFieldCompArray::UI1_ARRAY:
				 return new CSortFieldComparators::UI1(pArray->ui8array,pArray->arrayLen);
				 break;
			 case CSortFieldCompArray::I2_ARRAY:
				 return new CSortFieldComparators::I2(pArray->i16array,pArray->arrayLen);
				 break;
			 case CSortFieldCompArray::UI2_ARRAY:
				 return new CSortFieldComparators::UI2(pArray->ui16array,pArray->arrayLen);
				 break;
			 case CSortFieldCompArray::I4_ARRAY:
				 return new CSortFieldComparators::I4(pArray->i32array,pArray->arrayLen);
				 break;
			 case CSortFieldCompArray::UI4_ARRAY:
				 return new CSortFieldComparators::UI4(pArray->ui32array,pArray->arrayLen);
				 break;
			 case CSortFieldCompArray::I8_ARRAY:
				 return new CSortFieldComparators::I8(pArray->i64array,pArray->arrayLen);
				 break;
			 case CSortFieldCompArray::UI8_ARRAY:
				 return new CSortFieldComparators::UI8(pArray->ui64array,pArray->arrayLen);
				 break;
			 case CSortFieldCompArray::R4_ARRAY:
				 return new CSortFieldComparators::R4(pArray->farray,pArray->arrayLen);
				 break;			 
			 case CSortFieldCompArray::R8_ARRAY:
				 return new CSortFieldComparators::R8(pArray->dbarray,pArray->arrayLen);
				 break;
			 }
			 return NULL;
		 }
	 }
 }

