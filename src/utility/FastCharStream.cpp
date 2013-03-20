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
// Created	: 2005/12/16
//
#include "utility/StdHeader.h"
#include "utility/FastCharStream.h"

namespace firtex
{
	namespace utility
	{
		CFastCharStream::CFastCharStream(CStringReader* reader):	input(reader),m_col(1),m_line(1)
		{
		}
		CFastCharStream::~CFastCharStream()
		{
		}

		char CFastCharStream::getNext()
		{
			if (isEos())
			{
				throw CFileIOException("CFastCharStream.getNext : Read char over EOS.");
			}
			char ch = input->readChar();
			m_col = static_cast<int>(input->getFilePointer())+1;
			if(ch == '\n')
			{
				m_line++;
				m_col = 1;
			}
			return ch;
		}
		void CFastCharStream::unget()
		{
			if ( input->getFilePointer() == 0 )
				throw CFileIOException("error : CFastCharStream.unget : ungetted first char");
			input->seek(input->getFilePointer()-1);
		}

		char CFastCharStream::peek()
		{
			try
			{
				return input->peek();
			}
			catch(...)
			{
				throw CFileIOException("error : CFastCharStream.peek() failed");
			} 
			return 0;
		}	
	}
}
