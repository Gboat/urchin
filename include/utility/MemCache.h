//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
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
			 * @param cachesize 缓冲大小 cachesize = byte cache size /sizeof(T)			 
			 */
			CMemCache_base(size_t cachesize);

			/**
			 * @param cache 外部给定的缓冲区首地址
			 * @param cachesize 缓冲大小,以T大小为单位，即cachesize = byte cache size /sizeof(T)
			 */ 			
			CMemCache_base(T* cache, size_t cachesize);
			
			CMemCache_base();

			~CMemCache_base();
		public:
			/** 
			 * @param chunksize 需要获取的内存大小，以T大小为单位2的指数，例如，chunksize=10则申请(2^10)*sizeof(T)个字节
			 * @return 内存指针
			 */			
			T*				getMem(size_t chunksize);

		
			/** 相当于realloc函数，释放以前的内存申请更大的内存
			 * @param newsize 新内存大小，以T大小为单位2的指数，必须大于oldsize
			 * @param location 旧内存指针
			 * @param oldsize 旧内存大小,以字节为单位2的指数
			 * @return 如果内存不足则返回NULL，否则返回大小为newsize(以T大小为单位2的指数对齐)的新内存地址，旧内存内容将拷贝到新内存中
			 */       			
			T*				getMoreMem(size_t newsize, T* location, size_t oldsize);

			/** 
			 * 释放内存，并不会真正调用delete删除内存
			 * @param location 待释放内存指针
			 * @param memsize 释放内存大小,以T大小为单位2的指数
			 */			
			void			freeMem(T* location, size_t memsize);

			/**
			 * 清空缓冲，以供重新使用
			 */
			void			flushMem();

			/** 
			 * 缓存开始地址
			 */
			const T*		getBegin();

			/** 
			 * 闲缓存开始地址
			 */
			const T*		getEnd();

			/** 
			 * 缓存大小
			 */
			const size_t	getSize(){return m_size;}

			/**
			 * 判断缓存是否空
			 */
			bool			isEmpty(){return (m_end==m_begin);}

			/**
			 * 增长内存缓冲
			 */
			CMemCache_base<T>*	grow(size_t growSize);
			/**
			 * 判断是否增长过内存缓冲
			 */
			bool			isGrowed(){return (m_pGrowCache!=NULL);}

		private:
			/** returns pointer to memory holes if available
			 * returns NULL if nothing is available			 
			 * @param csize m_size of memory we're requesting
			 */
			T* getFromFree(size_t csize);

			T*		m_begin;                /// 缓存开始地址
			size_t  m_size;                 /// 缓冲大小
			T*		m_end;					/// 空闲缓存开始地址
			int		m_typesize;             /// sizeof(T) 值
			
			vector<T*>m_freelist[NLISTS];	/// 空闲内存块数组
			bool	m_bOurmem;				/// 是外部内存还是内部申请的内存

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

			// 优先使用m_freelist里的空闲内存
			retval = getFromFree(chunksize);
			if (retval != NULL) 
			{
				return retval;
			}

			// 从空闲块中获取
			if ((m_end - m_begin) + size > m_size) 
			{
				//从新增长的内存中获取
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

			//拷贝旧内存块中的数据到新内存块
			memcpy(retval, location, (size_t) (POW_TABLE[oldsize]*m_typesize));


			// 保存释放的内存块以重新利用
			m_freelist[oldsize-MINPOW].push_back(location);

			return retval;
		}

		template<typename T>
			void CMemCache_base<T>::freeMem(T* location, size_t memsize) 
		{
			// 保存释放的内存块以重新利用
			if( (location >= m_begin) && (location < (m_begin + m_size)))
				m_freelist[memsize-MINPOW].push_back(location);
			else 
			{
				//是新增长缓存中的内存
				if(m_pGrowCache)
					m_pGrowCache->freeMem(location,memsize);
			}
		}


		template<typename T>
			void CMemCache_base<T>::flushMem()
		{
			//清空
			m_end = m_begin;

			//清空freelist
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

