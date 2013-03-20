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
// Created	: 2005/12/9
//
#ifndef __STOREDFIELDSWRITER_H
#define __STOREDFIELDSWRITER_H

#include "../utility/StdHeader.h"
#include "../document/Document.h"
#include "../document/Field.h"
#include "../store/Directory.h"
#include "../store/IndexOutput.h"
#include <string>

#define STOREDFIELD_WRITEBUFFSIZE	4096		//4K

using namespace std;
using namespace firtex::store;
using namespace firtex;


namespace firtex
{
	namespace index
	{		
		class CFieldsInfo;
		class CStoredFieldsReader;
		class CStoredFieldsWriter
		{
		public:
			CStoredFieldsWriter(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo);
			CStoredFieldsWriter();
			~CStoredFieldsWriter(void);
		public:
			/**
			 * ����ĵ����洢�洢�ֶ�(Store Field)
			 * @param pDoc �ĵ�����			 
			 */
			void			addDocument(document::CDocument* pDoc);	

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

			/**
			 * ���µ��ĵ�����Ϣ�ļ�
			 * @param pDirectory �洢λ��
			 * @param barrels����Ͱ����
			 * @param fieldsInfo ����������Ϣ
			 */
			void			open(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo);

			/** �ر��ļ� */
			void			close();			
		protected:			
			CIndexOutput*	m_pFieldValueWriter;
			CIndexOutput*	m_pFieldIndexWriter;			

			CDirectory*		m_pDirectory;
			CFieldsInfo*	m_pFieldsInfo;
			tstring			m_sBarrel;
			bool			m_bDirty;

			CStoredFieldsReader*	m_pSFReader;
		};

	}
}

#endif
