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
// Created	: 2006/5/20
//
#ifndef _STRINGREADER_H
#define _STRINGREADER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../utility/StdHeader.h"
#include "../utility/StringUtils.h"
#include <string>
#include "Reader.h"
using namespace std;

namespace firtex
{
	namespace utility
	{
		class CStringReader :	public CReader
		{
		public:
			CStringReader()
				:CReader()
			{

			}
			CStringReader(const char* value)
				:CReader(firtex::string_utils::fx_strlen(value) + 1)
			{			
				if(value && value[0] != 0)
				{	
					strcpy(m_buffer,value);
					m_bufferLength = firtex::string_utils::fx_strlen(value);
					m_length = m_bufferLength;
				}				
			}
			CStringReader(const char* value,const size_t length)
				:CReader(length+1)
			{				
				memcpy(m_buffer,value,length);
				m_bufferLength = length;
				m_buffer[length] = 0; 				
				m_length = m_bufferLength;
			}
			CStringReader(const string& value)
				:CReader((size_t)value.size() + 1)
			{	
				m_bufferLength = value.size();
				if(m_bufferLength > 0)
				{					
					memcpy(m_buffer,value.c_str(),m_bufferLength);
					m_buffer[m_bufferLength] = 0; 
				}						
				m_length = m_bufferLength;
			}
			virtual ~CStringReader(void)
			{				
			}
		public:
			CReader*	clone(char* buffer,size_t buffsize)
			{				
				//CStringReader* pClone = new CStringReader(m_filename,buffer,buffsize);
				return NULL;
			}

			CReader*	clone()
			{
				CStringReader* pClone = new CStringReader(m_buffer,m_bufferLength);
				return pClone;
			}

			char	readChar()
			{
				if (m_bufferPosition >= m_bufferLength)
					refill();
				return m_buffer[m_bufferPosition++];
			}
			char	peek()
			{
				if (m_bufferPosition >= m_bufferLength)
					refill();
				return m_buffer[m_bufferPosition];
			}
			
			bool	open(const char* strBuffer,size_t len)
			{
				CReader::close();
				if(m_bOwnBuff)
					delete[] m_buffer;
				m_buffer = (char*)strBuffer;
				m_length = m_bufferSize = len;
				m_bOwnBuff = false;		
				return true;
			}			

			const char* getReaderName() const{return "StringReader";}
		protected:
			void	readInternal(char* b, size_t offset, size_t len){}			
			void	seekInternal(int64_t position){}				
		};
	}
}

#endif
