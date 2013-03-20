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
// Created	: 2006/7/2
//
#ifndef __MULTIFORWARDINDEXREADER_H
#define __MULTIFORWARDINDEXREADER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ForwardIndexReader.h"

namespace firtex
{
	namespace index
	{
		class CForwardIndexReaderEntry
		{
		public:
			CForwardIndexReaderEntry()
			{
				docStart = numDocs = 0;
				reader = NULL;
			}
			~CForwardIndexReaderEntry(){}
		protected:
			docid_t					docStart;		//�ĵ���ʼ���
			count_t					numDocs;		//�ĵ�����
			CForwardIndexReader*		reader;			//term vector ����

			friend class CMultiForwardIndexReader;
		};
		class CMultiForwardIndexReader :	public CForwardIndexReader
		{
		public:
			CMultiForwardIndexReader(int numReaders);
			virtual ~CMultiForwardIndexReader(void);
		public:

			/**
			 * ��reader,��CMultiForwardIndexReader��˺��������κ�����
			 * @param barrelName ����Ͱ����			 
			 */
			virtual void		open(const tchar* barrelName){}

			/**
			 * �ر�reader,�ͷ���Դ,��CMultiForwardIndexReader��˺��������κ�����
			 */
			virtual void		close(){}

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

		protected:
			void	addVectorReader(int order,docid_t docStart,count_t numDocs,const CForwardIndexReader* pReader);
		private:
			int							m_numReaders;
			CForwardIndexReaderEntry*		m_ppReaders;

			friend class CMultiIndexBarrelReader;
		};
	}
}

#endif
