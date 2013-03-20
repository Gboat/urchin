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
// Created	: 2005/12/9
//
#ifndef __INPUTSTREAM_H
#define __INPUTSTREAM_H

#include "../utility/StdHeader.h"
#include "../utility/FXString.h"


#define INDEXINPUT_BUFFSIZE		32768//4096

namespace firtex
{
	namespace store
	{
		class CIndexInput
		{	
		public:
			CIndexInput(char* buffer,size_t buffsize);
			CIndexInput(size_t buffsize=0);			
			virtual ~CIndexInput(void);
		public:
			/**
			 * 直接读取数据块
			 * @param data 目标数据块首地址
			 * @param length 目标数据块大小
			 */
			void read(char* data, size_t length);

			/**
			 * 读一个字节			 
			 */
			byte readByte();

			/**
			 * 读多个字节			 
			 * @param b 读取字节存储位置
			 * @param len 读取字节数
			 */
			void  readBytes(byte* b,size_t len);
			
			/**
			 * 读4个字节
			 */
			int32_t readInt();

			/**
			 * 读多个4个字节
			 * @param i 读取字节存储位置
			 * @param len 读取字节数，i长度必须至少为len，否则出现内存溢出
			 */
			void	readInts(int32_t* i,size_t len);

			/**
			 * 读变长存储的4个字节			 
			 */
			int32_t readVInt();
			
			/**
			 * 读8个字节			 
			 */
			int64_t readLong();
			
			/**
			 * 读变长存储的8个字节			 
			 */
			int64_t readVLong();

			/**
			 * 读字符串			 
			 */
			void readString(string& s);

			/**
			 * 读字符串			 
			 */
			void readString(CFXString& s);

			/**
			 * 读UTF-8编码存储的字符
			 * @param buffer 目标地址
			 * @param start buffer的偏移量
			 * @param length 长度
			 * @see CCacheWriter.writeChars
			 */			
			void  readChars(char* buffer, size_t start, size_t length);

			/**
			 * 跳过nNum个VInt
			 * @param nNum 跳过个数			 			 
			 */			
			void skipVInt(size_t nNum);

			/**
			 * 返回在文件中的当前位置，下一次读从这个位置开始
			 */			
			int64_t getFilePointer();

			/**
			 * 设置文件的当前读位置，下一次读从这个位置开始
			 * @param pos 定位的位置
			 */			
			void  seek(int64_t pos);

			/**
			 * 判断文件数据是否读完毕
			 */		
			bool	isEof();

			/**
			 * 返回文件长度			 
			 */					
			int64_t length()const;

			/**
			 * 设置文件长度，在读取时，最多读取用户设置的文件长度
			 */
			void	setLength(int64_t newLen);

			/**
			 * 设置CIndexInput的读取buffer,只能在CIndexInput 读取任何数据之前使用
			 * @param buf buffer地址，不作为内部对象，仍需外部删除
			 * @param bufSize buf的长度
			 */
			void	setBuffer(char* buf,size_t bufSize);

		public:
			/**
			 * read data from file, sub class must implement this method
			 * @param b target address
			 * @length data length to read
			 * @bCheck check read position or not
			 */			
			virtual void	readInternal(char* b,size_t length,bool bCheck = true) = 0;

			/**
			 * 实现文件定位操作，readInternal的读操作从这个位置开始，需子类实现
			 * @param pos 定位位置
			 */					
			virtual void	seekInternal(int64_t pos) = 0;
			
			/**
			 * 克隆一个CIndexInput对象,克隆对象使用buffer作为缓冲	 
			 * @param buffer 克隆对象使用的buffer
			 * @param buffsize buffer的大小
			 */					
			virtual CIndexInput*	clone(char* buffer,size_t buffsize) = 0;
			
			/**
			 * 克隆一个CIndexInput对象,克隆对象使用自己私有的内存作为缓冲	 
			 */					
			virtual CIndexInput*	clone() = 0;

			/**
			 * 关闭文件，子类需实现
			 */
			virtual void	close() = 0;
		protected:				
			/**
			 * 从文件中中读数据填充到缓冲区中
			 */
			void			refill();
		protected:
			char*		m_buffer;
			size_t		m_bufferSize;			

			int64_t		m_bufferStart;		/// position in file of m_buffer
			size_t		m_bufferLength;		/// end of valid bytes
			size_t		m_bufferPosition;	/// next byte to read

			int64_t		m_length;			/// set by subclasses
			bool		m_bOwnBuff;

			friend class CIndexOutput;
		};

		//////////////////////////////////////////////////////////////////////////
		//
		inline byte CIndexInput::readByte()
		{
			if (m_bufferPosition >= m_bufferLength)
				refill();
			return m_buffer[m_bufferPosition++];
		}
		inline int32_t CIndexInput:: readInt()
		{
			uint8_t b1 = readByte();
			uint8_t b2 = readByte();
			uint8_t b3 = readByte();
			uint8_t b4 = readByte();
			return ((b1 & 0xFF) << 24) | ((b2 & 0xFF) << 16) | ((b3 & 0xFF) <<  8)
				| (b4 & 0xFF);			
		}		

		inline int32_t CIndexInput::readVInt()
		{
			uint8_t b = readByte();
			int32_t i = b & 0x7F;
			for (int32_t shift = 7; (b & 0x80) != 0; shift += 7) {
				b = readByte();
				i |= (b & 0x7FL) << shift;
			}
			return i;
		}
	
		inline int64_t CIndexInput::readLong()
		{
			int32_t i1 = readInt();
			int32_t i2 = readInt();
			return (((int64_t)i1) << 32) | (i2 & 0xFFFFFFFFL);		
		}

		inline int64_t CIndexInput::readVLong()
		{
			uint8_t b = readByte();
			int64_t i = b & 0x7F;
			for (int32_t shift = 7; (b & 0x80) != 0; shift += 7)
			{
				b = readByte();
				i |= (b & 0x7FLL) << shift;
			}
			return i;
		}

		inline void CIndexInput::readString(string& s)
		{				
			size_t length = (size_t)readVInt();				
			char* chars = new char[length + 1];
			readChars(chars, 0, length);				
			chars[length] = '\0';		
			s = chars;
			delete[] chars;
		}

		inline void CIndexInput::readString(CFXString& s)
		{				
			size_t length = (size_t)readVInt();
			if(length <=0 )
				FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,"CIndexInput::readString():the length of string is invalid.");
			s.reserve(length);				
			readChars(s.data(),0,length);
			s.resize(length);
		}
		
		inline void CIndexInput::readChars(char* buffer, size_t start, size_t length)
		{
			size_t end = start + length;
			for (size_t i = start; i < end; i++)
			{
				byte b = readByte();
				if ((b & 0x80) == 0)
					buffer[i] = (char) (b & 0x7F);
				else if ((b & 0xE0) != 0xE0)
				{
					buffer[i] = (char) (((b & 0x1F) << 6) | (readByte() & 0x3F));
				}
				else
					buffer[i] = (char) (((b & 0x0F) << 12) | ((readByte() & 0x3F) << 6) | (readByte() & 0x3F));
			}
		}
		inline void CIndexInput::refill()
		{
			int64_t start = m_bufferStart + (int64_t)m_bufferPosition;
			int64_t end = start + m_bufferSize;
			if (end > m_length)	//超过了结尾					
				end = m_length;
			m_bufferLength = (size_t)(end - start);
			if (m_bufferLength <= 0)
				FIRTEX_THROW2(FILEIO_ERROR,"IndexInput:read past EOF.");

			if (m_buffer == NULL)
				m_buffer = new char[m_bufferSize]; // allocate m_buffer lazily
			readInternal(m_buffer,m_bufferLength);

			m_bufferStart = start;
			m_bufferPosition = 0;
		}
		inline void CIndexInput::skipVInt(size_t nNum)
		{
			for (int64_t i = 0;i<nNum;i++)
			{
				readVInt();
			}
		}

		inline int64_t CIndexInput::getFilePointer()
		{
			return m_bufferStart + (int64_t)m_bufferPosition;
		}

		inline void CIndexInput::seek(int64_t pos)
		{
			if(pos > m_length)
				FIRTEX_THROW3(FILEIO_ERROR,_T("CIndexInput.seek():pos>m_length"));
			if (pos >= m_bufferStart && pos < (m_bufferStart + (int64_t)m_bufferLength))
				m_bufferPosition = (size_t) (pos - m_bufferStart);//新位置在缓冲区间				
			else
			{
				m_bufferStart = pos;
				m_bufferPosition = 0;
				m_bufferLength = 0; // trigger refill() on read()
				seekInternal(pos);
			}
		}
		
		inline bool CIndexInput::isEof()
		{
			return ( (m_bufferStart + (int64_t )m_bufferPosition) >= m_length);
		}
			
		inline int64_t CIndexInput::length()const
		{
			return m_length;
		}
		
		inline void CIndexInput::setLength(int64_t newLen)
		{
			FIRTEX_ASSERT((newLen > 0),_T("CIndexInput.setLength():illegal parameter."));
			m_length = newLen;
		}
	}
}

#endif
