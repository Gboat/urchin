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
#ifndef _SORTFIELDCOMPARATOR_H
#define _SORTFIELDCOMPARATOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "ScoreDoc.h"

namespace firtex
{
	namespace search
	{
		class CSortFieldComparator
		{
		public:
			CSortFieldComparator(void){}
			virtual ~CSortFieldComparator(void){}
		public:
			virtual int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2) = 0;
		};

		class CSortFieldComparators
		{
		public:
			CSortFieldComparators(){}
			~CSortFieldComparators(){}
		public:			
			class Relevance : public CSortFieldComparator
			{				
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
				{
					if (pDoc1->m_fScore > pDoc2->m_fScore) return -1;
					if (pDoc1->m_fScore < pDoc2->m_fScore) return 1;
					return 0;
				}
			};

			class IndexOrder : public CSortFieldComparator
			{
			public:			
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
				{
					if (pDoc1->m_docID > pDoc2->m_docID) return -1;
					if (pDoc1->m_docID < pDoc2->m_docID) return 1;
					return 0;
				}
			};

			///I1
			class I1 : public CSortFieldComparator
			{
			public:
				I1(int8_t* compArray,int32_t len);
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2);
			protected:
				int8_t*	m_compArray;
				int32_t m_length;
			};

			///UI1
			class UI1 : public CSortFieldComparator
			{
			public:
				UI1(uint8_t* compArray,int32_t len);
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2);
			protected:
				uint8_t*	m_compArray;
				int32_t m_length;
			};

			///I2
			class I2 : public CSortFieldComparator
			{
			public:
				I2(int16_t* compArray,int32_t len);
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2);
			protected:
				int16_t*	m_compArray;
				int32_t		m_length;
			};

			///UI2
			class UI2 : public CSortFieldComparator
			{
			public:
				UI2(uint16_t* compArray,int32_t len);
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2);
			protected:
				uint16_t*	m_compArray;
				int32_t		m_length;
			};

			///I4
			class I4 : public CSortFieldComparator
			{
			public:
				I4(int32_t* compArray,int32_t len);
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2);
			protected:
				int32_t*	m_compArray;
				int32_t		m_length;
			};

			///UI4
			class UI4 : public CSortFieldComparator
			{
			public:
				UI4(uint32_t* compArray,int32_t len);
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2);
			protected:
				uint32_t*	m_compArray;
				int32_t		m_length;
			};

			///I8
			class I8 : public CSortFieldComparator
			{
			public:
				I8(int64_t* compArray,int32_t len);
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2);
			protected:
				int64_t*	m_compArray;
				int32_t		m_length;
			};

			///UI8
			class UI8 : public CSortFieldComparator
			{
			public:
				UI8(uint64_t* compArray,int32_t len);
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2);
			protected:
				uint64_t*	m_compArray;
				int32_t		m_length;
			};

			///R4
			class R4 : public CSortFieldComparator
			{
			public:
				R4(float* compArray,int32_t len);
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2);
			protected:
				float*	m_compArray;
				int32_t m_length;
			};

			///R8
			class R8 : public CSortFieldComparator
			{
			public:
				R8(double* compArray,int32_t len);
			public:
				int32_t compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2);
			protected:
				double*	m_compArray;
				int32_t m_length;
			};
		};
	}
}

#endif

