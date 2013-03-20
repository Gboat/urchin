//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: Kevin Zhang,¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/11/20

#include "analyzer/StandardAnalyzer.h"

#include "utility/Config.h"

using namespace firtex::utility;

namespace firtex
{
	namespace analyzer
	{		
		CStandardAnalyzer::CStandardAnalyzer(const char *sDicName)			
		{
			m_pData=0;
			m_nLength=0;
			m_nLowerBound=0;

			m_LowerChar=_CHARSET_SIZE;
			m_UpperChar=0;

			m_nItemCount=0;
			
			if( load(sDicName) == false)
				FIRTEX_THROW3(FILEIO_ERROR,_T("CStandardAnalyzer:load dictionary failed."));
		}

		CStandardAnalyzer::CStandardAnalyzer()			
		{
			m_pData=0;
			m_nLength=0;
			m_nLowerBound=0;

			m_LowerChar=_CHARSET_SIZE;
			m_UpperChar=0;
		
			m_nItemCount=0;
			tstring dict = GlobalConfig.General.Path + _T("//data//coredict.pdat");
			if( load(dict.c_str()) == false)
				FIRTEX_THROW3(FILEIO_ERROR,_T("CStandardAnalyzer:load dictionary failed."));
		}

		CStandardAnalyzer::~CStandardAnalyzer(void)
		{
			if (m_pData!=0)
			{
				free(m_pData);
				m_pData=0;
			}			
		}

		CTokens* CStandardAnalyzer::nextTokensInternal(CTokens* pUnAnalyzedTokens)
		{
			int32_t i = 0,nWordLen = 0,nCharLen = 0,j = 0,nStart = 0;
			size_t nLen;
			char* sLine = NULL;
			int32_t check = -2,base = 0,code,nPos;
			int32_t nWordHandle = 0;
			int32_t nPositionIncrement;
			termid_t* tokenBuff = (termid_t*)getTokenVector()->getTokenBuffer();
			CTokenXs<termid_t>* pTokenXs = (CTokenXs<termid_t>*)getTokenVector()->asTokensX();			
#if 0
			char buff[100];
			FILE* f = fopen("nextTokens.txt","wb");
#endif

#define INIT_STATE() \
	base = 0;\
	check = -2;\
	nWordLen = 0;\
	nStart = i;\
	nWordHandle = 0;

#define ADD_TOKEN(w)\
	if(!pTokenXs->addTokenX(w,nPositionIncrement,nStart,nStart + nWordLen))\
			return pTokenXs;


			CToken* pToken;
			pUnAnalyzedTokens->startIterator();
			while(pUnAnalyzedTokens->hasNext())
			{				
				pToken = pUnAnalyzedTokens->next();
				sLine = (char*)pToken->textValue();
				nLen = pToken->getTextLength();
				nPositionIncrement = pToken->getPositionIncrement();
				i = 0;

				while( (i < (int32_t)nLen) ||  ( nStart < ((int32_t)nLen - 1) && nWordLen > 0) )					
				{
					if (i  >= (int32_t)nLen)  //( (nStart-buffbase) < ((int32_t)nLen-1) && nWordLen>0)
					{//Not complete word in the last part						

						ADD_TOKEN(nWordHandle);

#if 0						
						strncpy(buff,sLine + nStart,nWordLen);
						buff[nWordLen] = 0;
						fprintf(f,"%s ",buff);						
#endif

						i = nStart + nWordLen;

						INIT_STATE();

						continue;
					}

					if ( (sLine[i] > 0) || ( i + 1 == nLen))
					{//Single byte character
						code = (uint8_t)sLine[i];
						nCharLen = 1;//Character Length
					}
					else//2-byte character
					{
						//code = 256*(uint8_t)sLine[i - buffbase] + (uint8_t)sLine[i+1-buffbase];//Get code
						code = ((uint32_t)((uint8_t)sLine[i]) << 8) + (uint8_t)sLine[i + 1];//Get code
						nCharLen = 2;//Character Length					
					}
					i += nCharLen;

					if (m_charset[code] == -1)
					{//Invalid Character
						if (nWordLen > 0)
						{							
							if( nWordHandle != 0 || (j > 0 && (tokenBuff[j - 1] != 0) ))
							{
								ADD_TOKEN(nWordHandle);
#if 0								
								strncpy(buff,sLine + nStart,nWordLen);
								buff[nWordLen] = 0;								
								fprintf(f,"%s ",buff);									
#endif
								j++;								
							}
							i = nStart + nWordLen;//added 06.5.12
						}	
						else 
						{
							if( j > 0 && (tokenBuff[j - 1] != 0) )
							{
								if(code != ' ')
								{
									ADD_TOKEN(0)
									j++;
								}

#if 0
								fprintf(f,"%#d ",0);
#endif															
							}
						}						

						INIT_STATE();
						continue;
					}

					nPos = base + m_charset[code];//current position
					if (nPos > m_nLowerBound || m_pData[nPos].check != check)
					{//Not exists
						if (nWordLen > 0)
						{//Have a word							
							if( nWordHandle != 0 || (j > 0 && (tokenBuff[j - 1] != 0) ))
							{
								ADD_TOKEN(nWordHandle);
#if 0
								strncpy(buff,sLine + nStart,nWordLen);
								buff[nWordLen] = 0;								
								fprintf(f,"%s ",buff);								
#endif
								j++;								
							}
							i = nStart + nWordLen;//Back off
						}
						else
						{//First Character, not exists							
							if( (j > 0 && (tokenBuff[j - 1] != 0) ))
							{
								ADD_TOKEN(0);
#if 0
								fprintf(f,"%#d ",0);
#endif
								j++;
							}
						}
						
						INIT_STATE();

						continue;
					}		

					if (m_pData[nPos].base < 0)
					{
						check = nPos;

						base = -m_pData[nPos].base;
						nWordLen = i - nStart;
						nWordHandle = m_pData[nPos].handle;//Record Handle

						if (base == nPos)//Leaf
						{
							//if (j==0&&lastID!=0||j>0&&m_pResultID[j-1]!=0||nWordHandle!=0)
							if( nWordHandle != 0 || (j>0 && (tokenBuff[j-1] != 0) ))
							{
								ADD_TOKEN(nWordHandle);
#if 0								
								strncpy(buff,sLine + nStart,nWordLen);
								buff[nWordLen] = 0;								
								fprintf(f,"%s ",buff);							
#endif
								j++;
								//m_pResultID[j++]=(unsigned short)nWordHandle;
							}

							INIT_STATE();

							continue;
						}
					}
					else
					{
						base = m_pData[nPos].base;
						if (nWordLen == 0)//Single Char being a word
						{
							nWordLen = nCharLen;
						}
						check = nPos;
					}
				}//end while
				
			};		
#if 0
			fclose(f);
#endif
			return pTokenXs;
		}

		wordid_t CStandardAnalyzer::getWordId(const char *cWord)
		{
			size_t len = strlen(cWord);
			size_t i = 0;
			int32_t nPos=-1,base=0,code,check=-2;

			for (i=0;i<len;i++)
			{
				if (cWord[i] > 0 || i+1 == len)
				{//Single byte character
					code = (uint8_t)cWord[i];
				}
				else//2-byte character
				{
					//code = 256*(uint8_t)cWord[i] + (uint8_t)cWord[i+1];//Get code
					code = (((uint32_t)((uint8_t)cWord[i])) << 8) + (uint8_t)cWord[i+1];//Get code
					i++;
				}
				if (code<0||m_charset[code]==-1)
				{//Invalid Character
					return -1;
				}
				nPos=base+m_charset[code];//current position
				if (nPos>m_nLowerBound||m_pData[nPos].check!=check)
				{//Not exists
					return -1;
				}		
				check = nPos;

				base = abs(m_pData[nPos].base);
			}

			if (nPos>m_nLowerBound||m_pData[nPos].base<0)
			{//Reach the last character
				return m_pData[nPos].handle;
			}
			return -1;			
		}	
		
		/*********************************************************************
		*
		*  Func Name  : load
		*
		*  Description: load dictionary from file
		*
		*  Parameters : sFilename: file name
		*               
		*  Returns    : true or false
		*
		*  Author     :   
		*  History    : 
		*              1.create 16:8:2005
		*********************************************************************/
		bool CStandardAnalyzer::load(const tchar *sFilename)
		{
			FILE *fp;
			fp=_tfopen(sFilename,_T("rb"));
			if (fp==NULL)
			{//Open file fail.
				return false;
			}
			fread(m_charset,_CHARSET_SIZE,sizeof(int32_t),fp);
			//Read charset
			fread(&m_nLowerBound,1,sizeof(int32_t),fp);
			//read lower bound
			fread(&m_nItemCount,1,sizeof(int32_t),fp);
			//read Item Count

			if (m_pData)
			{
				free(m_pData);
			}
			m_nLength=m_nLowerBound+1;
			m_pData=0;
			m_pData=(PSTATE)malloc(sizeof(STATE)*m_nLength);
			fread(m_pData,m_nLength,sizeof(STATE),fp);
			//read data

			fclose(fp);
			return true;
		}		
		CTokens* CStandardAnalyzer::newTokenVector()
		{
			CTokens* pTokens = new CTokenXs<termid_t>(getMaxTokensToAnalyze());
			pTokens->setType(CToken::TERMID);
			return pTokens;
		};
		const tchar* CStandardAnalyzer::acceptableEncoding()
		{
			return _T("GBK");			
		}	
	}
}

