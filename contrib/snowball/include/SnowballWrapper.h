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
// Created	: 2007/3/6
//

#ifndef _SNOWBALLWRAPPER_H
#define _SNOWBALLWRAPPER_H

#include <map>
#include "utility/StdHeader.h"

using namespace std;

struct sb_stemmer;

namespace firtex
{
	namespace analyzer
	{
		class CSnowballWrapper
		{
		public:
			CSnowballWrapper();
			/**
			 * constructor
			 * @param szLanguage language,NULL for English			 
			 */
			CSnowballWrapper(const tchar* szLanguage);			
			/**
			 * constructor
			 * @param szLanguage language,NULL for English			 
			 * @szEnc encode, NULL for utf-8
			 */
			CSnowballWrapper(const tchar* szLanguage,const tchar* szEnc);

			/**
			 * constructor
			 * @param szLanguage language,NULL for English
			 * @szIrregularWords irregular words file
			 * @szEnc encode, NULL for utf-8
			 */
			CSnowballWrapper(const tchar* szLanguage,const tchar* szIrregularWords,const tchar* szEnc);
			~CSnowballWrapper();
		public:
			/**
			 * stemmer a buffer
			 * @param szTextBuf input text buffer
			 * @param nTextLen length of szTextBuff
			 * @return output buffer,internal buffer,nTextLen is the length of buffer
			 */
			char* stemmer(char* szTextBuf,size_t& nTextLen);
		protected:
			
		protected:
			struct sb_stemmer*	m_pStemmer;            
			map<string,string>*	m_pIrregularWords;

			tstring		m_sIrregularWords;
			char*		m_pStemmerBuffer;		///buffer for stemmer
			size_t		m_nBufferSize;			///length of buffer
		};
	}
}

#endif
