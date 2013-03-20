//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: π˘»Ω‹(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/7/3
//
#ifndef __FORWARDINDEXWRITER_H
#define __FORWARDINDEXWRITER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "../document/Document.h"
#include "../document/Field.h"
#include "../store/IndexOutput.h"
#include "../store/Directory.h"

using namespace firtex;
using namespace firtex::store;


namespace firtex
{
	namespace index
	{
		class CTermSequenceVector;
		class CTermFreqVector;
		class CTermPositionVector;
		class CFactoryFinder;
		class CForwardIndexWriter
		{
		public:
			CForwardIndexWriter(CDirectory* pDirectory);
			virtual ~CForwardIndexWriter(void);
		public:
			void	open(const tchar* barrel);
			void	addDocument(document::CDocument* pDoc);
			void	close();

			/** …Ë÷√”Úπ§≥ß */
			void			setFactoryFinder(CFactoryFinder* pFactories){m_pFieldFactories = pFactories;}
		private:
			CDirectory*		m_pDirectory;
			CIndexOutput*	m_pTVIOutput;
			CIndexOutput*	m_pTVDOutput;
			CIndexOutput*	m_pTVVOutput;
			
			CTermSequenceVector*	m_pSequenceWriter;
			CTermFreqVector*		m_pFreqWriter;
			CTermPositionVector*	m_pPositionWriter;

			CFactoryFinder*		m_pFieldFactories;
		};
	}
}

#endif
