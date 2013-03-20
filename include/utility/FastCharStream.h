//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈğ½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/12/16
//
#ifndef _FASTCHARSTREAM_H
#define _FASTCHARSTREAM_H

#include "StringReader.h"

namespace firtex
{
	namespace utility
	{
		class CStringReader;
		class CFastCharStream
		{
			int m_col;
			int m_line;
						
		public:			
			CFastCharStream(CStringReader* reader);
			~CFastCharStream();
		public:
			char	getNext();
			void	unget();			
			char	peek();		
			bool	isEos(){return input->isEof();}		
			int		column(){return m_col;}			
			int		line(){return m_line;}
			
			CStringReader* input;
		};
	}
}

#endif
