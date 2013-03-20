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
#include "collection/TWCollection.h"
#include "utility/dirent.h"
#include "index/IndexWriter.h"
#include "../utility/zlib/zlib.h"
#include "utility/Number.h"
#include "utility/StringUtils.h"

#include <memory>

using namespace std;

using namespace firtex::utility;

#define DOC_MAXLENGTH 10000000

namespace firtex
{
	namespace collection
	{
		CTWCollection::CTWCollection(const tchar* dir)			
			:m_sDirectory(dir)			
			,m_ppTags(NULL)
			,m_pContentBuffer(NULL)
			,m_nBufSize(0)			
			,m_sDocTag(_T("version"))
			,m_bZipped(false)
			,m_pUnzipBuffer(NULL)
		{
		}

		CTWCollection::CTWCollection(const tchar* dir,CIndexWriter* pWriter)
			:CCollection(pWriter)
			,m_sDirectory(dir)			
			,m_ppTags(NULL)
			,m_pContentBuffer(NULL)
			,m_nBufSize(0)			
			,m_sDocTag(_T("version"))
			,m_bZipped(false)
			,m_pUnzipBuffer(NULL)
		{
		}

		CTWCollection::~CTWCollection(void)
		{
			if(m_pContentBuffer)
			{
				delete[] m_pContentBuffer;
				m_pContentBuffer = NULL;
			}
			if(m_pUnzipBuffer)
			{
				delete m_pUnzipBuffer;
				m_pUnzipBuffer = NULL;
			}
			for(int32_t i = 0;i<m_numTags;i++)
			{
				delete m_ppTags[i];
			}
			delete[] m_ppTags;
			m_ppTags = NULL;
		}

		void CTWCollection::scanInternal()
		{			
			indexDocs(m_sDirectory.c_str());		
		}
		void CTWCollection::indexDocs(const tchar* directory)
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

		void CTWCollection::parseBuffer(char* buffer,size_t bufSize)
		{				
			try
			{
				firtex::index::CIndexWriter* pIndexWriter = getWriter();
				firtex::document::CDocument* pDoc = NULL;				

				char*	pStart = buffer;
				char*	pEnd = buffer + bufSize;
				char*	text = NULL;
				size_t	textLen = 0;
				int32_t	nDataLen = 0;	
				char*	pTmp = NULL;
				char*	pContentBuffer = buffer;
				int32_t	nPropTags = m_numTags - 1;
				int32_t nUnzipLength = 0;
				size_t	nSkipHeader = 0;
				int32_t i = 0;
				if(m_bZipped)
					nPropTags--;
				numbervar nv;

				CTWTag* p;
				pDoc = pIndexWriter->getDoc();
				while (pStart < (pContentBuffer + bufSize) && stopRequested() == false)
				{		
					p = m_ppTags[DOC_TAG];					
					if(processTag(pStart,pEnd,p,text,textLen,true) == false)///find "version:"
					{
						pStart += m_ppTags[DOC_TAG]->nTagLen + 1;
						continue;///some problem,skip it
					}								
					TW_SKIP_BLANK(pStart);
					
					for(i = 1;i < nPropTags;i++)
					{
						p = m_ppTags[i];
						if(processTag(pStart,pEnd,p,text,textLen,(m_ppTags[i]->schemaid == -1)) == false)
						{
							TW_SKIP_LINE(pStart);
							i--;
							continue;
						}	

						TW_SKIP_BLANK(pStart);
						
						if(p->schemaid != -1)
						{						
							pDoc->addField(p->schemaid,text,textLen,true);
						}						
					}									
					if(m_bZipped)
					{
						p = m_ppTags[i++];
						if(processTag(pStart,pEnd,p,text,textLen,false))
						{
							TW_SKIP_BLANK(text);
							nUnzipLength = strtol(text,&pTmp,10);							
						}
						else nUnzipLength = 0;
						if(p->schemaid != -1)
						{							
							nv.nt = CNumber::INT32;
							nv.value.i32val = nUnzipLength;
							pDoc->addField(p->schemaid,nv);
						}	
					}
					p = m_ppTags[i];
					///process last "length:" tag
					if(processTag(pStart,pEnd,p,text,textLen,false) == false)
					{
						///can't find "length:" tag, skip it
						pDoc->clear();
						continue;
					}
					TW_SKIP_BLANK(text);
					nDataLen = strtol(text,&pTmp,10);
					pStart++;///skip '\n'
					if(nDataLen > DOC_MAXLENGTH)///too long document, skip it
					{
						pStart += (nDataLen + 1);
						continue;
					}
					if(m_bZipped && nUnzipLength > 0)
					{
						if(nUnzipLength > (int32_t)m_nUnzipBufSize)
						{
							if(m_pUnzipBuffer)
								delete[] m_pUnzipBuffer;
							m_pUnzipBuffer = new char[nUnzipLength];
							m_nUnzipBufSize = nUnzipLength;
						}
						if ( int nError = ::uncompress((Bytef*)m_pUnzipBuffer,(uLongf*)&nUnzipLength,(const byte *)pStart,(uLong)nDataLen) ) 
						{	
							///uncompress failed.
							FIRTEX_CLOG(level::warn) << _T("CTWCollection::parseBuffer():uncompress data failed.") << FIRTEX_ENDL;
							pStart += (nDataLen + 1);
							pDoc->clear();
							continue;
						}
						else 
						{							
							nSkipHeader = skipHeader(m_pUnzipBuffer,m_pUnzipBuffer + nUnzipLength);
							if(nSkipHeader == nUnzipLength)
							{
								pDoc->clear();
								continue;
							}
							pDoc->getInput()->setBufferInput(0,-1,m_pUnzipBuffer + nSkipHeader,nUnzipLength - nSkipHeader);
							pIndexWriter->flushDocuments();
						}
					}
					else 
					{						
						nSkipHeader = skipHeader(pStart,pStart + nDataLen);
						if(nSkipHeader == nDataLen)
						{
							pDoc->clear();
							continue;
						}
						pDoc->getInput()->setBufferInput(0,-1,pStart + nSkipHeader,nDataLen - nSkipHeader);
					}
					
					pDoc = pIndexWriter->getDoc();

					pStart += (nDataLen + 1);
				}				

				pIndexWriter->flushDocuments();
			}
			catch (CFirteXException& e)
			{
				FIRTEX_CLOG(level::err) << _T("CTWCollection::parseBuffer() failed:") << e.what() <<FIRTEX_ENDL;
			}
		}

		void CTWCollection::defineSchema(CDocumentSchema* pSchema)
		{	
			CSchemaItem* pSchemaItem;			
			if(pSchema->getCount() > 0)
			{
				m_numTags = pSchema->getCount() + 1;
				m_ppTags = new CTWTag*[m_numTags];
				m_ppTags[0] = new CTWTag(-1,m_sDocTag.c_str());
				int32_t i = 1;
				pSchema->startIterator();
				while (pSchema->hasNext())
				{
					pSchemaItem = pSchema->next();
					m_ppTags[i] = new CTWTag(pSchemaItem->getId(),pSchemaItem->getName());					
					i++;
				}
			}
			else
			{
				m_numTags = 3;
				m_ppTags = new CTWTag*[m_numTags];
				m_ppTags[0] = new CTWTag(-1,m_sDocTag.c_str());
				m_ppTags[1] = new CTWTag(-1,"url");
				m_ppTags[1]->schemaid = pSchema->addUnIndexedItem(_T("url"));
				m_ppTags[2] = new CTWTag(-1,"length");
			}
			if(!fx_strcmp(m_ppTags[m_numTags - 2]->pTag,"unzip-length:"))
				m_bZipped = true;
		}
		void CTWCollection::setParam(const tchar* pszParam)
		{
			if(firtex::string_utils::startsWithNoCase(pszParam,_T("doc_tag=")))
			{
				m_sDocTag = pszParam + 8;
				firtex::string_utils::trim(m_sDocTag);
				//m_nDocTagLen = m_sDocTag.length();
			}
		}
	}
}
