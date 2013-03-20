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
// Created	: 2005/12/10
//
#ifndef _FIELDINDEXER_H
#define _FIELDINDEXER_H

#include "../utility/StdHeader.h"
#include "../document/Field.h"
#include "OutputDescriptor.h"
#include "Posting.h"
#include "thread/FirteXThreads.h"
using namespace firtex;
using namespace firtex::document;

namespace firtex
{
	namespace index
	{
		class CTermReader;
		class CFieldIndexer : public LOCK_BASE
		{
		public:

			CFieldIndexer(void)
			{
			}

			virtual ~CFieldIndexer(void)
			{
			}
		public:		
			/**
			 * ��������ֶ�
			 * @param did �ĵ��ڲ����
			 * @param pField �������ֶζ���ָ��
			 */
			virtual void			addField(docid_t did,CField* pField) = 0;

			/**
			 * ������������ֶΣ��Լ��ٺ������ÿ�����pdid��ppField����һһ��Ӧ����ppField[i]���ĵ����Ϊpdid[i]
			 * @param pdid �ĵ��ڲ�������飬��СΪnumFields
			 * @param ppField �������ֶ����飬��СΪnumFields
			 * @param numFields pdid��ppFields�����С
			 */
			virtual void			addFields(docid_t* pdid,CField** ppField,int32_t numFields) = 0;
			
			/**
			 * write field index data
			 * @param pWriterDesc out descriptor
			 * @return offset of field vocabulary descriptor
			 */
			virtual fileoffset_t	write(COutputDescriptor* pWriterDesc) = 0;
			
			/**
			 * �����������tdi�ļ��е�ƫ��λ��
			 * @param off tdi�ļ�ƫ�Ƶ�ַ
			 */
			virtual	void			setFilePointer(fileoffset_t off) = 0;

			/**
			 * ��ȡ�������tdi�ļ��е�ƫ��λ��
			 */
			virtual	fileoffset_t	getFilePointer() = 0;

			/**
			 * ��ȡ���ظ��������ʸ���
			 */
			virtual uint64_t		distinctNumTerms() = 0;

			/** 
			 * �������������ڴ�ʵ��Term Reader
			 * @return Term Reader �������ⲿɾ��
			 */
			virtual CTermReader*	termReader() = 0;

			/**
			 * reset postings of the indexer
			 */
			virtual void			reset() = 0;
		public:
			/** ��ȡ�������������� */
			INDEX_LEVEL				getIndexLevel(){return m_eIndexLevel;}
			
			/** ������������������ */
			void					setIndexLevel(INDEX_LEVEL eLevel){m_eIndexLevel = eLevel;}

			/**
			 * set field name of the indexer
			 * @param field field name
			 */
			void					setField(const tchar* field){m_sField = field;}

			/**
			 * get field name of the indexer
			 * @return field name
			 */
			const tchar*			getField(){return m_sField.c_str();}
		private:
			INDEX_LEVEL		m_eIndexLevel;
			tstring			m_sField;
		};
	}
}

#endif
