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
#include "utility/StdHeader.h"
#include "analyzer/Tokens.h"


namespace firtex
{
	namespace analyzer
	{
		CTokens::CTokens(void)
			: m_nIterator(0)
			, m_bOwnBuffer(true)
			, m_pCurTokenPointer(0)			
			, m_nNumTokens(0)
			, m_nTokenPointer(0)
			, m_nMaxTokens(CTokens::MAX_TOKENS)
			, m_type(CToken::TEXT)
			, m_bHasOffset(false)
			, m_bHasPositionIncrement(false)
			, m_pOffsetBuffer(NULL)
			, m_pPositionIncrement(NULL)
			, m_encoding(charset::UNKNOWN)
			, m_pExtractedToken(NULL)
		{
			m_nBufferSize = CTokens::MAX_TOKENS*DEFAULT_TOKEN_SIZE;
			m_pTokensBuffer = new char[m_nBufferSize];
		}				
		CTokens::CTokens(size_t nMaxTokens,token_t tokenType)
			: m_nIterator(0)
			, m_bOwnBuffer(true)
			, m_nNumTokens(0)
			, m_nTokenPointer(0)
			, m_nMaxTokens(nMaxTokens)
			, m_pCurTokenPointer(0)			
			, m_type(tokenType)
			, m_bHasOffset(false)
			, m_bHasPositionIncrement(false)
			, m_pOffsetBuffer(NULL)
			, m_pPositionIncrement(NULL)
			, m_encoding(charset::UNKNOWN)
			, m_pExtractedToken(NULL)
		{			
			m_nBufferSize = nMaxTokens*DEFAULT_TOKEN_SIZE;
			m_pTokensBuffer = new char[m_nBufferSize];
		}		
		CTokens::CTokens(char* buffer,size_t nBufSize)
			: m_nNumTokens(0)
			, m_nIterator(0)
			, m_nTokenPointer(0)
			, m_pCurTokenPointer(0)
			, m_type(CToken::TEXT)
			, m_bHasOffset(false)
			, m_bHasPositionIncrement(false)
			, m_pOffsetBuffer(NULL)
			, m_pPositionIncrement(NULL)
			, m_encoding(charset::UNKNOWN)
			, m_nMaxTokens(CTokens::MAX_TOKENS)
			, m_pExtractedToken(NULL)
		{
			if(!buffer)
			{
				buffer = new char[nBufSize];
				m_bOwnBuffer = true;
			}
			else m_bOwnBuffer = false;

			m_pTokensBuffer = buffer;			
			m_nBufferSize = nBufSize;			
		}
		CTokens::CTokens(const CTokens& clone)
			: m_nBufferSize(clone.m_nBufferSize)
			, m_nIterator(0)
			, m_pCurTokenPointer(0)
			, m_nNumTokens(clone.m_nNumTokens)
			, m_nMaxTokens(clone.m_nMaxTokens)
			, m_nTokenPointer(clone.m_nTokenPointer)
			, m_type(CToken::TEXT)
			, m_bHasOffset(false)
			, m_bHasPositionIncrement(false)
			, m_pOffsetBuffer(NULL)
			, m_pPositionIncrement(NULL)
			, m_encoding(charset::UNKNOWN)
		{
			m_pTokensBuffer = new char[m_nBufferSize];
			memcpy(m_pTokensBuffer,clone.m_pTokensBuffer,m_nBufferSize);
			m_bOwnBuffer = true;
			m_pExtractedToken = NULL;
		}
		CTokens::~CTokens(void)
		{
			if(m_bOwnBuffer)
			{
				if(m_pTokensBuffer)
				{
					delete[] m_pTokensBuffer;
					m_pTokensBuffer = NULL;
				}
			}
			if(m_pOffsetBuffer)
			{
				delete[] m_pOffsetBuffer;
				m_pOffsetBuffer = NULL;
			}
			if(m_pPositionIncrement)
			{
				delete[] m_pPositionIncrement;
				m_pPositionIncrement = NULL;
			}
			clear();
		}

		void CTokens::startIterator()
		{			
			if(!m_pExtractedToken)
			{
				m_pExtractedToken = new CToken();
				m_pExtractedToken->setType(getType());
			}
			
			m_nIterator = 0;
			m_pCurTokenPointer = 0;
		}
		bool CTokens::hasNext()
		{
			return (m_nIterator < m_nNumTokens);
		}	

		CToken* CTokens::next()
		{
			size_t len;
			if(m_nNumTokens == 1)
			{
				if(m_nTokenPointer > 0)
					len = m_nTokenPointer;
				else len = m_nBufferSize;				
			}
			m_pExtractedToken->setTextValue(m_pTokensBuffer + m_pCurTokenPointer,len,true);
			m_pCurTokenPointer +=  (len + 1);

			extractExtra();

			m_nIterator++;
			return m_pExtractedToken;
		}
		void CTokens::clear()
		{
			if(m_pExtractedToken)
			{
				delete m_pExtractedToken;
				m_pExtractedToken = NULL;
			}			
			m_nNumTokens = 0;
		}

		bool CTokens::growBuffer(size_t nNewSize)
		{
			if(nNewSize == 0)
				nNewSize = m_nBufferSize*2;
			if( (m_nBufferSize >= nNewSize) || (m_bOwnBuffer == false) )
				return false;
			char* newBuffer = new char[nNewSize];
			memcpy(newBuffer,m_pTokensBuffer,m_nBufferSize);
			m_nBufferSize = nNewSize;
			delete[] m_pTokensBuffer;
			m_pTokensBuffer = newBuffer;
			return true;
		}

		void CTokens::setMaxTokens(int32_t nMaxTokens)
		{			
			m_nMaxTokens = nMaxTokens;
			if(m_bHasOffset && m_pOffsetBuffer)
			{
				delete[] m_pOffsetBuffer;
				m_pOffsetBuffer = new int32_t[nMaxTokens*2];
			}
			if(m_bHasPositionIncrement && m_pPositionIncrement)
			{
				delete[] m_pPositionIncrement;
				m_pPositionIncrement = new int32_t[nMaxTokens];
			}
		}			

		CTokens* CTokens::asTokensX()
		{
			FIRTEX_THROW3(CAST_ERROR,_T("Not a TokenXs object."));
		}

		void CTokens::setOffset(bool bHasOffset)
		{
			if(m_bHasOffset != bHasOffset)
			{
				if(m_bHasOffset && m_pOffsetBuffer)
				{
					delete[] m_pOffsetBuffer;
					m_pOffsetBuffer = NULL;
				}
				if(bHasOffset)
				{
					m_pOffsetBuffer = new int32_t[m_nMaxTokens*2];					
				}
				m_bHasOffset = bHasOffset;
			}
		}
		void CTokens::setPositionIncrement(bool bHasPositionIncrement)
		{
			if(m_bHasPositionIncrement != bHasPositionIncrement)
			{
				if(m_bHasPositionIncrement && m_pPositionIncrement)
				{
					delete[] m_pPositionIncrement;
					m_pPositionIncrement = NULL;
				}
				if(bHasPositionIncrement)
				{
					m_pPositionIncrement = new int32_t[m_nMaxTokens];					
				}
				m_bHasPositionIncrement = bHasPositionIncrement;
			}
		}

		void CTokens::setTokenBuffer(char* pTokenBuffer,size_t nBufSize)
		{
			if(m_bOwnBuffer && m_pTokensBuffer)
			{
				delete[] m_pTokensBuffer;
			}
			if(!pTokenBuffer)
			{
				m_pTokensBuffer = new char[nBufSize];
				m_nBufferSize = nBufSize;
				m_bOwnBuffer = true;
			}
			else 
			{
				m_pTokensBuffer = pTokenBuffer;
				m_nBufferSize = nBufSize;
				m_bOwnBuffer = false;
			}			
		}
	}
}
