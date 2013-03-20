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
// Created	: 2006/7/17
//
#include "index/TermIterator.h"
namespace firtex
{
	namespace index
	{
		CTermIterator::CTermIterator(void)
			:m_pBuffer(NULL)
			,m_nBuffSize(0)
		{
		}

		CTermIterator::~CTermIterator(void)
		{
			m_pBuffer = NULL;
			m_nBuffSize = 0;
		}
		size_t CTermIterator::setBuffer(char* pBuffer,size_t bufSize)
		{
			m_pBuffer = pBuffer;
			m_nBuffSize = bufSize;
			return bufSize;
		}
	}
}
