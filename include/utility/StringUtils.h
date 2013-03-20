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
#ifndef _STRINGUTILS_H
#define _STRINGUTILS_H

#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <algorithm>

//#include "tchar.h"
#include "platform.h"
using namespace std;

namespace firtex
{
	namespace string_utils
	{
		template<typename TString, typename TSep>
			std::vector<TString> split(const TString& szText, TSep Separator = " ")
		{
			std::vector<TString> vec;
			TString str(szText);
			TString sep(Separator);
			size_t n = 0, nOld=0;
			while (n != TString::npos)
			{
				n = str.find(sep,n);
				if (n != TString::npos)
				{
					if (n != nOld)
						vec.push_back(str.substr(nOld, n-nOld));
					n += sep.length();
					nOld = n;
				}
			}
			vec.push_back(str.substr(nOld, str.length()-nOld));

			return vec;
		}

		template<typename TString>
			std::vector<TString> split(const TString& szText, char Separator = ' ')
		{
			TString sep(" ");
			sep[0] = Separator;
			return split<TString, TString>(szText, sep);
		}

		template<typename TString>
			std::vector<TString> split(const TString& szText, wchar_t Separator = ' ')
		{
			TString sep(" ");
			sep[0] = Separator;
			return split<TString, TString>(szText, sep);
		}
		template<typename _TX>
			string append(const string& szLeft,const _TX xRight)
		{
			stringstream ss;
			ss << szLeft << xRight;
			return ss.str();
		}
		template<typename _TX>
			wstring append(const wstring& szLeft,const _TX xRight)
		{
			wstringstream ss;
			ss << szLeft << xRight;
			return ss.str();
		}

		template <typename TCont, typename TGlue>
			TCont join(const TCont& container, const TGlue& glue)
		{
			TCont szResp = container;
			szResp += glue;
			return szResp;
			/*TCont szResp("");
			TCont::const_iterator iter;
			size_t s = container.size();
			for (iter = container.begin(); iter != container.end(); iter++)
			{
				szResp += (*iter);
				if (--s)
					szResp += glue;
			}

			return szResp;*/
		}

		template <typename TCont>
			TCont join(const TCont& container)
		{
			return join(container, ' ');
		}

		template <typename TString, typename TSearch, typename TReplace>
			TString replace(const TString& baseStr, const TSearch& searchStr, const TReplace& replaceStr)
		{
			TString szResp(baseStr);
			TString szSearch(searchStr);
			TString szReplace(replaceStr);
			if (searchStr == replaceStr)
				return szResp;

			size_t searchSize = szSearch.size();
			size_t replaceSize = szReplace.size();
			size_t n = 0;

			while (n != TString::npos)
			{
				n = szResp.find(szSearch, n);
				if (n != TString::npos)
				{
					szResp.replace(n, searchSize, szReplace);
					n += replaceSize;
				}
			}
			return szResp;
		}


		template <typename TString, typename TSearch,typename CharT>
			TString replace(const TString& baseStr, const TSearch& searchStr, CharT replaceChar)
		{
			TString stdReplaceStr(" ");
			stdReplaceStr[0] = replaceChar;
			return replace(baseStr, searchStr, stdReplaceStr);
		}

		template <typename TString, typename TReplace,typename CharT>
			TString replace(const TString& baseStr, CharT searchChar, const TReplace& replaceStr)
		{
			TString stdSearchStr(" ");
			stdSearchStr[0] = searchChar;
			return replace(baseStr, stdSearchStr, replaceStr);
		}

		template <typename TString,typename CharT>
			TString replace(const TString& baseStr, CharT searchChar, CharT replaceChar)
		{
			TString stdSearchStr(" ");
			TString stdReplaceStr(" ");
			stdSearchStr[0] = searchChar;
			stdReplaceStr[0] = replaceChar;
			return replace(baseStr, stdSearchStr, stdReplaceStr);
		}

		template <typename TString>
			TString properCase(const TString& str)
		{
			TString szResp(str);
			szResp[0] = std::toupper(szResp[0]);
			for (size_t i = 1; i < szResp.size(); i++)
				if (std::isspace(szResp[i-1]))
					szResp[i] = std::toupper(szResp[i]);
				else
					szResp[i] = std::tolower(szResp[i]);

			return szResp;
		}

		
		template <typename TString>
			TString toUpper(const TString& str)
		{
			TString szResp(str);
			std::transform(szResp.begin(),szResp.end(), szResp.begin(), (int(*)(int))std::toupper);

			return szResp;
		}		
		template <typename TString>
			TString toLower(const TString& str)
		{
			TString szResp(str);
			std::transform(szResp.begin(),szResp.end(), szResp.begin(), (int(*)(int))std::tolower);

			return szResp;
		}

				
		void trimleft(string& s );

		void trimright( string& s );

		void trim( string& s );
		inline string trim( const string& s );

		void trimleft(wstring& s );

		void trimright( wstring& s );

		void trim( wstring& s );

		wstring trim( const wstring& s );

		/** 
		 * 判断shortString是否是longString的前缀
		 * @param longString 长字符串
		 * @param shortString 需要判断前缀的短字符串
		 * @return 当shortString是longString的前缀时返回true，否则返回false
		 */
		bool startsWith(const tchar *longString,const tchar *shortString);

		/** 
		 * 判断shortString是否是longString的前缀,,不区分大小写
		 * @param longString 长字符串
		 * @param shortString 需要判断前缀的短字符串
		 * @return 当shortString是longString的前缀时返回true，否则返回false
		 */
		bool startsWithNoCase(const tchar *longString,const tchar *shortString);

		/** 
		 * 判断shortString是否是longString的后缀
		 * @param longString 长字符串
		 * @param shortString 需要判断后缀的短字符串
		 * @return 当shortString是longString的后缀时返回true，否则返回false
		 */
		bool endsWith(const tchar *longString, const tchar *shortString);

		/** 
		 * 判断shortString是否是longString的后缀,不区分大小写
		 * @param longString 长字符串
		 * @param shortString 需要判断后缀的短字符串
		 * @return 当shortString是longString的后缀时返回true，否则返回false
		 */
		bool endsWithNoCase(const tchar *longString, const tchar *shortString);

		//////////////////////////////////////////////////////////////////////////
		///declares different (hopefully quicker) implementations of the standard library string functions

		/** 
		 * version of strlen
		 * 
		 */
		size_t fx_strlen(const char *s);

		/**
		 * version of strlen with maximum size 
		 */
		size_t fx_strnlen(const char *s, size_t maxlen);

		/**
		 * version of _strcmp 
		 */
		int fx_strcmp(const char *s1, const char *s2);

		/**
		 * version of strncmp. 
		 */
		int fx_strncmp(const char *s1, const char *s2, size_t size);

		/* equivalent to str_ncmp, except that both strings have a limiting length.  If
		 * the strings are identical until one ends, the shorter string is considered 
		 * smaller. 
		 */
		int fx_strnncmp(const char *s1, size_t size1, const char *s2, size_t size2);

		/**
		 * version of strcasecmp.  return an integer less than, equal to or greater
		 * than 0 if s1 is less than, equal to or greater than s2, ignoring case.
		 * If a letter gets compared with []\^_` the letter will be evaluate to more
		 * than these characters (equivalent to conversion to lowercase) 
		 */
		int fx_strcasecmp(const char *s1, const char *s2);

		/**
		 * version of strncasecmp.  return an integer less than, equal to or greater
		 * than 0 if the first size characters of s1 are less than, equal to or greater 
		 * than the first size charactesr of s2, ignoring case.
		 * If a letter gets compared with []\^_` the letter will be evaluate to more
		 * than these characters (equivalent to conversion to lowercase) 
		 */
		int fx_strncasecmp(const char *s1, const char *s2, size_t size);

		/**
		 * version of strchr.strstr
		 **/
		char* fx_strchr(const char *s, int c);

		/**
		 * version of strchr.
		 */
		char *fx_strrchr(const char *s, int c);

		/** 
		 * version of strstr
		 */
		char* fx_strstr (const char * str1,const char * str2);

		/** 
		 * version of strstr
		 */
		char* fx_strstr2 (const char * str1,const char * str2);

		/** 
		 * version of strstr
		 */
		char* fx_strstr3 (const char * str1,const char * str2);

		/** 
		 * search a sub-string in a range
		 */
		char* fx_strstr_rang (const char * strB,const char* strE,const char * str);
	}
}

#endif
