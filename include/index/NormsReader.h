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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/1/4
//
#ifndef _NORMSREADER_H
#define _NORMSREADER_H
#include "../utility/StdHeader.h"
#include "../store/IndexInput.h"
#include "../store/Directory.h"
#include "../document/Field.h"
#include "FieldsInfo.h"
#include "NormBytes.h"
#include <sstream>
using namespace firtex::document;
using namespace firtex::store;
#include <map>
using namespace std;

namespace firtex
{
	namespace index
	{		
		class CCachedNormBytes;
		class CNormsReader
		{
			class CNorms
			{
			public:
				CNorms()
					:m_pNormsBytes(NULL)
					,m_bOwnNorms(false)
				{									
				}
				~CNorms()
				{	
					if(m_pNormsBytes)
					{
						if(m_bOwnNorms == false)
							m_pNormsBytes->setNormBytes(0,NULL);
						delete m_pNormsBytes;
					}	
					m_pNormsBytes = NULL;
				}
			public:
				inline bool	read(CIndexInput* pNormStream);
			public:
				CNormsBytes*	m_pNormsBytes;
				bool			m_bOwnNorms;
			};
		public:
			CNormsReader(CDirectory* pDirectory);
			virtual ~CNormsReader(void);
		public:
			void			open(const tchar* barrelName,CFieldsInfo* pFieldsInfo);
			void			open(CCachedNormBytes** ppCachedNormBytes,int32_t numFields,CFieldsInfo* pFieldsInfo);
			CNormsBytes*	getNorms(const tchar* fieldname);
			CNormsBytes*	getNorms(fieldid_t fid);
			void			close();
		protected:
			CDirectory*				m_pDirectory;

			map<fieldid_t,CNorms*>	m_normsByID;
			map<tstring,CNorms*>	m_normsByName;
		};
		//////////////////////////////////////////////////////////////////////////
		//Inline Functions
		inline bool CNormsReader::CNorms::read(CIndexInput* pNormStream)
		{
			try
			{
				if(m_pNormsBytes && m_bOwnNorms)
					delete m_pNormsBytes;
				size_t len = (size_t)pNormStream->length()/sizeof(int32_t);
				if(len <= 0)
					return false;
				CNormBytes* pNormBytes = new CNormBytes(len);
				m_bOwnNorms = true;												
				pNormStream->readInts(pNormBytes->getBytes(),pNormBytes->getLength());
				m_pNormsBytes = new CNormsBytes();
				m_pNormsBytes->setNormBytes(0,pNormBytes);
			}
			catch (CFirteXException& e)
			{					
				tstring str = _T("CNorms::read():Read norms failed.");
				str += e.what();						
				FIRTEX_THROW3(FILEIO_ERROR,str);
				return false;
			}

			return true;
		}	
	}
}


#endif
