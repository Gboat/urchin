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
// Created	: 2007/2/1
//


#ifndef _TWCOLLECTION_H
#define _TWCOLLECTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Collection.h"
#include "../utility/StringUtils.h"

using namespace firtex::string_utils;

#define DOC_TAG			0
#define MAX_READ_BUFF	100000000

namespace firtex
{
	namespace collection
	{

#define TW_SKIP_BLANK(p)\
	while(*p == '\n' || *p == ' ' || *p == '\t')\
	p++;

#define TW_SKIP_LINE(p)\
	while(*p != '\n') p++;\
	p++;

		/**
		 * Collection, parse TianWang(ÌìÍø) collection.(Chinese Web Test collection)
		 */
		class CTWCollection :	public CCollection
		{
		public:		
			class Creator : public CCollection::Creator
			{
			public:
				Creator()
				{
					m_identifier += _T(".tw");
				}
				~Creator(){}
			public:
				CCollection* create(const tchar* path){return new CTWCollection(path);}

			public:
				static Creator* getInstance()
				{
					static CTWCollection::Creator INSTANCE;
					return &INSTANCE;
				}
			};
		protected:
			class CTWTag
			{
			public:
				CTWTag(schemaid_t sid,const char* tag)
					:schemaid(sid)
				{
					nTagLen = (int32_t)fx_strlen(tag);
					pTag = new char[nTagLen + 2];
					strcpy(pTag,tag);
					pTag[nTagLen] = ':';
					pTag[nTagLen + 1] = 0;					
				}
				~CTWTag()
				{
					delete[] pTag;					
				}
			public:
				schemaid_t	schemaid;
				char*		pTag;				
				int32_t		nTagLen;
			};
		public:
			/**
			 * constructor
			 * @param dir directory of documents
			 */
			CTWCollection(const tchar* dir);

			/**
			 * constructor
			 * @param dir directory of documents
			 * @param pIndexWriter index writer object
			 */
			CTWCollection(const tchar* dir,CIndexWriter* pWriter);
			virtual ~CTWCollection(void);
		public:
			/**
			 * scan collection,sub class must implement it			
			 */
			void			scanInternal();

			/** 
			 * define schema of collection
			 * @param pSchema schema to define 
			 */
			void			defineSchema(CDocumentSchema* pSchema);

			/**
			 * parse one document
			 * @param filename path of document
			 */
			inline void		parseFile(const tchar* filename);

			/**
			 * parse buffer of TREC document
			 * @param buffer buffer of TREC document
			 * @param bufSize size of buffer
			 */
			void			parseBuffer(char* buffer,size_t bufSize);

			/**
			 * set parameter for collection
			 * @param pszParam parameter for collection. format:param_name=param_value
			 */
			void			setParam(const tchar* pszParam);

			/**
			 * get document tag which used to identify documents, for standard TREC corpus, it is always "DOC"
			 * @return tag of document
			 */
			const tchar*	getDocTag(){return m_sDocTag.c_str();}
		protected:
			/**
			 * parse specific tags,such as version:
			 * @param start start address of text
			 * @param end end address of text
			 * @param vale store the start address of tag's value
			 * @param valueLen length of value
			 * @param whether skip this value or not
			 * @return parse success or not
			 */
			inline bool		processTag(char*& start,char* end,CTWTag* p,char*& value,size_t& valueLen,bool skip);			

			/** 
			 * skip header
			 * @param pStart begin of html buffer
			 * @param pEnd end of html buffer
			 */
			inline size_t skipHeader(char* pStart,char* pEnd);

			/**
			 * enumerate directory and index files in it
			 * @param dir directory path
			 */
			virtual void	indexDocs(const tchar* dir);
		protected:			
			tstring			m_sDirectory;

			tstring			m_sDocTag;			///tag of document			

			CTWTag**		m_ppTags;			///tag array
			int32_t			m_numTags;			///number of tags

			bool			m_bZipped;

			char*			m_pContentBuffer;	///buffer for reading
			size_t			m_nBufSize;			///length of buffer

			char*			m_pUnzipBuffer;
			size_t			m_nUnzipBufSize;
		};
		//////////////////////////////////////////////////////////////////////////
		//		
		inline bool CTWCollection::processTag(char*& start,char* end,CTWTag* p,char*& value,size_t& valueLen,bool skip)
		{			
			TW_SKIP_BLANK(start);
			//if(strncmp(start,p->beginTag,p->nTagLen+2))
			if(fx_strncmp(start,p->pTag,p->nTagLen + 1))
			{
				do
				{
					TW_SKIP_LINE(start);
				}while (fx_strncmp(start,p->pTag,p->nTagLen + 1) && start < end);
				
				if(start >= end)
					return false;
			}	

			//char* eTag = strstr(start,p->endTag);
			char* eTag = fx_strchr(start,'\n');
			if(eTag == NULL)
			{
				//FIRTEX_CLOG(level::warn) << "can't find end tag : " << p->endTag<<FIRTEX_ENDL;
				start += p->nTagLen + 1;
				do 
				{
					start++;
					//eTag = strstr(start,p->endTag);
					eTag = fx_strchr(start,'\n');
				} while(start < end && !eTag);
				if(eTag == NULL)
					return false;
			}	
			char* bTag = (start + p->nTagLen + 1);
			if(!skip)
			{
				value = bTag;
				valueLen = eTag - bTag;
			}			
			start = eTag + 1;
			return true;
		}		
		inline void CTWCollection::parseFile(const tchar* filename)
		{
			if(!filename || !(*filename))
				return ;            			
			int32_t fileHandle = ::fileOpen(filename, O_BINARY | O_RDONLY | O_RANDOM, _S_IREAD );
			if (fileHandle < 0)
			{
				FIRTEX_CLOG(level::warn) << _T("Open file error:") << filename << FIRTEX_ENDL;
				return ;
			}			
			size_t length = (size_t)fileSize(fileHandle);
			if(length > m_nBufSize)
			{
				if(length > MAX_READ_BUFF)
				{
					if(m_pContentBuffer)
						delete[] m_pContentBuffer;
					m_nBufSize = MAX_READ_BUFF;
					m_pContentBuffer = new char[m_nBufSize + 1];

					char* pLastDoc = NULL;
					char* pReadB = m_pContentBuffer;
					size_t nLeft;
					size_t nReaded = ::fileRead(fileHandle,pReadB,(unsigned int)m_nBufSize);
					while (nReaded > 0)
					{
						char* pLastDoc = pReadB + nReaded;
						///find last "version:"
						do
						{
							pLastDoc--;
							while (*pLastDoc != ':')
								pLastDoc--;
						}while(fx_strncmp(pLastDoc - 7,"version:",8));
						pLastDoc -= 7;
						nLeft = pReadB + nReaded - pLastDoc;
						m_pContentBuffer[pLastDoc - m_pContentBuffer] = 0;
						parseBuffer(m_pContentBuffer,pLastDoc - m_pContentBuffer);
						memcpy(m_pContentBuffer,pLastDoc,nLeft);
						pReadB = m_pContentBuffer + nLeft;
						nReaded = ::fileRead(fileHandle,pReadB,(unsigned int)m_nBufSize - nLeft);
					}	
					::fileClose(fileHandle);
					return;
				}
				else
				{
					if(m_pContentBuffer)
						delete[] m_pContentBuffer;
					m_pContentBuffer = new char[length + 1];
					m_nBufSize = length;
				}				
			}

			///read content
			::fileRead(fileHandle,m_pContentBuffer,(unsigned int)length);
			::fileClose(fileHandle);
			m_pContentBuffer[length] = 0;
			parseBuffer(m_pContentBuffer,length);
		}	
		inline size_t CTWCollection::skipHeader(char* pStart,char* pEnd)
		{			
			char* p = pStart;
			p--;
			do 
			{
				if(p >= (pEnd - 6))
					return (pEnd - pStart);
				p++;
				while (*p != '<' && (p < (pEnd - 6)))
				{
					p++;
				}
			} while(fx_strncasecmp(p,"<html>",6));		

			return (p - pStart);
		}
	}
}

#endif
