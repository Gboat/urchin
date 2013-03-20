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
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/11/20
//
#include "analyzer/StandardStopFilter.h"
#include "utility/Config.h"
#include "utility/Exception.h"
#include "index/IndexWriter.h"

using namespace firtex::utility;
using namespace firtex::index;

namespace firtex
{
	namespace analyzer
	{
		CStandardStopFilter::CStandardStopFilter(const tchar* stopfile)
			:m_sStopwordFile(stopfile)
		{
			m_pStopList = 0;
			m_minTerm = m_maxTerm = 0;
			try 
			{
				load(m_sStopwordFile.c_str());
			}
			catch (CFirteXException& ex)
			{				
				FIRTEX_CLOG(level::warn) << _T("load stopword file failed: ") << ex.what() << FIRTEX_ENDL;
			}
		}
		CStandardStopFilter::CStandardStopFilter()			
		{	
			m_pStopList = 0;
			m_minTerm = m_maxTerm = 0;			
		}

		CStandardStopFilter::~CStandardStopFilter(void)
		{
			if(m_pStopList)
			{
				delete[] m_pStopList;
				m_pStopList = NULL;
			}
		}

		void CStandardStopFilter::load(const tchar* filename)
		{
			ifstream ifterm(filename,ios::in|ios::binary);	
			if (!ifterm.is_open()) 
			{
				FIRTEX_THROW2(FILEIO_ERROR, _T("Cannot open Stopper list file"));
				return;
			}	

			termid_t termid;
			ifterm.read((char*)&m_minTerm,sizeof(termid_t));
			ifterm.seekg(-((int)sizeof(termid_t)),ios::end);
			ifterm.read((char*)&m_maxTerm,sizeof(termid_t));
			m_pStopList = new bool[m_maxTerm - m_minTerm + 1];
			memset(m_pStopList,0,sizeof(bool)*(m_maxTerm - m_minTerm + 1));

			ifterm.seekg(ios::beg);
			while (! ifterm.eof()) 
			{
				ifterm.read((char*)&termid,sizeof(termid_t));
				add(termid);
			}

			ifterm.close();
		}
		void CStandardStopFilter::add(const termid_t tid)
		{
			if(tid < m_minTerm || tid > m_maxTerm)
				FIRTEX_THROW2(OUTOFORDER_ERROR,"The terms in stop list file is not sorted.");
			m_pStopList[tid - m_minTerm] = true;
		}

		void CStandardStopFilter::clear()
		{			
			m_minTerm = m_maxTerm = 0;
			if(m_pStopList)
			{
				delete[] m_pStopList;
				m_pStopList = NULL;
			}
		}	
	
		CTokens* CStandardStopFilter::filterInternal(CTokens* pInputTokens)
		{
			termid_t* terms = (termid_t*)pInputTokens->getTokenBuffer();
			size_t tt = pInputTokens->getNumTokens();
			for(size_t i = 0;i < tt;i++)
			{
				if(stopWord(terms[i]))
				{
					terms[i] = STOPWORD_ID;
				}
			}
			return pInputTokens;
		}
	}
}

