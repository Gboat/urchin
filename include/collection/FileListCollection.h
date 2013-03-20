//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/12/9
//
#ifndef _FILELISTCOLLECTION_H
#define _FILELISTCOLLECTION_H

#include "../utility/StdHeader.h"
#include "Collection.h"
#include <string>
#include <fstream>
using namespace std;

namespace firtex
{
	namespace collection
	{
		class CFileListCollection :	public CCollection
		{
		public:		
			class Creator : public CCollection::Creator
			{
			public:
				Creator()
				{
					m_identifier += _T(".filelist");
				}
				~Creator(){}
			public:
				CCollection* create(const tchar* path){return new CFileListCollection(path);}

			public:
				static Creator* getInstance()
				{
					static CFileListCollection::Creator INSTANCE;
					return &INSTANCE;
				}
			};
		public:
			/**
			 * ���캯��
			 * @param filelist �ĵ��б����ļ�ÿ�б�ʶһ���ĵ��Ĵ��λ��			 
			 * @param pIndexWriter Index Writer����			 
			 */
			CFileListCollection(const tchar* filelist);
			/**
			 * ���캯��
			 * @param filelist �ĵ��б����ļ�ÿ�б�ʶһ���ĵ��Ĵ��λ��			 
			 * @param pIndexWriter Index Writer����
			 */
			CFileListCollection(const tchar* filelist,CIndexWriter* pIndexWriter);
			virtual ~CFileListCollection(void);

		public:
			void		addFile(const tchar* filename);
			/*void		close();*/
		protected:			
			void		scanInternal();
			/** 
			 * ����Collection��Schema
			 * @param pSchema ������������ڶ���Collection��Schema
			 */
			void		defineSchema(CDocumentSchema* pSchema);
		protected:			
			tstring		m_sListFile;
			schemaid_t	m_schemas[2];

#ifdef _UNICODE
			wifstream	m_streamList;
#else
			ifstream	m_streamList;
#endif
		};
	}
}



#endif

