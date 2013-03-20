//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie),²Ü¶¬ÁÖ
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2007/3/6
//


#include <string.h>
#include <stdlib.h>
#include <string>
#include <map>
#include "include/SnowballWrapper.h"
#include "include/libstemmer.h"
#include "utility/Exception.h"


using namespace std;
//struct sb_stemmer * stemmer;
//map<string ,string> SpeWordList;
//static char * StemmedBuf=NULL;


namespace firtex
{
	namespace analyzer
	{
		CSnowballWrapper::CSnowballWrapper()
			:m_pStemmerBuffer(NULL)
			,m_nBufferSize(0)
			,m_pIrregularWords(NULL)
		{			
			m_pStemmer = sb_stemmer_new("english", NULL);///using utf-8 
		}
		CSnowballWrapper::CSnowballWrapper(const tchar* szLanguage)
			:m_pStemmerBuffer(NULL)
			,m_nBufferSize(0)
			,m_pIrregularWords(NULL)
		{			
			if(szLanguage)
				m_pStemmer = sb_stemmer_new(szLanguage, NULL);///using utf-8 
			else 
				m_pStemmer = sb_stemmer_new("english", NULL);///using utf-8 
		}		
		CSnowballWrapper::CSnowballWrapper(const tchar* szLanguage,const tchar* szEnc)
			:m_pStemmerBuffer(NULL)
			,m_nBufferSize(0)
			,m_pIrregularWords(NULL)
		{			
			if(szLanguage)
				m_pStemmer = sb_stemmer_new(szLanguage, szEnc);
			else 
				m_pStemmer = sb_stemmer_new("english", szEnc);
		}
		CSnowballWrapper::CSnowballWrapper(const tchar* szLanguage,const tchar* szIrregularWords,const tchar* szEnc)
			:m_pStemmerBuffer(NULL)
			,m_nBufferSize(0)
			,m_pIrregularWords(NULL)
		{			
			if(szIrregularWords)
			{
				m_pIrregularWords = new map<string,string>();
				m_sIrregularWords = szIrregularWords;
			}
			if(szLanguage)
				m_pStemmer = sb_stemmer_new(szLanguage, szEnc);
			else 
				m_pStemmer = sb_stemmer_new("english", szEnc);
		}
		CSnowballWrapper::~CSnowballWrapper()
		{
			if(m_pStemmer)
				sb_stemmer_delete(m_pStemmer);
			if(m_pStemmerBuffer)
			{
				delete[] m_pStemmerBuffer;
				m_pStemmerBuffer = NULL;
				m_nBufferSize = 0;
			}
			if(m_pIrregularWords)
			{
				m_pIrregularWords->clear();
				delete m_pIrregularWords;
				m_pIrregularWords = NULL;
			}
		}

		char* CSnowballWrapper::stemmer(char* szTextBuf,size_t& nTextLen)
		{			
			if(nTextLen <= 0)
				return NULL;
			if(m_pStemmerBuffer == NULL)
			{
				m_nBufferSize = nTextLen<<1;
				m_pStemmerBuffer = new char[m_nBufferSize];
				if(m_pStemmerBuffer == NULL)
				{
					m_nBufferSize = 0;
					return NULL;
				}
			}
			else
			{
				if(m_nBufferSize < (nTextLen<<1) )
				{
					delete[] m_pStemmerBuffer;
					m_nBufferSize = nTextLen<<1;
					m_pStemmerBuffer = new char[m_nBufferSize];
					if(m_pStemmerBuffer == NULL)
					{
						m_nBufferSize = 0;
						return NULL;
					}
				}
			}

			bool bFlagEnglish = true;
			int i;
			register char*	pWord;
			int  nWordLen;
			register char* pStemmedBufStart = m_pStemmerBuffer;
			register char *pStemmedBufEnd;
			register char *pWordStart,*pWordEnd;
			pWordStart = szTextBuf;
			char* pTextEnd = szTextBuf + nTextLen;
			while(pWordStart < pTextEnd)
			{
				
				while(pWordStart < pTextEnd && !((*pWordStart >= 'a' && *pWordStart <= 'z') || (*pWordStart >= 'A' && *pWordStart <= 'Z')))//(*pWordStart==' ' || *pWordStart=='\xd' || *pWordStart=='\xa'))
				{
					*pStemmedBufStart = *pWordStart;
					pWordStart++;
					pStemmedBufStart++;
				}
				if(pWordStart >= pTextEnd)
					break;

				pWordEnd = pWordStart + 1;
				while((pWordEnd < (pTextEnd)) && ((*pWordEnd >= 'a' && *pWordEnd <= 'z') || (*pWordEnd >= 'A' && *pWordEnd <= 'Z')))//*pWordEnd!=' ' && *pWordEnd!='\xd' && *pWordEnd!='\xa' && *pWordEnd!='.' && *pWordEnd!='?' && *pWordEnd!=',')
					pWordEnd++;

				pStemmedBufEnd = pStemmedBufStart;
				for(i=0;i < pWordEnd - pWordStart;i++)
				{
					if(pWordStart[i] >= 'A' && pWordStart[i] <= 'Z')
						*pStemmedBufEnd = 'a'-'A' + pWordStart[i];
					else
					{
						if(!(pWordStart[i] >= 'a' && pWordStart[i] <= 'z'))
							bFlagEnglish = false;
						*pStemmedBufEnd = pWordStart[i];
					}
					pStemmedBufEnd++;
				}
				if(bFlagEnglish)
				{					
					if(pWordEnd != pWordStart)
					{							
						if(*(pWordEnd-1) != 'y')
						{
							
							pWord = (char*)sb_stemmer_stem(m_pStemmer, (const sb_symbol*)pStemmedBufStart,(int)(pStemmedBufEnd - pStemmedBufStart) );
							nWordLen = sb_stemmer_length(m_pStemmer);								
							for(i=0;i < nWordLen;i++)
							{
								*pStemmedBufStart = pWord[i];
								pStemmedBufStart++;
							}							
						}
						else
						{
							pStemmedBufStart = pStemmedBufEnd;
						}
					}			
				}
				else
				{
					pStemmedBufStart = pStemmedBufEnd;
				}
				pWordStart = pWordEnd;
			}
			*pStemmedBufStart = 0;
			nTextLen = pStemmedBufStart - m_pStemmerBuffer;
			return m_pStemmerBuffer;
		}
	}
}

