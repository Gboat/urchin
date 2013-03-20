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
#include "utility/StringUtils.h"
#include <string.h>
#include <stdio.h>


namespace firtex
{
	namespace string_utils
	{
		void trimleft(string& s )
		{
			string::iterator it;

			for( it = s.begin(); it != s.end(); it++ )
				if( !std::isspace(*it))
					break;

			s.erase( s.begin(), it );
		}

		void trimright( string& s )
		{
			string::difference_type dt;
			string::reverse_iterator it;

			for( it = s.rbegin(); it != s.rend(); it++ )
				if( !std::isspace( *it ) )
					break;

			dt = s.rend() - it;

			s.erase( s.begin() + dt, s.end() );
		}

		void trim( string& s )
		{
			trimleft( s );
			trimright( s );
		}

		string trim( const string& s )
		{
			string t = s;
			trim( t );
			return t;
		}

		void trimleft(wstring& s )
		{
			wstring::iterator it;

			for( it = s.begin(); it != s.end(); it++ )
				if( !std::isspace(*it))
					break;

			s.erase( s.begin(), it );
		}

		void trimright( wstring& s )
		{
			wstring::difference_type dt;
			wstring::reverse_iterator it;

			for( it = s.rbegin(); it != s.rend(); it++ )
				if( !std::isspace( *it ) )
					break;

			dt = s.rend() - it;

			s.erase( s.begin() + dt, s.end() );
		}

		void trim( wstring& s )
		{
			trimleft( s );
			trimright( s );
		}

		wstring trim( const wstring& s )
		{
			wstring t = s;
			trim( t );
			return t;
		}

		bool startsWith(const tchar *longString,const tchar *shortString)
		{
			if ((longString == NULL) || (shortString == NULL))
				return false;
			return (_tcsncmp(longString,shortString,_tcslen(shortString)) == 0);
		}

		bool startsWithNoCase(const tchar *longString,const tchar *shortString)
		{
			if ((longString == NULL) || (shortString == NULL))
				return false;
			return (_tcsnicmp(longString,shortString,_tcslen(shortString)) == 0);
		}
		
		bool endsWith(const tchar *longString, const tchar *shortString)
		{
			if ((longString == NULL) || (shortString == NULL))
				return false;
			size_t longLen = _tcslen(longString);
			size_t shortLen = _tcslen(shortString);
			if (shortLen > longLen)
				return false;
			tchar* longString2 = (tchar*)&longString[longLen - shortLen];
			return (_tcscmp(longString2, shortString) == 0);
		}
		bool endsWithNoCase(const tchar *longString, const tchar *shortString)
		{
			if ((longString == NULL) || (shortString == NULL))
				return false;
			size_t longLen = _tcslen(longString);
			size_t shortLen = _tcslen(shortString);
			if (shortLen > longLen)
				return false;
			tchar* longString2 = (tchar*)&longString[longLen - shortLen];
			return (_tcsicmp(longString2, shortString) == 0);
		}

		//////////////////////////////////////////////////////////////////////////
		///declares different (hopefully quicker) implementations of the standard library string functions
		size_t fx_strlen(const char *s) 
		{
			register const char *pos = s;
			while (*pos) 
			{
				pos++;
			}
			return pos - s;
		}

		size_t fx_strnlen(const char *s, size_t maxlen) 
		{
			register const char *pos = s,*end = s + maxlen;

			while (*pos && (pos < end)) 
			{
				pos++;
			}
			return pos - s;
		}

		int fx_strcmp(const char *s1, const char *s2) 
		{
			while (*s1 && (*s1 == *s2)) 
			{
				s1++;
				s2++;
			}
			return (unsigned char) *s1 - (unsigned char) *s2;
		}

		int fx_strncmp(const char *s1, const char *s2, size_t size) 
		{
			while (size && *s1 && (*s1 == *s2)) 
			{
				s1++;
				s2++;
				size--;
			}

			if (size) 
			{
				return (unsigned char) *s1 - (unsigned char) *s2;
			} 
			else 
			{
				return 0;
			}
		}

		int fx_strnncmp(const char *s1, size_t size1, const char *s2, size_t size2) 
		{
			int def;
			size_t len;

			if (size1 < size2) 
			{
				def = -1;
				len = size1;
			} 
			else if (size1 > size2)
			{
				def = 1;
				len = size2;
			} 
			else 
			{
				len = size1;
				def = 0;
			}

			while (len && *s1 && (*s1 == *s2)) 
			{
				s1++;
				s2++;
				len--;
			}

			if (len) 
			{
				return (unsigned char) *s1 - (unsigned char) *s2;
			} 
			else 
			{
				return def;
			}
		}

		/* lookup array of lowercase converted characters (note that we'll offset
		* lookups into this array by 128 to get rid of signed/unsigned problems) */
		static char lookup[] = {
			/* first comes the extended character set in case chars are signed */
			(char) 128, (char) 129, (char) 130, (char) 131, (char) 132, (char) 133, 
			(char) 134, (char) 135, (char) 136, (char) 137, (char) 138, (char) 139, 
			(char) 140, (char) 141, (char) 142, (char) 143, (char) 144, (char) 145, 
			(char) 146, (char) 147, (char) 148, (char) 149, (char) 150, (char) 151, 
			(char) 152, (char) 153, (char) 154, (char) 155, (char) 156, (char) 157, 
			(char) 158, (char) 159, (char) 160, (char) 161, (char) 162, (char) 163, 
			(char) 164, (char) 165, (char) 166, (char) 167, (char) 168, (char) 169, 
			(char) 170, (char) 171, (char) 172, (char) 173, (char) 174, (char) 175, 
			(char) 176, (char) 177, (char) 178, (char) 179, (char) 180, (char) 181, 
			(char) 182, (char) 183, (char) 184, (char) 185, (char) 186, (char) 187, 
			(char) 188, (char) 189, (char) 190, (char) 191, (char) 192, (char) 193, 
			(char) 194, (char) 195, (char) 196, (char) 197, (char) 198, (char) 199, 
			(char) 200, (char) 201, (char) 202, (char) 203, (char) 204, (char) 205, 
			(char) 206, (char) 207, (char) 208, (char) 209, (char) 210, (char) 211, 
			(char) 212, (char) 213, (char) 214, (char) 215, (char) 216, (char) 217, 
			(char) 218, (char) 219, (char) 220, (char) 221, (char) 222, (char) 223, 
			(char) 224, (char) 225, (char) 226, (char) 227, (char) 228, (char) 229, 
			(char) 230, (char) 231, (char) 232, (char) 233, (char) 234, (char) 235, 
			(char) 236, (char) 237, (char) 238, (char) 239, (char) 240, (char) 241, 
			(char) 242, (char) 243, (char) 244, (char) 245, (char) 246, (char) 247, 
			(char) 248, (char) 249, (char) 250, (char) 251, (char) 252, (char) 253, 
			(char) 254, (char) 255,

			/* then the regular 7-bit character set */
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 
			21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, '!', '"', '#', '$', '%', 
			'&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', 
			'5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'a', 'b', 'c', 
			'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 
			's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\', ']', '^', '_', '`', 'a', 
			'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 
			'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', 127, 

			/* then the extended character set again in case chars are unsigned */
			(char) 128, (char) 129, (char) 130, (char) 131, (char) 132, (char) 133, 
			(char) 134, (char) 135, (char) 136, (char) 137, (char) 138, (char) 139, 
			(char) 140, (char) 141, (char) 142, (char) 143, (char) 144, (char) 145, 
			(char) 146, (char) 147, (char) 148, (char) 149, (char) 150, (char) 151, 
			(char) 152, (char) 153, (char) 154, (char) 155, (char) 156, (char) 157, 
			(char) 158, (char) 159, (char) 160, (char) 161, (char) 162, (char) 163, 
			(char) 164, (char) 165, (char) 166, (char) 167, (char) 168, (char) 169, 
			(char) 170, (char) 171, (char) 172, (char) 173, (char) 174, (char) 175, 
			(char) 176, (char) 177, (char) 178, (char) 179, (char) 180, (char) 181, 
			(char) 182, (char) 183, (char) 184, (char) 185, (char) 186, (char) 187, 
			(char) 188, (char) 189, (char) 190, (char) 191, (char) 192, (char) 193, 
			(char) 194, (char) 195, (char) 196, (char) 197, (char) 198, (char) 199, 
			(char) 200, (char) 201, (char) 202, (char) 203, (char) 204, (char) 205, 
			(char) 206, (char) 207, (char) 208, (char) 209, (char) 210, (char) 211, 
			(char) 212, (char) 213, (char) 214, (char) 215, (char) 216, (char) 217, 
			(char) 218, (char) 219, (char) 220, (char) 221, (char) 222, (char) 223, 
			(char) 224, (char) 225, (char) 226, (char) 227, (char) 228, (char) 229, 
			(char) 230, (char) 231, (char) 232, (char) 233, (char) 234, (char) 235, 
			(char) 236, (char) 237, (char) 238, (char) 239, (char) 240, (char) 241, 
			(char) 242, (char) 243, (char) 244, (char) 245, (char) 246, (char) 247, 
			(char) 248, (char) 249, (char) 250, (char) 251, (char) 252, (char) 253, 
			(char) 254, (char) 255
		};

		int fx_strcasecmp(const char *s1, const char *s2) 
		{
			const char *us1 = (const char*) s1,*us2 = (const char*) s2;

			while (*us1 && (lookup[*us1 + 128] == lookup[*us2 + 128])) 
			{
				us1++;
				us2++;
			}

			return (unsigned char) lookup[*us1 + 128] - (unsigned char) lookup[*us2 + 128];
		}

		int fx_strncasecmp(const char *s1, const char *s2,size_t size) 
		{
			const char *us1 = (const char*) s1,*us2 = (const char*) s2;

			while (size && *us1 && (lookup[*us1 + 128] == lookup[*us2 + 128])) 
			{
				us1++;
				us2++;
				size--;
			}

			if (size) 
			{
				return (unsigned char) lookup[*us1 + 128] 
				- (unsigned char) lookup[*us2 + 128];
			}
			else 
			{
				return 0;
			}
		}
		char *fx_strchr(const char* s, int c) 
		{
			while (*s && *s != (char)c)
				s++;
			if (*s == (char)c)
				return((char *)s);
			return(NULL);
		}

		char *fx_strrchr(const char *s, int c) 
		{
			char *last = NULL;

			do 
			{
				if (*s == c) 
				{
					last = (char*) s;
				} 

				/* can't else here otherwise we can't detect c as \0 */
				if (!*s) 
				{
					return last;
				}
				else 
				{
					s++;
				}
			} while (1);
		}
		char* fx_strstr (const char * str1,const char * str2)
		{
			char *cp = (char *) str1;
			char *s1, *s2;

			if ( !*str2 )
				return((char *)str1);

			while (*cp)
			{
				s1 = cp;
				s2 = (char *) str2;

				while ( *s1 && *s2 && !(*s1-*s2) )
					s1++, s2++;

				if (!*s2)
					return(cp);

				cp++;
			}
			return(NULL);
		}
		
		char* fx_strstr2 (const char * str1,const char * str2)
		{
			 register char*a,*b;   
      
            ///First   scan   quickly   through   the   two   strings   looking   for   a   
            ///single-character   match.     When   it's   found,   then   compare   the   
            ///rest   of   the   str2.   
            ///   
      
            b = (char*)str2;   
            if(*b == 0)
			{   
				return (char*)str1;
			}   
            for(;*str1 != 0;str1 += 1)
			{   
				if(*str1 != *b)   
				{   
					continue;   
				}   
				a = (char*)str1;
				while(1)
				{   
					if(*b == 0)
					{   
						return (char*)str1;   
					}   
					if(*a++ != *b++)   
					{   
						break;   
					}   
				}   
				b = (char*)str2;   
			}   
			return (char*) 0;   
		}
		
		char* fx_strstr3 (const char * str1,const char * str2)
		{
			const unsigned char* haystack,*needle;
			const unsigned char   *rneedle;
			unsigned b;   
			haystack = (const unsigned char*)str1;

			if((b =*(needle = (const unsigned char *)str2)))   
			{   
				unsigned c;   
				haystack--; /*   possible   ANSI   violation   */   
				{   
					unsigned a;   
					do   
					{
						if(!(a = *++haystack))
							goto   ret0;   
					}while(a != b);   
				}   

				if(!(c   =   *++needle))   
					goto   foundneedle; 
				++needle;   
				goto jin;   

				for(;;)
				{   
					{   
						unsigned a;   
						if(0)   
					jin:{   
							if((a = *++haystack) == c)   
								goto   crest;   
						}   
						else   
							a = *++haystack;   
						do   
						{   
							for(;a != b; a = *++haystack)   
							{   
								if(!a)
									goto   ret0;   
								if((a = *++haystack) == b)   
									break;   
								if   (!a)   
									goto   ret0;   
							}   
						} while((a = *++haystack) != c);   
					}   
crest:   
					{   
						unsigned a;   
						{   
							const unsigned char* rhaystack;   
							if(*(rhaystack = haystack-- + 1) == (a = *(rneedle = needle)))   
								do   
								{   
									if(!a)   
										goto foundneedle;   
									if(*++rhaystack != (a = *++needle))   
										break;   
									if(!a)
										goto foundneedle;   
								}while(*++rhaystack == (a = *++needle));
								needle  =  rneedle; /*   took   the   register-poor   aproach   */   
						}   
						if (!a)
							break;   
					}   
				}   
			}   
foundneedle:   
			return (char*) haystack;   
ret0:   
			return 0;   
		}

		char* fx_strstr_rang (const char * strB,const char* strE,const char * str)
		{
			char *cp = (char *) strB;
			char *s1, *s2;

			if ( !*str )
				return((char *)strB);

			while ((cp < strE) && *cp)
			{
				s1 = cp;
				s2 = (char *) str;

				while ( *s1 && *s2 && !(*s1-*s2) )
					s1++, s2++;

				if (!*s2)
					return(cp);

				cp++;
			}
			return(NULL);
		}
	}
}

