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
// Created	: 2006/5/20
//
#ifndef _READER_H
#define _READER_H

#include "StdHeader.h"
#include "charsetdef.h"
#include <string>
using namespace std;


namespace firtex
{
	namespace utility
	{		
		class CReader
		{	
		public:
			CReader(char* buffer,size_t buffsize);
			CReader(size_t buffsize = 0);			
			virtual ~CReader(void);
		public:
			/**
			 * 读取数据块
			 * @param data 目标数据块首地址
			 * @param length 目标数据块大小
			 */
			void		read(char* data, size_t length);
			
			/**
			 * 直接读取数据块,没有数据拷贝			 
			 * @param length 数据块大小
			 * @return 数据块首地址
			 */
			char*		readWithoutCopy(size_t& length);		

			/**
			 * 返回在文件中的当前位置，下一次读从这个位置开始
			 */			
			int64_t		getFilePointer(){return m_bufferStart + m_bufferPosition;}

			/**
			 * 设置文件的当前读位置，下一次读从这个位置开始
			 * @param pos 定位的位置
			 */			
			void		seek(int64_t pos);	

			/**
			 * 返回文件长度			 
			 */					
			int64_t		length(){return m_length;}

			/**
			 * set encoding of the reader
			 * @param _charset charset of the content 
			 */
			void		setEncoding(charset_t _charset){m_encoding = _charset;}

			/**
			 * get encoding of the reader 
			 * @return encoding 
			 */
			charset_t	getEncoding(){return m_encoding;}

			/**
			 * 判断文件数据是否读完毕
			 */		
			virtual bool	isEof(){return ( (m_bufferStart + (int64_t )m_bufferPosition) >= m_length);}

			/**
			 * 关闭文件，子类需实现
			 */
			virtual void  close();
		public:
			/**
			 * 克隆一个CReader对象,克隆对象使用buffer作为缓冲	 
			 */					
			virtual CReader*	clone(char* buffer,size_t buffsize) = 0;
			/**
			 * 克隆一个CReader对象,克隆对象使用自己私有的内存作为缓冲	 
			 */					
			virtual CReader*	clone() = 0;
		protected:
			/**
			 * 实现文件定位操作，readInternal的读操作从这个位置开始，需子类实现
			 * @param pos 定位位置
			 */					
			virtual void  seekInternal(int64_t position) = 0;

			/**
			 * 从文件中当前位置读数据，子类需实现此函数
			 * @param b 目标地址
			 * @param offset 偏移量
			 * @length 长度
			 */			
			virtual void  readInternal(char* b, size_t offset, size_t length) = 0;	
			
		protected:
			/**
			 * 从文件中中读数据填充到缓冲区中
			 */
			void	refill();	
		public:
			const static int READER_BUFFSIZE = 32768*10;
		protected:
			char*		m_buffer;
			size_t		m_bufferSize;			

			int64_t		m_bufferStart;		/// position in file of m_buffer
			size_t		m_bufferLength;		/// end of valid bytes
			size_t		m_bufferPosition;	/// next byte to read

			int64_t		m_length;			/// set by subclasses
			bool		m_bOwnBuff;

			charset_t	m_encoding;			///encoding name
		};

		//////////////////////////////////////////////////////////////////////////
		//
		inline void  CReader::refill()
		{
			int64_t start = m_bufferStart + (int64_t)m_bufferPosition;
			int64_t end = start + (int64_t)m_bufferSize;
			if (end > m_length)	//超过了结尾					
				end = m_length;
			m_bufferLength = (size_t) (end - start);
			if (m_bufferLength == 0)
				return;

			if (m_buffer == NULL)
				m_buffer = new char[m_bufferSize]; // allocate m_buffer lazily
			readInternal(m_buffer, 0, m_bufferLength);

			m_bufferStart = start;
			m_bufferPosition = 0;
		}
	}
}


#endif
