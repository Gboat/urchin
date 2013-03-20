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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/6/10
//

#ifndef _DATEFIELDMERGER_H
#define _DATEFIELDMERGER_H

#include "../utility/StdHeader.h"
#include <vector>
#include "FieldMerger.h"
#include "../store/IndexInput.h"
#include "TermInfo.h"
using namespace std;
using namespace firtex::store;


namespace firtex
{
	namespace index
	{
		class CDateTimeMerger : public CFieldMerger
		{
		public:
			CDateTimeMerger(CFieldFactory* pFactory);
			virtual ~CDateTimeMerger();
		public:
			/**
			 * flush merged term info, Subclasses must define this one method. 			 
			 * @param pOutputDescriptor output descriptor
			 * @param ppTermInfo merged term infos
			 * @param numTermInfos number of term infos.
			 */
			void flushTermInfo(COutputDescriptor* pOutputDescriptor,CMergeTermInfo** ppTermInfo,int32_t numTermInfos);

			/**
			 * merge is over, Subclasses must define this one method. 			 
			 * @param pOutputDescriptor output descriptor
			 * @return offset of vocabulary descriptor.
			 */
			fileoffset_t endMerge(COutputDescriptor* pOutputDescriptor);
		protected:
			int64_t			m_termCount;	///total term count
			int64_t			m_lastTerm;		///last term id
			fileoffset_t	m_lastPOffset;	///last posting offset
			fileoffset_t	m_beginOfVoc;	///begin of vocabulary data
		};				
	}
}

#endif
