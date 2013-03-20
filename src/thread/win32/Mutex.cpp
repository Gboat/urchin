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
// Created	: 2007/6/2
//

#include "thread/win32/Mutex.h"

#define READER_RECURRENCE_UNIT 0x00000001
#define READER_RECURRENCE_MASK 0x0000FFFF
#define WRITER_RECURRENCE_UNIT 0x00010000

namespace firtex
{
	namespace thread
	{
		CReadWriteMutex::CReadWriteMutex()
			: m_hSafeToWriteEvent(INVALID_HANDLE_VALUE)
			, m_hSafeToReadEvent(INVALID_HANDLE_VALUE)
			, m_nNumOfWriter(0)
			, m_nNumOfReaderEntered(0)
			, m_nNumOfReaderWaiting(0)
			, m_rLock(*this)
			, m_wLock(*this)
		{			
#if (_WIN32_WINNT >= 0x0403)
			InitializeCriticalSectionAndSpinCount(&m_cs, READER_WRITER_SPIN_COUNT);
#else
			InitializeCriticalSection(&m_cs);
#endif
		}
		CReadWriteMutex::~CReadWriteMutex()
		{
			if(m_hSafeToReadEvent != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hSafeToReadEvent);
				m_hSafeToReadEvent = INVALID_HANDLE_VALUE;
			}
			if(m_hSafeToWriteEvent)
			{
				CloseHandle(m_hSafeToWriteEvent);
				m_hSafeToWriteEvent = INVALID_HANDLE_VALUE;
			}			
			DeleteCriticalSection(&m_cs);
		}

		void CReadWriteMutex::acquireWriterLock()
		{
			const DWORD dwCurrentThreadId = GetCurrentThreadId();
			EnterCriticalSection(&m_cs);

			const DWORD dwThreadState = (m_map[dwCurrentThreadId] += WRITER_RECURRENCE_UNIT);
			if(WRITER_RECURRENCE_UNIT == dwThreadState)
			{
				acquireWriterLock_i( (m_nNumOfWriter + m_nNumOfReaderEntered) != 0);
			}
			else if ( (WRITER_RECURRENCE_UNIT*2) <= dwThreadState)
			{
				// This thread was already a writer
				LeaveCriticalSection(&m_cs);
			}
			else
			{
				// Upgrading from reader to writer
				acquireWriterLock_i((--m_nNumOfReaderEntered) != 0);
			}
		}
		void CReadWriteMutex::releaseWriterLock()
		{
			const DWORD dwCurrentThreadId = GetCurrentThreadId();
			EnterCriticalSection(&m_cs);

			CMapThreadToState::iterator ite = m_map.find(dwCurrentThreadId);
			FIRTEX_ASSERT2( (ite != m_map.end()) && (WRITER_RECURRENCE_UNIT <= ite->second));

			const DWORD dwThreadState = (ite->second -= WRITER_RECURRENCE_UNIT);
			if(0 == dwThreadState)
			{
				m_map.erase(ite);
				releaseWriterLock_i(false);
			}
			else if (WRITER_RECURRENCE_UNIT <= dwThreadState)
			{
				// Still be is a writer
				LeaveCriticalSection(&m_cs);
			}
			else
			{
				// Down-grading from writer to reader
				releaseWriterLock_i(true);
			}
		}
		void CReadWriteMutex::acquireReaderLock()
		{		
			const DWORD dwCurrentThreadId = GetCurrentThreadId();
			EnterCriticalSection(&m_cs);

			CMapThreadToState::iterator ite = m_map.find(dwCurrentThreadId);
			if(ite == m_map.end())
				m_map.insert(make_pair(dwCurrentThreadId,0));
			const DWORD dwThreadState = (m_map[dwCurrentThreadId] += READER_RECURRENCE_UNIT);
			if(READER_RECURRENCE_UNIT == dwThreadState)
			{
				acquireReaderLock_i();
			}
			else
			{
				LeaveCriticalSection(&m_cs);
			}
		}
		void CReadWriteMutex::releaseReaderLock()
		{
			const DWORD dwCurrentThreadId = GetCurrentThreadId();
			EnterCriticalSection(&m_cs);

			CMapThreadToState::iterator ite = m_map.find(dwCurrentThreadId);
			FIRTEX_ASSERT2( (ite != m_map.end()) && (READER_RECURRENCE_MASK & ite->second));

			const DWORD dwThreadState = (ite->second -= READER_RECURRENCE_UNIT);
			if(0 == dwThreadState)
			{
				m_map.erase(ite);
				releaseReaderLock_i();
			}
			else
			{
				LeaveCriticalSection(&m_cs);
			}
		}	

		CLockable& CReadWriteMutex::getReadLock()
		{
			return m_rLock;
		}
		
		CLockable& CReadWriteMutex::getWriteLock()
		{
			return m_wLock;
		}

		//////////////////////////////////////////////////////////////////////////
		//CReadWriteMutex::RLock
		void CReadWriteMutex::RLock::acquire()
		{
			m_rwLock.acquireReaderLock();
		}

		bool CReadWriteMutex::RLock::tryAcquire(unsigned long timeout)
		{
			return false;
		}

		void CReadWriteMutex::RLock::release()
		{
			m_rwLock.releaseReaderLock();
		}

		//////////////////////////////////////////////////////////////////////////
		//CReadWriteMutex::WLock
		void CReadWriteMutex::WLock::acquire()
		{
			m_rwLock.acquireWriterLock();
		}

		bool CReadWriteMutex::WLock::tryAcquire(unsigned long timeout)
		{
			return false;
		}

		void CReadWriteMutex::WLock::release()
		{
			m_rwLock.releaseWriterLock();
		}
	}
}
