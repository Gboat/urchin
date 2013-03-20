//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/7/24
//

#include "analyzer/DateTimeAnalyzer.h"

namespace firtex
{
	namespace analyzer
	{

#define STR_DIGIT_SEEK(pszCur, pszEnd, cnt, seek_to)				\
	while (pszCur != pszEnd && isdigit(*pszCur))					\
		{																\
		++cnt;														\
		++pszCur;													\
		if (cnt == seek_to)											\
		{															\
		break;													\
		}															\
		}

#define IS_SEP2(pszCur, offset, cSep, szSep)	\
	( (offset=1 && *pszCur == cSep) || ( offset=2 && strncmp(pszCur,szSep, strlen(szSep)) == 0 ) ) 

#define IS_SEPS(pszCur, szSep)	\
	( strncmp(pszCur,szSep, strlen(szSep)) == 0 )

#define IS_SEP2C(pszCur, cSep1, cSep2)	\
	((*pszCur == cSep1) || (*pszCur == cSep2))

#define IS_SEP3(pszCur, offset, cSep1, cSep2, szSep)	\
	( ((offset=1) && ((*pszCur == cSep1) || (*pszCur == cSep2))) || (offset=2 && strncmp(pszCur,szSep, strlen(szSep)) == 0) )

#define INC_POS()		\
	++pszCur;			\
	--remain;

#define GET_NUMBER(num)			\
	*pszCur = '\0';				\
	num = atoi(pszTag);			\
	pszTag = pszCur + offset;	\
	pszCur = pszTag;			\
	remain -= offset;
		
		CDateTimeAnalyzer::CDateTimeAnalyzer(void)			
		{
			setMaxTokensToAnalyze(1);
		}

		CDateTimeAnalyzer::~CDateTimeAnalyzer(void)
		{
		}

		CTokens* CDateTimeAnalyzer::newTokenVector()
		{			
			CTokens* pTokens = new CTokenXs<int64_t>(getMaxTokensToAnalyze());
			pTokens->setType(CToken::INT64);
			return pTokens;
		}
		CTokens* CDateTimeAnalyzer::nextTokensInternal(CTokens* pUnAnalyzedTokens)
		{
			char* pText = pUnAnalyzedTokens->getTokenBuffer();
			size_t length = pUnAnalyzedTokens->getTokenBufferSize();
			
			CTokenXs<int64_t>* pTokenXs = (CTokenXs<int64_t>*)getTokenVector()->asTokensX();
			int64_t dt = parse((const tchar*)pText);
			if(dt != -1)
				pTokenXs->addTokenX(dt);
			return pTokenXs;
		}			

		int64_t CDateTimeAnalyzer::parse(const tchar* str)
		{			
			//TODO:
			/*struct tm t;
			_stscanf(str,"%d-%d-%d %d:%d:%d",&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec);
			t.tm_year-=1900;
			t.tm_mon-=1;
			return (int64_t)_mktime64(&t);
			return -1;*/
			DateTime dt;
			memset(&dt,0,sizeof(dt));
			tstring s = str;
			string_utils::trim(s);
			if(checkDate(s.c_str(),dt) == false)
				return -1;
			struct tm t;
			t.tm_year = dt.year - 1900;
			t.tm_mon = dt.month - 1;
			t.tm_mday = dt.day;
			t.tm_hour = dt.hour;
			t.tm_min = dt.minute;
			t.tm_sec = dt.second;
#ifdef WIN32
			return (int64_t)_mktime64(&t);
#else 
			return (int64_t)mktime(&t);
#endif
			
		}

		bool CDateTimeAnalyzer::checkDate(const char* pszDate, DateTime &dt)
		{
			if (!pszDate)
			{
				return false;
			}
			if (strlen(pszDate)<4 || (*pszDate & 0x80))
			{
				return false;
			}

			char *pszTag, *pszEnd, *pszCur;
			int cnt = 0;
			int offset = 0, remain;

			remain = (int)strlen(pszDate);
			pszEnd = (char*)pszDate + remain;
			pszCur = (char*)pszDate;
			pszTag = (char*)pszDate;

			//check year
			STR_DIGIT_SEEK(pszCur, pszEnd, cnt, 4);
			/*while (pszCur != pszEnd && isdigit(*pszCur))
			{
			++cnt;
			++pszCur;
			if (cnt == 4)
			{
			break;
			}
			}*/

			if (cnt < 4)
			{
				return false;
			}

			if (pszCur == pszEnd)
			{
				//only year, is it ok?
				return false;	
			}

			if (IS_SEP2C(pszCur,  '/', '-'))
			{
				offset = 1;
			}									
			else									
			{										
				if ( remain>1 && IS_SEPS(pszCur, "年"))
				{									
					offset = 2;
				}
				else
				{
					return false;
				}
			}

			GET_NUMBER(dt.year)
				remain -= 4;

			//chek month
			if (remain < 1 || !isdigit(*pszTag))
			{
				//or may be just return year
				//return false;
				return true;
			}

			offset = 0;
			if (*pszTag == '0')
			{
				if (remain < 2)
				{
					//return false;
					return true;
				}
				++pszTag;
				--remain;
				pszCur = pszTag;
				if (*pszTag != '0' && isdigit(*pszTag))
				{
					INC_POS();
					if (IS_SEP2C(pszCur,  '/', '-'))
					{
						offset = 1;
					}									
					else									
					{										
						if ( remain>1 && IS_SEPS(pszCur, "月"))
						{									
							offset = 2;
						}
						else
						{
							//return false;
							return true;
						}
					}
				}
				else
				{
					if (IS_SEP2C(pszCur,  '/', '-'))
					{
						offset = 1;
					}									
					else									
					{										
						if ( remain>1 && IS_SEPS(pszCur, "月"))
						{									
							offset = 2;
						}
						else
						{
							//return false;
							return true;
						}
					}
				}
			}
			else if (*pszTag == '1')
			{
				INC_POS()
					if ((*pszCur == '0' || *pszCur == '1' || *pszCur == '2'))
					{
						INC_POS()
							if (IS_SEP2C(pszCur,  '/', '-'))
							{
								offset = 1;
							}									
							else									
							{										
								if ( remain>1 && IS_SEPS(pszCur, "月"))
								{									
									offset = 2;
								}
								else
								{
									//return false;
									return true;
								}
							}
					}
					else
					{
						if (IS_SEP2C(pszCur,  '/', '-'))
						{
							offset = 1;
						}									
						else									
						{										
							if ( remain>1 && IS_SEPS(pszCur, "月"))
							{									
								offset = 2;
							}
							else
							{
								//return false;
								return true;
							}
						}
					}
			}
			else
			{
				INC_POS()
					if (IS_SEP2C(pszCur,  '/', '-'))
					{
						offset = 1;
					}									
					else									
					{										
						if ( remain>1 && IS_SEPS(pszCur, "月"))
						{									
							offset = 2;
						}
						else
						{
							//return false;
							return true;
						}
					}
			}

			GET_NUMBER(dt.month)

				//check day
				//note: doesn't check legal day, just check the number
				if (remain < 1 || !isdigit(*pszTag))
				{
					//or may be just return year month
					//return false;
					return true;
				}

				offset = 0;
				if (*pszTag == '0')
				{
					if (remain < 2)
					{
						//return false;
						return true;
					}
					++pszTag;
					--remain;
					pszCur = pszTag;
					if (*pszTag != '0' && isdigit(*pszTag))
					{
						INC_POS();
						if (*pszCur == ' ')
						{
							offset = 1;
						}									
						else									
						{										
							if ( remain>1 && IS_SEPS(pszCur, "日"))
							{									
								offset = 2;
							}
							else
							{
								//return false;
								return true;
							}
						}
					}
					else
					{
						if (*pszCur == ' ')
						{
							offset = 1;
						}									
						else									
						{										
							if ( remain>1 && IS_SEPS(pszCur, "日"))
							{									
								offset = 2;
							}
							else
							{
								//return false;
								return true;
							}
						}
					}
				}
				else if (*pszTag == '3')
				{
					INC_POS()
						if ((*pszCur == '0' || *pszCur == '1') )
						{
							INC_POS()
								if (*pszCur == ' ')
								{
									offset = 1;
								}									
								else									
								{										
									if ( remain>1 && IS_SEPS(pszCur, "日"))
									{									
										offset = 2;
									}
									else
									{
										//return false;
										return true;
									}
								}
						}
						else
						{
							if (*pszCur == ' ')
							{
								offset = 1;
							}									
							else									
							{										
								if ( remain>1 && IS_SEPS(pszCur, "日"))
								{									
									offset = 2;
								}
								else
								{
									//return false;
									return true;
								}
							}
						}
				}
				else if (*pszTag == '1' || *pszTag == '2')
				{
					INC_POS()
						if (isdigit(*pszCur))
						{
							INC_POS()
						}
						if (*pszCur == ' ')
						{
							offset = 1;
						}									
						else									
						{										
							if ( remain>1 && IS_SEPS(pszCur, "日"))
							{									
								offset = 2;
							}
							else
							{
								//return false;
								return true;
							}
						}
				}
				else
				{
					//return false;
					return true;
				}

				GET_NUMBER(dt.day)

					//check hour
					if (remain < 1 || !isdigit(*pszTag))
					{
						//or may be just return year month
						return false;
					}

					offset = 0;
					if (*pszTag == '0')
					{
						if (remain < 2)
						{
							//return false;
							return true;
						}
						++pszTag;
						--remain;
						pszCur = pszTag;
						if (*pszTag != '0' && isdigit(*pszTag))
						{
							INC_POS();
							if (*pszCur == ':')
							{
								offset = 1;
							}									
							else									
							{										
								if ( remain>1 && IS_SEPS(pszCur, "点"))
								{									
									offset = 2;
								}
								else
								{
									//return false;
									return true;
								}
							}
						}
						else
						{
							if (*pszCur == ':')
							{
								offset = 1;
							}									
							else									
							{										
								if ( remain>1 && IS_SEPS(pszCur, "点"))
								{									
									offset = 2;
								}
								else
								{
									//return false;
									return true;
								}
							}
						}
					}
					else if (*pszTag == '2')
					{
						INC_POS()
							if ((*pszCur >= '0' && *pszCur < '4') )
							{
								INC_POS()
									if (*pszCur == ':')
									{
										offset = 1;
									}									
									else									
									{										
										if ( remain>1 && IS_SEPS(pszCur, "点"))
										{									
											offset = 2;
										}
										else
										{
											//return false;
											return true;
										}
									}
							}
							else
							{
								if (*pszCur == ':')
								{
									offset = 1;
								}									
								else									
								{										
									if ( remain>1 && IS_SEPS(pszCur, "点"))
									{									
										offset = 2;
									}
									else
									{
										//return false;
										return true;
									}
								}
							}
					}
					else if (*pszTag == '1')
					{
						INC_POS()
							if (isdigit(*pszCur))
							{
								INC_POS()
							}
							if (*pszCur == ':')
							{
								offset = 1;
							}									
							else									
							{										
								if ( remain>1 && IS_SEPS(pszCur, "点"))
								{									
									offset = 2;
								}
								else
								{
									//return false;
									return true;
								}
							}
					}
					else
					{
						INC_POS()

							if (*pszCur == ':')
							{
								offset = 1;
							}									
							else									
							{										
								if ( remain>1 && IS_SEPS(pszCur, "点"))
								{									
									offset = 2;
								}
								else
								{
									//return false;
									return true;
								}
							}
					}

					GET_NUMBER(dt.hour)

						//check minute
						if (remain < 1 || !isdigit(*pszTag))
						{
							//or may be just return year month day hour
							//return false;
							return true;
						}

						offset = 0;
						if (*pszTag == '0')
						{
							if (remain < 2)
							{
								//return false;
								return true;
							}
							++pszTag;
							--remain;
							pszCur = pszTag;
							if (*pszTag != '0' && isdigit(*pszTag))
							{
								INC_POS();
								if (*pszCur == ':')
								{
									offset = 1;
								}									
								else									
								{										
									if ( remain>1 && IS_SEPS(pszCur, "分"))
									{
										offset = 2;
									}
									else
									{
										//return false;
										return true;
									}
								}
							}
							else
							{
								if (*pszCur == ':')
								{
									offset = 1;
								}									
								else									
								{										
									if ( remain>1 && IS_SEPS(pszCur, "分"))
									{
										offset = 2;
									}
									else
									{
										//return false;
										return true;
									}
								}
							}
						}
						else if (*pszTag > '0' && *pszTag < '6')
						{
							INC_POS()
								if (isdigit(*pszCur))
								{
									INC_POS()
								}
								if (*pszCur == ':')
								{
									offset = 1;
								}									
								else									
								{										
									if ( remain>1 && IS_SEPS(pszCur, "分"))
									{									
										offset = 2;
									}
									else
									{
										//return false;
										return true;
									}
								}
						}
						else
						{
							if (*pszCur == ':')
							{
								offset = 1;
							}									
							else									
							{										
								if ( remain>1 && IS_SEPS(pszCur, "分"))
								{
									offset = 2;
								}
								else
								{
									//return false;
									return true;
								}
							}
						}

						GET_NUMBER(dt.minute)

							//check second
							if (remain < 1 || !isdigit(*pszTag))
							{
								//or may be just return year month day hour minute
								//return false;
								return true;
							}

							dt.second=atoi(pszTag);

							return true;
		}
	}
}

