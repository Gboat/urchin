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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/4/12
//
#ifndef _STOREDFIELDREADER_H
#define _STOREDFIELDREADER_H

#include "../utility/StdHeader.h"
#include "../document/Document.h"
#include "../document/Field.h"
#include "../store/Directory.h"
#include "../store/IndexInput.h"
#include <string>

#define STOREDFIELD_READERBUFFSIZE	4096		//4K

using namespace std;
using namespace firtex::store;
using namespace firtex;

namespace firtex
{
	namespace index
	{
		class CFieldsInfo;
		class CStoredFieldsReader
		{
		public:
			CStoredFieldsReader(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo);
			~CStoredFieldsReader(void);
		public:
			/**
			 * ��ȡָ���ĵ���ŵ��ĵ����󣬴��ĵ����������������ʱ�д洢��־���ֶ�
			 * @param docID �ĵ����
			 * @return �ĵ�����
			 * @memory ���ڲ�����,���ⲿɾ��
			 */
			document::CDocument*	document(docid_t docID);

			/**
			 * ��ȡָ���ĵ����ָ���ֶα�ŵ��ĵ�����
			 * @param docID �ĵ����
			 * @param fid �ֶα��
			 * @return �ֶζ���,�����ֶβ��Ǵ洢�ֶλ򲻴���ʱ����NULL
			 * @memory ���ڲ�����,���ⲿɾ��
			 */
			document::CField*		field(docid_t docID,fieldid_t fid);

			void			open(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo);
			void			close();
		protected:
			CIndexInput*	m_pFieldValueReader;
			CIndexInput*	m_pFieldIndexReader;			

			CFieldsInfo*	m_pFieldsInfo;
		};
	}
}


#endif
