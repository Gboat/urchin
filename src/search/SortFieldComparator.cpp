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

#include "search/SortFieldComparator.h"
#include "utility/Exception.h"
using namespace firtex::utility;

namespace firtex
{
	namespace search
	{
		//////////////////////////////////////////////////////////////////////////
		//I1
		CSortFieldComparators::I1::I1(int8_t* compArray,int32_t len)
			:m_compArray(compArray)
			,m_length(len)
		{
		}
		int32_t CSortFieldComparators::I1::compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{
			FIRTEX_ASSERT(pDoc1->m_docID < m_length, "pDoc1->m_docID>=length");
			FIRTEX_ASSERT(pDoc2->m_docID < m_length, "pDoc2->m_docID>=length");
			int8_t f1 = m_compArray[pDoc1->m_docID];
			int8_t f2 = m_compArray[pDoc2->m_docID];
			if (f1 < f2) return -1;
			if (f1 > f2) return 1;
			return 0;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//UI1
		CSortFieldComparators::UI1::UI1(uint8_t* compArray,int32_t len)
			:m_compArray(compArray)
			,m_length(len)
		{
		}

		int32_t CSortFieldComparators::UI1::compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{
			FIRTEX_ASSERT(pDoc1->m_docID < m_length, "pDoc1->m_docID>=length");
			FIRTEX_ASSERT(pDoc2->m_docID < m_length, "pDoc2->m_docID>=length");
			uint8_t f1 = m_compArray[pDoc1->m_docID];
			uint8_t f2 = m_compArray[pDoc2->m_docID];
			if (f1 < f2) return -1;
			if (f1 > f2) return 1;
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//I2
		CSortFieldComparators::I2::I2(int16_t* compArray,int32_t len)
			:m_compArray(compArray)
			,m_length(len)
		{
		}

		int32_t CSortFieldComparators::I2::compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{
			FIRTEX_ASSERT(pDoc1->m_docID < m_length, "pDoc1->m_docID>=length");
			FIRTEX_ASSERT(pDoc2->m_docID < m_length, "pDoc2->m_docID>=length");
			int16_t f1 = m_compArray[pDoc1->m_docID];
			int16_t f2 = m_compArray[pDoc2->m_docID];
			if (f1 < f2) return -1;
			if (f1 > f2) return 1;
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//UI2
		CSortFieldComparators::UI2::UI2(uint16_t* compArray,int32_t len)
			:m_compArray(compArray)
			,m_length(len)
		{
		}

		int32_t CSortFieldComparators::UI2::compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{
			FIRTEX_ASSERT(pDoc1->m_docID < m_length, "pDoc1->m_docID>=length");
			FIRTEX_ASSERT(pDoc2->m_docID < m_length, "pDoc2->m_docID>=length");
			uint16_t f1 = m_compArray[pDoc1->m_docID];
			uint16_t f2 = m_compArray[pDoc2->m_docID];
			if (f1 < f2) return -1;
			if (f1 > f2) return 1;
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//I4
		CSortFieldComparators::I4::I4(int32_t* compArray,int32_t len)
			:m_compArray(compArray)
			,m_length(len)
		{
		}

		int32_t CSortFieldComparators::I4::compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{
			FIRTEX_ASSERT(pDoc1->m_docID < m_length, "pDoc1->m_docID>=length");
			FIRTEX_ASSERT(pDoc2->m_docID < m_length, "pDoc2->m_docID>=length");
			int32_t f1 = m_compArray[pDoc1->m_docID];
			int32_t f2 = m_compArray[pDoc2->m_docID];
			if (f1 < f2) return -1;
			if (f1 > f2) return 1;
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//UI4
		CSortFieldComparators::UI4::UI4(uint32_t* compArray,int32_t len)
			:m_compArray(compArray)
			,m_length(len)
		{
		}

		int32_t CSortFieldComparators::UI4::compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{
			FIRTEX_ASSERT(pDoc1->m_docID < m_length, "pDoc1->m_docID>=length");
			FIRTEX_ASSERT(pDoc2->m_docID < m_length, "pDoc2->m_docID>=length");
			uint32_t f1 = m_compArray[pDoc1->m_docID];
			uint32_t f2 = m_compArray[pDoc2->m_docID];
			if (f1 < f2) return -1;
			if (f1 > f2) return 1;
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//I8
		CSortFieldComparators::I8::I8(int64_t* compArray,int32_t len)
			:m_compArray(compArray)
			,m_length(len)
		{
		}

		int32_t CSortFieldComparators::I8::compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{
			FIRTEX_ASSERT(pDoc1->m_docID < m_length, "pDoc1->m_docID>=length");
			FIRTEX_ASSERT(pDoc2->m_docID < m_length, "pDoc2->m_docID>=length");
			int64_t f1 = m_compArray[pDoc1->m_docID];
			int64_t f2 = m_compArray[pDoc2->m_docID];
			if (f1 < f2) return -1;
			if (f1 > f2) return 1;
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//UI8
		CSortFieldComparators::UI8::UI8(uint64_t* compArray,int32_t len)
			:m_compArray(compArray)
			,m_length(len)
		{
		}

		int32_t CSortFieldComparators::UI8::compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{
			FIRTEX_ASSERT(pDoc1->m_docID < m_length, "pDoc1->m_docID>=length");
			FIRTEX_ASSERT(pDoc2->m_docID < m_length, "pDoc2->m_docID>=length");
			uint64_t f1 = m_compArray[pDoc1->m_docID];
			uint64_t f2 = m_compArray[pDoc2->m_docID];
			if (f1 < f2) return -1;
			if (f1 > f2) return 1;
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//R4
		CSortFieldComparators::R4::R4(float* compArray,int32_t len)
			:m_compArray(compArray)
			,m_length(len)
		{
		}

		int32_t CSortFieldComparators::R4::compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{
			FIRTEX_ASSERT(pDoc1->m_docID < m_length, "pDoc1->m_docID>=length");
			FIRTEX_ASSERT(pDoc2->m_docID < m_length, "pDoc2->m_docID>=length");
			float f1 = m_compArray[pDoc1->m_docID];
			float f2 = m_compArray[pDoc2->m_docID];
			if (f1 < f2) return -1;
			if (f1 > f2) return 1;
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////
		//R8
		CSortFieldComparators::R8::R8(double* compArray,int32_t len)
			:m_compArray(compArray)
			,m_length(len)
		{
		}

		int32_t CSortFieldComparators::R8::compare(CScoreDoc* pDoc1,CScoreDoc* pDoc2)
		{
			FIRTEX_ASSERT(pDoc1->m_docID < m_length, "pDoc1->m_docID>=length");
			FIRTEX_ASSERT(pDoc2->m_docID < m_length, "pDoc2->m_docID>=length");
			double f1 = m_compArray[pDoc1->m_docID];
			double f2 = m_compArray[pDoc2->m_docID];
			if (f1 < f2) return -1;
			if (f1 > f2) return 1;
			return 0;
		}		
	}
}
