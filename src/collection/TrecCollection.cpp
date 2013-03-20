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
#include "collection/TrecCollection.h"
#include "utility/dirent.h"
#include "index/IndexWriter.h"

#include <memory>

using namespace std;

using namespace firtex::utility;

#define DOC_MAXLENGTH 10000000

namespace firtex
{
	namespace collection
	{	
		CTrecCollection::CTrecCollection(const tchar* dir)			
			:m_sDirectory(dir)			
			,m_ppTags(NULL)
			,m_contentBuffer(NULL)
			,m_length(0)
			,m_tagDOCHDR(-1)
			,m_tagTEXT(-1)
			,m_sDocTag(_T("DOC"))
			,m_nDocTagLen(3)
			,m_nNumTags(0)
		{			
		}

		CTrecCollection::CTrecCollection(const tchar* dir,CIndexWriter* pWriter)
			:CCollection(pWriter)
			,m_sDirectory(dir)			
			,m_ppTags(NULL)
			,m_contentBuffer(NULL)
			,m_length(0)
			,m_tagDOCHDR(-1)
			,m_tagTEXT(-1)
			,m_sDocTag(_T("DOC"))
			,m_nDocTagLen(3)
			,m_nNumTags(0)
		{
		}

		CTrecCollection::~CTrecCollection(void)
		{
			if(m_contentBuffer)
			{
				delete[] m_contentBuffer;
				m_contentBuffer = NULL;
			}
			for(size_t i = 0;i<m_nNumTags;i++)
			{
				delete m_ppTags[i];
			}
			delete[] m_ppTags;
			m_ppTags = NULL;
		}

		void CTrecCollection::scanInternal()
		{			
			indexDocs(m_sDirectory.c_str());			
		}
		void CTrecCollection::indexDocs(const tchar* directory)
		{
			DIR* dir = opendir(directory);
			if ( dir != NULL )
			{
				struct dirent* fl;

				struct fileStat buf;
				char path[FX_MAX_PATH];
				_tcscpy(path,directory);
				_tcscat(path,PATH_DELIMITERA);
				tchar* pathP = path + _tcslen(path);
				fl = readdir(dir);
				while ( fl != NULL )
				{
					if ( (_tcscmp(fl->d_name, ".")) && (_tcscmp(fl->d_name, "..")) ) 
					{
						pathP[0]=0;
						strcat(pathP,fl->d_name);
						int32_t ret = fileStat(path,&buf);
						if ( buf.st_mode & S_IFDIR ) 
						{
							indexDocs(path );
						}
						else
						{
							if(stopRequested())
								break;
							FIRTEX_CLOG(level::info) << _T("Processing file: ") << path <<FIRTEX_ENDL;
							parseFile(path);						
						}
					}
					fl = readdir(dir);
				}
				closedir(dir);
			}
			else
			{
				FIRTEX_CLOG(level::warn) << _T("Open directory failed: ") << directory << FIRTEX_ENDL;
			}
		}		

		void CTrecCollection::parseBuffer(char* buffer,size_t bufSize)
		{				
			try
			{
				firtex::index::CIndexWriter* pIndexWriter = getWriter();
				firtex::document::CDocument* pDoc = NULL;				

				char*	pStart = buffer;
				char*	pEnd = buffer + bufSize;
				char*	text = NULL;
				size_t	textLen = 0;
				char*	contentBuffer = buffer;
				char*	pDocTagB;
				char*	pDocTagE = pStart;
				CTrecTagPair* p;
				while (pStart < (contentBuffer + bufSize) && stopRequested()==false)
				{		
					p = m_ppTags[DOC_TAG];
					pDocTagB = pStart;
					pStart = pDocTagE;
					pDocTagB = pStart;
					if(processTag(pStart,pEnd,p,text,textLen,true) == false)///find <DOC>
					{
						pStart += m_ppTags[DOC_TAG]->tagLen + 2;
						continue;///some problem
					}
					
					SKIP_BLANK(pDocTagB);

					pDocTagE = pStart;						///save the end of </DOC>
					pStart = pDocTagB + m_nDocTagLen + 2;	///set to the end of <DOC>					

					if(pDocTagE - pStart > DOC_MAXLENGTH)	///too long document, skip it
					{
						pStart = pDocTagE;
						continue;
					}

					pDoc = pIndexWriter->getDoc();
					for(size_t i = 1;i < m_nNumTags;i++)
					{
						p = m_ppTags[i];
						if(processTag(pStart,pDocTagE,p,text,textLen,(m_ppTags[i]->schemaid == -1)) == false)
						{
							SKIP_BLANK(pStart);
							pIndexWriter->ungetDoc();
							break;
						}		

						SKIP_BLANK(pStart);

						if(i == m_nNumTags - 1)///last tag
						{													
							if((size_t)(pDocTagE - pStart) > m_nDocTagLen + 3)
								pDoc->getInput()->setBufferInput(0,-1,pStart,pDocTagE - pStart - m_sDocTag.length() - 3);
						}
						if(p->schemaid != -1)
						{						
							pDoc->addField(p->schemaid,text,textLen,true);
						}						
					}										
				}													
				pIndexWriter->flushDocuments();
			}
			catch (CFirteXException& e)
			{
				FIRTEX_CLOG(level::err) << _T("CTrecCollection::parseBuffer() failed:") << e.what() <<FIRTEX_ENDL;
			}
		}
		
		void CTrecCollection::defineSchema(CDocumentSchema* pSchema)
		{	
			CSchemaItem* pSchemaItem;
			if(pSchema->getCount() > 0)
			{
				m_nNumTags = pSchema->getCount() + 1;
				m_ppTags = new CTrecTagPair*[m_nNumTags];
				m_ppTags[0] = new CTrecTagPair(-1,m_sDocTag.c_str());
				int32_t i = 1;
				pSchema->startIterator();
				while (pSchema->hasNext())
				{
					pSchemaItem = pSchema->next();
					m_ppTags[i] = new CTrecTagPair(pSchemaItem->getId(),pSchemaItem->getName());
					i++;
				}
			}
			else
			{
				m_nNumTags = 3;
				m_ppTags = new CTrecTagPair*[m_nNumTags];
				m_ppTags[0] = new CTrecTagPair(-1,m_sDocTag.c_str());
				m_ppTags[1] = new CTrecTagPair(-1,"DOCNO");
				m_ppTags[1]->schemaid = pSchema->addUnIndexedItem(_T("DOCNO"));
				m_ppTags[2] = new CTrecTagPair(-1,"DOCHDR");			
			}			
		}		
		void CTrecCollection::setParam(const tchar* pszParam)
		{
			if(firtex::string_utils::startsWithNoCase(pszParam,_T("doc_tag=")))
			{
				m_sDocTag = pszParam + 8;
				firtex::string_utils::trim(m_sDocTag);
				m_nDocTagLen = m_sDocTag.length();
			}
		}
	}
}
