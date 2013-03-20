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
			 * ���ļ��ж�ȡ���ݲ�д�����ļ�
			 * @param pos ��λλ��
			 */					
			void	write(CIndexInput* pInput,int64_t length);

			/**
			 * д���ݵ��ļ�
			 * @param data ���ݿ��ַ
			 * @param length ���ݳ���
			 */		
			void	write(const char* data,size_t length);

			/**
			 * дһ���ֽ�			 
			 */		
			void	writeByte(byte b);

			/**
			 * д������ֽ�			 
			 */		
			void	writeBytes(byte* b, size_t length);
						
			/**
			 * д4���ֽ�			 
			 */		
			void	writeInt(int32_t i);

			/**
			 * д���4���ֽ�
			 * @param pInts д���ֽڴ洢λ��
			 * @param len д���ֽ�����pInts���ȱ�������Ϊlen����������ڴ����
			 */		
			void	writeInts(int32_t* pInts,size_t len);
			
			/**
			 * д4���䳤���ֽ�			 
			 */		
			void	writeVInt(int32_t i);

			/**
			 * д8���ֽ�			 
			 */		
			void	writeLong(int64_t i);
			
			/**
			 * д8���䳤���ֽ�			 
			 */		
			void	writeVLong(int64_t i);

			/**
			 * д�ַ������洢��ʽΪ:len(VInt)|string
			 */		
			void	writeString(const CFXString& s);

			/**
			 * ��UTF-8����д�ַ�����
			 * @param s �ַ���
			 * @param start ��ʼλ��
			 * @param length ����
			 */					
			void	writeChars(const char* s, size_t start, size_t length);

			/**
			 * ��ȡ4�ֽ������䳤�洢�ĳ���
			 */
			byte	getVIntLength(int32_t i);

			/**
			 * �����ڲ�buffer
			 */
			void	setBuffer(char* buf,size_t bufSize);			

			/**
			 * �����ļ��ĵ�ǰλ��
			 */
			int64_t getFilePointer();
		
			/**
			 * flush buffered data to output
			 */
			void	flush();			
		public:
			/**
			 * ʵ��дbuffer�������ڵ�ǰλ��д���ݣ����麯��������ʵ��
			 * @param b ����buffer
			 * @param len b�ĳ���
			 */
			virtual void  flushBuffer(char* b,size_t len) = 0;

			/**
			 * �����ļ�����
			 */
			virtual int64_t length() = 0;

			/**
			 * �ر��ļ�,�ͷ���Դ,����������ش˺�����������ʽ����<code>CIndexOutput::close()</code>
			 */
			virtual void  close();

			/**
			 * �ļ���λ������������,������ʽ����<code>CIndexOutput::seek()</code>
			 * @param pos ��λλ��
			 */
			virtual void  seek(int64_t pos);			
		protected:
            char*			m_buffer;
			size_t			m_buffersize;
			int64_t			m_bufferStart;		// m_buffer ���ļ��е�ƫ����
			size_t			m_bufferPosition;	// m_buffer �е�λ��
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
				flushBuffer((char*)data,length);//ֱ��д					
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
