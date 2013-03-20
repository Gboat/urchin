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
// Created	: 2006/8/5
//
#ifndef _FIRTEXCOLLECTION_H
#define _FIRTEXCOLLECTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Collection.h"
#include "utility/StringUtils.h"

namespace firtex
{
	namespace collection
	{
		class CFirteXCollection : public CCollection
		{
		public:		
			class Creator : public CCollection::Creator
			{
			public:
				Creator()
				{
					m_identifier += _T(".firtex");
				}
				~Creator(){}
			public:
				CCollection* create(const tchar* path){return new CFirteXCollection(path);}
			public:
				static Creator* getInstance()
				{
					static CFirteXCollection::Creator INSTANCE;
					return &INSTANCE;
				}
			};
		public:
			/**
			 * constructor
			 * @param location of FirteX collection			 
			 * @return bufsize size of buffer for reading files
			 */
			CFirteXCollection(const tchar* location,size_t bufsize = 1048576);

			/**
			 * constructor
			 * @param location of FirteX collection
			 * @param pWriter index writer object
			 * @return bufsize size of buffer for reading files
			 */
			CFirteXCollection(const tchar* location,CIndexWriter* pWriter,size_t bufsize = 1048576);

			virtual ~CFirteXCollection(void);
		public:
			/**
			 * get document tag which used to identify documents, for standard TREC corpus, it is always "DOC"
			 * @return tag of document
			 */
			const tchar*	getDocTag(){return m_sDocTag.c_str();}

			/** get collection location */
			const tchar*	getCollection(){return m_sDirectory.c_str();}

		public:
			/**
			 * add a single file to the collection
			 * @param filename the file path
			 */
			void			addFile(const tchar* filename);

			/**
			 * set parameter for collection
			 * @param pszParam parameter for collection. format:param_name=param_value
			 */
			void			setParam(const tchar* pszParam);
		protected:
			class CTagPair
			{
			public:
				enum TAG_TYPE
				{
					NONE		= 0,
					DOCNO,				///<DOCNO> tag 
					MAINFILE,			///<MAINFILE> t
					OFFSET,				///<OFFSET> tag
					LENGTH				///<LENGTH> tag
				};
			public:
				CTagPair(schemaid_t sid,const tchar* tag)
					: schemaid(sid)
					, tagType(CTagPair::NONE)
				{
					tagLen = (int32_t)_tcslen(tag);
					beginTag = new tchar[tagLen + 3];
					beginTag[0] = _T('<');
					_tcscpy(beginTag + 1,tag);
					beginTag[tagLen + 1] = _T('>');
					beginTag[tagLen + 2] = 0;
					endTag = new tchar[tagLen + 4];
					_tcscpy(endTag + 2,tag);
					endTag[0] = _T('<');
					endTag[1] = _T('/');
					endTag[tagLen + 2] = _T('>');
					endTag[tagLen + 3] = 0;
				}
				~CTagPair()
				{
					delete[] beginTag;
					delete[] endTag;
				}
			public:
				schemaid_t	schemaid;
				tchar*		beginTag;
				tchar*		endTag;
				int32_t		tagLen;
				TAG_TYPE	tagType;
			};
		protected:
			/**
			 * scan collection, find files and index them.
			 */
			void		scanInternal();

			/** 
			 * 定义Collection的Schema
			 * @param pSchema 输入参数，用于定义Collection的Schema
			 */
			void		defineSchema(CDocumentSchema* pSchema);
		protected:
			void		indexDocs(const tchar* directory);
			void		readIndexFile(const tchar* indexfile);

			/**
			 * parse specific tags,such as <DOCNO>doc no</DOCNO>
			 * @param start start address of text
			 * @param end end address of text
			 * @param vale store the start address of tag's value
			 * @param valueLen length of value
			 * @param whether skip this value or not
			 * @return parse success or not
			 */
			inline bool		processTag(char*& start,char* end,CTagPair* p,char*& value,size_t& valueLen,bool skip);		
		private:
			tstring		m_sDirectory;

			tstring		m_sDocTag;
			size_t		m_nDocTagLen;		///length of tag

			CTagPair**	m_ppTags;			///tags of collection
			int32_t		m_nNumTags;			///number of tags

			char*		m_buffer;			///buffer for reading files
			size_t		m_bufsize;			///size of buffer	
		};
		//////////////////////////////////////////////////////////////////////////

#define FXC_SKIP_BLANK(p)\
	while(*p == '\n' || *p == '\r' || *p == ' ' || *p == '\t')\
	p++;
		
		inline bool CFirteXCollection::processTag(char*& start,char* end,CTagPair* p,char*& value,size_t& valueLen,bool skip)
		{			
			FXC_SKIP_BLANK(start);

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
	}
}

#endif
