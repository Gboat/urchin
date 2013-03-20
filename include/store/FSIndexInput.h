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
// Created	: 2005/11/9
//
#ifndef __FSINDEXINPUT_H
#define __FSINDEXINPUT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <fstream>
#include <string>
#include "IndexInput.h"
#include "utility/Config.h"

using namespace std;

namespace firtex
{
	namespace store
	{
		class CFSIndexInput : public CIndexInput
		{				
		public:
			CFSIndexInput(const tchar* filename);
			CFSIndexInput(const tchar* filename,char* buffer,size_t buffsize);
			CFSIndexInput(const tchar* filename,size_t buffsize);

			virtual ~CFSIndexInput();
		public:
			/**
			 * read data from file, sub class must implement this method
			 * @param b target address
			 * @length data length to read
			 * @bCheck check read position or not
			 */			
			void			readInternal(char* b,size_t length,bool bCheck = true);

			/**
			 * ��¡һ��CIndexInput����,��¡����ʹ��buffer��Ϊ����	 
			 * @param buffer ��¡����ʹ�õ�buffer
			 * @param buffsize buffer�Ĵ�С
			 */					
			CIndexInput*	clone(char* buffer,size_t buffsize);

			/**
			 * ��¡һ��CIndexInput����,��¡����ʹ���Լ�˽�е��ڴ���Ϊ����	 
			 */
			CIndexInput*	clone();

			/**
			 * �ر��ļ����ͷ���Դ
			 */
			void			close();
		protected:
			void  seekInternal(int64_t position);
		protected:						
			int				m_fileHandle;
			tstring			m_filename;
		};		
	}
}


#endif
