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
// Created	: 2007/4/14
//

#ifndef _DICTIONARYIMPORTER_H
#define _DICTIONARYIMPORTER_H

#include <set>
#include "../utility/StdHeader.h"
#include "../utility/StringUtils.h"
#include "TokenFilter.h"

using namespace std;

namespace firtex
{
	namespace analyzer
	{
		struct ltstr
		{
			bool operator()(const str_t* s1,const str_t* s2) const 
			{				
				return firtex::string_utils::fx_strnncmp(s1->str,s1->length,s2->str,s2->length) < 0;
			}
		};
		typedef std::set<const str_t*,ltstr> dictionary;
		class CAnteTokenFilter;
		class CDictionaryImporter
		{
		public:
			CDictionaryImporter(void);
			virtual ~CDictionaryImporter(void);
		public:
			/**
			 * import a dictionary 
			 * @param szDictionary path of dictionary
			 */
			void				import(const tchar* szDictionary);

			/**
			 * export a dictionary 
			 * @param szDict path of dictionary
			 */
			void				exportAs(const tchar* szDict);

			/**
			 * attach a ante-filter, generally, it is a stemming filter
			 * @param pAnteTokenFilter ante-filter
			 */
			void				attachAnteFilter(CTokenFilter* pAnteTokenFilter);

			/**
			 * detach a ante-filter
			 * @return ante-filter,NULL if there is no ante-filter
			 */
			CTokenFilter*	detachAnteFilter();
		protected:
            void				_import(char* buf,size_t len);
		protected:
			class CBuffers
			{
			public:
				CBuffers()
					:m_pBuffer(NULL)
					,m_nBufSize(0)
					,m_pNext(NULL)
				{
				}
				CBuffers(size_t nBufSize)
					:m_nBufSize(nBufSize)
					,m_pNext(NULL)
				{
					m_pBuffer = new char[nBufSize];
				}
				~CBuffers()
				{
					if(m_pBuffer)
					{
						delete m_pBuffer;
						m_pBuffer = NULL;
						m_nBufSize = 0;
					}
					if(m_pNext)
					{
						delete m_pNext;
						m_pNext = NULL;
					}
				}
			public:
				char*	newBuffer(size_t nBufSize)
				{
					CBuffers* pBuf = new CBuffers(nBufSize);
					CBuffers* pNext = this;
					while (pNext->m_pNext)
					{
						pNext = pNext->m_pNext;
					}
					pNext->m_pNext = pBuf;
					return pBuf->m_pBuffer;
				}
			protected:
				char*				m_pBuffer;
				size_t				m_nBufSize;
				CBuffers*			m_pNext;

				friend class CDictionaryImporter;
			};

		protected:
			CTokenFilter*		m_pAnteFilter;
			dictionary			m_dictionary;
			
			CTokens*			m_pTokenVector;
			CDictionaryImporter::CBuffers* m_pBuffers;		
		};
	}
}

#endif
