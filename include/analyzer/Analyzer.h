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
// Created	: 2005/11/20
//
#ifndef __ANALYZER_H
#define __ANALYZER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "../document/Document.h"
#include "../parser/Parser.h"
#include "Tokens.h"
#include "TokenFilter.h"
#include "../document/doctypedef.h"

using namespace firtex;
using namespace firtex::parser;
using namespace firtex::index;

namespace firtex
{
	namespace analyzer
	{		
		class CAnalyzer
		{
		public:
			class Creator
			{
			protected:
				Creator()
					: m_identifier(_T("analyzer"))
				{}
			public:
				~Creator(){}
			public:
				/**
				 * create a collection object
				 * @return create a analyzer
				 */
				virtual CAnalyzer* create() = 0;		

				/** 
				* get the identifier of collection
				*/
				virtual const tchar* getIdentifier(){return m_identifier.c_str();}			
			protected:
				tstring m_identifier;
			};
		public:
			CAnalyzer();			
			virtual ~CAnalyzer(void);		
		public:
			/**
			 * attach a post-filter, generally, it is a stop word filter
			 * @param pPostTokenFilter post-filter
			 */
			void				attachPostFilter(CTokenFilter* pPostTokenFilter);

			/**
			 * detach a post-filter
			 * @return post-filter,NULL if there is no post-filter
			 */
			CTokenFilter*		detachPostFilter();

			/**
			 * attach a ante-filter, generally, it is a stemming filter
			 * @param pAnteTokenFilter ante-filter
			 */
			void				attachAnteFilter(CTokenFilter* pAnteTokenFilter);

			/**
			 * detach a ante-filter
			 * @return ante-filter,NULL if there is no ante-filter
			 */
			CTokenFilter*		detachAnteFilter();
			
		public:
			/**
			 * analyze text reader, generate tokens
			 * @param pReader text reader
			 * @param pInput input tokens provided by outside, it can be NULL
			 * @return tokens,if pInput is not null, then return pInput,otherwise create a tokens and return it.
			 */
			inline CTokens*		nextTokens(CReader* pReader);

			/**
			 * analyze text buffer, generate tokens
			 * @param _buf text buffer
			 * @param len length of the buffer
			 * @param encoding encoding of the text
			 * @param pInput input tokens provided by outside, it can be NULL
			 * @return tokens,if pInput is not null, then return pInput,otherwise create a tokens and return it.
			 */
			inline CTokens*		nextTokens(char* _buf,size_t len,charset_t encoding);

			/**
			 * analyze text buffer, generate tokens
			 * @param _buf text buffer
			 * @param len length of the buffer
			 * @param pInput input tokens provided by outside, it can be NULL
			 * @return tokens,if pInput is not null, then return pInput,otherwise create a tokens and return it.
			 */
			inline CTokens*		nextTokens(wchar_t* _buf,size_t len);

			/**
			 * analyze text buffer, generate tokens
			 * @param text text buffer			 
			 * @param encoding encoding of the text
			 * @param pInput input tokens provided by outside, it can be NULL
			 * @return tokens,if pInput is not null, then return pInput,otherwise create a tokens and return it.
			 */
			inline CTokens*		nextTokens(const str_t& text,charset_t encoding);

			/**
			 * analyze text buffer, generate tokens
			 * @param text text buffer(Unicode)			 
			 * @param pInput input tokens provided by outside, it can be NULL
			 * @return tokens,if pInput is not null, then return pInput,otherwise create a tokens and return it.
			 */
			inline CTokens*		nextTokens(const wstr_t& text);

			/**
			 * get tokens analyzed by this analyzer
			 * @return tokens, internal object 
			 */
			inline CTokens*		getTokenVector();

		public:
			/**
			 * has offset or not
			 * @return true if we record the token's start/end offset
			 */
			bool		hasOffset(){return getTokenVector()->hasOffset();}

			/**
			 * to record the token offset or not
			 * @param bHasOffset true if need to record the token's offset
			 */
			void		setOffset(bool bHasOffset){getTokenVector()->setOffset(bHasOffset);}

			/**
			 * has position increment or not
			 * @return true if we record the token's position increment
			 */
			bool		hasPositionIncrement(){return getTokenVector()->hasPositionIncrement();}

			/**
			 * to record the token's position increment or not
			 * @param bHasPositionIncrement true if need to record the token's position increment
			 */
			void		setPositionIncrement(bool bHasPositionIncrement){getTokenVector()->setPositionIncrement(bHasPositionIncrement);}
        
			/**
			 * set max tokens to analyze
			 * @param nMaxTokensToAnalyze max tokens to analyze
			 */
			inline void			setMaxTokensToAnalyze(int32_t nMaxTokensToAnalyze){m_nMaxTokensToAnalyze = nMaxTokensToAnalyze;}

			/**
			 * get max tokens to analyze
			 * @return max tokens to analyze
			 */
			inline int32_t		getMaxTokensToAnalyze(){return m_nMaxTokensToAnalyze;}
		public:
			/** 
			 * create a analyzer specific CTokens for storing analyzed tokens
			 * @return CTokens object, an internal object
			 */
			virtual	CTokens*		newTokenVector() = 0;

			/**
			 * get a preferred indexer
			 * @return preferred indexer
			 */
			virtual const tchar*	getRecommendIndexer() = 0;

			/**
			 * acceptable encoding of analyzer, sub class must implement it
			 * @return acceptable encoding name of analyzer
			 */
			virtual const tchar*	acceptableEncoding() = 0;

			/**
			 * get identifier of analyzer
			 * @return the identifier
			 */
			virtual const tchar*	getIdentifier() = 0;
		public:
			/**
			 * set parameter for analyzer
			 * @param pszParam parameter for analyzer. format:param_name=param_value
			 */
			virtual void			setParam(const tchar* pszParam);
		protected:
			/**
			 * analyze text reader, generate tokens, must implement by sub class
			 * @param pUnAnalyzedTokens unanalyzed tokens
			 * @return analyzed tokens,an internal object
			 */
			virtual CTokens*		nextTokensInternal(CTokens* pUnAnalyzedTokens) = 0;			
		protected:
			void					attachEncodingFilter();
		protected:
			inline CTokens*			nextTokens_i();
		protected:			
			CTokens*		m_pUnAnalyzedTokens;
			CTokens*		m_pAnalyzedTokens;
			charset_t		m_lastEncoding;
			bool			m_bConvertFilterAttached;
			CTokenFilter*	m_pAnteTokenFilter;	///ante-filter
			CTokenFilter*	m_pPostTokenFilter;	///post-filter		

			int32_t			m_nMaxTokensToAnalyze;
		protected:
			char*			m_pReaderBuffer;		///buffer for reader
			size_t			m_nBufSize;			///buffer size
			size_t			m_nTextLength;		///length of text 
		protected:
			static int32_t DEFAULT_MAX_BYTES_TO_ANALYZE;
		};
		//////////////////////////////////////////////////////////////////////////
		//inline functions				
		inline CTokens* CAnalyzer::nextTokens(CReader* pReader)
		{			
			if( (m_lastEncoding != pReader->getEncoding()) && (m_bConvertFilterAttached == false))
			{
				attachEncodingFilter();
			}
			size_t len;
			char* buf = pReader->readWithoutCopy(len);
			char* pTokenBuf = buf;
			size_t nTokenBufSize = len;
			if(len < pReader->length())
			{
				if(pReader->length() > m_nBufSize)
				{
					if(m_pReaderBuffer)
						delete[] m_pReaderBuffer;
					m_nBufSize = (size_t)pReader->length() + 1;
					if(m_nBufSize > (size_t)DEFAULT_MAX_BYTES_TO_ANALYZE)
						m_nBufSize = DEFAULT_MAX_BYTES_TO_ANALYZE;
					m_pReaderBuffer = new char[m_nBufSize];
				}
				m_nTextLength = 0;
				memcpy(m_pReaderBuffer,buf,len);
				m_nTextLength += len;
				len = (size_t)pReader->length() - len;
				if(len > m_nBufSize - m_nTextLength - 1)
					len = m_nBufSize - m_nTextLength - 1;
				pReader->read(m_pReaderBuffer + m_nTextLength,len);
				m_nTextLength += len;
								
				pTokenBuf = m_pReaderBuffer;
				nTokenBufSize = m_nTextLength;
				pTokenBuf[m_nTextLength] = 0;
			}
			else 
			{
				pTokenBuf = buf;
				nTokenBufSize = len;
			}
            
			if(!m_pUnAnalyzedTokens)
			{
				m_pUnAnalyzedTokens = new CTokens(pTokenBuf,nTokenBufSize);
				m_pUnAnalyzedTokens->setNumTokens(1);
			}
			else
			{
				m_pUnAnalyzedTokens->setTokenBuffer(pTokenBuf,nTokenBufSize);
			}

			if(m_pUnAnalyzedTokens->getEncoding() != pReader->getEncoding())
			{
				m_pUnAnalyzedTokens->setEncoding(pReader->getEncoding());
				if(m_bConvertFilterAttached == false)
					attachEncodingFilter();
			}
			
			return nextTokens_i();
		}

		inline CTokens* CAnalyzer::nextTokens(char* _buf,size_t len,charset_t encoding)
		{
			if(!m_pUnAnalyzedTokens)
			{
				m_pUnAnalyzedTokens = new CTokens(_buf,len);
				m_pUnAnalyzedTokens->setNumTokens(1);
			}
			else
			{
				m_pUnAnalyzedTokens->setTokenBuffer(_buf,len);
			}
			
			if(m_pUnAnalyzedTokens->getEncoding() != encoding)
			{
				m_pUnAnalyzedTokens->setEncoding(encoding);
				if(m_bConvertFilterAttached == false)
					attachEncodingFilter();
			}

			return nextTokens_i();
		}

		inline CTokens* CAnalyzer::nextTokens(wchar_t* _buf,size_t len)
		{				
			if(!m_pUnAnalyzedTokens)
			{
				m_pUnAnalyzedTokens = new CTokens((char*)_buf,len*sizeof(wchar_t));
				m_pUnAnalyzedTokens->setNumTokens(1);
			}
			else
			{
				m_pUnAnalyzedTokens->setTokenBuffer((char*)_buf,len*sizeof(wchar_t));
			}

			m_pUnAnalyzedTokens->setEncoding(charset::UTF_16);
			if(m_bConvertFilterAttached == false)
				attachEncodingFilter();

			return nextTokens_i();
		}

		inline CTokens*	CAnalyzer::nextTokens(const str_t& text,charset_t encoding)
		{
			return nextTokens(text.str,text.length,encoding);
		}
		inline CTokens*	CAnalyzer::nextTokens(const wstr_t& text)
		{
			return nextTokens(text.str,text.length);
		}
		inline CTokens* CAnalyzer::nextTokens_i()
		{
			CTokens* pTokens = m_pUnAnalyzedTokens;
			if(m_pAnteTokenFilter)
				pTokens = m_pAnteTokenFilter->filter(pTokens);
			getTokenVector()->clear();
			pTokens = nextTokensInternal(pTokens);
			if(m_pPostTokenFilter)
				return m_pPostTokenFilter->filter(pTokens);
			return pTokens;		
		}
		inline CTokens* CAnalyzer::getTokenVector()
		{
			if(!m_pAnalyzedTokens)
				m_pAnalyzedTokens = newTokenVector();
			return m_pAnalyzedTokens;
		}
	}
}


#endif
