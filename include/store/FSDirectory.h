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
#ifndef _FSDIRECTORY_H
#define _FSDIRECTORY_H
#include "Directory.h"

#include <string>
#include <map>
using namespace std;

namespace firtex
{
	namespace store
	{
		class CIndexInput;
		class CIndexOutput;
		class CFSDirectory : public CDirectory
		{
			typedef map<tstring,CFSDirectory*> directory_map;
			typedef directory_map::iterator directory_iterator;
		protected: 
			CFSDirectory(const tstring& path,bool bCreate=false);			
		public:
			virtual ~CFSDirectory(void);
		public:
			static CFSDirectory*	getDirectory(const tstring& path,bool bCreate);			
		protected:
			/**
			 * get global map of directory
			 */
			static CFSDirectory::directory_map&	getDirectoryMap(); 
		public:
			/**
			 * �г�Ŀ¼�������ļ�
			 */
			FileList*		list();

			/**
			 * �ж��ļ��Ƿ����
			 */
			bool			fileExists(const tstring& name) const;
			
			/**
			 * ��ȡ�ļ����޸�����
			 */
			int64_t			fileModified(const tstring& name) const;

			/**
			 * ��ȡ�ļ�����
			 */
			int64_t			fileLength(const tstring& name) const;
			/**
			 * ���ļ�
			 */
			CIndexInput*	openInput(const tstring& name);

			/**
			 * ʹ��ָ�����ļ���������ļ�
			 */
			CIndexInput*	openInput(const tstring& name,char* buf,size_t bufsize);

			/**
			 * ɾ���ļ�
			 */
			void			deleteFile(const tstring& filename,bool throwError = true);
			/**
			 * �޸��ļ���
			 */
			void			renameFile(const tstring& from, const tstring& to);

			/**
			 * ����ɾ������ɾ����������Ϊfilename���ļ����൱��ɾ���ļ�filenam.*
			 * @param filename  ����ڱ�Directory�ľ��ļ�����������չ��
			 * @param throwError ɾ���ļ������Ƿ��׳��쳣
			 */
			void			batDeleteFiles(const tstring& filename,bool throwError = true);

			/**
			 * �������������ļ����ļ�from.*�ĳ�to.*
			 * @param filename  ����ڱ�Directory�ľ��ļ�����������չ��
			 * @param throwError ɾ���ļ������Ƿ��׳��쳣
			 */
			void			batRenameFiles(const tstring& from, const tstring& to);

			/**
			 * �ļ����������ļ����ļ���pSrcFile������filename
			 * @param filename  ����ڱ�Directory�ľ��ļ�����������չ��
			 * @param throwError �����ļ������Ƿ��׳��쳣
			 */
			void			copyFile(const tstring& filename,CIndexInput* pSrcFile,bool throwError = true);

			/** 
			 * ���������ļ���ʵ��copy from.* to.*
			 * @param pFromDir ԴĿ¼
			 * @param from �����pFromDir���ļ���,������չ��
			 * @param to ���������Ŀ¼���ļ�����������չ��
			 * @param throwError �����ļ�����ʱ�Ƿ��׳��쳣
			 */
			void			batCopyFiles(CDirectory*pFromDir,const tstring& from,const tstring& to,bool throwError = true);
			
			/**
			 * �����ļ�
			 */
			CIndexOutput*	createOutput(const tstring& name);

			/**
			 * ���ļ����޸�ʱ�����óɵ�ǰʱ��
			 */
			void			touchFile(const tstring& name);

			///**
			// * ��¡һ���µĶ���
			// * @return ��¡����
			// * @memory ���ڲ�����,�ⲿɾ��
			// */
			//CDirectory*		clone();

			/**
			 * �رմ洢����
			 */
			void			close();

		protected:
			void			create();
		protected:
			tstring		m_directory;
			int			m_nRefCount;//���ü���
		};
	}
}


#endif
