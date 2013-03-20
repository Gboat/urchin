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
// Created	: 2005/11/18
//

#ifndef _THREAD_LINUX_H
#define _THREAD_LINUX_H

#include <pthread.h>
#include <assert.h>

#include "thread/Runnalble.h"


namespace firtex
{
	namespace thread
	{
		class CThread
		{
		public:
			CThread()
				: m_hThread(0)
				, m_pid(0)
			{
			}
			~CThread()
			{
			}		

		protected:
			static void* _dispatch(void *arg)
			{
				CRunnalble* task = reinterpret_cast<CRunnalble*>(arg);
				//m_pid = ::getpid();

				task->run();

				pthread_exit((void**)0);
				return (void*)0;
			}
		public:
			bool spawn(CRunnalble* task, bool bDetach = false) 
			{
				int nRet = pthread_create(&m_hThread, 0, _dispatch, task);
				if(bDetach)
					pthread_detach(m_hThread);
				return (nRet != 0);
			}			
			pid_t getpid(){return m_pid;}
		public:
			static int			kill(pthread_t hThread){ return pthread_cancel(hThread); }

			static int			detach(pthread_t hThread){ return pthread_detach(hThread); }			

			static void			exit(){ pthread_exit(0); }

			static pthread_t	self(){return pthread_self();}
			static void sleep(int ms)
			{
				if (ms <= 1)
					return;
				struct timespec t1, t2;
				t1.tv_sec = (ms / 1000);
				t1.tv_nsec = (ms % 1000) * 1000000;
				int result = nanosleep(&t1, &t2);
				if (result < 0) {
					t1 = t2;
					nanosleep(&t1, &t2);
				}
			}
		protected:
			pthread_t	m_hThread;	
			pid_t		m_pid;
		};
	}
}


#endif
