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
// Created	: 2006/8/5
//

#include "collection/FirteXCollection.h"
#include "utility/Misc.h"
#include "utility/dirent.h"
#include "utility/StringUtils.h"

using namespace firtex::utility;

#define FXC_DOC_TAG		0

namespace firtex
{
	namespace collection
	{
		CFirteXCollection::CFirteXCollection(const tchar* location,size_t bufsize)
			: m_sDirectory(location)
			, m_ppTags(NULL)
			, m_buffer(new char[bufsize])
			, m_bufsize(bufsize)
			, m_sDocTag(_T("DOC"))
			, m_nDocTagLen(3)
		{			
			
		}
		CFirteXCollection::CFirteXCollection(const tchar* location,CIndexWriter* pWriter,size_t bufsize)
			:CCollection(pWriter)
			,m_sDirectory(location)
			,m_ppTags(NULL)
			,m_buffer(new char[bufsize])
			,m_bufsize(bufsize)
		{	
		}

		CFirteXCollection::~CFirteXCollection(void)
		{
			for(int32_t i = 0;i< m_nNumTags; i++)
			{
				delete m_ppTags[i];
			}
			delete[] m_ppTags;
			m_ppTags = NULL;

			if(m_buffer)
			{
				delete[] m_buffer;
				m_buffer = NULL;
				m_bufsize = 0;
			}
		}

		void CFirteXCollection::scanInternal()
		{					
			indexDocs(m_sDirectory.c_str());			
		}
		void CFirteXCollection::indexDocs(const tchar* directory)
		{
			DIR* dir = opendir(directory);
			if ( dir != NULL )
			{
				struct dirent* fl;

				firtex::index::CIndexWriter* pWriter = getWriter();
				document::CDocument* pDoc = NULL;
				struct fileStat buf;
				tchar path[3*FX_MAX_PATH];				
				_tcscpy(path,directory);
				_tcscat(path,PATH_DELIMITERA);
				tchar* pathP = path + _tcslen(path);
				fl = readdir(dir);
				while ( fl != NULL )
				{
					if ( (_tcscmp(fl->d_name, _T("."))) && (_tcscmp(fl->d_name, _T(".."))) ) 
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
							if ( _tcscmp(fl->d_name, _T("schema.xml")) && (!_tcsicmp((const tchar*)( (tchar*)path + _tcslen(path) - 3),_T("xml"))))
							{
								FIRTEX_CLOG(level::info) << _T("Processing file: ") << path <<FIRTEX_ENDL;		
								readIndexFile(path);
							}								
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

		void CFirteXCollection::addFile(const tchar* filename)
		{
			readIndexFile(filename);
		}

		void CFirteXCollection::readIndexFile(const tchar* indexfile)
		{
			if(!indexfile || !(*indexfile))
				return ;            			
			try
			{
				char*	indexfilebuff;
				int32_t indexfilebufflen;

				char*	buffer = m_buffer;
				char*	overflowDocBuf = NULL;
				size_t	overflowDocBufSize = 0;
				bool	bDocOverflowed = false;

				firtex::index::CIndexWriter* pIndexWriter = getWriter();
				firtex::document::CDocument* pDoc = NULL;

				int32_t fileHandle = fileOpen(indexfile, O_BINARY | O_RDONLY | O_RANDOM, _S_IREAD );
				if (fileHandle < 0)
				{
					FIRTEX_CLOG(level::warn) << _T("Open file error:") << indexfile << FIRTEX_ENDL;
					return ;
				}					
				indexfilebufflen = (int32_t)fileSize(fileHandle);				
				indexfilebuff = new char[indexfilebufflen + 1];

				fileRead(fileHandle,indexfilebuff,indexfilebufflen);
				::fileClose(fileHandle);

				indexfilebuff[indexfilebufflen] = 0;

				tstring sBaseDir = indexfile;
				tstring::size_type nDeliPos = sBaseDir.rfind(PATH_DELIMITERA);
				if(nDeliPos != tstring::npos)
					sBaseDir.erase(nDeliPos + 1,sBaseDir.length() - nDeliPos - 1);
				else 
					sBaseDir = m_sDirectory + PATH_DELIMITERA;

				char* pStart = indexfilebuff;
				char* pEnd = indexfilebuff + indexfilebufflen;

				CFXStringA curContentFile;
				int32_t hContentFile = -1;

				char	log[80];
				char*	value;
				char*	valueEnd;
				size_t	valueLen;
				size_t	length;
				bool	bSkip = false;

				char*	pDocTagB;
				char*	pDocTagE = pStart;
				CTagPair* p;

				while (pStart < (indexfilebuff + indexfilebufflen) && stopRequested()==false)
				{						
					if(bDocOverflowed)///there is an overflowed document 
					{
						pIndexWriter->flushDocuments();
						bDocOverflowed = false;
						buffer = m_buffer;
					}

					p = m_ppTags[FXC_DOC_TAG]; /// begin doc tag
					pStart = pDocTagE;
					pDocTagB = pStart;
					if(processTag(pStart,pEnd,p,value,valueLen,true) == false)///find <DOC>
					{
						pStart += m_ppTags[FXC_DOC_TAG]->tagLen + 2;
						continue;///some problem
					}

					FXC_SKIP_BLANK(pDocTagB);

					pDocTagE = pStart;		///save the end of </DOC>
					pStart = pDocTagB + m_nDocTagLen + 2;	///set to the end of <DOC>

					pDoc = pIndexWriter->getDoc();
					for(int32_t i = 1;i < m_nNumTags;i++)
					{
						p = m_ppTags[i];		
						if( (p->schemaid != -1) || (p->tagType != CTagPair::NONE) )
							bSkip = false;
						else bSkip = true;
						if(processTag(pStart,pDocTagE,p,value,valueLen,bSkip) == false) 
						{
							FXC_SKIP_BLANK(pStart);
							continue;
						}
						FXC_SKIP_BLANK(pStart);

						if(p->schemaid != -1)
						{						
							pDoc->addField(p->schemaid,value,valueLen,false);
						}
						switch(p->tagType)
						{
						case CTagPair::DOCNO:
							{
								if(tlogger::isEnabled(level::info))
								{
									FIRTEX_ASSERT((valueLen < 80),_T(""));
									strncpy(log,value,valueLen);
									log[valueLen] = 0;
									FIRTEX_CLOG(level::info) << _T("Processing file:id =  ") << log <<FIRTEX_ENDL;
									if(!strncmp(value,_T("3559"),4))
										int a = 0;
								}
							}
							break;
						case CTagPair::MAINFILE:
							{
								if(!curContentFile.c_str() || strnicmp(curContentFile.c_str(),value,valueLen))
								{
									if(hContentFile!=-1)
										::fileClose(hContentFile);
									curContentFile.assign(value,valueLen,false);
									tstring confile = sBaseDir;
									confile += curContentFile.c_str();
									hContentFile = fileOpen(confile.c_str(), O_BINARY | O_RDONLY | O_RANDOM | O_LARGEFILE, _S_IREAD );
									if (hContentFile < 0)
									{
										FIRTEX_CLOG(level::warn) << _T("Open file error:") << confile << FIRTEX_ENDL;
										break;
									}				
								}
							}
							break;
						case CTagPair::OFFSET:
							{
								valueEnd = value + valueLen;
								fileSeek(hContentFile,_strtoi64(value,&valueEnd,10),SEEK_SET);
							}
							break;
						case CTagPair::LENGTH:
							{							
								valueEnd = value + valueLen;
								length = strtol(value,&valueEnd,10);
								if(length >= (m_bufsize - (buffer - m_buffer)))///buffer run out
								{
									if(m_buffer != buffer)///already cached some documents
									{
										if(overflowDocBufSize <= length)///allocate buffer for overflow doc
										{
											if(overflowDocBuf)
												delete[] overflowDocBuf;
											overflowDocBufSize = length + 1;
											overflowDocBuf = new char[overflowDocBufSize];
										}
										buffer = overflowDocBuf;
										bDocOverflowed = true;
									}
									if(length >= m_bufsize)///reading buffer must hold more than one document
									{									
										delete[] m_buffer;
										buffer = m_buffer = new char[length + 1];
										m_bufsize = length + 1;
									}								
								}
								fileRead(hContentFile,buffer,(unsigned int)length);
								buffer[length] = 0;							
								if(m_ppTags[i]->schemaid != -1)
									pDoc->addField(m_ppTags[i]->schemaid,value,valueLen,false);
								pDoc->getInput()->setBufferInput(0,-1,buffer,length);
								buffer += length;
							}
							break;
						}						
					}					
				}///end while					

				pIndexWriter->flushDocuments();

				if(hContentFile > 0)
				{
					::fileClose(hContentFile);
				}				
				delete[] indexfilebuff;		

				if(overflowDocBuf)
				{
					delete[] overflowDocBuf;
				}
			}
			catch (CFirteXException& e)
			{
				FIRTEX_CLOG(level::err) << _T("Error:") << e.what() <<FIRTEX_ENDL;
			}			
		}
		void CFirteXCollection::defineSchema(CDocumentSchema* pSchema)
		{

#define IS_TAG_DEFINED(tag) \
	if(!_tcsicmp(pSchemaItem->getName(),tag) || (pSchemaItem->getAlias() && !_tcsicmp(pSchemaItem->getAlias(),tag) ) )

			CSchemaItem* pSchemaItem;
			if(pSchema->getCount() > 0)
			{
				int32_t nStdTags = 4;				
				m_nNumTags = pSchema->getCount() + 1;
				m_ppTags = new CTagPair*[m_nNumTags];
				memset(m_ppTags,0,m_nNumTags*sizeof(CTagPair*));
				m_ppTags[0] = new CTagPair(-1,m_sDocTag.c_str());
				int32_t i = 1;
				pSchema->startIterator();
				while (pSchema->hasNext())
				{
					pSchemaItem = pSchema->next();
					m_ppTags[i] = new CTagPair(pSchemaItem->getId(),pSchemaItem->getName());

					IS_TAG_DEFINED(_T("DOCNO"))
					{
						m_ppTags[i]->tagType = CTagPair::DOCNO;
						nStdTags--;
					}
					else IS_TAG_DEFINED(_T("MAINFILE"))
					{
						m_ppTags[i]->tagType = CTagPair::MAINFILE;						
						nStdTags--;
					}
					else IS_TAG_DEFINED(_T("OFFSET"))
					{
						m_ppTags[i]->tagType = CTagPair::OFFSET;
						nStdTags--;
					}
					else IS_TAG_DEFINED(_T("LENGTH"))
					{
						m_ppTags[i]->tagType = CTagPair::LENGTH;
						nStdTags--;
					}

					i++;
				}
				if(nStdTags != 0)
				{
					for (int j = 0;j<m_nNumTags;j++)
					{
						if(m_ppTags[j])
							delete m_ppTags[j];
					}
					delete[] m_ppTags;
					m_ppTags = NULL;
					m_nNumTags = 0;

					FIRTEX_THROW3(BAD_PARAMETER_ERROR,_T("Bad FirteX schema."));
				}
			}
			else
			{
				m_nNumTags = 5;
				m_ppTags = new CTagPair*[m_nNumTags];
				m_ppTags[0] = new CTagPair(-1,m_sDocTag.c_str());
				m_ppTags[1] = new CTagPair(-1,"DOCNO");
				m_ppTags[1]->schemaid = pSchema->addUnIndexedItem(_T("DOCNO"));
				m_ppTags[2] = new CTagPair(-1,"MAINFILE");
				m_ppTags[3] = new CTagPair(-1,"OFFSET");
				m_ppTags[4] = new CTagPair(-1,"LENGTH");
			}
		}
		void CFirteXCollection::setParam(const tchar* pszParam)
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
