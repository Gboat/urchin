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
// Created	: 2007/6/10
//

#include "index/DateTimeFactory.h"
#include "index/DateTimeIndexer.h"
#include "index/DateTimeMerger.h"
#include "index/DateTimeTermReader.h"

namespace firtex
{
	namespace index
	{
		CDateTimeFactory::CDateTimeFactory(void)
		{
			m_identifier += _T(".datetime");
		}

		CDateTimeFactory::~CDateTimeFactory(void)
		{
		}

		CFieldIndexer* CDateTimeFactory::createIndexer(CMemCache* pMemCache)
		{
			return new CDateTimeIndexer(pMemCache);	
		}
		CFieldMerger* CDateTimeFactory::createMerger()
		{
			return new CDateTimeMerger(this);
		}
		CTermReader* CDateTimeFactory::createTermReader()
		{
			return new CDateTimeTermReader();			
		}

		CTermSequenceVector* CDateTimeFactory::createTermSequenceVector()
		{
			return NULL;
		}
		CTermFreqVector* CDateTimeFactory::createTermFreqVector()
		{
			return NULL;
		}
		CTermPositionVector* CDateTimeFactory::createTermPositionVector()
		{
			return NULL;		
		}

		CDateTimeFactory* CDateTimeFactory::getInstance()
		{
			static CDateTimeFactory INSTANCE;
			return &INSTANCE;
		}		
	}
}
