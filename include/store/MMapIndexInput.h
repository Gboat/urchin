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
// Created	: 2007/4/23
//

#ifndef __MMAPINDEXINPUT_H
#define __MMAPINDEXINPUT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IndexInput.h"
#include "utility/Config.h"

#ifdef _WIN32
	#include <windows.h>
#endif

namespace firtex
{
	namespace store
	{
		class CMMapIndexInput :	public CIndexInput
		{
		public:			
			CMMapIndexInput(const tchar* filename);
			virtual ~CMMapIndexInput(void);
		public:
			/**
			 * read data from file, sub class must implement this method
			 * @param b target address
			 * @length data length to read
			 * @bCheck check read position or not
			 */			
			void			readInternal(char* b,size_t length,bool bCheck = true);
			
			/**
			 * 克隆一个CIndexInput对象,克隆对象使用buffer作为缓冲	 
			 * @param buffer 克隆对象使用的buffer
			 * @param buffsize buffer的大小
			 */					
			CIndexInput*	clone(char* buffer,size_t buffsize);			

			/**
			 * 克隆一个CIndexInput对象,克隆对象使用自己私有的内存作为缓冲	 
			 */
			CIndexInput*	clone();

			/**
			 * 关闭文件，释放资源
			 */
			void			close();
		protected:
			/**
			 * 实现文件定位操作，readInternal的读操作从这个位置开始，需子类实现
			 * @param pos 定位位置
			 */		
			void  seekInternal(int64_t position);	
		protected:			
			tstring			m_filename;

			uint8_t*		m_pData;
			int64_t			m_nPos;

#ifdef WIN32
			HANDLE			m_hMap;
			HANDLE			m_hFile;
#else
			int				m_hFile;
#endif
		};
	}
}


#endif
