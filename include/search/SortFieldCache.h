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
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/7/13
//
#ifndef _SORTFIELDCACHE_H
#define _SORTFIELDCACHE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SortFieldComparator.h"
#include "../index/IndexReader.h"
using namespace firtex::index;

namespace firtex
{
	namespace search
	{
		class CSortFieldCompArray;
		class CSortFieldCache
		{
		public:
			CSortFieldCache(void);
			virtual ~CSortFieldCache(void);
		public:
			static CSortFieldCache*	DEFAULT;
		public:
			/**
			 * 获取字段比较算子
			 * @param pReader 用于读取字段的索引数据
			 * @param field 字段名
			 * @return 字段比较算子
			 * @memory 非内部对象,需外部删除
			 */
			virtual CSortFieldComparator*	getComparator(CIndexReader* pReader, const tchar* field);

			/**
			 * 获取用于字段比较的数组
			 * @param pReader 用于读取字段的索引数据
			 * @param field 字段名
			 */
			virtual	CSortFieldCompArray*	getCompArray(CIndexReader* pReader, const tchar* field) = 0;
		};

		class CSortFieldCompArray
		{
		public:
			CSortFieldCompArray()
			{
				i8array = NULL;
			}			
			~CSortFieldCompArray()
			{
				switch(arrayType)
				{
				case I1_ARRAY:
					delete[] i8array;
					break;
				case UI1_ARRAY:
					delete[] ui8array;
					break;
				case I2_ARRAY:
					delete[] i16array;
					break;
				case UI2_ARRAY:
					delete[] ui16array;
					break;
				case I4_ARRAY:
					delete[] i32array;
					break;
				case UI4_ARRAY:
					delete[] ui32array;
					break;
				case I8_ARRAY:
					delete[] i64array;
					break;
				case UI8_ARRAY:
					delete[] ui64array;
					break;
				case R4_ARRAY:
					delete[] farray;
					break;
				case R8_ARRAY:
					delete[] dbarray;
					break;
				}
			}

			enum ARRAYENUM
			{				
				I1_ARRAY		= 1,
				UI1_ARRAY		= 2,
				I2_ARRAY		= 3,
				UI2_ARRAY		= 4,
				I4_ARRAY		= 5,
				UI4_ARRAY		= 6,
				I8_ARRAY		= 7,
				UI8_ARRAY		= 8,

				R4_ARRAY		= 9,
				R8_ARRAY		= 10,

				SORT_COMPARATOR	= 11,
			};

			typedef unsigned short array_type;
			
			array_type	arrayType;			
			int32_t		arrayLen;		//数组长度,当阿rraylType = SORT_COMPARATOR时无效
			union
			{				
				int8_t*					i8array;
				uint8_t*				ui8array;
				int16_t*				i16array;
				uint16_t*				ui16array;
				int32_t*				i32array;
				uint32_t*				ui32array;
				int64_t*				i64array;
				uint64_t*				ui64array;

				float*					farray;
				double*					dbarray;

				CSortFieldComparator*	sortComparator;	
			};		
		};
	}
}

#endif
