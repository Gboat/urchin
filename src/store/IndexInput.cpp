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
// Created	: 2005/12/09
//
#include "store/IndexInput.h"

namespace firtex
{
	namespace store
	{
		CIndexInput::CIndexInput(char* buffer,size_t buffsize)
		{

			m_buffer = buffer;
			m_bufferSize = buffsize;
			m_bOwnBuff = false;

			m_bufferStart = 0; 
			m_bufferLength = 0; 
			m_bufferPosition = 0;

			m_length = 0;
		}
		CIndexInput::CIndexInput(size_t buffsize)
		{
			if(buffsize > 0)
			{
				m_buffer = NULL;
				m_bufferSize = buffsize;
			}
			else 
			{
				m_buffer = NULL;
				m_bufferSize = INDEXINPUT_BUFFSIZE;
			}

			m_bOwnBuff = true;

			m_bufferStart = 0; 
			m_bufferLength = 0; 
			m_bufferPosition = 0;

			m_length = 0;
		}	

		CIndexInput::~CIndexInput(void)
		{
			if(m_bOwnBuff)
			{
				if(m_buffer)
				{
					delete[]m_buffer;
					m_buffer = NULL;
				}
			}
		}
		void CIndexInput::read(char* data, size_t length)
		{
			if (m_bufferPosition >= m_bufferLength)
				refill();
			if(length <= (m_bufferLength-m_bufferPosition))
			{
				memcpy(data,m_buffer + m_bufferPosition,length);
				m_bufferPosition += length;
			}
			else 
			{
				size_t start = m_bufferLength - m_bufferPosition;
				if(start > 0)//缓存中还有数据，先读缓存
				{
					memcpy(data,m_buffer + m_bufferPosition,start);						
				}

				//readInternal(data,start,length - start); //直接读		
				readInternal(data + start,length - start); //直接读
				m_bufferStart += length;
			}
		}
		/**
		* 读多个字节			 
		*/
		void CIndexInput::readBytes(byte* b,size_t len)
		{
			if (len < m_bufferSize)
			{
				for (size_t i = 0; i < len; i++)
					// read byte-by-byte
					b[i] = (byte) readByte();
			}
			else
			{
				// read all-at-once
				int64_t start = getFilePointer();
				seekInternal(start);
				readInternal((char*)b,len);

				m_bufferStart = start + len; // adjust stream variables
				m_bufferPosition = 0;
				m_bufferLength = 0; // trigger refill() on read
			}
		}
		void CIndexInput::readInts(int32_t* i,size_t len)
		{
			for(size_t l = 0;l < len;l++)
				i[l] = readInt();
		}

		void CIndexInput::setBuffer(char* buf,size_t bufSize)
		{
			//if(m_bufferStart!=0 || m_bufferPosition != 0)
			if(m_bufferPosition != 0)
			{
				FIRTEX_THROW2(UNSUPPORTED_ERROR," void CIndexInput::setBuffer(char* buf,size_t bufSize):you must call setBuffer() before reading any data.");
			}
			if(m_bOwnBuff && m_buffer)
			{
				delete[] m_buffer;
			}
			m_buffer = buf;
			m_bufferSize = bufSize;
			m_bOwnBuff = false;
		}

	}
}