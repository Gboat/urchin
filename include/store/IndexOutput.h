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
#ifndef __INDEXOUTPUT_H
#define __INDEXOUTPUT_H

#include "../utility/StdHeader.h"
#include "IndexInput.h"

#define INDEXOUTPUT_BUFFSIZE		524288//32768//4096


namespace firtex
{
	namespace store
	{
		class CIndexOutput
		{
		public:
			CIndexOutput(char* buffer,size_t buffsize);
			CIndexOutput(size_t buffsize=0);
			virtual ~CIndexOutput(void);
		public:			
			/**
			 * 从文件中读取数据并写到新文件
			 * @param pos 定位位置
			 */					
			void	write(CIndexInput* pInput,int64_t length);

			/**
			 * 写数据到文件
			 * @param data 数据块地址
			 * @param length 数据长度
			 */		
			void	write(const char* data,size_t length);

			/**
			 * 写一个字节			 
			 */		
			void	writeByte(byte b);

			/**
			 * 写多个个字节			 
			 */		
			void	writeBytes(byte* b, size_t length);
						
			/**
			 * 写4个字节			 
			 */		
			void	writeInt(int32_t i);

			/**
			 * 写多个4个字节
			 * @param pInts 写入字节存储位置
			 * @param len 写入字节数，pInts长度必须至少为len，否则出现内存溢出
			 */		
			void	writeInts(int32_t* pInts,size_t len);
			
			/**
			 * 写4个变长的字节			 
			 */		
			void	writeVInt(int32_t i);

			/**
			 * 写8个字节			 
			 */		
			void	writeLong(int64_t i);
			
			/**
			 * 写8个变长的字节			 
			 */		
			void	writeVLong(int64_t i);

			/**
			 * 写字符串，存储方式为:len(VInt)|string
			 */		
			void	writeString(const CFXString& s);

			/**
			 * 以UTF-8编码写字符序列
			 * @param s 字符串
			 * @param start 开始位置
			 * @param length 长度
			 */					
			void	writeChars(const char* s, size_t start, size_t length);

			/**
			 * 获取4字节整数变长存储的长度
			 */
			byte	getVIntLength(int32_t i);

			/**
			 * 设置内部buffer
			 */
			void	setBuffer(char* buf,size_t bufSize);			

			/**
			 * 返回文件的当前位置
			 */
			int64_t getFilePointer();
		
			/**
			 * flush buffered data to output
			 */
			void	flush();			
		public:
			/**
			 * 实现写buffer操作，在当前位置写数据，纯虚函数由子类实现
			 * @param b 数据buffer
			 * @param len b的长度
			 */
			virtual void  flushBuffer(char* b,size_t len) = 0;

			/**
			 * 返回文件长度
			 */
			virtual int64_t length() = 0;

			/**
			 * 关闭文件,释放资源,子类可以重载此函数但必须显式调用<code>CIndexOutput::close()</code>
			 */
			virtual void  close();

			/**
			 * 文件定位，子类需重载,必须显式调用<code>CIndexOutput::seek()</code>
			 * @param pos 定位位置
			 */
			virtual void  seek(int64_t pos);			
		protected:
            char*			m_buffer;
			size_t			m_buffersize;
			int64_t			m_bufferStart;		// m_buffer 在文件中的偏移量
			size_t			m_bufferPosition;	// m_buffer 中的位置
			bool			m_bOwnBuff;
		};
		//////////////////////////////////////////////////////////////////////////
		//
		inline void CIndexOutput::write(const char* data,size_t length)
		{				
			if ((m_bufferPosition>0) && ( (int64_t)(m_bufferPosition + length) >= m_buffersize) )
				flush();
			if(m_buffersize < (int64_t)length)
			{
				flushBuffer((char*)data,length);//直接写					
				m_bufferStart+=length;
			}
			else 
			{
				memcpy(m_buffer + m_bufferPosition,data,length);
				m_bufferPosition += length;
			}
		}
		inline void CIndexOutput::writeByte(byte b)
		{
			if (m_bufferPosition >= m_buffersize)
				flush();
			m_buffer[m_bufferPosition++] = b;
		}
			
		inline void CIndexOutput::writeBytes(byte* b, size_t length)
		{
			for (size_t i= 0; i < length; i++)
				writeByte(b[i]);
		}
		
		inline void  CIndexOutput::writeInt(int32_t i)
		{
			writeByte((byte) (i >> 24));
			writeByte((byte) (i >> 16));
			writeByte((byte) (i >> 8));
			writeByte((byte) i);
		}
		inline void CIndexOutput::writeInts(int32_t* pInts,size_t len)
		{
			for (size_t i= 0; i < len; i++)
				writeInt(pInts[i]);
		}

		inline void  CIndexOutput::writeVInt(int32_t i)
		{
			uint32_t ui = i;
			while ((ui & ~0x7F) != 0) 
			{
				writeByte((uint8_t)((ui & 0x7f) | 0x80));
				ui >>= 7; 
			}
			writeByte( (uint8_t)ui );		
		}

		inline void  CIndexOutput::writeLong(int64_t i)
		{
			writeInt((int32_t) (i >> 32));
			writeInt((int32_t) i);
		}
			
		inline void  CIndexOutput::writeVLong(int64_t i)
		{
			uint64_t ui = i;
			while ((ui & ~0x7F) != 0) 
			{
				writeByte((uint8_t)((ui & 0x7f) | 0x80));
				ui >>= 7; 
			}
			writeByte((uint8_t)ui);
		}			
		inline void  CIndexOutput::writeString(const CFXString& s)
		{
			int32_t length = (int32_t)s.length();
			writeVInt(length);
			writeChars(s.c_str(), 0, length);
		}		
		inline void  CIndexOutput::writeChars(const char* s, size_t start, size_t length)
		{
			uint64_t end = start + length;
			for (size_t i = start; i < end; i++)
			{
				int32_t code = (int32_t) s[i];
				if (code >= 0x01 && code <= 0x7F)
					writeByte((byte) code);
				else if (((code >= 0x80) && (code <= 0x7FF)) || code == 0)
				{
					writeByte((byte) (0xC0 | (code >> 6)));
					writeByte((byte) (0x80 | (code & 0x3F)));
				}
				else
				{
					writeByte((byte) (0xE0 | (((uint32_t) code) >> 12)));
					writeByte((byte) (0x80 | ((code >> 6) & 0x3F)));
					writeByte((byte) (0x80 | (code & 0x3F)));
				}
			}
		}

		inline byte CIndexOutput::getVIntLength(int32_t i)
		{
			byte l = 1;
			uint32_t ui = i;
			while ((ui & ~0x7F) != 0) 
			{
				l++;
				ui >>= 7; //doing unsigned shift
			}
			return l;
		}

		inline void CIndexOutput::setBuffer(char* buf,size_t bufSize)
		{
			if(m_bufferStart!=0 || m_bufferPosition != 0)
			{
				FIRTEX_THROW2(UNSUPPORTED_ERROR," void CIndexOutput::setBuffer(char* buf,size_t bufSize):you must call setBuffer() before reading any data.");
			}
			if(m_bOwnBuff && m_buffer)
			{
				delete[] m_buffer;
			}
			m_buffer = buf;
			m_buffersize = bufSize;
			m_bOwnBuff = false;
		}
		inline void CIndexOutput:: flush()
		{
			flushBuffer(m_buffer, m_bufferPosition);
			m_bufferStart += m_bufferPosition;
			m_bufferPosition = 0;
		}
		inline int64_t CIndexOutput::getFilePointer()
		{
			return m_bufferStart + (int64_t)m_bufferPosition;
		}
	}
}

#endif
