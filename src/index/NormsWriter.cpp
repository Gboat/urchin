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
// Created	: 2006/1/4
// Modified : 2006/10/3 change norms from byte to int32_t,no precision lost.
//

#include "index/NormsWriter.h"
#include "index/NormsReader.h"

namespace firtex
{
	namespace index
	{	
		CNormsWriter::CNormsWriter(firtex::store::CDirectory* pDirectory)
			:m_pDirectory(pDirectory)
			,m_nFieldNum(-1)
			,m_ppCachedNormBytes(NULL)
		{			
		}
		CNormsWriter::CNormsWriter(firtex::store::CDirectory* pDirectory,const tchar* barrelName)
			:m_pDirectory(pDirectory)
			,m_nFieldNum(-1)
			,m_ppCachedNormBytes(NULL)
		{					
			open(barrelName);
		}
		CNormsWriter::~CNormsWriter(void)
		{
			close();
		}	
		void CNormsWriter::open(const tchar* barrelName)
		{
			close();
			m_barrelName = barrelName;
		}

		void CNormsWriter::close()
		{
			for(int32_t i = 0;i < m_nFieldNum;i++)
			{
				if(m_ppCachedNormBytes[i])
				{
					if(m_ppCachedNormBytes[i]->m_length > 0)
					{
						tstring str = m_barrelName;
						str += _T(".n");
						str = firtex::string_utils::append(str,i);
						CIndexOutput* pOutput = m_pDirectory->createOutput(str);
						pOutput->writeInts(m_ppCachedNormBytes[i]->m_bytes,m_ppCachedNormBytes[i]->m_length);
						delete pOutput;					
					}
					delete m_ppCachedNormBytes[i];
				}
			}

			if(m_nFieldNum > 0)
			{
				delete[] m_ppCachedNormBytes;
				m_ppCachedNormBytes = NULL;
			}
			m_nFieldNum = 0;
		}		

		CNormsReader* CNormsWriter::normsReader(CFieldsInfo* pFieldsInfo)
		{
			if(m_nFieldNum <= 0)
				return NULL;
			CNormsReader* pNormReader = new CNormsReader(m_pDirectory);
			pNormReader->open(m_ppCachedNormBytes,m_nFieldNum,pFieldsInfo);
			return pNormReader;
		}
	}
}
