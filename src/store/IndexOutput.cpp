//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/12/9
//
#include "store/IndexOutput.h"

namespace firtex
{
	namespace store
	{
		CIndexOutput::CIndexOutput(char* buffer,size_t buffsize)
		{
			if(buffer)
			{
				m_buffer = buffer;
				m_buffersize = buffsize;
				m_bOwnBuff = false;
			}
			else 
			{
				FIRTEX_THROW2(ILLEGALARGUMENT_ERROR,":CIndexOutput(char* buffer,size_t buffsize)");
			}
			m_bufferStart = 0;
			m_bufferPosition = 0;
		}
		CIndexOutput::CIndexOutput(size_t buffsize)
		{
			try
			{
				if(buffsize > 0)
				{
					m_buffer = new char[buffsize];
					m_buffersize = buffsize;
				}
				else 
				{
					m_buffer = new char[INDEXOUTPUT_BUFFSIZE];
					m_buffersize = INDEXOUTPUT_BUFFSIZE;
				}				

				m_bOwnBuff = true;

				m_bufferStart = 0;
				m_bufferPosition = 0;
			}
			catch (std::bad_alloc& be)
			{
				FIRTEX_THROW3(OUTOFMEM_ERROR,be.what());
			}
		}			

		CIndexOutput::~CIndexOutput(void)
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

		void CIndexOutput::write(CIndexInput* pInput,int64_t length)
		{
			if ( (m_bufferPosition + length) >= (int64_t)m_buffersize)
				flush();
			if(length <= (int64_t)(m_buffersize - m_bufferPosition) )
			{
				pInput->readInternal(m_buffer + m_bufferPosition,(size_t)length);
				pInput->seek(pInput->getFilePointer() + length);
				m_bufferPosition += (size_t)length;
			}
			else 
			{
				int64_t n=0;
				size_t nwrite=0;
				while(n < length)
				{
					nwrite = m_buffersize;
					if( (length - n) < (int64_t)nwrite)
						nwrite = (size_t)(length - n);
					pInput->readInternal(m_buffer,nwrite);
					pInput->seek(pInput->getFilePointer() + nwrite);
					if(nwrite == m_buffersize)
					{
						m_bufferPosition = nwrite;
						flush();
					}
					else m_bufferPosition += nwrite;
					n += nwrite;
				}
			}
		}
		void CIndexOutput::seek(int64_t pos)
		{
			flush();
			m_bufferStart = pos;
		}
		void CIndexOutput::close()
		{
			flush();
			m_bufferStart = 0;
			m_bufferPosition = 0;
		}
	}
}