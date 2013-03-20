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
			 * ��ȡ��ǰʱ��
			 */
			static int64_t	currentTimeMillis();

			//for std::string			
			static tchar*	getBuffer(tstring& s,size_t len);	
			
			/**
			 * �ж�Ŀ¼�Ƿ����
			 */
			static bool		dirExists(const tchar* path);

			/**
			 * �����ַ�ת����UTF-8
			 * @param outbuf ת�����
			 * @param ch �������ַ�
			 */
			static size_t	wctoutf8(char * utf, const wchar_t wc);

			/**
			 * �����ַ�ת����UTF-8
			 * @param result ת�����
			 * @param str ���ַ�
			 */
			static size_t	wcstoutf8(char * utf,size_t ulen, const wchar_t * wcs, size_t wlen);

			/**
			 * ��UTF-8�ַ�ת���ɿ��ַ�
			 * @param wc ת�����
			 * @param utf UTF-8�ֽ�
			 * @param ulen UTF-8�ֽڳ���
			 * @return utf��ʵ��ת������
			 */
			static size_t	utf8towc(wchar_t *wc, const char *utf, size_t ulen);
			/**
			 * ��UTF-8�ַ�ת���ɿ��ַ�
			 * @param pwc ת�����
			 * @param p UTF-8�ַ���
			 */
			static size_t	utf8towcs(wchar_t * wcs,size_t wlen,const char * utf, size_t ulen);

			/**
			 * �����ַ��ַ���ת���ɶ��ֽ��ַ��ַ���
			 */
			static size_t	_wcstombs(char *mbs,size_t mlen,const wchar_t *wcs, size_t wlen);

			/**
			 * �����ֽ��ַ���ת���ɿ��ֽ��ַ���
			 */
			static size_t	_mbstowcs(wchar_t *wcs, size_t wlen,const char *mbs, size_t mlen);

			/** ����val�Ķ�����ȡ���� */
			static inline size_t LOG2_UP(size_t val);
			/** ����val�Ķ�����ȡ���� */
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
