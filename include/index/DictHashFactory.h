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

#ifndef _ENCODEDFIELDFACTORY_H
#define _ENCODEDFIELDFACTORY_H

#include "FieldFactory.h"

namespace firtex
{
	namespace index
	{
		class CDictHashFactory : public CFieldFactory
		{		
		protected:
			CDictHashFactory(void);
		public:
			virtual ~CDictHashFactory(void);
		public:
			static CDictHashFactory* getInstance();
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
