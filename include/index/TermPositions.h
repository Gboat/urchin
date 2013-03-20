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
// Created	: 2005/7/17
//
#ifndef _TERMPOSITIONS_H
#define _TERMPOSITIONS_H

#include "TermDocFreqs.h"

namespace firtex
{
	namespace index
	{
		class CTermPositions : public CTermDocFreqs
		{
		public:

			CTermPositions(void)
			{
			}

			virtual ~CTermPositions(void)
			{
			}
		public:
			
			/**
			 * ��һ��λ��
			 */
			virtual loc_t nextPosition()=0;
			
			/**
			 * ��һ��λ��
			 * @param positions λ������
			 * @return ʵ��λ�ø�����positions�����С
			 */
			virtual count_t nextPositions(loc_t*& positions)=0;

			/** 
			 * �ƶ�����һ��λ�ô��ڻ����target��λ��			 
			 * @param target Ŀ��λ��
			 * @param nearTarget �������ɹ�ʱnearTarget=target,���򣬷��ش���target������target��ӽ���λ��,
			 *             ���target��������ֵ�򷵻�С��target����target��ӽ���λ��
			 * @return ��������targetλ��ʱ����true
			 */ 
			virtual bool skipToPosition(loc_t target,loc_t& nearTarget)=0;
		};
	}
}


#endif
