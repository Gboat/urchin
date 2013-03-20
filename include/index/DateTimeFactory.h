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

#ifndef _DATETIMEFACTORY_H
#define _DATETIMEFACTORY_H

#include "FieldFactory.h"

namespace firtex
{
	namespace index
	{
		class CDateTimeFactory : public CFieldFactory
		{			
		protected:
			CDateTimeFactory(void);
		public:
			virtual ~CDateTimeFactory(void);
		public:
			static CDateTimeFactory* getInstance();
		public:
			virtual CFieldIndexer*			createIndexer(CMemCache* pMemCache);
			virtual CFieldMerger*			createMerger();
			virtual CTermReader*			createTermReader();

			virtual CTermSequenceVector*	createTermSequenceVector();
			virtual CTermFreqVector*		createTermFreqVector();
			virtual CTermPositionVector*	createTermPositionVector();
		};
	}
}

#endif
