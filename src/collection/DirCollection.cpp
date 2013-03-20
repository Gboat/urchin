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
// Created	: 2006/7/3
//
#include "collection/DirCollection.h"
#include "utility/StdHeader.h"
#include "utility/dirent.h"
#include "utility/Config.h"
#include "utility/StringUtils.h"

using namespace firtex::utility;

namespace firtex
{
	namespace collection
	{
		CDirCollection::CDirCollection(const tchar* dir)
			:m_sDirectory(dir)
			,m_bAutoDetectDocType(false)
		{	
			m_schemas[0] = -1;
			m_schemas[1] = -1;
		}
		CDirCollection::CDirCollection(const tchar* dir,CIndexWriter* pIndexWriter)
			: CCollection(pIndexWriter)
			,m_sDirectory(dir)
			,m_bAutoDetectDocType(false)
		{	
			m_schemas[0] = -1;
			m_schemas[1] = -1;
		}
		CDirCollection::CDirCollection(const tchar* dir,bool bAutoDetectDocType)
			:m_sDirectory(dir)
			,m_bAutoDetectDocType(bAutoDetectDocType)
		{	
			m_schemas[0] = -1;
			m_schemas[1] = -1;
		}
		CDirCollection::CDirCollection(const tchar* dir,CIndexWriter* pIndexWriter,bool bAutoDetectDocType)
			: CCollection(pIndexWriter)
			,m_sDirectory(dir)
			,m_bAutoDetectDocType(bAutoDetectDocType)
		{	
			m_schemas[0] = -1;
			m_schemas[1] = -1;
		}		

		CDirCollection::~CDirCollection(void)
		{			
		}

		void CDirCollection::scanInternal()
		{			
			indexDocs(m_sDirectory.c_str());
			getWriter()->flushDocuments();		
		}		
		void CDirCollection::indexDocs(const tchar* directory)
		{
			DIR* dir = opendir(directory);
			if ( dir != NULL )
			{
				struct dirent* fl;

				firtex::index::CIndexWriter* pWriter = getWriter();
				document::CDocument* pDoc = NULL;
				struct fileStat buf;
				char path[3*FX_MAX_PATH];
				numbervar nb;
				nb.nt = CNumber::INT64;
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
							pDoc = pWriter->getDoc();
							pDoc->setDocPath(-1,path);
							pDoc->addField(m_schemas[0],path);
							if(m_bAutoDetectDocType)
							{
								pDoc->setDocType(getDocType(path));
							}

							if(m_schemas[1] != -1)						
							{
								nb.value.i64val = buf.st_mtime;
								pDoc->addField(m_schemas[1],nb);							
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

		void CDirCollection::defineSchema(CDocumentSchema* pSchema)
		{			
			document::CSchemaItem* pItem = pSchema->findSchema(_T("PATH"));
			if(!pItem)
			{				
				tstring sPathFieldName = _T("PATH");
				m_schemas[0] = pSchema->addUnIndexedItem(sPathFieldName.c_str());
			}
			else
			{
				m_schemas[0] = pItem->getId();
			}

			///ÐÞ¸ÄÈÕÆÚ
			pItem = pSchema->findSchema(_T("MODIFIED"));
			if(pItem)
			{
				m_schemas[1] = pItem->getId();
			}
		}

		void CDirCollection::addFile(const tchar* filename)
		{
			if(m_pIndexWriter)
			{
				m_pIndexWriter->addDocument(filename);				
			}
		}					

		void CDirCollection::setParam(const tchar* pszParam)
		{
			if(firtex::string_utils::startsWithNoCase(pszParam,_T("auto_type=")))
			{
				if(firtex::string_utils::endsWithNoCase(pszParam,_T("true")))
					m_bAutoDetectDocType = true;
				else m_bAutoDetectDocType = false;
			}
		}
	}
}

