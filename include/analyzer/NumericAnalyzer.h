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
// Created	: 2007/6/16
//
#ifndef __NUMERICANALYZER_H
#define __NUMERICANALYZER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Analyzer.h"
#include "utility/Number.h"

namespace firtex
{
	namespace analyzer
	{
		class CNumericAnalyzer : public CAnalyzer
		{		
		public:
			class Creator : public CAnalyzer::Creator
			{
			protected:
				Creator()					
				{
					m_identifier += _T(".numeric");
				}
			public:
				~Creator(){}
			public:
				/**
				 * create a collection object
				 * @return create a analyzer
				 */
				virtual CAnalyzer* create(){return new CNumericAnalyzer();}

			public:
				static Creator* getInstance()
				{
					static CNumericAnalyzer::Creator INSTANCE;
					return &INSTANCE;
				}
			};

		public:
			CNumericAnalyzer(void);
			virtual ~CNumericAnalyzer(void);
		public:		
			/** 
			 * create a analyzer specific CTokens for storing analyzed tokens
			 * @return CTokens object
			 */
			CTokens*		newTokenVector();

			/**
			 * get a suitable indexer for the analyzer
			 * @return indexer name
			 */
			const tchar*	getRecommendIndexer(){return _T("indexer.datetime");};

			/**
			 * acceptable encoding of analyzer, sub class must implement it
			 * @return acceptable encoding name of analyzer
			 */
			const tchar*	acceptableEncoding(){return _T("");};

			/**
			 * get identifier of analyzer
			 * @return the identifier
			 */
			const tchar*	getIdentifier(){return CNumericAnalyzer::Creator::getInstance()->getIdentifier();};

			/**
			 * set parameter for analyzer
			 * @param pszParam parameter for analyzer. format:param_name=param_value
			 */
			void			setParam(const tchar* pszParam);
		protected:
			/**
			 * analyze text reader, generate tokens, must implement by sub class
			 * @param pUnAnalyzedTokens unanalyzed tokens
			 * @return analyzed tokens,an internal object
			 */
			CTokens*		nextTokensInternal(CTokens* pUnAnalyzedTokens);
		public:
			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,bool* pNumBuf,size_t nMaxNum);

			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,int8_t* pNumBuf,size_t nMaxNum);

			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,uint8_t* pNumBuf,size_t nMaxNum);

			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,int16_t* pNumBuf,size_t nMaxNum);

			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,uint16_t* pNumBuf,size_t nMaxNum);

			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,int32_t* pNumBuf,size_t nMaxNum);

			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,uint32_t* pNumBuf,size_t nMaxNum);

			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,int64_t* pNumBuf,size_t nMaxNum);

			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,uint64_t* pNumBuf,size_t nMaxNum);

			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,float* pNumBuf,size_t nMaxNum);

			/**
			 * convert string to bool value
			 * @param str string
			 * @param pNumBuf number's buffer
			 * @param nMaxNum max numbers the buffer can contain.
			 * @return number of values
			 */
			static int32_t		parse(const tchar* str,double* pNumBuf,size_t nMaxNum);
		protected:			
			firtex::utility::number_t	m_numberType;
		};
	}
}

#endif
