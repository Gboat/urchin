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
// Created	: 2007/1/17
//

#include "search/Searcher.h"

namespace firtex
{
	namespace search
	{
		CSearcher::CSearcher(void)
			:m_pQueryBuffer(NULL)
			,m_nBufferSize(0)
			,m_bOwnBuffer(false)
		{
			m_pSimilarity = CSimilarity::getDefault();//创建默认的Similarity 
		}

		CSearcher::~CSearcher(void)
		{				
			m_pSimilarity = NULL;
			if(m_bOwnBuffer && m_pQueryBuffer)
			{
				delete[] m_pQueryBuffer;
				m_pQueryBuffer = NULL;
			}
			m_nBufferSize = 0;
		}

		void CSearcher::setBuffer(int32_t* buffer,size_t length)
		{
			if(m_bOwnBuffer && m_pQueryBuffer)
			{
				delete[] m_pQueryBuffer;
				m_pQueryBuffer = NULL;
			}
			m_pQueryBuffer = buffer;
			m_nBufferSize = length;
			m_bOwnBuffer = false;
		}
		int32_t* CSearcher::getBuffer()
		{
			if(m_pQueryBuffer == NULL)
				createBuffer();
			return m_pQueryBuffer;
		}
		size_t CSearcher::getBufferSize()
		{
			if(m_nBufferSize <= 0)
				createBuffer();
			return m_nBufferSize;
		}
		void CSearcher::createBuffer()
		{
			try
			{
				m_nBufferSize = (size_t)(GlobalConfig.Search.memory/sizeof(int32_t));
				m_pQueryBuffer = new int32_t[m_nBufferSize];
				m_bOwnBuffer = true;			
			}
			catch (std::bad_alloc& )
			{				
				FIRTEX_THROW3(RUNTIME_ERROR,_T("Allocate memory failed."));
			}			
			if(!m_pQueryBuffer)
			{				
				FIRTEX_THROW3(RUNTIME_ERROR,_T("Allocate memory failed."));
			}
		}
	}
}
