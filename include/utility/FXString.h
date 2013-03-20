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
#ifndef _FXSTRING_H
#define _FXSTRING_H

#include "typedefs.h"
#include "utility/Exception.h"
#include <string>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace firtex;
using namespace std;

namespace firtex
{
	namespace utility
	{
		template <typename CharT>
		class CFXString_base
		{
		public:
			typedef  CharT char_type;
		public:
			CFXString_base(void);
			CFXString_base(const char_type* str);								//will copy the string
			CFXString_base(char_type* str,bool bNoCopy);						//will or will NOT copy the string,depends on bNoCopy
			CFXString_base(const char_type* str,size_t len);					//will copy the string
			CFXString_base(char_type* str,size_t len,bool bNoCopy);				//will or will NOT copy the string,depends on bNoCopy
			CFXString_base(const _str<char_type>& str);							//will copy the string
			CFXString_base(const _str<char_type>& str,bool bNoCopy);			//will or will NOT copy the string,depends on bNoCopy
			CFXString_base(const basic_string<char_type>& str);					//will copy the string
			CFXString_base(const CFXString_base& src);							
			~CFXString_base(void);			
		public:			
			CFXString_base& operator=(const _str<char_type>& str);				//will copy the string
			CFXString_base& operator=(const char_type* str);					//will copy the string
			CFXString_base& operator=(const CFXString_base<char_type>& str);		//will copy the string
			CFXString_base& operator=(const basic_string<char_type>& str);		//will copy the string

			operator char_type*();
			operator _str<char_type>();
			operator basic_string<char_type>();

			bool operator==(const char_type* str)const;						
			bool operator==(const basic_string<char_type>& str)const;
			bool operator==(const CFXString_base<char_type>& right)const;

			bool operator!=(const char_type* str)const;
			bool operator!=(const basic_string<char_type>& str)const;
			bool operator!=(const CFXString_base<char_type>& right)const;
		public:
			/**
			 * return C-style string
			 */
			const char_type*	c_str()const;

			/**
			 * return the internal buffer of string
			 */
			char_type*	data();

			/**
			 * get the length of string
			 */
			size_t		length()const;

			/**
			 * get the max length of string
			 */
			size_t		maxLength()const{return m_maxLen;}

			/**
			 * reserve space for string
			 * @param len the length of reserved space
			 */
			void		reserve(size_t len);

			/**
			 * resize string
			 * @param len the new length of string
			 */
			void		resize(size_t len);

			/**
			 * assign string value.
			 * @param data string value
			 * @len length of data
			 * @bNoCopy no copy if true
			 */
			void		assign(char_type* data,size_t len,bool bNoCopy);

			void		assign (const char_type* str);
			void		assign (const char_type* str,size_t len);
			void		assign (const _str<char_type>& str);
		public:
			void		append(const char_type* str,size_t len);
			void		append(char_type ch);
			void		append(const basic_string<char_type>& str);
			char_type*	toString();
		public:
			int			compare(const CFXString_base<char_type>& right)const;
			int			compare(const char_type* str)const;
			int			compare(const basic_string<char_type>& str)const;
			int			compare(const _str<char_type>& str)const;

			void		clear();

		protected:		
			inline void	init();
		private:
			_str<char_type>	m_str;
			size_t			m_maxLen;
			bool			m_bOwner;
		};

		//////////////////////////////////////////////////////////////////////////
		//Inline Functions
		template <class CharT>
			inline CFXString_base<CharT>::CFXString_base(void)
		{
			init();		

			m_str.str = new CharT;
			m_bOwner = true;
			*(m_str.str) = 0;
		}
		template <class CharT>
			inline CFXString_base<CharT>::CFXString_base(const CharT* str)
		{
			init();
			assign(str);
		}
		template <class CharT>
			inline CFXString_base<CharT>::CFXString_base(CharT* str,bool bNoCopy)
		{
			if(bNoCopy)
			{
				m_str.str = str;
				m_maxLen = m_str.length = (sizeof(CharT)==sizeof(char))?strlen(str):wcslen(str);				 
				m_bOwner = false;
			}
			else 
			{
				init();
				assign(str);
			}
			
		}
		template <class CharT>
			inline CFXString_base<CharT>::CFXString_base(const CharT* str,size_t len)
		{			
			init();
			assign(str,len);
		}
		template <class CharT>
			inline CFXString_base<CharT>::CFXString_base(CharT* str,size_t len,bool bNoCopy)
		{
			init();
			if(bNoCopy)
			{
				m_str.str = str;
				m_maxLen = m_str.length = len;				 
				m_bOwner = false;
			}
			else assign(str,len);
		}
		template <class CharT>
			inline CFXString_base<CharT>::CFXString_base(const basic_string<CharT>& str)
		{
			init();
			assign(str.c_str(),str.length());			
		}
		template <class CharT>
			inline CFXString_base<CharT>::CFXString_base(const CFXString_base& src)
		{
			init();
			if(src.m_bOwner)
				assign(src.m_str.str,src.m_str.length);
			else 
			{
				m_str = src.m_str;				
			}
			m_maxLen = src.m_maxLen;
		}
		template <class CharT>
			inline CFXString_base<CharT>::~CFXString_base(void)
		{			
			if(m_bOwner && m_str.str)		
			{
				delete[] m_str.str;
			}
		}
		template <class CharT>
			inline void CFXString_base<CharT>::init()
		{
			m_str.str = NULL;			
			m_str.length = 0;
			m_bOwner = false;
			m_maxLen = 0;
		}

		template<class CharT>
			inline void CFXString_base<CharT>::clear()
		{
			if(m_str.length > 0)
			{
				if(m_bOwner && m_str.str)
				{
					delete[] m_str.str;
				}

				m_str.str = new CharT;
				m_bOwner = true;
				*(m_str.str) = 0;
				m_str.length = 0;
				m_maxLen = 0;
			}
		}

		template <>
			inline void CFXString_base<char>::assign (const char* str)
		{			
			m_str.length = strlen(str);
			if(!m_bOwner)
			{
				m_maxLen = m_str.length;
				m_str.str = new char[m_str.length + 1];
			}
			else 
			{
				if((m_str.length > m_maxLen))
				{
					if(m_str.str)
						delete[] m_str.str;
					m_str.str = new char[m_str.length + 1];
					m_maxLen = m_str.length;
				}
			}			
			strcpy(m_str.str,str);
			m_bOwner = true;
		}
		template <>
			inline void CFXString_base<wchar_t>::assign (const wchar_t* str)
		{
			m_str.length = wcslen(str);
			if(!m_bOwner)
			{
				m_maxLen = m_str.length;
				m_str.str = new wchar_t[m_str.length + 1];
			}
			else 
			{
				if((m_str.length > m_maxLen))
				{
					if(m_str.str)
						delete[] m_str.str;
					m_str.str = new wchar_t[m_str.length + 1];
					m_maxLen = m_str.length;
				}
			}			
			wcscpy(m_str.str,str);
			m_bOwner = true;
		}

		template <>
			inline void CFXString_base<char>::assign (const char* str,size_t len)
		{			
			if(len == 0)
				len = strlen(str);
			m_str.length = len;
			if(!m_bOwner)
			{
				m_maxLen = len;
				m_str.str = new char[len + 1];
			}
			else 
			{
				if((len > m_maxLen))
				{
					if(m_str.str)
						delete[] m_str.str;
					m_str.str = new char[len + 1];
					m_maxLen = len;
				}
			}			
			strncpy(m_str.str,str,len);
			m_str.str[len] = 0;
			m_bOwner = true;
			
		}
		template <>
			inline void CFXString_base<wchar_t>::assign (const wchar_t* str,size_t len)
		{
			if(len == 0)
				len = wcslen(str);
			m_str.length = len;
			if(!m_bOwner)
			{
				m_maxLen = len;
				m_str.str = new wchar_t[len + 1];
			}
			else 
			{
				if((len > m_maxLen))
				{
					if(m_str.str)
						delete[] m_str.str;
					m_str.str = new wchar_t[len + 1];
					m_maxLen = len;
				}
			}			
			wcsncpy(m_str.str,str,len);
			m_str.str[len] = 0;
			m_bOwner = true;
		}

		template <class CharT>
			inline CFXString_base<CharT>& CFXString_base<CharT>::operator=(const _str<CharT>& str)
		{
			assign(str.str,str.length);			
			return (*this);
		}
		template <class CharT>
			inline CFXString_base<CharT>& CFXString_base<CharT>::operator=(const CharT* str)
		{
			assign(str);
			return (*this);
		}
		template <class CharT>
			inline CFXString_base<CharT>& CFXString_base<CharT>::operator=(const CFXString_base<CharT>& str)
		{
            assign(str.m_str.str,str.m_str.length);			
			m_maxLen = str.m_maxLen;
			
			return (*this);
		}
		template <class CharT>
			inline CFXString_base<CharT>& CFXString_base<CharT>::operator=(const basic_string<CharT>& str)
		{
			assign(str.c_str(),str.length());			
			return (*this);
		}
		template <class CharT>
			inline CFXString_base<CharT>::operator CharT*()
		{
			return m_str.str;
		}
		template <class CharT>
			inline CFXString_base<CharT>::operator _str<CharT>()
		{
			return m_str;
		}
		template <class CharT>
			inline CFXString_base<CharT>::operator basic_string<CharT>()
		{
			basic_string<CharT> s;
			s.append(m_str.str,m_str.length);
			return s;
		}


		template <class CharT>
			inline void CFXString_base<CharT>::assign(CharT* data,size_t len,bool bNoCopy)
		{
			m_str.length = len;
			if(bNoCopy)
			{
				if(m_bOwner)
					delete[] m_str.str;
				m_str.str = data;
				m_bOwner = false;
			}
			else 
			{
				if(!m_bOwner)
                    m_str.str = new CharT[len + 1];
				else 
				{
					if((len > m_maxLen))
					{
						if(m_str.str)
							delete[] m_str.str;
						m_str.str = new CharT[len + 1];
						m_maxLen = len;
					}
				}
				memcpy(m_str.str,data,len*sizeof(CharT));
				m_str.str[len] = 0;
				m_bOwner = true;
			}
		}

		template <class CharT>
			inline bool CFXString_base<CharT>::operator==(const CharT* str)const
		{
			return (compare(str)==0);
		}
		template <class CharT>
			inline bool CFXString_base<CharT>::operator==(const basic_string<CharT>& str)const
		{
			return (compare(str)==0);
		}
		template <class CharT>
			inline bool CFXString_base<CharT>::operator==(const CFXString_base<CharT>& right)const
		{
			return (compare(right)==0);
		}

		template <class CharT>
			inline bool CFXString_base<CharT>::operator!=(const CharT* str)const
		{
			return (compare(str) != 0);
		}
		template <class CharT>
			inline bool CFXString_base<CharT>::operator!=(const basic_string<CharT>& str)const
		{
			return (compare(str) != 0);
		}
		template <class CharT>
			inline bool CFXString_base<CharT>::operator!=(const CFXString_base<CharT>& right)const
		{
			return (compare(right)!=0);
		}

		template<>
			inline int CFXString_base<char>::compare(const CFXString_base<char>& right)const
		{
			if(m_str.length == 0 || right.m_str.length == 0)
			{
				return (int)(m_str.length - right.m_str.length);
			}
			int ret = strncmp(m_str.str,right.m_str.str,(m_str.length > right.m_str.length)?right.m_str.length:m_str.length);
			if (ret != 0) return ret;
			else return (int)(m_str.length - right.m_str.length);
		}

		template<>
			inline int CFXString_base<wchar_t>::compare(const CFXString_base<wchar_t>& right)const
		{
			if(m_str.length == 0 || right.m_str.length == 0)
			{
				return (int)(m_str.length - right.m_str.length);
			}
			int ret = wcsncmp(m_str.str,right.m_str.str,(m_str.length > right.m_str.length)?right.m_str.length:m_str.length);
			if (ret != 0) return ret;
			else return (int)(m_str.length - right.m_str.length);
		}

		template<>
			inline int CFXString_base<char>::compare(const char* str)const
		{
			size_t rlen = strlen(str);
			if(m_str.length == 0 || rlen == 0)
			{
				return (int)(m_str.length - rlen);
			}
			int ret = strncmp(m_str.str,str,(m_str.length > rlen)?rlen:m_str.length);
			if (ret != 0) return ret;
			else return (int)(m_str.length - rlen);
		}

		template<>
			inline int CFXString_base<wchar_t>::compare(const wchar_t* str)const
		{
			size_t rlen = wcslen(str);
			if(m_str.length == 0 || rlen == 0)
			{
				return (int)(m_str.length - rlen);
			}
			int ret = wcsncmp(m_str.str,str,(m_str.length > rlen)?rlen:m_str.length);
			if (ret != 0) return ret;
			else return (int)(m_str.length - rlen);
		}
		template<>
			inline int CFXString_base<char>::compare(const basic_string<char>& str)const
		{
			size_t rlen = str.length();
			if(m_str.length == 0 || rlen == 0)
			{
				return (int)(m_str.length - rlen);
			}
			int ret = strncmp(m_str.str,str.c_str(),(m_str.length > rlen)?rlen:m_str.length);
			if (ret != 0) return ret;
			else return (int)(m_str.length - rlen);
		}
		template<>
			inline int CFXString_base<wchar_t>::compare(const basic_string<wchar_t>& str)const
		{
			size_t rlen = str.length();
			if(m_str.length == 0 || rlen == 0)
			{
				return (int)(m_str.length - rlen);
			}
			int ret = wcsncmp(m_str.str,str.c_str(),(m_str.length > rlen)?rlen:m_str.length);
			if (ret != 0) return ret;
			else return (int)(m_str.length - rlen);
		}
		template<>
			inline int CFXString_base<wchar_t>::compare(const _str<wchar_t>& str)const
		{
			size_t rlen = str.length;
			if(m_str.length == 0 || rlen == 0)
			{
				return (int)(m_str.length - rlen);
			}
			int ret = wcsncmp(m_str.str,str.str,(m_str.length > rlen)?rlen:m_str.length);
			if (ret != 0) return ret;
			else return (int)(m_str.length - rlen);
		}
		template <class CharT>
			inline const CharT* CFXString_base<CharT>::c_str()const
		{
			return m_str.str;
		}

		template <class CharT>
			inline CharT* CFXString_base<CharT>::data()
		{
			return m_str.str;
		}

		template <class CharT>
			inline size_t CFXString_base<CharT>::length()const
		{
			return m_str.length;
		}
		
		template <class CharT>
			void CFXString_base<CharT>::reserve(size_t len)
		{
			if(len > m_maxLen)
			{				
				CharT* tmp = new CharT[len + 1];
				memset(tmp,0,sizeof(CharT)*(len + 1));
				if(m_str.length > 0)
				{
					memcpy(tmp,m_str.str,(m_str.length>len)?len:m_str.length);					
				}
				if(m_bOwner && m_str.str)
					delete[] m_str.str;
				m_str.str = tmp;
				//m_str.length = len;
				m_bOwner = true;
				m_maxLen = len;
			}			
		}

		template <class CharT>
			void CFXString_base<CharT>::resize(size_t len)
		{
			if(len > m_maxLen)
			{
				if(m_bOwner)
					reserve(len);
				else 
					FIRTEX_THROW3(BAD_PARAMETER_ERROR,"void CFXString_base<CharT>::resize(size_t len):can't resize string,maybe the string is not owned by CFXString.");
			}
			m_str.str[len] = 0;
			m_str.length = len;
		}
		template <class CharT>
			void CFXString_base<CharT>::append(const CharT* str,size_t len)
		{
			if((m_str.length + len) > m_maxLen)
			{
				if(m_bOwner)
					reserve(m_str.length + len);
				else 
					FIRTEX_THROW3(BAD_PARAMETER_ERROR,"void CFXString_base<CharT>::append(CharT* str,size_t len):can't append string,maybe the string is not owned by CFXString.");
			}
			memcpy(m_str.str+m_str.length,str,len*sizeof(CharT));
			m_str.length += len;
			m_str.str[m_str.length] = 0;
		}

		template <class CharT>
			void CFXString_base<CharT>::append(CharT ch)
		{
			if((m_str.length + 1) > m_maxLen)
			{
				if(m_bOwner)
					reserve(m_str.length + 1);
				else 
					FIRTEX_THROW3(BAD_PARAMETER_ERROR,"void CFXString_base<CharT>::append(CharT ch):can't append char,maybe the string is not owned by CFXString.");
			}
			m_str.str[m_str.length] = ch;
			m_str.length += 1;
			m_str.str[m_str.length] = 0;
		}

		template<class CharT>
			void CFXString_base<CharT>::append(const basic_string<char_type>& str)
		{
			append(str.c_str(),str.length());
		}
		
		template<class CharT>
			CharT* CFXString_base<CharT>::toString()
		{
			CharT* ret = new CharT[m_str.length + 1];
			memcpy(ret,m_str.str,m_str.length*sizeof(CharT));
			ret[m_str.length] = 0;
			return ret;
		}
	
		typedef CFXString_base<char>	CFXStringA;
		typedef CFXString_base<wchar_t> CFXStringW;
		 
#ifdef _UNICODE
		typedef CFXStringW CFXString;
#else
		typedef CFXStringA CFXString;
#endif
		
	}
}

#endif
