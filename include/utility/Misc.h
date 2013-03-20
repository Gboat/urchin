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
// Created	: 2006/5/20
//
#ifndef _MISC_H
#define _MISC_H
#include "typedefs.h"
#include <string>
#include <algorithm>


namespace firtex
{
	namespace utility
	{
		static size_t POW_TABLE[] = 
		{
			1,2,4,8,16,32,64,128,256,512,
			1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,
			1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,
			1073741824,2147483648,
		};

		class CMisc
		{
		public:
			CMisc(void);
			~CMisc(void);
		public:

			/**
			 * 获取当前时间
			 */
			static int64_t	currentTimeMillis();

			//for std::string			
			static tchar*	getBuffer(tstring& s,size_t len);	
			
			/**
			 * 判断目录是否存在
			 */
			static bool		dirExists(const tchar* path);

			/**
			 * 将宽字符转换成UTF-8
			 * @param outbuf 转换结果
			 * @param ch 单个宽字符
			 */
			static size_t	wctoutf8(char * utf, const wchar_t wc);

			/**
			 * 将宽字符转换成UTF-8
			 * @param result 转换结果
			 * @param str 宽字符
			 */
			static size_t	wcstoutf8(char * utf,size_t ulen, const wchar_t * wcs, size_t wlen);

			/**
			 * 将UTF-8字符转换成宽字符
			 * @param wc 转换结果
			 * @param utf UTF-8字节
			 * @param ulen UTF-8字节长度
			 * @return utf的实际转换长度
			 */
			static size_t	utf8towc(wchar_t *wc, const char *utf, size_t ulen);
			/**
			 * 将UTF-8字符转换成宽字符
			 * @param pwc 转换结果
			 * @param p UTF-8字符串
			 */
			static size_t	utf8towcs(wchar_t * wcs,size_t wlen,const char * utf, size_t ulen);

			/**
			 * 将宽字符字符串转换成多字节字符字符串
			 */
			static size_t	_wcstombs(char *mbs,size_t mlen,const wchar_t *wcs, size_t wlen);

			/**
			 * 将多字节字符串转换成宽字节字符串
			 */
			static size_t	_mbstowcs(wchar_t *wcs, size_t wlen,const char *mbs, size_t mlen);

			/** 计算val的对数，取上限 */
			static inline size_t LOG2_UP(size_t val);
			/** 计算val的对数，取下限 */
			static inline size_t LOG2_DOWN(size_t val);
		};

		//////////////////////////////////////////////////////////////////////////
		///
		inline size_t CMisc::LOG2_UP(size_t val)
		{
			for (int32_t i = 0;i<31;i++)
			{
				if(val <= POW_TABLE[i] )
					return i;
			}
			return 0;
		}
		inline size_t CMisc::LOG2_DOWN(size_t val)
		{
			for (int32_t i = 1;i<31;i++)
			{
				if(val == POW_TABLE[i] )
					return i;
				else if(val < POW_TABLE[i])
					return i-1;				
			}
			return 0;
		}
	}
}

#endif
