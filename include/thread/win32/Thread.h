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
#ifndef _THREAD_WIN32_H
#define _THREAD_WIN32_H

#include <windows.h>
#include <errno.h>

#ifdef _CRT_
# include <process.h>
# ifdef _USE_BEGINTHREAD
#   define THREAD_CALL                __cdecl
#   define THREAD_HANDLE              uintptr_t
#   define THREAD_RET_T               void
#   define CREATE_THREAD_FAILED       (-1L)
#   define CREATE_THREAD_ERROR        (errno)
#   define CREATE_THREAD(_S,_F,_P)    ((HANDLE)_beginthread((void (__cdecl *)(void *))_F,_S,(void *)_P))
#   define EXIT_THREAD                _endthread()
#   define CLOSE_HANDLE(x)            1
#   define THREAD_RETURN(x)           return
# else
#   define THREAD_CALL                WINAPI
#   define THREAD_HANDLE              HANDLE
#   define THREAD_RET_T               UINT
#   define CREATE_THREAD_FAILED       (0L)
#   define CREATE_THREAD_ERROR        (errno)
#   define CREATE_THREAD(_S,_F,_P)    ((HANDLE)_beginthreadex(0,_S,(UINT (WINAPI *)(void *))_F,(void *)_P,0,0))
#   define EXIT_THREAD                _endthreadex(0)
#   define CLOSE_HANDLE(x)            CloseHandle(x)
#   define THREAD_RETURN(x)           return(x)
# endif
#else
# define THREAD_CALL                WINAPI
# define THREAD_HANDLE              HANDLE
# define THREAD_RET_T               DWORD
# define CREATE_THREAD_FAILED       (0L)
# define CREATE_THREAD_ERROR        GetLastError()
# define CREATE_THREAD(_S,_F,_P)    ((HANDLE)CreateThread(0,_S,(DWORD (WINAPI *)(void *))_F,(void *)_P,0,0))
# define EXIT_THREAD                ExitThread(0)
# define CLOSE_HANDLE(x)            CloseHandle(x)
# define THREAD_RETURN(x)           return(x)
#endif

#include "Runnalble.h"

namespace firtex
{
	namespace thread
	{
		class CThread
		{
			typedef HANDLE	pthread_t;
			typedef DWORD	pid_t;
		public:
			CThread() 
				: m_pid(0)
				, m_hThread(NULL)
			{
			}
			~CThread()
			{
			}
		public:

			bool spawn(CRunnalble* task, bool bDetach = false)
			{
				m_hThread = CREATE_THREAD(0,_dispatch,task);

				if ( m_hThread == CREATE_THREAD_FAILED )
				{					
					return false;
				}
			
				if ( bDetach ) CLOSE_HANDLE(m_hThread);
				return true;			
			}

			pid_t getpid(){return m_pid;}

			void wait()
			{
				if(m_hThread == NULL)
					return;
				::WaitForSingleObject(m_hThread,INFINITE);
			}			
		public:
			static void exit(){ EXIT_THREAD; }		

			static void sleep(unsigned long timeout)
			{
				Sleep(timeout);
			}

			static pthread_t self()
			{
				//pthread_t hHandle = NULL;
				//DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),(LPHANDLE)&hHandle,NULL,0,NULL);
				//return hHandle;

				// only a pseudo-handle!
				return (pthread_t)GetCurrentThread();
			}

			static int kill( pthread_t hThread ){ return TerminateThread(hThread,0) ? 0 : EINVAL; }			

			static int detach( pthread_t hThread ){ return (CLOSE_HANDLE(hThread)? 0 : EINVAL); }
		protected:
			static unsigned int __stdcall _dispatch(void*arg)
			{
				CRunnalble* task = reinterpret_cast<CRunnalble*>(arg);

				//m_pid = GetCurrentProcessId();

				task->run();

				// Exit the thread
				exit();
				THREAD_RETURN(0);
			}
		protected:
			pid_t		m_pid;		///process id
			pthread_t	m_hThread;	///thread handle		
		};
	}
}

#endif
