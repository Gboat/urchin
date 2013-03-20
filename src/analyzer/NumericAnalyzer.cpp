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
// Created	: 2005/7/24
//

#include "analyzer/NumericAnalyzer.h"
#include "utility/ParamParser.h"

namespace firtex
{
	namespace analyzer
	{
		CNumericAnalyzer::CNumericAnalyzer(void)
			: m_numberType (CNumber::INT64)
		{
			setMaxTokensToAnalyze(1);
		}

		CNumericAnalyzer::~CNumericAnalyzer(void)
		{
		}

		CTokens* CNumericAnalyzer::newTokenVector()
		{
			int32_t nMaxTokens = getMaxTokensToAnalyze();
			CTokens* pTokens = NULL;
			switch(m_numberType)
			{
			case CNumber::BOOL:
				pTokens = new CTokenXs<bool>(nMaxTokens);
				pTokens->setType(CToken::BOOL);				
				break;
			case CNumber::INT8:
				pTokens = new CTokenXs<int8_t>(nMaxTokens);
				pTokens->setType(CToken::INT8);
				break;
			case CNumber::UINT8:
				pTokens = new CTokenXs<uint8_t>(nMaxTokens);
				pTokens->setType(CToken::UINT8);
				break;
			case CNumber::INT16:
				pTokens = new CTokenXs<int16_t>(nMaxTokens);
				pTokens->setType(CToken::INT16);
				break;
			case CNumber::UINT16:
				pTokens = new CTokenXs<uint16_t>(nMaxTokens);
				pTokens->setType(CToken::UINT16);
				break;
			case CNumber::INT32:
				pTokens = new CTokenXs<int32_t>(nMaxTokens);
				pTokens->setType(CToken::INT32);
				break;
			case CNumber::UINT32:
				pTokens = new CTokenXs<uint32_t>(nMaxTokens);
				pTokens->setType(CToken::UINT32);
				break;
			case CNumber::INT64:
				pTokens = new CTokenXs<int64_t>(nMaxTokens);
				pTokens->setType(CToken::INT64);
				break;
			case CNumber::UINT64:
				pTokens = new CTokenXs<uint64_t>(nMaxTokens);
				pTokens->setType(CToken::UINT64);
				break;
			case CNumber::FLOAT:
				pTokens = new CTokenXs<float>(nMaxTokens);
				pTokens->setType(CToken::FLOAT);
				break;
			case CNumber::DOUBLE:
				pTokens = new CTokenXs<double>(nMaxTokens);
				pTokens->setType(CToken::DOUBLE);
				break;
			}	
			if(!pTokens)
				FIRTEX_THROW3(ANALYZE_ERROR,_T("Create token container faild."));
			return pTokens;
		}
		CTokens* CNumericAnalyzer::nextTokensInternal(CTokens* pUnAnalyzedTokens)
		{			
			char* pText = pUnAnalyzedTokens->getTokenBuffer();
			size_t length = pUnAnalyzedTokens->getTokenBufferSize();
			CTokens* pInput = getTokenVector();
			void*	pTokensBuf = (void*)pInput->getTokenBuffer();
			size_t	nMaxTokens = pInput->getMaxTokens();

			switch(m_numberType)
			{
			case CNumber::BOOL:
				{
					CTokenXs<bool>* pTokenXs = (CTokenXs<bool>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(bool*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			case CNumber::INT8:
				{
					CTokenXs<int8_t>* pTokenXs = (CTokenXs<int8_t>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(int8_t*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			case CNumber::UINT8:
				{
					CTokenXs<uint8_t>* pTokenXs = (CTokenXs<uint8_t>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(uint8_t*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			case CNumber::INT16:
				{
					CTokenXs<int16_t>* pTokenXs = (CTokenXs<int16_t>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(int16_t*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			case CNumber::UINT16:
				{
					CTokenXs<uint16_t>* pTokenXs = (CTokenXs<uint16_t>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(uint16_t*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			case CNumber::INT32:
				{
					CTokenXs<int32_t>* pTokenXs = (CTokenXs<int32_t>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(int32_t*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			case CNumber::UINT32:
				{
					CTokenXs<uint32_t>* pTokenXs = (CTokenXs<uint32_t>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(uint32_t*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			case CNumber::INT64:
				{
					CTokenXs<int64_t>* pTokenXs = (CTokenXs<int64_t>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(int64_t*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			case CNumber::UINT64:
				{
					CTokenXs<uint64_t>* pTokenXs = (CTokenXs<uint64_t>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(uint64_t*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			case CNumber::FLOAT:
				{
					CTokenXs<float>* pTokenXs = (CTokenXs<float>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(float*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			case CNumber::DOUBLE:
				{
					CTokenXs<double>* pTokenXs = (CTokenXs<double>*)pInput->asTokensX();
					int32_t nNumTokens = parse((const tchar*)pText,(double*)pTokensBuf,nMaxTokens);
					if(nNumTokens > 0)
						pTokenXs->setNumTokens(nNumTokens);
				}
				break;
			}				
			
			return pInput;
		}			

		int32_t CNumericAnalyzer::parse(const tchar* str,bool* pNumBuf,size_t nMaxNum)
		{
			return 0;
		}

		int32_t CNumericAnalyzer::parse(const tchar* str,int8_t* pNumBuf,size_t nMaxNum)
		{
			return 0;
		}

		int32_t CNumericAnalyzer::parse(const tchar* str,uint8_t* pNumBuf,size_t nMaxNum)
		{
			return 0;
		}
		
		int32_t CNumericAnalyzer::parse(const tchar* str,int16_t* pNumBuf,size_t nMaxNum)
		{
			return 0;
		}

		int32_t	CNumericAnalyzer::parse(const tchar* str,uint16_t* pNumBuf,size_t nMaxNum)
		{
			return 0;
		}

		int32_t CNumericAnalyzer::parse(const tchar* str,int32_t* pNumBuf,size_t nMaxNum)
		{
			return 0;
		}

		int32_t CNumericAnalyzer::parse(const tchar* str,uint32_t* pNumBuf,size_t nMaxNum)
		{
			return 0;
		}
		
		int32_t CNumericAnalyzer::parse(const tchar* str,int64_t* pNumBuf,size_t nMaxNum)
		{
			char* pTmp;
			*pNumBuf = (int64_t)strtol(str,&pTmp,10);
			return 1;
		}
	
		int32_t CNumericAnalyzer::parse(const tchar* str,uint64_t* pNumBuf,size_t nMaxNum)
		{
			return 0;
		}
		
		int32_t CNumericAnalyzer::parse(const tchar* str,float* pNumBuf,size_t nMaxNum)
		{
			return 0;
		}

		int32_t CNumericAnalyzer::parse(const tchar* str,double* pNumBuf,size_t nMaxNum)
		{
			return 0;
		}



		void CNumericAnalyzer::setParam(const tchar* pszParam)
		{
			firtex::utility::CParamParser p;
			if(p.parse(pszParam))
			{
				tstring sVal;
				if(p.getParam(_T("num_type"),sVal))
				{
					if(!_tcsicmp(sVal.c_str(),_T("bool")))
					{
						m_numberType = CNumber::BOOL;
					}
					else if(!_tcsicmp(sVal.c_str(),_T("int8")))
					{
						m_numberType = CNumber::INT8;
					}
					else if(!_tcsicmp(sVal.c_str(),_T("uint8")))
					{
						m_numberType = CNumber::UINT8;
					}
					else if(!_tcsicmp(sVal.c_str(),_T("int16")))
					{
						m_numberType = CNumber::INT16;
					}
					else if(!_tcsicmp(sVal.c_str(),_T("uint16")))
					{
						m_numberType = CNumber::UINT16;
					}
					else if(!_tcsicmp(sVal.c_str(),_T("int32")))
					{
						m_numberType = CNumber::INT32;
					}
					else if(!_tcsicmp(sVal.c_str(),_T("uint32")))
					{
						m_numberType = CNumber::UINT32;
					}
					else if(!_tcsicmp(sVal.c_str(),_T("int64")))
					{
						m_numberType = CNumber::INT64;
					}
					else if(!_tcsicmp(sVal.c_str(),_T("uint64")))
					{
						m_numberType = CNumber::UINT64;
					}
					else if(!_tcsicmp(sVal.c_str(),_T("float")))
					{
						m_numberType = CNumber::FLOAT;
					}
					else if(!_tcsicmp(sVal.c_str(),_T("double")))
					{
						m_numberType = CNumber::DOUBLE;
					}
				}
			}
			CAnalyzer::setParam(pszParam);            
		}
	}
}

