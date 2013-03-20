/////////////////////////////////////////////////////////////////////
//  Written by Phillip Sitbon
//  Copyright 2003
//
//  Posix/Mutex.h
//    - Resource locking mechanism using Posix mutexes
//
/////////////////////////////////////////////////////////////////////

#ifndef _MUTEX_LINUX_H
#define _MUTEX_LINUX_H

#include "../Lockable.h"

#include <pthread.h>

namespace firtex
{
	namespace thread
	{
		class CMutex : public CLockable
		{  
		public:
			CMutex()
			{
				pthread_mutexattr_t attr;
				pthread_mutexattr_init(&attr);
				pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
				pthread_mutex_init(&M,&attr);
				pthread_mutexattr_destroy(&attr);
			}

			virtual ~CMutex()
			{
				pthread_mutex_unlock(&M); 
				pthread_mutex_destroy(&M); 
			}

			void acquire()
			{ 
				pthread_mutex_lock(&M); 
			}

			bool tryAcquire()
			{
				return pthread_mutex_trylock(&M); 
			}

			void release()
			{ 
				pthread_mutex_unlock(&M); 
			}

		protected:
			pthread_mutex_t M;  
		};

		class CNullMutex
		{
		public:
			CNullMutex(){}
			~CNullMutex(){}
		public:
			inline void acquire(){}
			inline bool tryAcquire(unsigned long timeout){return true;}
			inline void release(){}
		};
	}
}

#endif 
