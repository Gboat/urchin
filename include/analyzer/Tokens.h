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
#ifndef _TOKENS_H
#define _TOKENS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "Token.h"
#include "../utility/StringUtils.h"
#include "utility/charsetdef.h"

using namespace std;

namespace firtex
{
	namespace analyzer
	{		
		class CTokens
		{	
		public:
			const static int32_t MAX_TOKENS = 10000;
			const static int32_t DEFAULT_TOKEN_SIZE = 4;			
		public:
			CTokens(void);			
			CTokens(size_t nMaxTokens,token_t tokenType);
			CTokens(char* buffer,size_t nBufSize);
			CTokens(const CTokens& clone);
			virtual ~CTokens(void);
		public:			
			virtual void	startIterator();
			virtual CToken*	next();
			bool			hasNext();
		public:
			/**
			 * get number of tokens
			 * @return number of tokens  
			 */			
			size_t		getNumTokens(){return m_nNumTokens;}

			/**
			 * set number of tokens
			 * @param nNumTokens number of tokens  
			 */			
			void		setNumTokens(size_t nNumTokens);

			/** get/set max tokens */
			size_t		getMaxTokens(){return m_nMaxTokens;}
			void		setMaxTokens(int32_t nMaxTokens);

			/** clear tokens */
			void		clear();	

			/** 
			 * get internal token buffer
			 * @return internal token buffer for storing tokens
			 */
			char*		getTokenBuffer(){return m_pTokensBuffer;}

			/**
			 * get token buffer size
			 * @return size of token buffer
			 */
			size_t		getTokenBufferSize(){return m_nBufferSize;}

			/**
			 * set buffer for storing tokens
			 * @param pTokenBuffer buffer
			 * @param nBufSize size of buffer
			 */
			void		setTokenBuffer(char* pTokenBuffer,size_t nBufSize);

			/** 
			 * get token type
			 * @return token type
			 */
			token_t		getType(){return m_type;};

			/**
			 * set token type 
			 * @param t token type
			 */
			void		setType(token_t t){m_type = t;}

			/**
			 * has offset or not
			 * @return true if we record the token's start/end offset
			 */
			bool		hasOffset(){return m_bHasOffset;}

			/**
			 * to record the token offset or not
			 * @param bHasOffset true if need to record the token's offset
			 */
			void		setOffset(bool bHasOffset);

			/** 
			 * get internal token's offset buffer which used to store the start/end offset of token
			 * @return internal token offset buffer for storing token's offsets, only available if {@link hasOffset()} return true. 
			 */			
			int32_t*	getOffsetBuffer(){return m_pOffsetBuffer;}

			/**
			 * has position increment or not
			 * @return true if we record the token's position increment
			 */
			bool		hasPositionIncrement(){return m_bHasPositionIncrement;}

			/**
			 * to record the token's position increment or not
			 * @param bHasPositionIncrement true if need to record the token's position increment
			 */
			void		setPositionIncrement(bool bHasPositionIncrement);

			/** 
			 * get internal token offset buffer which used to store the start/end offset of token
			 * @return internal token offset buffer for storing token offsets, only available if 
			 *			{@link hasOffset()} return true. 
			 */			
			int32_t*	getPositionIncrementBuffer(){return m_pPositionIncrement;}

		public:
			/**
			 * get encoding of text tokens
			 * @return encoding type of tokens, always charset::UNKNOWN if it is not a text token
			 */
			firtex::utility::charset_t	getEncoding(){return m_encoding;}

			/**
			 * set encoding type of text tokens
			 * @param enc encoding type
			 */
			void		setEncoding(firtex::utility::charset_t enc){m_encoding = enc;}

		public:			
			virtual CTokens* asTokensX();
		public:
			/**
			 * add a token
			 * @param pszToken token which ended with '\0'
			 * @return true if success, otherwise false
			 */
			inline bool	addToken(const tchar* pszToken);

			/**
			 * add a token
			 * @param pszToken token text which ended with '\0'
			 * @param nPositionIncrement position increment
			 * @param nStartOffset start offset of token
			 * @param nEndOffset end offset of token
			 * @return true if success, otherwise false
			 */
			inline bool	addToken(const tchar* pszToken,int32_t nPositionIncrement,int32_t nStartOffset,int32_t nEndOffset);

			/**
			 * add a token
			 * @param pszToken token text
			 * @param nTokenLen length of token
			 * @return true if success, otherwise false
			 */
			inline bool	addToken(const tchar* pszToken,size_t nTokenLen);

			/**
			 * add a text token
			 * @param pszToken token text
			 * @param nTokenLen length of token
			 * @param nPositionIncrement position increment
			 * @param nStartOffset start offset of token
			 * @param nEndOffset end offset of token
			 * @return true if success, otherwise false
			 */
			inline bool	addToken(const tchar* pszToken,size_t nTokenLen,int32_t nPositionIncrement,int32_t nStartOffset,int32_t nEndOffset);
		protected:
			/**
			 * grow buffer 
			 * @param nNewCapacity target size,if = 0 then TargetSize = 2*OldSize
			 * @return success or not
			 */
			bool		growBuffer(size_t nNewSize = 0);

			/**
			 * extract start/end offset or position increment of token 
			 */
			inline void	extractExtra();
		protected:									
			char*		m_pTokensBuffer;			///buffer for tokens
			size_t		m_nBufferSize;				///size of buffer											
			size_t		m_nTokenPointer;			///buffer used
			bool		m_bOwnBuffer;				///own the buffer or not

			bool		m_bHasOffset;				///has start/end offset or not
			int32_t*	m_pOffsetBuffer;			///offset buffer, it's length is m_nMaxTokens*2, only available when m_bHasOffset=true

			bool		m_bHasPositionIncrement;	///has position increment or not
			int32_t*	m_pPositionIncrement;		///position increment buffer, it's length is m_nMaxTokens, only available when m_bHasPositionIncrement = true
		
			size_t		m_nNumTokens;				///number of tokens
			size_t		m_nMaxTokens;				///max tokens 			

			token_t		m_type;						///token type

			firtex::utility::charset_t	m_encoding;					///charset of text if this is a text token, 
		protected:
			CToken*		m_pExtractedToken;

			size_t		m_nIterator;				///save iterator position
			size_t		m_pCurTokenPointer;			///where we get in the tokens buffer when iterate
		};

		//////////////////////////////////////////////////////////////////////////
		///TokenX
		template<typename TokenT>
		class CTokenXs : public CTokens
		{
		public:
			CTokenXs(){}
			CTokenXs(size_t nMaxTokens)
				: CTokens(NULL,nMaxTokens*sizeof(TokenT))
			{
				m_nMaxTokens = nMaxTokens;
				m_type = CToken::UNKNOWN;
			}
			virtual~CTokenXs(){}
		public:
			virtual void		startIterator();
			virtual CToken*		next();
			virtual CTokens*	asTokensX(){return this;}
		public:
			inline	bool		addTokenX(TokenT token);

			/**
			 * add a token
			 * @param token token
			 * @param nPositionIncrement position increment
			 * @param nStartOffset start offset of token
			 * @param nEndOffset end offset of token
			 * @return true if success, otherwise false
			 */
			inline bool	addTokenX(TokenT token,int32_t nPositionIncrement,int32_t nStartOffset,int32_t nEndOffset);
		};
		//////////////////////////////////////////////////////////////////////////
		//inline Function		
		inline bool	CTokens::addToken(const tchar* pszToken)
		{
			if(m_nNumTokens >= m_nMaxTokens)
				return false;				
			size_t nTokenLen = firtex::string_utils::fx_strlen(pszToken);
			if((m_nTokenPointer + nTokenLen + 1) >= m_nBufferSize)
			{
				if(!growBuffer())
					return false;
			}
			strcpy(m_pTokensBuffer + m_nTokenPointer,pszToken);
			m_nTokenPointer += (nTokenLen + 1);
			m_nNumTokens++;
			return true;
		}	

		inline bool	CTokens::addToken(const tchar* pszToken,int32_t nPositionIncrement,int32_t nStartOffset,int32_t nEndOffset)
		{
			bool bRet = addToken(pszToken);
			if(bRet)
			{
				if(m_bHasPositionIncrement)
					m_pPositionIncrement[m_nNumTokens - 1] =nPositionIncrement;
				if(m_bHasOffset)
				{
					size_t nPos = (m_nNumTokens - 1) << 2;
					m_pOffsetBuffer[nPos] = nStartOffset;
					m_pOffsetBuffer[nPos + 1] = nEndOffset;
				}					
			}
			return bRet;
		}

		inline bool	CTokens::addToken(const tchar* pszToken,size_t nTokenLen)
		{
			if(m_nNumTokens >= m_nMaxTokens)
				return false;						
			if((m_nTokenPointer + nTokenLen + 1) >= m_nBufferSize)
			{
				if(!growBuffer())
					return false;
			}
			strncpy(m_pTokensBuffer + m_nTokenPointer,pszToken,nTokenLen);
			m_nTokenPointer += (nTokenLen + 1);
			m_pTokensBuffer[m_nTokenPointer - 1] = 0;
			m_nNumTokens++;
			return true;
		}

		inline bool	CTokens::addToken(const tchar* pszToken,size_t nTokenLen,int32_t nPositionIncrement,int32_t nStartOffset,int32_t nEndOffset)
		{
			bool bRet = addToken(pszToken,nTokenLen);
			if(bRet)
			{
				if(m_bHasPositionIncrement)
					m_pPositionIncrement[m_nNumTokens - 1] =nPositionIncrement;
				if(m_bHasOffset)
				{
					size_t nPos = (m_nNumTokens - 1) << 2;
					m_pOffsetBuffer[nPos] = nStartOffset;
					m_pOffsetBuffer[nPos + 1] = nEndOffset;
				}					
			}
			return bRet;
		}

		inline void CTokens::setNumTokens(size_t nNumTokens)
		{
			if(nNumTokens > m_nMaxTokens)
				m_nNumTokens = m_nMaxTokens;
			else 
				m_nNumTokens = nNumTokens;
		}

		inline void	CTokens::extractExtra()
		{
			if(m_bHasOffset)
			{
				m_pExtractedToken->setStartOffset(m_pOffsetBuffer[2*m_nIterator]);
				m_pExtractedToken->setEndOffset(m_pOffsetBuffer[2*m_nIterator + 1]);
			}			
			if(m_bHasPositionIncrement)
			{
				m_pExtractedToken->setPositionIncrement(m_pPositionIncrement[m_nIterator]);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		///

		template<class TokenT>
			inline bool CTokenXs<TokenT>::addTokenX(TokenT token)
		{
			if(m_nNumTokens >= m_nMaxTokens)
				return false;				
			*(((TokenT*)m_pTokensBuffer) + m_nNumTokens) = token;
			m_nNumTokens++;
			return true;
		}	

		template<class TokenT>
			inline bool CTokenXs<TokenT>::addTokenX(TokenT token,int32_t nPositionIncrement,int32_t nStartOffset,int32_t nEndOffset)
		{
			bool bRet = addTokenX(token);
			if(bRet)
			{
				if(m_bHasPositionIncrement)
					m_pPositionIncrement[m_nNumTokens - 1] =nPositionIncrement;
				if(m_bHasOffset)
				{
					size_t nPos = (m_nNumTokens - 1) << 1;
					m_pOffsetBuffer[nPos] = nStartOffset;
					m_pOffsetBuffer[nPos + 1] = nEndOffset;
				}					
			}
			return bRet;
		}

		template<class TokenT>
			void CTokenXs<TokenT>::startIterator()
		{
			if(!m_pExtractedToken)
			{
				m_pExtractedToken = new CTokenX<TokenT>();
			}

			m_nIterator = 0;
			m_pCurTokenPointer = 0;
		}

		template<class TokenT>
			CToken*	CTokenXs<TokenT>::next()
		{
			((CTokenX<TokenT>*)m_pExtractedToken)->setValue(*(((TokenT*)m_pTokensBuffer) + m_nIterator) );
			
			extractExtra();

			m_nIterator++;
			return m_pExtractedToken;
		}
	}
}

#endif
