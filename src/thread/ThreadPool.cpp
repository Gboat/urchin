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
// Created	: 2005/11/28
//
#include "thread/ThreadPool.h"

namespace firtex
{
	namespace thread
	{
		CThreadPool::CThreadPool(void)
		{
		}

		CThreadPool::~CThreadPool(void)
		{
			clear();
		}
		void CThreadPool::add(CRunnalble* aRun)
		{
			CThread* pThread = new CThread();
			m_threads.push_back(pThread);
			pThread->spawn(aRun);
		}
		void CThreadPool::wait()
		{
			/*vector<CThread*>::iterator iter = m_threads.begin();
			while(iter != m_threads.end())
			{
				(*iter)->wait();
				iter++;
			}*/
		}

		void CThreadPool::clear()
		{
			vector<CThread*>::iterator iter = m_threads.begin();
			while(iter != m_threads.end())
			{
				delete *iter;
				iter++;
			}
			m_threads.clear();
		}


	}
}

