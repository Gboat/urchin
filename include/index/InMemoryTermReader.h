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
// Created	: 2006/12/02
//
#ifndef _IN_MEMORYTERMREADER_H
#define _IN_MEMORYTERMREADER_H

#include "TermReader.h"
#include "../utility/Exception.h"

namespace firtex
{
	namespace index
	{
		class CInMemoryTermReader :	public CTermReader
		{
		public:

			CInMemoryTermReader(void)
			{
			}

			virtual ~CInMemoryTermReader(void)
			{
			}
		public:
			/**
			 * ��ָ������Ͱ����������
			 * @param pDirectory �����洢λ��
			 * @param barrelname ����Ͱ��
			 * @param fieldPointer ����������ʼλ��
			 * @param deletedDocs ɾ���ĵ�����
			 */
			virtual void			open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* deletedDocs)
			{
				FIRTEX_THROW2(UNSUPPORTED_ERROR,_T("CInMemoryTermReader::open():unsupported operation."));
			};
		protected:
			/**
			 * get in-memory posting
			 * @return reference to in-memory posting
			 */
			virtual CInMemoryPosting*	inMemoryPosting() = 0;

			friend class CBarrelTermDocFreqs;
		};

	}
}

#endif
