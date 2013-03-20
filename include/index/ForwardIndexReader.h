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
// Created	: 2006/7/3
//
#ifndef __FORWARDINDEXREADER_H
#define __FORWARDINDEXREADER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "TermFreqVector.h"
#include "TermPositionVector.h"
#include "TermSequenceVector.h"
#include "../store/Directory.h"
#include "FieldsInfo.h"
#include "../store/IndexInput.h"

using namespace firtex::store;

namespace firtex
{
	namespace index
	{		
		template <class ForwardIndexPtrT>
		class CForwardIndexArray
		{
		public:
			CForwardIndexArray(vector<ForwardIndexPtrT>* freqVectors)
			{
				m_numFields = freqVectors->size();
				m_ppForwardIndexs = new ForwardIndexPtrT[m_numFields];
				for (size_t i = 0;i < m_numFields;i++)
				{
					m_ppForwardIndexs[i] = (*freqVectors)[i];
				}
			}
			~CForwardIndexArray()
			{
				for(count_t i = 0;i<m_numFields;i++)
				{
					delete m_ppForwardIndexs[i];
				}
				delete[] m_ppForwardIndexs;
				m_ppForwardIndexs = NULL;
			}			
		public:
			size_t			numFields(){return m_numFields;}
			ForwardIndexPtrT	operator[](size_t order)
			{
				assert(order >=0 && order < m_numFields);
				return m_ppForwardIndexs[order];
			}
			ForwardIndexPtrT	operator[](const tchar* field)
			{				
				for(count_t i = 0;i<m_numFields;i++)
				{
					if(m_ppForwardIndexs[i])
					{
						if(!_tcscmp(m_ppForwardIndexs[i]->getField(),field))
							return m_ppForwardIndexs[i];
					}
				}
				return NULL;
			}
		private:
			ForwardIndexPtrT*	m_ppForwardIndexs; 
			size_t			m_numFields;
		};
		
		class CFactoryFinder;
		class CForwardIndexReader
		{
		public:
			CForwardIndexReader();
			CForwardIndexReader(CDirectory* pDirectory,CFieldsInfo* pFieldsInfo);
			virtual ~CForwardIndexReader(void);
		public:
			/**
			 * ��reader
			 * @param barrelName ����Ͱ����
			 * @throw  
			 */
			virtual void		open(const tchar* barrelName);

			/**
			 * �ر�reader,�ͷ���Դ
			 */
			virtual void		close();

			/**
			 * ��ȡǰ����������,�μ�{@link #ForwardIndex}
			 */
			virtual forwardindex_t	getForwardIndexType(const tchar* field);

			/**
			 * ��ȡTerm Freq Vector
			 * @param did �ĵ����
			 * @param field �ֶ�����
			 * @return Term Freq Vector ָ��
			 * @memory ���ڲ�ָ��,���ⲿɾ��
			 */
			virtual CTermFreqVector*		getFreqVector(docid_t did,const tchar* field);

			/**
			 * ��ȡTerm Freq Vector
			 * @param did �ĵ����			 
			 * @return Term Freq Vector ��������
			 * @memory ���ڲ�ָ��,���ⲿɾ��
			 */
			virtual CForwardIndexArray<CTermFreqVector*>*	 getFreqVectors(docid_t did);

			/**
			 * ��ȡTerm Position Vector
			 * @param did �ĵ����
			 * @param field �ֶ�����
			 * @return Term Position Vector ָ��
			 * @memory ���ڲ�ָ��,���ⲿɾ��
			 */
			virtual CTermPositionVector*	getPositionVector(docid_t did,const tchar* field);

			/**
			 * ��ȡTerm Position Vector
			 * @param did �ĵ����			 
			 * @return Term Position Vector ��������
			 * @memory ���ڲ�ָ��,���ⲿɾ��
			 */
			virtual CForwardIndexArray<CTermPositionVector*>*	getPositionVectors(docid_t did);

			/**
			 * ��ȡTerm Sequence Vector
			 * @param did �ĵ����
			 * @param field �ֶ�����
			 * @return Term Sequence Vector ָ��
			 * @memory ���ڲ�ָ��,���ⲿɾ��
			 */
			virtual CTermSequenceVector*	getSquenceVector(docid_t did,const tchar* field);

			/**
			 * ��ȡTerm Sequence Vector
			 * @param did �ĵ����			 
			 * @return Term Sequence Vector ��������
			 * @memory ���ڲ�ָ��,���ⲿɾ��
			 */
			virtual CForwardIndexArray<CTermSequenceVector*>*	getSquenceVectors(docid_t did);

			/** �����򹤳� */
			void			setFactoryFinder(CFactoryFinder* pFactories){m_pFieldFactories = pFactories;}
		private:
			CDirectory*			m_pDirectory;
			CFieldsInfo*		m_pFieldsInfo;
			CIndexInput*		m_pFIMInput;
			CIndexInput*		m_pFIDInput;
			CIndexInput*		m_pFIVInput;

			CFactoryFinder*	m_pFieldFactories;
		};
	}
}

#endif
