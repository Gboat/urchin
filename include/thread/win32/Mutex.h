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
#ifndef _MUTEX_WIN32_H
#define _MUTEX_WIN32_H

#include "thread/Lockable.h"
#include "thread/NonCopyable.h"
#include "utility/Exception.h"
#include "thread/ReadWriteLock.h"
#include <windows.h>
#include <map>

namespace firtex
{
	namespace thread
	{
		class CMutex : public CLockable//,private CNonCopyable
		{
			//mutable CRITICAL_SECTION m_CS;
			CRITICAL_SECTION m_CS;
		public:
			CMutex()
			{
				InitializeCriticalSection(&m_CS); 
			}

			virtual ~CMutex()
			{
				DeleteCriticalSection(&m_CS);
			}

			void acquire()
			{
				EnterCriticalSection(&m_CS);
			}

#if(_WIN32_WINNT >= 0x0400)
			bool tryAcquire(unsigned long timeout)
			{
				return (TryEnterCriticalSection(&m_CS)?true:false); 
			}
#else
			bool tryAcquire(unsigned long timeout)
			{
				return true; 
			}
#endif


			void release()
			{
				LeaveCriticalSection(&m_CS);
			}
		};

		class CReadWriteMutex : public ReadWriteLock
		{
			typedef std::map<DWORD,DWORD> CMapThreadToState;
		public:
			CReadWriteMutex();
			virtual ~CReadWriteMutex();
		public:
			/**
			 * Get a reference to the read-only Lockable.
			 * @return <em>Lockable&</em> reference to a Lockable that provides read-only access.
			 */
			virtual CLockable& getReadLock();

			/**
			 * Get a reference to the read-write Lockable.
			 * @return <em>Lockable&</em> reference to a Lockable that provides read-write access.
			 */
			virtual CLockable& getWriteLock();

		protected:
			class RLock : public CLockable
			{
				CReadWriteMutex& m_rwLock;
			public:
				RLock(CReadWriteMutex& rwLock) : m_rwLock(rwLock)
				{
				}
				virtual ~RLock(){}
			public:
				virtual void acquire();
				virtual bool tryAcquire(unsigned long timeout);
				virtual void release();			
			};

			class WLock : public CLockable
			{
				CReadWriteMutex& m_rwLock;
			public:
				WLock(CReadWriteMutex& rwLock) : m_rwLock(rwLock)
				{
				}
				virtual ~WLock(){}
			public:
				virtual void acquire();

				virtual bool tryAcquire(unsigned long timeout);

				virtual void release();
			protected:			
			};

		public:			
			void acquireWriterLock();
			void releaseWriterLock();
			void acquireReaderLock();
			void releaseReaderLock();
		protected:			
			/// Internal/Real implementation
			inline void acquireReaderLock_i();
			inline void releaseReaderLock_i();
			inline void acquireWriterLock_i(bool bMustWait);
			inline void releaseWriterLock_i(bool bDowngrade);
		private:
			/// A critical section to guard all the other members
			mutable CRITICAL_SECTION	m_cs;
			
			/// Auto-reset event, will be dynamically created/destroyed on demand
			HANDLE				m_hSafeToWriteEvent;
			
			/// Manual-reset event, will be dynamically created/destroyed on demand
			HANDLE				m_hSafeToReadEvent;
			
			/// Total number of writers on this object
			int					m_nNumOfWriter;
			
			/// Total number of readers have already owned this object
			int					m_nNumOfReaderEntered;
			
			/// Total number of readers are waiting to be owners of this object
			int					m_nNumOfReaderWaiting;		
			
			CMapThreadToState	m_map;

			RLock				m_rLock;
			WLock				m_wLock;

			friend class RLock;
			friend class WLock;		
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

		class CNullRWMutex : public ReadWriteLock
		{
		public:
			CNullRWMutex(){}
			virtual~CNullRWMutex(){}
		public:
			/**
			 * Get a reference to the read-only Lockable.
			 * @return <em>Lockable&</em> reference to a Lockable that provides read-only access.
			 */
			virtual CLockable& getReadLock(){return m_rLock;}

			/**
			 * Get a reference to the read-write Lockable.
			 * @return <em>Lockable&</em> reference to a Lockable that provides read-write access.
			 */
			virtual CLockable& getWriteLock(){return m_wLock;}
		protected:
			class NullRLock : public CLockable
			{
			public:
				NullRLock();
				~NullRLock();
			public:
				void acquire(){}
				bool tryAcquire(unsigned long timeout){return true;}
				void release(){}
			};

			class NullWLock : public CLockable
			{
			public:
				NullWLock();
				~NullWLock();
			public:
				void acquire(){}
				bool tryAcquire(unsigned long timeout){return true;}
				void release(){}
			};
		protected:
			NullRLock m_rLock;
			NullWLock m_wLock;
		
		};		

		//////////////////////////////////////////////////////////////////////////
		///
		inline void CReadWriteMutex::acquireReaderLock_i()
		{
			if(m_nNumOfWriter)
			{
				++m_nNumOfReaderWaiting;
				if(NULL == m_hSafeToReadEvent)
				{
					//////////////////////////////////////////////////////
					// This is a long-time task inside Critical Section
					// However, there isn't any effective way to overcome
					m_hSafeToReadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				}

				do
				{
					LeaveCriticalSection(&m_cs);
					WaitForSingleObject(m_hSafeToReadEvent, INFINITE);
					// There might be one or more Writers entered, that's
					// why we need DO-WHILE loop here
					EnterCriticalSection(&m_cs);
				}
				while(m_nNumOfWriter);

				// Enter successful after wait
				++m_nNumOfReaderEntered;

				if(0==--m_nNumOfReaderWaiting)
				{
					HANDLE hTemp = m_hSafeToReadEvent;
					m_hSafeToReadEvent = NULL;
					LeaveCriticalSection(&m_cs);

					CloseHandle(hTemp);
				}
				else
				{
					LeaveCriticalSection(&m_cs);
				}
			}
			else
			{
				// Enter successful without wait
				++m_nNumOfReaderEntered;
				LeaveCriticalSection(&m_cs);
			}
		}
		inline void CReadWriteMutex::releaseReaderLock_i()
		{
			int  nNumOfReaderEntered = --m_nNumOfReaderEntered;
			FIRTEX_ASSERT2(0 <= nNumOfReaderEntered);

			if( (0 == nNumOfReaderEntered) && (NULL != m_hSafeToWriteEvent) )
			{
				LeaveCriticalSection(&m_cs);

				/// INTERESTING point: We signal one of queued WRITERs outside
				/// Critical Section without any problem in logic but better
				/// in performance
				SetEvent(m_hSafeToWriteEvent);
			}
			else
			{
				LeaveCriticalSection(&m_cs);
			}
		}
		inline void CReadWriteMutex::acquireWriterLock_i(bool bMustWait)
		{
			// Increase Writer-counter & reset Reader-event if necessary
			int nNumOfWriter = ++m_nNumOfWriter;
			if(	(1 == nNumOfWriter) && (NULL != m_hSafeToReadEvent) )
			{
				ResetEvent(m_hSafeToReadEvent);
			}

			if(bMustWait && (NULL == m_hSafeToWriteEvent) )
			{
				m_hSafeToWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			}
			LeaveCriticalSection(&m_cs);

			if(bMustWait)
			{
				WaitForSingleObject(m_hSafeToWriteEvent, INFINITE);
			}
		}
		inline void CReadWriteMutex::releaseWriterLock_i(bool bDowngrade)
		{
			FIRTEX_ASSERT2(0 == m_nNumOfReaderEntered);

			if(bDowngrade)
			{
				++m_nNumOfReaderEntered;
			}

			if(0 == --m_nNumOfWriter)
			{
				if(m_hSafeToReadEvent)
				{
					/// This is a long-time task inside Critical Section
					/// However, there isn't any effective way to overcome
					SetEvent(m_hSafeToReadEvent);
				}

				if(m_hSafeToWriteEvent)
				{
					HANDLE hTemp = m_hSafeToWriteEvent;
					m_hSafeToWriteEvent = NULL;
					LeaveCriticalSection(&m_cs);

					/// Close handle outside CS to increase throughput
					CloseHandle(hTemp);
				}
				else
				{
					LeaveCriticalSection(&m_cs);
				}
			}
			else
			{
				//////////////////////////////////////////////////////////////////////////
				// Some WRITERs are queued
				FIRTEX_ASSERT2( (0 < m_nNumOfWriter) && (NULL != m_hSafeToWriteEvent));
				LeaveCriticalSection(&m_cs);

				if(false == bDowngrade)
				{

					/// INTERESTING point: We signal one of queued WRITERs outside
					/// Critical Section without any problem in logic but better
					/// in performance
					SetEvent(m_hSafeToWriteEvent);			
				}
			}
		}
	}
}


#endif
