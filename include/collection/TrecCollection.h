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
// Created	: 2006/8/2
//

#ifndef _TRECCOLLECTION_H
#define _TRECCOLLECTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Collection.h"
#include "../utility/StringUtils.h"

using namespace firtex::string_utils;

#define DOC_TAG		0

namespace firtex
{
	namespace collection
	{

#define SKIP_BLANK(p)\
	while(*p == '\n' || *p == '\r' || *p == ' ' || *p == '\t')\
	p++;

		/**
		 * Collection, parse TREC collection.
		 */
		class CTrecCollection :	public CCollection
		{
		public:		
			class Creator : public CCollection::Creator
			{
			protected:
				Creator()
				{
					m_identifier += _T(".trec");
				}
			public:
				~Creator(){}
			public:
				CCollection* create(const tchar* path){return new CTrecCollection(path);}

			public:
				static Creator* getInstance()
				{
					static CTrecCollection::Creator INSTANCE;
					return &INSTANCE;
				}
			};
		protected:
			class CTrecTagPair
			{
			public:
				CTrecTagPair(schemaid_t sid,const char* tag)
					:schemaid(sid)
				{
					tagLen = (int32_t)strlen(tag);
					beginTag = new char[tagLen + 3];
					beginTag[0] = '<';
					strcpy(beginTag + 1,tag);
					beginTag[tagLen + 1] = '>';
					beginTag[tagLen + 2] = 0;
					endTag = new char[strlen(tag) + 4];
					strcpy(endTag + 2,tag);
					endTag[0] = '<';
					endTag[1] = '/';
					endTag[tagLen + 2] = '>';
					endTag[tagLen + 3] = 0;
				}
				~CTrecTagPair()
				{
					delete[] beginTag;
					delete[] endTag;
				}
			public:
				schemaid_t	schemaid;
				char*		beginTag;
				char*		endTag;
				int32_t		tagLen;
			};
		public:
			/**
			 * constructor
			 * @param dir directory of documents
			 */
			CTrecCollection(const tchar* dir);

			/**
			 * constructor
			 * @param dir directory of documents
			 * @param pIndexWriter index writer object
			 */
			CTrecCollection(const tchar* dir,CIndexWriter* pWriter);
			virtual ~CTrecCollection(void);
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
			 * @param filename paht of document
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
			 * parse specific tags,such as <DOCNO>doc no</DOCNO>
			 * @param start start address of text
			 * @param end end address of text
			 * @param vale store the start address of tag's value
			 * @param valueLen length of value
			 * @param whether skip this value or not
			 * @return parse success or not
			 */
			inline bool		processTag(char*& start,char* end,CTrecTagPair* p,char*& value,size_t& valueLen,bool skip);

			/**
			 * enumerate directory and index files in it
			 * @param dir directory path
			 */
			virtual void	indexDocs(const tchar* dir);
		protected:			
			tstring			m_sDirectory;

			tstring			m_sDocTag;			///tag of document
			size_t			m_nDocTagLen;		///length of tag
			int32_t			m_tagDOCHDR;		///offset of DOCHDR in tag array
			int32_t			m_tagTEXT;			///offset of TEXT in tag array			

			CTrecTagPair**	m_ppTags;				///tag array
			size_t			m_nNumTags;			///number of tags

			char*			m_contentBuffer;	///buffer for reading
			size_t			m_length;			///length of buffer
		};
		//////////////////////////////////////////////////////////////////////////
		//		
		inline bool CTrecCollection::processTag(char*& start,char* end,CTrecTagPair* p,char*& value,size_t& valueLen,bool skip)
		{
			SKIP_BLANK(start);

			start = firtex::string_utils::fx_strstr_rang(start,end,p->beginTag);
			if(!start)
			{
				start = end;
				return false;
			}	
			char* eTag = firtex::string_utils::fx_strstr_rang(start,end,p->endTag);
			if(eTag == NULL)
			{
				//FIRTEX_CLOG(level::warn) << "can't find end tag : " << p->endTag<<FIRTEX_ENDL;
				do 
				{
					start += p->tagLen + 3;					
					eTag = firtex::string_utils::fx_strstr_rang(start,end,p->endTag);
				} while(start < end && !eTag);
				if(eTag == NULL)
					return false;
			}	
			char* bTag = (start + p->tagLen + 2);
			if(!skip)
			{
				value = bTag;
				valueLen = eTag - bTag;				
			}			
			start = eTag + p->tagLen + 3;
			return true;
		}		

		inline void CTrecCollection::parseFile(const tchar* filename)
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
			if(length > m_length)
			{
				if(m_contentBuffer)
					delete[] m_contentBuffer;
				m_contentBuffer = new char[length + 1];
				m_length = length;
			}

			///read content
			::fileRead(fileHandle,m_contentBuffer,(unsigned int)length);
			::fileClose(fileHandle);
			m_contentBuffer[length] = 0;
			parseBuffer(m_contentBuffer,length);
		}	
	}
}


#endif
