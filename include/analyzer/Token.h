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
#ifndef _TOKEN_H
#define _TOKEN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "../utility/FXString.h"
#include "../index/Term.h"
#include "utility/Number.h"


namespace firtex
{
	namespace analyzer
	{
		typedef uint8_t token_t;
		class CToken
		{		
		public:
			const static int32_t UNKNOWN	= 0;
			const static int32_t BOOL		= firtex::utility::CNumber::BOOL;
			const static int32_t INT8		= firtex::utility::CNumber::INT8;
			const static int32_t UINT8		= firtex::utility::CNumber::UINT8;
			const static int32_t INT16		= firtex::utility::CNumber::INT16;
			const static int32_t UINT16		= firtex::utility::CNumber::UINT16;
			const static int32_t INT32		= firtex::utility::CNumber::INT32;
			const static int32_t UINT32		= firtex::utility::CNumber::UINT32;
			const static int32_t INT64		= firtex::utility::CNumber::INT64;
			const static int32_t UINT64		= firtex::utility::CNumber::UINT64;

			const static int32_t FLOAT		= firtex::utility::CNumber::FLOAT;
			const static int32_t DOUBLE		= firtex::utility::CNumber::DOUBLE;

			const static int32_t TERMID		= CToken::DOUBLE + 1;
			const static int32_t TEXT		= CToken::TERMID + 1;
		public:
			CToken(void) 
				: m_nStartOffset(0)
				, m_nEndOffset(0)
				, m_nPositionIncrement(1)
				, m_type(CToken::UNKNOWN)
			{				
			}			
			CToken(const tchar* text)
				: m_sText(text)
				, m_nStartOffset(0)
				, m_nEndOffset(0)
				, m_nPositionIncrement(1)
				, m_type(CToken::UNKNOWN)
			{
			}
			CToken(const tstring& text)
				: m_sText(text)
				, m_nStartOffset(0)
				, m_nEndOffset(0)
				, m_nPositionIncrement(1)
				, m_type(CToken::UNKNOWN)
			{
			}
			CToken(const CToken& clone)
				: m_sText(clone.m_sText)
			{					
				m_nStartOffset = clone.m_nStartOffset;
				m_nEndOffset = clone.m_nEndOffset;
				m_nPositionIncrement = clone.m_nPositionIncrement;
				m_type = clone.m_type;
			}
			CToken(const tchar* text,int32_t nStartOffset,int32_t nEndOffset,int32_t nPositionIncrement)
				: m_sText(text)
				, m_nStartOffset(nStartOffset)
				, m_nEndOffset(nEndOffset)
				, m_nPositionIncrement(nPositionIncrement)
			{
			}
			virtual ~CToken(void)
			{				
			}
		public:			
			/**
			 * get text value of token
			 * @return text of token
			 */
			inline const tchar*	textValue();

			/**
			 * get size of text
			 * @return size of text
			 */
			inline size_t		getTextLength();


			/**
			 * set text value of token
			 * @param text text string
			 */
			void			setTextValue(const tchar* text);

			/**
			 * set text value of token
			 * @param text text string
			 */
			void			setTextValue(const tchar* text,size_t length);

			/**
			 * set text value of token
			 * @param text text string
			 */
			void			setTextValue(tchar* text,size_t length,bool bNoCopy);

			/** 
			 * get token type
			 * @return token type
			 */
			token_t			getType(){return m_type;};

			/**
			 * set token type 
			 * @param t token type
			 */
			void			setType(token_t t){m_type = t;}

			/**
			 * cast CToken to CTokenX
			 * @return CTokenX object
			 */
			virtual CToken*	asTokenX()
			{
				FIRTEX_THROW3(UNSUPPORTED_ERROR,_T("CToken::asTokenX()"));
				return NULL;
			}

			/**
			 * convert a token to a term
			 * @param field field of token
			 * @return term object
			 */
			virtual firtex::index::CTerm*	toTerm(const tchar* field){return new firtex::index::CTerm(field);}

			/**
			 * get start offset of token
			 * @return the offset
			 */
			inline int32_t	startOffset()const{return m_nStartOffset;}

			/**
			 * set start offset of token
			 * @param nStartOffset start offset
			 */
			inline void		setStartOffset(int32_t nStartOffset){m_nStartOffset = nStartOffset;}

			/**
			 * get end offset of token 
			 * @return the offset
			 */
			inline int32_t	endOffset()const{return m_nEndOffset;}

			/**
			 * set end offset of token
			 * @param nEndOffset end offset of token
			 */

			inline void		setEndOffset(int32_t nEndOffset){m_nEndOffset = nEndOffset;}

			/**
			 * get position increment of token
			 * @return position increment of token
			 */
			inline int32_t	getPositionIncrement(){return m_nPositionIncrement;}

			/**
			 * set position increment of token
			 * @param nPositionIncrement position increment of token
			 */
			inline void		setPositionIncrement(int32_t nPosInc){m_nPositionIncrement = nPosInc;}
		protected:			
			CFXString	m_sText;				///the token text
			int32_t		m_nStartOffset;			///start offset of token
			int32_t		m_nEndOffset;			///end offset of token
			int32_t		m_nPositionIncrement;	///position increment

			token_t		m_type;

			friend class CTokens;
		};

		//////////////////////////////////////////////////////////////////////////
		///inline functions

		inline const tchar* CToken::textValue()
		{
			return m_sText.c_str();
		}

		inline size_t CToken::getTextLength()
		{
			return m_sText.length();
		}

		inline void CToken::setTextValue(const tchar* text)
		{
			m_sText.assign(text);
		}

		inline void CToken::setTextValue(const tchar* text,size_t length)
		{
			m_sText.assign(text,length);			
		}

		inline void CToken::setTextValue(tchar* text,size_t length,bool bNoCopy)
		{
			m_sText.assign(text,length,bNoCopy);			
		}

		//////////////////////////////////////////////////////////////////////////
		///CTokenX
		template<typename TokenT>
		class CTokenX : public CToken
		{
		public:
			CTokenX(){}
			CTokenX(TokenT value)
				: m_value(value)
			{
			}
			virtual ~CTokenX(){}
		public:
			TokenT			getValue()const{return m_value;}
			void			setValue(const TokenT value){m_value = value;}
			CToken*			asTokenX(){return this;}			
			firtex::index::CTerm*			toTerm(const tchar* field){return new firtex::index::CTermX<TokenT>(field,m_value);}
		private:
			TokenT	m_value;
		};

		
	}
}

#endif
