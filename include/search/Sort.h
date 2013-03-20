//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
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
				DOCSCORE = 1,		//�ĵ����
				DOC = 2,			//�ĵ����
				AUTO = 3,			//�Զ��ж��ֶ�����				
				CUSTOM = 4,			//�Զ���
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
			CFXString				m_field;		//�����ֶ���
			SortFieldType			m_type;			//�����ֶ�����
			bool					m_bReverse;		//�Ƚ�˳���Ƿ���Ҫ�ߵ�
			CSortFieldComparator*	m_comparator;	//�ĵ��ֶαȽ���

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
			 * ������Ҫ������ֶ���Ϣ��û�����õ���Ϣ��ʹ��Ĭ��ֵ
			 * @param field �����ֶ���
			 * @param bReverse �Ƿ�ߵ�����
			 */
			void	setSortField(const tchar* field,bool bReverse);

			/**
			 * ������Ҫ������ֶ���Ϣ��û�����õ���Ϣ��ʹ��Ĭ��ֵ
			 * @param field ��������ֶ���
			 * @param nNumFields �ֶθ���			 
			 */
			void	setSortField(const tchar** field,int32_t nNumFields);

			/**
			 * ������Ҫ������ֶ���Ϣ��û�����õ���Ϣ��ʹ��Ĭ��ֵ
			 * @param field ��������ֶ���
			 * @param nNumFields �ֶθ���
			 * @param bReverse �Ƿ�ߵ�����
			 */
			void	setSortField(const tchar** field,bool* bReverses,int32_t nNumFields);

			/**
			 * ������Ҫ������ֶ���Ϣ
			 * @param pSortField �����ֶ�,�ڲ�ɾ��			 
			 */
			void	setSortField(CSortField* pSortField);

			/**
			 * ���ö����Ҫ������ֶ���Ϣ,λ��Խ��ǰ���ֶΣ�����ʱ���ȼ�Խ��
			 * @param pSortFields ��������ֶ�,�ڲ�ɾ��
			 */
			void	setSortField(CSortFields* pSortFields);		

			/**
			 * �������������Ϣ
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
