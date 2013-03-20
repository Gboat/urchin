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
			 * ֱ�Ӷ�ȡ���ݿ�
			 * @param data Ŀ�����ݿ��׵�ַ
			 * @param length Ŀ�����ݿ��С
			 */
			void read(char* data, size_t length);

			/**
			 * ��һ���ֽ�			 
			 */
			byte readByte();

			/**
			 * ������ֽ�			 
			 * @param b ��ȡ�ֽڴ洢λ��
			 * @param len ��ȡ�ֽ���
			 */
			void  readBytes(byte* b,size_t len);
			
			/**
			 * ��4���ֽ�
			 */
			int32_t readInt();

			/**
			 * �����4���ֽ�
			 * @param i ��ȡ�ֽڴ洢λ��
			 * @param len ��ȡ�ֽ�����i���ȱ�������Ϊlen����������ڴ����
			 */
			void	readInts(int32_t* i,size_t len);

			/**
			 * ���䳤�洢��4���ֽ�			 
			 */
			int32_t readVInt();
			
			/**
			 * ��8���ֽ�			 
			 */
			int64_t readLong();
			
			/**
			 * ���䳤�洢��8���ֽ�			 
			 */
			int64_t readVLong();

			/**
			 * ���ַ���			 
			 */
			void readString(string& s);

			/**
			 * ���ַ���			 
			 */
			void readString(CFXString& s);

			/**
			 * ��UTF-8����洢���ַ�
			 * @param buffer Ŀ���ַ
			 * @param start buffer��ƫ����
			 * @param length ����
			 * @see CCacheWriter.writeChars
			 */			
			void  readChars(char* buffer, size_t start, size_t length);

			/**
			 * ����nNum��VInt
			 * @param nNum ��������			 			 
			 */			
			void skipVInt(size_t nNum);

			/**
			 * �������ļ��еĵ�ǰλ�ã���һ�ζ������λ�ÿ�ʼ
			 */			
			int64_t getFilePointer();

			/**
			 * �����ļ��ĵ�ǰ��λ�ã���һ�ζ������λ�ÿ�ʼ
			 * @param pos ��λ��λ��
			 */			
			void  seek(int64_t pos);

			/**
			 * �ж��ļ������Ƿ�����
			 */		
			bool	isEof();

			/**
			 * �����ļ�����			 
			 */					
			int64_t length()const;

			/**
			 * �����ļ����ȣ��ڶ�ȡʱ������ȡ�û����õ��ļ�����
			 */
			void	setLength(int64_t newLen);

			/**
			 * ����CIndexInput�Ķ�ȡbuffer,ֻ����CIndexInput ��ȡ�κ�����֮ǰʹ��
			 * @param buf buffer��ַ������Ϊ�ڲ����������ⲿɾ��
			 * @param bufSize buf�ĳ���
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
			 * ʵ���ļ���λ������readInternal�Ķ����������λ�ÿ�ʼ��������ʵ��
			 * @param pos ��λλ��
			 */					
			virtual void	seekInternal(int64_t pos) = 0;
			
			/**
			 * ��¡һ��CIndexInput����,��¡����ʹ��buffer��Ϊ����	 
			 * @param buffer ��¡����ʹ�õ�buffer
			 * @param buffsize buffer�Ĵ�С
			 */					
			virtual CIndexInput*	clone(char* buffer,size_t buffsize) = 0;
			
			/**
			 * ��¡һ��CIndexInput����,��¡����ʹ���Լ�˽�е��ڴ���Ϊ����	 
			 */					
			virtual CIndexInput*	clone() = 0;

			/**
			 * �ر��ļ���������ʵ��
			 */
			virtual void	close() = 0;
		protected:				
			/**
			 * ���ļ����ж�������䵽��������
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
			if (end > m_length)	//�����˽�β					
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
				m_bufferPosition = (size_t) (pos - m_bufferStart);//��λ���ڻ�������				
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
