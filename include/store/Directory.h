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
// Created	: 2005/11/9
//
#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include "../utility/StdHeader.h"
#include <vector>
using namespace std;

namespace firtex
{
	namespace store
	{
		class CIndexInput;
		class CIndexOutput;
		typedef vector<tstring>FileList;
		class CDirectory
		{
		public:
			CDirectory(void)
			{
			}
			virtual ~CDirectory(void)
			{
			}
		public:			
			/**
			 * �ж��ļ��Ƿ����
			 * @param name �ļ���
			 */
			virtual bool			fileExists(const tstring& name) const = 0;
		
			/**
			 * ��ȡ�ļ����޸�����
			 * @param name �ļ���
			 */
			virtual int64_t			fileModified(const tstring& name) const = 0;

			/**
			 * ��ȡ�ļ�����
			 * @param name �ļ���
			 */
			virtual int64_t			fileLength(const tstring& name) const = 0;

			/**
			 * �г�Ŀ¼�������ļ�
			 * @return �ļ��б�����
			 * @memory ���ڲ�����,�ⲿɾ��
			 */
			virtual FileList*		list() = 0;

			/**
			 * ���ļ�
			 * @param name �ļ���
			 */
			virtual CIndexInput*	openInput(const tstring& name) = 0;

			/**
			 * ʹ��ָ�����ļ���������ļ�
			 * @param name �ļ���
			 * @param buf �ṩ�����ص�CIndexInput����Ŀ����ڴ�
			 * @param bufSize buf�Ĵ�С
			 */
			virtual CIndexInput*	openInput(const tstring& name,char* buf,size_t bufsize) = 0;

			/**
			 * ɾ���ļ�
			 * @param filename ���·���ļ���������չ��
			 * @param throwError ɾ���ļ������Ƿ��׳��쳣
			 */
			virtual void			deleteFile(const tstring& filename,bool throwError = true) = 0;

			/**
			 * �޸��ļ���
			 * @param from ԭ�ļ���
			 * @param to Ŀ���ļ���
			 */
			virtual void			renameFile(const tstring& from, const tstring& to) = 0;

			/**
			 * ����ɾ������ɾ����������Ϊfilename���ļ����൱��ɾ���ļ�filenam.*
			 * @param filename ����ڱ�Directory���ļ�����������չ��
			 * @param throwError ɾ���ļ������Ƿ��׳��쳣
			 */
			virtual void			batDeleteFiles(const tstring& filename,bool throwError = true) = 0;

			/**
			 * �ļ����������ļ����ļ���pSrcFile������filename
			 * @param filename  ����ڱ�Directory�ľ��ļ�����������չ��
			 * @param throwError �����ļ������Ƿ��׳��쳣
			 */
			virtual void			copyFile(const tstring& filename,CIndexInput* pSrcFile,bool throwError = true) = 0;

			/** 
			 * ���������ļ���ʵ��copy from.* to.*
			 * @param pFromDir ԴĿ¼
			 * @param from �����pFromDir���ļ���,������չ��
			 * @param to ���������Ŀ¼���ļ�����������չ��
			 * @param throwError �����ļ�����ʱ�Ƿ��׳��쳣
			 */
			virtual void			batCopyFiles(CDirectory*pFromDir,const tstring& from,const tstring& to,bool throwError = true) = 0;

			/**
			 * �����޸��ļ�������from.* ������to.*
			 * @param from ����ڱ�Directory��Դ�ļ�����������չ��
			 * @param to ����ڱ�Directory��Ŀ���ļ�����������չ��
			 */
			virtual void			batRenameFiles(const tstring& from, const tstring& to) = 0;
			
			/**
			 * �����ļ�
			 * @param name �ļ���
			 */
			virtual CIndexOutput*	createOutput(const tstring& name) = 0;

			/**
			 * ���ļ����޸�ʱ�����óɵ�ǰʱ��
			 * @param name �ļ���
			 */
			virtual void			touchFile(const tstring& name) = 0;

			/**
			 * �رմ洢����
			 */
			virtual void			close() = 0;		
		};
	}
}

#endif
