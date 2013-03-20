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
// Created	: 2006/11/13
//
#include "index/DictHashFactory.h"
#include "index/DictHashMerger.h"
#include "index/DictHashIndexer.h"
#include "index/DictHashTermReader.h"
#include "index/DictHashSequenceVector.h"
#include "index/DictHashFreqVector.h"
#include "index/DictHashPositionVector.h"


#include "document/Field.h"
using namespace firtex::document;

namespace firtex
{
	namespace index
	{
		CDictHashFactory::CDictHashFactory(void)			
		{
			m_identifier += _T(".dicthash");
		}

		CDictHashFactory::~CDictHashFactory(void)
		{
		}

		CFieldIndexer* CDictHashFactory::createIndexer(CMemCache* pMemCache)
		{
			return new CDictHashIndexer(pMemCache);			
		}
		CFieldMerger* CDictHashFactory::createMerger()
		{
			return new CDictHashMerger(this);
		}
		CTermReader* CDictHashFactory::createTermReader()
		{
			return new CDictHashTermReader();			
		}

		CTermSequenceVector* CDictHashFactory::createTermSequenceVector()
		{
			return new CDictHashSequenceVector();			
		}
		CTermFreqVector* CDictHashFactory::createTermFreqVector()
		{
			return new CDictHashFreqVector();			
		}
		CTermPositionVector* CDictHashFactory::createTermPositionVector()
		{
			return new CDictHashPositionVector();			
		}

		CDictHashFactory* CDictHashFactory::getInstance()
		{
			static CDictHashFactory INSTANCE;
			return &INSTANCE;			
		}		
	}
}
