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
// Created	: 2006/6/10
//

#include "index/DateTimeMerger.h"
#include "store/IndexOutput.h"

namespace firtex
{
	namespace index
	{
		CDateTimeMerger::CDateTimeMerger(CFieldFactory* pFactory)
			:CFieldMerger(pFactory)
			,m_termCount(0)
			,m_lastTerm(0)
			,m_lastPOffset(0)
			,m_beginOfVoc(0)
		{

		}
		CDateTimeMerger::~CDateTimeMerger()
		{

		}	
		void CDateTimeMerger::flushTermInfo(COutputDescriptor* pOutputDescriptor,CMergeTermInfo** ppTermInfo,int32_t numTermInfos)
		{
			CIndexOutput* pVocOutput = pOutputDescriptor->getVocOutput();

			if(m_termCount == 0)
				m_beginOfVoc = pVocOutput->getFilePointer();
			CTermX<int64_t>* pTermX = NULL;
			int64_t tid;
			fileoffset_t poffset;
			for (int32_t i = 0;i<numTermInfos;i++)
			{
				pTermX = (CTermX<int64_t>*)ppTermInfo[i]->getTerm()->asTermX();
				tid = pTermX->getValue();
				pVocOutput->writeVLong(tid - m_lastTerm);				
				pVocOutput->writeVInt(ppTermInfo[i]->getTermInfo()->docFreq());
				poffset = ppTermInfo[i]->getTermInfo()->docPointer();
				pVocOutput->writeVLong(poffset - m_lastPOffset);
				m_lastTerm = tid;
				m_lastPOffset = poffset;

				m_termCount++;
			}

		}		
		fileoffset_t CDateTimeMerger::endMerge(COutputDescriptor* pOutputDescriptor)
		{
			CIndexOutput* pVocOutput = pOutputDescriptor->getVocOutput();
			fileoffset_t voffset = pVocOutput->getFilePointer();
			///begin write vocabulary descriptor
			pVocOutput->writeVLong(voffset - m_beginOfVoc);
			pVocOutput->writeVLong(m_termCount);
			///end write vocabulary descriptor
			return voffset;
		}		
	}
}

