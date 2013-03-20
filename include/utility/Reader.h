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
			 * ��ȡ���ݿ�
			 * @param data Ŀ�����ݿ��׵�ַ
			 * @param length Ŀ�����ݿ��С
			 */
			void		read(char* data, size_t length);
			
			/**
			 * ֱ�Ӷ�ȡ���ݿ�,û�����ݿ���			 
			 * @param length ���ݿ��С
			 * @return ���ݿ��׵�ַ
			 */
			char*		readWithoutCopy(size_t& length);		

			/**
			 * �������ļ��еĵ�ǰλ�ã���һ�ζ������λ�ÿ�ʼ
			 */			
			int64_t		getFilePointer(){return m_bufferStart + m_bufferPosition;}

			/**
			 * �����ļ��ĵ�ǰ��λ�ã���һ�ζ������λ�ÿ�ʼ
			 * @param pos ��λ��λ��
			 */			
			void		seek(int64_t pos);	

			/**
			 * �����ļ�����			 
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
			 * �ж��ļ������Ƿ�����
			 */		
			virtual bool	isEof(){return ( (m_bufferStart + (int64_t )m_bufferPosition) >= m_length);}

			/**
			 * �ر��ļ���������ʵ��
			 */
			virtual void  close();
		public:
			/**
			 * ��¡һ��CReader����,��¡����ʹ��buffer��Ϊ����	 
			 */					
			virtual CReader*	clone(char* buffer,size_t buffsize) = 0;
			/**
			 * ��¡һ��CReader����,��¡����ʹ���Լ�˽�е��ڴ���Ϊ����	 
			 */					
			virtual CReader*	clone() = 0;
		protected:
			/**
			 * ʵ���ļ���λ������readInternal�Ķ����������λ�ÿ�ʼ��������ʵ��
			 * @param pos ��λλ��
			 */					
			virtual void  seekInternal(int64_t position) = 0;

			/**
			 * ���ļ��е�ǰλ�ö����ݣ�������ʵ�ִ˺���
			 * @param b Ŀ���ַ
			 * @param offset ƫ����
			 * @length ����
			 */			
			virtual void  readInternal(char* b, size_t offset, size_t length) = 0;	
			
		protected:
			/**
			 * ���ļ����ж�������䵽��������
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
			if (end > m_length)	//�����˽�β					
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
