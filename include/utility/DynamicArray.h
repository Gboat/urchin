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
// Created	: 2006/7/16
//
#ifndef _DYNAMICARRAY_H
#define _DYNAMICARRAY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"

namespace firtex
{
	namespace utility
	{
		#define DYNARRAY_DEFAULTBLKSIZE		128

		template<class T>
			class Const_NullValue
		{
		public:
			inline operator T()const{return (T)0;}
			
		};

		template<class ElemT,class NullValue = Const_NullValue<ElemT> >
		class CDynamicArray
		{	
		public:
			typedef ElemT element_type;
		public:
			CDynamicArray(void);
			CDynamicArray(int32_t blkSize);
			CDynamicArray(size_t initSize,int32_t blkSize);
			CDynamicArray(const CDynamicArray& src);
			~CDynamicArray(void);
		public:
			class CDynamicArrayIterator
			{
			public:
				CDynamicArrayIterator(CDynamicArray<ElemT,NullValue>* pParent)
					:m_pParent(pParent)
					,m_curBlk(-1)
					,m_curOffset(-1)					
				{
				}
				CDynamicArrayIterator(const CDynamicArrayIterator& clone)
					:m_pParent(clone.m_pParent)
					,m_curBlk(clone.m_curBlk)
					,m_curOffset(clone.m_curOffset)					
				{
				}
				~CDynamicArrayIterator(){}
			public:
				bool			next()
				{				
					if(m_curBlk == -1)
					{
						m_curBlk = m_pParent->m_blkBase;
						while(!m_pParent->m_blocks[m_curBlk] && (m_curBlk < m_pParent->m_numBlks) )
							m_curBlk++;
						if(m_curBlk >= m_pParent->m_numBlks)
							return false;
					}
					do
					{
						m_curOffset++;
						if(m_curOffset >= m_pParent->m_blkSize)
						{
							m_curBlk++;
							while(!m_pParent->m_blocks[m_curBlk] && (m_curBlk < m_pParent->m_numBlks) )
								m_curBlk++;
							if(m_curBlk>=m_pParent->m_numBlks)
								return false;
							m_curOffset = 0;
						}
					}while(m_pParent->m_blocks[m_curBlk][m_curOffset] == (ElemT)NullValue());
					
					return true;
				}
				element_type	element()
				{
					return m_pParent->m_blocks[m_curBlk][m_curOffset];
				}
				size_t			position()
				{
					return m_curBlk*m_pParent->m_blkSize + m_curOffset;
				}
			protected:
				CDynamicArray<ElemT,NullValue>*	m_pParent;
				int32_t					m_curBlk;
				int32_t					m_curOffset;					
			};

			typedef CDynamicArrayIterator array_iterator;
		public:
			/**
			 * ��ȡָ��λ�õ�Ԫ�أ������λ��Ԫ��Ϊ�����Զ������Ÿ�Ԫ�ص�Block
			 */
			ElemT&		operator[](size_t order);

			/**
			 * ��ȡָ��λ�õ�Ԫ�أ�����һ������ͬ���ǣ������λ��Ԫ��Ϊ�ղ����Զ������Ÿ�Ԫ�ص�Block
			 */
			ElemT		getAt(size_t order);

			/**
			 * ��ָ��λ�ò���һ��Ԫ��
			 * @param order λ��
			 * @param val Ԫ��ֵ
			 */
			void		insert(size_t order,const ElemT& val);

			/**
			 * ��ȡ����ĵ�����			 
			 */
			array_iterator	elements();

			/** ��ȡ���鳤�� */
            size_t		length();

			/**
			 * �������鳤��
			 * @param newLen �����³���
			 */
			size_t		setMaxlength(size_t newMaxLen);

			/** �������,�ͷ���Դ */
			void		clear();
			
			/** �������,�����ͷ���Դ */
			void		reset();
		protected:			
			int32_t		block(size_t order);
			int32_t		offset(size_t order);

			/**
			 * ���鶯̬����
			 * @param newLen �����´�С
			 */
			void		grow(size_t newLen);

			/** Ϊ��blk��Block����ռ䲢��ʼ��*/
			void		allocBlock(int32_t blk);
		private:
			ElemT**		m_blocks;	///��
			int32_t		m_blkSize;	///������ĳ���
			int32_t		m_blkBase;	///�����
			int32_t		m_numBlks;	///������
			size_t		m_maxLength;///����
		};
		//////////////////////////////////////////////////////////////////////////
		//
		template<class ElemT,class NullValue>
			CDynamicArray<ElemT,NullValue>::CDynamicArray(void)
			:m_blkSize(DYNARRAY_DEFAULTBLKSIZE)
			,m_blkBase(0)
			,m_numBlks(1)
			,m_maxLength(DYNARRAY_DEFAULTBLKSIZE)
		{
			m_blocks = new ElemT*[m_numBlks];
			m_blocks[0] = NULL;			
		}
		template<class ElemT,class NullValue>
			CDynamicArray<ElemT,NullValue>::CDynamicArray(int32_t blkSize)
			:m_blkSize(blkSize)
			,m_blkBase(0)
			,m_numBlks(1)
			,m_maxLength(blkSize)
		{
			m_blocks = new ElemT*[m_numBlks];
			m_blocks[0] = NULL;			
		}
		template<class ElemT,class NullValue>
			CDynamicArray<ElemT,NullValue>::CDynamicArray(size_t initSize,int32_t blkSize)
			:m_blkSize(blkSize)
			,m_blkBase(0)
			,m_maxLength(initSize)
		{
			m_numBlks = (int32_t)((initSize + blkSize - 1)/blkSize);
			m_blocks = new ElemT*[m_numBlks];
			memset(m_blocks,0,m_numBlks*sizeof(ElemT*));
		}

		template<class ElemT,class NullValue>
			CDynamicArray<ElemT,NullValue>::CDynamicArray(const CDynamicArray& src)
		{
			m_blkSize = src.m_blkSize;
			m_blkBase = src.m_blkBase;
			m_numBlks = src.m_numBlks;
			m_maxLength = src.m_maxLength;
			m_blocks = new ElemT*[m_numBlks];	//��
			memcpy(m_blocks,src.m_blocks,m_numBlks*sizeof(ElemT*));
			for(int32_t i = 0;i<m_numBlks;i++)
			{
				allocBlock(i);
				memcpy(m_blocks[i],src.m_blocks[i],m_blkSize*sizeof(ElemT));
			}
		}

		template<class ElemT,class NullValue>
			CDynamicArray<ElemT,NullValue>::~CDynamicArray(void)
		{
			clear();
		}

		/*template<class ElemT,class NullValue>
			ElemT const& CDynamicArray<ElemT,NullValue>::operator[](size_t order)const
		{
			FIRTEX_ASSERT((order < m_maxLength),_T("out of range"));

			int32_t blk = block(order);			
			FIRTEX_ASSERT((m_blocks[blk]!=NULL),_T("access empty element"));
			int32_t off = offset(order);
			return m_blocks[blk][off];
		}*/

		template<class ElemT,class NullValue>
			ElemT& CDynamicArray<ElemT,NullValue>::operator[](size_t order)
		{
			if(order >= m_maxLength)
				grow(order+1);		//����
			int32_t blk = block(order);
			int32_t off = offset(order);
			if(m_blocks[blk] == NULL)
				allocBlock(blk);
			return m_blocks[blk][off];//
		}		

		template<class ElemT,class NullValue>
			ElemT CDynamicArray<ElemT,NullValue>::getAt(size_t order)
		{
			if(order >= m_maxLength)
				return NULL;
			int32_t blk = block(order);
			int32_t off = offset(order);
			if(m_blocks[blk] == NULL)
				return NULL;
			return m_blocks[blk][off];
		}		

		template<class ElemT,class NullValue>
			typename CDynamicArray<ElemT,NullValue>::array_iterator CDynamicArray<ElemT,NullValue>::elements()
		{
			return typename CDynamicArray<ElemT,NullValue>::array_iterator(this);
		}
		template<class ElemT,class NullValue>
			void CDynamicArray<ElemT,NullValue>::insert(size_t order,const ElemT& val)
		{
			if(order >= m_maxLength)
				grow(order+1);		//����
			int32_t blk = block(order);
			int32_t off = offset(order);
			if(m_blocks[blk] == NULL)
				allocBlock(blk);
			m_blocks[blk][off] = val;
		}

		template<class ElemT,class NullValue>
			void CDynamicArray<ElemT,NullValue>::grow(size_t newLen)
		{
			int32_t numBlks = (int32_t)((newLen + m_blkSize - 1)/m_blkSize);
			ElemT** tmpBlks = new ElemT*[numBlks];
			memset(tmpBlks,0,numBlks*sizeof(ElemT*));
			memcpy(tmpBlks,m_blocks,m_numBlks*sizeof(ElemT*));
			delete[] m_blocks;
			m_blocks = tmpBlks;
			m_numBlks = numBlks;
			m_maxLength = newLen;
		}		
		template<class ElemT,class NullValue>
			void CDynamicArray<ElemT,NullValue>::allocBlock(int32_t blk)
		{
			FIRTEX_ASSERT((blk<m_numBlks),_T("CDynamicArray::allocBlock():ouf of range."));
			m_blocks[blk] = new ElemT[m_blkSize];
			for (int32_t i = 0;i<m_blkSize;i++)
			{
				m_blocks[blk][i] = NullValue();
			}
			//memset(m_blocks[blk],(int32_t)NullValue,m_blkSize*sizeof(ElemT));
		}

		template<class ElemT,class NullValue>
			void CDynamicArray<ElemT,NullValue>::clear()
		{
			if(m_blocks)
			{
				for(int32_t i = 0;i<m_numBlks;i++)
				{
					if(m_blocks[i])
						delete[] m_blocks[i];
				}
				delete[] m_blocks;
				m_blocks = NULL;
				m_numBlks = 0;
			}
			m_maxLength = 0;
		}
		template<class ElemT,class NullValue>
			void CDynamicArray<ElemT,NullValue>::reset()
		{
			if(m_blocks)
			{
				for(int32_t i = 0;i<m_numBlks;i++)
				{
					if(m_blocks[i])
					{
						memset(m_blocks[i],(int)0,m_blkSize*sizeof(ElemT));
					}
				}
			}			
		}
		//////////////////////////////////////////////////////////////////////////
		//inline functions
		template<class ElemT,class NullValue>
			inline size_t CDynamicArray<ElemT,NullValue>::length()
		{
			int32_t length = 0;
			for(int32_t i = 0;i<m_numBlks;i++)
			{
				if(m_blocks[i - m_blkBase])
				{
					for(int32_t j = 0;j<m_blkSize;j++)
						if(m_blocks[i - m_blkBase][j] != (ElemT)NullValue())
							length++;
				}
			}
			return length;
		}
		template<class ElemT,class NullValue>
			inline size_t CDynamicArray<ElemT,NullValue>::setMaxlength(size_t newMaxLen)
		{
			if(newMaxLen > m_maxLength)
			{
				grow(newMaxLen);
				return newMaxLen;
			}
			else return m_maxLength;
		}
		template<class ElemT,class NullValue>
			inline int32_t CDynamicArray<ElemT,NullValue>::block(size_t order)
		{
			return ((int32_t)(order/m_blkSize) - m_blkBase);
		}
		template<class ElemT,class NullValue>
			inline int32_t CDynamicArray<ElemT,NullValue>::offset(size_t order)
		{
			return (int32_t)(order%m_blkSize);
		}
	}
}


#endif
