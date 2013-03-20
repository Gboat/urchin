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
// Created	: 2006/7/12
//
#ifndef _SORTER_H
#define _SORTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include "../utility/StdHeader.h"
#include "../utility/FXString.h"
#include "../search/SortFieldComparator.h"
#include "../index/IndexReader.h"
using namespace firtex::utility;
using namespace firtex::index;

namespace firtex
{
	namespace search
	{
		class CSortField
		{
		public:
			enum SortFieldType
			{
				DOCSCORE = 1,		//文档打分
				DOC = 2,			//文档编号
				AUTO = 3,			//自动判断字段类型				
				CUSTOM = 4,			//自定义
			};
		public:
			CSortField();
			CSortField(const CSortField& src);
			CSortField(const tchar* field);
			CSortField(const tchar* field,bool bReverse);
			CSortField(const tchar* field,SortFieldType type,bool bReverse = false);
			CSortField(const tchar* field,CSortFieldComparator* pComparator,bool bReverse = false);
			~CSortField();			
		public:			
		public:
			const tchar*			getField(){return m_field.c_str();}
			SortFieldType			getType(){return m_type;}
			bool					getReverse(){return m_bReverse;}
			CSortFieldComparator*	getComparator(){return m_comparator;}
		protected:
			static CSortField* getScoreField();
			static CSortField* getDocField();
		protected:
			CFXString				m_field;		//排序字段名
			SortFieldType			m_type;			//排序字段类型
			bool					m_bReverse;		//比较顺序是否需要颠倒
			CSortFieldComparator*	m_comparator;	//文档字段比较器

			friend class CSort;
			friend class CSortFields;
		};

		class CSortFields
		{
		public:
			CSortFields(int32_t nNumFields);
			CSortFields(const CSortFields& src);
			~CSortFields();
		public:
			CSortField*& operator[](int32_t order);
		protected:
			CSortField**	m_sortFields;
			int32_t			m_numFields;

			friend class CSort;
		};

		class CSortFieldCache;
		class CSort
		{
		public:
			CSort(void);
			CSort(const tchar* field,bool reverse);
			CSort(const tchar** field,int32_t nNumFields);
			CSort(const tchar** field,bool* bReverses,int32_t nNumFields);
			CSort(CSortField* pSortField);
			CSort(CSortFields* pSortFields);
			~CSort(void);			
		public:
			/**
			 * 设置需要排序的字段信息，没有设置的信息将使用默认值
			 * @param field 排序字段名
			 * @param bReverse 是否颠倒排序
			 */
			void	setSortField(const tchar* field,bool bReverse);

			/**
			 * 设置需要排序的字段信息，没有设置的信息将使用默认值
			 * @param field 多个排序字段名
			 * @param nNumFields 字段个数			 
			 */
			void	setSortField(const tchar** field,int32_t nNumFields);

			/**
			 * 设置需要排序的字段信息，没有设置的信息将使用默认值
			 * @param field 多个排序字段名
			 * @param nNumFields 字段个数
			 * @param bReverse 是否颠倒排序
			 */
			void	setSortField(const tchar** field,bool* bReverses,int32_t nNumFields);

			/**
			 * 设置需要排序的字段信息
			 * @param pSortField 排序字段,内部删除			 
			 */
			void	setSortField(CSortField* pSortField);

			/**
			 * 设置多个需要排序的字段信息,位置越靠前的字段，排序时优先级越高
			 * @param pSortFields 多个排序字段,内部删除
			 */
			void	setSortField(CSortFields* pSortFields);		

			/**
			 * 清除所有排序信息
			 */
			void	clear();
		public:
			bool	lessThan(CScoreDoc* pDoc1,CScoreDoc* pDoc2);

			void	getComparators(CIndexReader* pReader,CSortFieldCache* pFieldCache);
		protected:
			static CSort* getRelevanceSort();
			static CSort* getDocOrderSort();
		protected:
			CSortFields*	m_pSortFields;
		};
		//////////////////////////////////////////////////////////////////////////
		//
		inline CSortField*& CSortFields::operator[](int32_t order)
		{
			assert(order >=0 && order < m_numFields);
			return m_sortFields[order];
		}
		//////////////////////////////////////////////////////////////////////////
		//
		inline bool CSort::lessThan(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{			
			int32_t c = 0;
			int32_t i=0; 
			int32_t nNumFields = m_pSortFields->m_numFields;
			CSortField** pSortFields = m_pSortFields->m_sortFields;
			while((i<nNumFields) &&(c==0))
			{
				c = (pSortFields[i]->getReverse()) ? pSortFields[i]->m_comparator->compare (pDoc2, pDoc1) 
					: pSortFields[i]->m_comparator->compare (pDoc1, pDoc2);
				i++;
			}
			
			if (c == 0)
				return pDoc1->m_docID > pDoc2->m_docID;
			return c > 0;			
		}
	}
}

#endif
