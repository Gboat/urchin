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
// Created	: 2006/12/9
//
#include "utility/StdHeader.h"
#include "collection/FileListCollection.h"

#include "index/IndexWriter.h"
#include "utility/Config.h"

#include <string>

using namespace std;

using namespace firtex::utility;
using namespace firtex::index;

namespace firtex
{
	namespace collection
	{
		CFileListCollection::CFileListCollection(const tchar* filelist) 			
		{
			m_sListFile = filelist;

			m_schemas[0] = -1;
			m_schemas[1] = -1;					
		}
		CFileListCollection::CFileListCollection(const tchar* filelist,CIndexWriter* pIndexWriter) : CCollection(pIndexWriter)
		{
			m_sListFile = filelist;

			m_schemas[0] = -1;
			m_schemas[1] = -1;					
		}

		CFileListCollection::~CFileListCollection(void)
		{			
		}

		void CFileListCollection::scanInternal()
		{
			if(m_sListFile.empty())
			{
				FIRTEX_CLOG(level::err) << _T("Before read() you must open a list file first.") << FIRTEX_ENDL;				
			}
			try
			{
				firtex::index::CIndexWriter* pWriter = getWriter();
				document::CDocument* pDoc = NULL;

				numbervar nb;
				nb.nt = CNumber::INT64;
				struct fileStat buf;
				m_streamList.open(m_sListFile.c_str(),ios::in);
				if(!m_streamList.is_open())
				{
					FIRTEX_THROW2(FILEIO_ERROR,"Open file list fail..");
				}
				tstring filename;						
				bool bSchemaLineSkipped = false;
				while (getline(m_streamList, filename)) 
				{
					if(filename.empty())
						continue;
					if(_tcsnicmp(filename.c_str(),_T("SCHEMA:"),7) == 0)//skip schema line
						continue;
					else 
						break;
				}
				do
				{
					if(filename == _T(""))
					{
						if(getline(m_streamList, filename))
							continue;
						else break;
					}	
					FIRTEX_CLOG(level::info) << _T("Parsing file: ") << filename <<FIRTEX_ENDL;

					pDoc = pWriter->getDoc();
					pDoc->setDocPath(-1,filename.c_str());
					pDoc->addField(m_schemas[0],filename.c_str());

					if(m_schemas[1] != -1)///index modified time
					{						
						if ( fileStat(filename.c_str(),&buf) == -1 )
						{
							FIRTEX_CLOG(level::warn) << _T("Get modified time failed: ") << filename <<FIRTEX_ENDL;
							nb.value.i64val = 0;
						}
						else
							nb.value.i64val = buf.st_mtime;
						
						pDoc->addField(m_schemas[1],nb);
					}						
				}while (!stopRequested() && getline(m_streamList, filename));
				pWriter->flushDocuments();
			}			
			catch (CFirteXException &ex) 
			{
				FIRTEX_RETHROW(ex);
			}
			catch(...)
			{
				FIRTEX_THROW2(UNKNOWN_ERROR,_T(" bool CFileListCollection::scan()."));
			}			
		}		
		void CFileListCollection::addFile(const tchar* filename)
		{
			if(m_pIndexWriter)
			{
				m_pIndexWriter->addDocument(filename);				
			}								
		}		
		/*void CFileListCollection::close()
		{
			if(m_streamList.is_open() == true)
				m_streamList.close();
			CCollection::close();
		}*/
		void CFileListCollection::defineSchema(CDocumentSchema* pSchema)
		{	
			document::CSchemaItem* pItem = pSchema->findSchema(_T("path"));
			if(!pItem)
			{
				/*const tchar* szAlias = getAlias(_T("path"));
				tstring sPathFieldName;
				if(szAlias != NULL)
					sPathFieldName = szAlias;
				else sPathFieldName = _T("path");		*/
				tstring sPathFieldName = _T("PATH");
				m_schemas[0] = pSchema->addUnIndexedItem(sPathFieldName.c_str());
			}
			///ÐÞ¸ÄÈÕÆÚ
			pItem = pSchema->findSchema(_T("modified"));
			if(pItem)
			{
				m_schemas[1] = pItem->getId();
			}
		}
	}
}
