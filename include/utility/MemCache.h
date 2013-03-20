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
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/10/19
//

#ifndef _MEMCACHE_H
#define _MEMCACHE_H

#include "Config.h"
#include "Misc.h"
#include <cmath>
#include <vector>
using namespace std;

#define MINPOW 5
#define MAXPOW 31
#define NLISTS MAXPOW-MINPOW+1

namespace firtex
{
	namespace utility
	{
		template<typename T>
		class CMemCache_base
		{
		public:
			/**
			 * @param cachesize �����С cachesize = byte cache size /sizeof(T)			 
			 */
			CMemCache_base(size_t cachesize);

			/**
			 * @param cache �ⲿ�����Ļ������׵�ַ
			 * @param cachesize �����С,��T��СΪ��λ����cachesize = byte cache size /sizeof(T)
			 */ 			
			CMemCache_base(T* cache, size_t cachesize);
			
			CMemCache_base();

			~CMemCache_base();
		public:
			/** 
			 * @param chunksize ��Ҫ��ȡ���ڴ��С����T��СΪ��λ2��ָ�������磬chunksize=10������(2^10)*sizeof(T)���ֽ�
			 * @return �ڴ�ָ��
			 */			
			T*				getMem(size_t chunksize);

		
			/** �൱��realloc�������ͷ���ǰ���ڴ����������ڴ�
			 * @param newsize ���ڴ��С����T��СΪ��λ2��ָ�����������oldsize
			 * @param location ���ڴ�ָ��
			 * @param oldsize ���ڴ��С,���ֽ�Ϊ��λ2��ָ��
			 * @return ����ڴ治���򷵻�NULL�����򷵻ش�СΪnewsize(��T��СΪ��λ2��ָ������)�����ڴ��ַ�����ڴ����ݽ����������ڴ���
			 */       			
			T*				getMoreMem(size_t newsize, T* location, size_t oldsize);

			/** 
			 * �ͷ��ڴ棬��������������deleteɾ���ڴ�
			 * @param location ���ͷ��ڴ�ָ��
			 * @param memsize �ͷ��ڴ��С,��T��СΪ��λ2��ָ��
			 */			
			void			freeMem(T* location, size_t memsize);

			/**
			 * ��ջ��壬�Թ�����ʹ��
			 */
			void			flushMem();

			/** 
			 * ���濪ʼ��ַ
			 */
			const T*		getBegin();

			/** 
			 * �л��濪ʼ��ַ
			 */
			const T*		getEnd();

			/** 
			 * �����С
			 */
			const size_t	getSize(){return m_size;}

			/**
			 * �жϻ����Ƿ��
			 */
			bool			isEmpty(){return (m_end==m_begin);}

			/**
			 * �����ڴ滺��
			 */
			CMemCache_base<T>*	grow(size_t growSize);
			/**
			 * �ж��Ƿ��������ڴ滺��
			 */
			bool			isGrowed(){return (m_pGrowCache!=NULL);}

		private:
			/** returns pointer to memory holes if available
			 * returns NULL if nothing is available			 
			 * @param csize m_size of memory we're requesting
			 */
			T* getFromFree(size_t csize);

			T*		m_begin;                /// ���濪ʼ��ַ
			size_t  m_size;                 /// �����С
			T*		m_end;					/// ���л��濪ʼ��ַ
			int		m_typesize;             /// sizeof(T) ֵ
			
			vector<T*>m_freelist[NLISTS];	/// �����ڴ������
			bool	m_bOurmem;				/// ���ⲿ�ڴ滹���ڲ�������ڴ�

			CMemCache_base<T>*	m_pGrowCache;
		};		


		//////////////////////////////////////////////////////////////////////////
		//implementation
		template<typename T>
			CMemCache_base<T>::CMemCache_base(size_t cachesize) 
		{
			m_typesize = sizeof(T);
			m_size = cachesize;
			m_begin = (T*) malloc(m_size*m_typesize);
			if(m_begin == NULL)
			{
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CMemCache_base alloc memory failed."));
			}
			m_end = m_begin;			
			m_bOurmem = true;
			m_pGrowCache = NULL;
		}

		template<typename T>
			CMemCache_base<T>::CMemCache_base(T* cache, size_t cachesize) 
		{
			m_size = cachesize;
			m_begin = cache;
			m_end = m_begin;
			m_typesize = sizeof(T);
			m_bOurmem = false;
			m_pGrowCache = NULL;
		}

		template<typename T>
			CMemCache_base<T>::CMemCache_base() 
		{
			m_size = 0;
		}

		template<typename T>
			CMemCache_base<T>::~CMemCache_base() 
		{
			if (m_bOurmem)
				free(m_begin);
			for (int i=0; i<NLISTS; i++) 
			{
				m_freelist[i].clear();
			}

			if(m_pGrowCache)
			{
				delete m_pGrowCache;
				m_pGrowCache = NULL;
			}
		}

		template<typename T>
			T* CMemCache_base<T>::getMem(size_t chunksize) 
		{
			if ((chunksize < MINPOW) || (chunksize > MAXPOW))
				return NULL;

			size_t size = POW_TABLE[chunksize];

			T *retval = NULL;

			// ����ʹ��m_freelist��Ŀ����ڴ�
			retval = getFromFree(chunksize);
			if (retval != NULL) 
			{
				return retval;
			}

			// �ӿ��п��л�ȡ
			if ((m_end - m_begin) + size > m_size) 
			{
				//�����������ڴ��л�ȡ
				if(m_pGrowCache)
					return m_pGrowCache->getMem(chunksize);
				return NULL;
			}
			// give a chunk from the m_end, update m_end
			retval = m_end;
			m_end += size;

			return retval;
		}


		template<typename T>
			T* CMemCache_base<T>::getMoreMem(size_t newsize, T* location, size_t oldsize) 
		{
			if (oldsize >= newsize)
				return NULL;

			if ((oldsize < MINPOW) || (oldsize > MAXPOW))
				return NULL;

			if ((newsize < MINPOW) || (newsize > MAXPOW))
				return NULL;

			T* retval = getMem(newsize);
			if (retval == NULL)
				return NULL;

			//�������ڴ���е����ݵ����ڴ��
			memcpy(retval, location, (size_t) (POW_TABLE[oldsize]*m_typesize));


			// �����ͷŵ��ڴ������������
			m_freelist[oldsize-MINPOW].push_back(location);

			return retval;
		}

		template<typename T>
			void CMemCache_base<T>::freeMem(T* location, size_t memsize) 
		{
			// �����ͷŵ��ڴ������������
			if( (location >= m_begin) && (location < (m_begin + m_size)))
				m_freelist[memsize-MINPOW].push_back(location);
			else 
			{
				//�������������е��ڴ�
				if(m_pGrowCache)
					m_pGrowCache->freeMem(location,memsize);
			}
		}


		template<typename T>
			void CMemCache_base<T>::flushMem()
		{
			//���
			m_end = m_begin;

			//���freelist
			for (int i=0; i<NLISTS; i++) 
			{
				m_freelist[i].clear();
			}
			if(m_pGrowCache)
			{
				delete m_pGrowCache;
				m_pGrowCache = NULL;
			}
		}

		template<typename T>
			const T* CMemCache_base<T>::getBegin() 
		{
			return m_begin;
		}

		template<typename T>
			const T* CMemCache_base<T>::getEnd() 
		{
			return m_end;
		}

		template<typename T>		
			inline T* CMemCache_base<T>::getFromFree(size_t csize) 
		{
			if (m_freelist[csize-MINPOW].empty())
			{
				if(m_pGrowCache)
				{
					return m_pGrowCache->getFromFree(csize);
				}
				return NULL;
			}

			T* retval = m_freelist[csize-MINPOW].back();
			m_freelist[csize-MINPOW].pop_back();

			return retval;		
		}		

		template<typename T>		
		CMemCache_base<T>* CMemCache_base<T>::grow(size_t growSize)
		{
			if(m_pGrowCache)
				return m_pGrowCache->grow(growSize);

			m_pGrowCache = new CMemCache_base(growSize);
			return m_pGrowCache;
		}

#include "typedefs.h"
		typedef CMemCache_base<uint8_t> CMemCache;
	}
}



#endif

