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
// Created	: 2006/7/2
//
#ifndef __RAMDIRECTORY_H
#define __RAMDIRECTORY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Directory.h"
#include "IndexInput.h"
#include "IndexOutput.h"
#include "../utility/Misc.h"
#include <map>
#include <vector>
using namespace std;


namespace firtex
{
	namespace store
	{

		#define RAMFILE_STREAMSIZE	4096		//4K
		#define RAMFILE_MAXSEGS		10000	

		class CRAMFile
		{
		public:
			CRAMFile()
			{
				m_length = 0;
				m_lastModified = firtex::utility::CMisc::currentTimeMillis();
				m_streamBufSize = RAMFILE_STREAMSIZE;
			}
			~CRAMFile()
			{
				vector<byte*>::iterator iter = m_buffers.begin();
				while (iter != m_buffers.end())
				{
					delete[] (*iter);
					iter++;
				}
				m_buffers.clear();
			}			
		public:
			CRAMFile*	clone()
			{
				CRAMFile* pRAM = new CRAMFile();
				pRAM->m_length = m_length;
				pRAM->m_lastModified = m_lastModified;
				pRAM->m_streamBufSize = m_streamBufSize;
				byte* tmp;
				vector<byte*>::iterator iter = m_buffers.begin();
				while (iter != m_buffers.end())
				{
					tmp = new byte[m_streamBufSize];
					memcpy(tmp,(*iter),m_streamBufSize);
					pRAM->m_buffers.push_back(tmp);
					iter++;
				}
				return pRAM;
			}
		protected:
			int64_t		m_length;			//�ļ�����
			int64_t		m_lastModified;		//����޸�ʱ��

			vector<byte*>	m_buffers;		//�ļ�����buffer
			int32_t			m_streamBufSize;//m_buffers��ÿ��buffer�Ĵ�С

			friend class CRAMDirectory;
			friend class CRAMIndexInput;
			friend class CRAMIndexOutput;
		};

		class CRAMIndexInput : public CIndexInput
		{
		public:
			CRAMIndexInput(CRAMFile* file);
			CRAMIndexInput(CRAMFile* file,char* buffer,size_t bufSize);
			CRAMIndexInput(const CRAMIndexInput& clone);
			virtual ~CRAMIndexInput();
		public:
			/**
			 * read data from file, sub class must implement this method
			 * @param b target address
			 * @length data length to read
			 * @bCheck check read position or not
			 */			
			void			readInternal(char* b,size_t length,bool bCheck = true);
			
			/**
			 * ��¡һ��CIndexInput����,��¡����ʹ��buffer��Ϊ����	 
			 * @param buffer ��¡����ʹ�õ�buffer
			 * @param buffsize buffer�Ĵ�С
			 */					
			CIndexInput*	clone(char* buffer,size_t buffsize);			

			/**
			 * ��¡һ��CIndexInput����,��¡����ʹ���Լ�˽�е��ڴ���Ϊ����	 
			 */
			CIndexInput*	clone();

			/**
			 * �ر��ļ����ͷ���Դ
			 */
			void			close();
		protected:
			/**
			 * ʵ���ļ���λ������readInternal�Ķ����������λ�ÿ�ʼ��������ʵ��
			 * @param pos ��λλ��
			 */		
			void  seekInternal(int64_t position);		
		protected:
			CRAMFile*	m_file;
			int64_t		m_pointer;

			friend class CRAMDirectory;
		};

		class CRAMIndexOutput : public CIndexOutput
		{
		public:
			CRAMIndexOutput();
			CRAMIndexOutput(CRAMFile* ramFile);
			virtual ~CRAMIndexOutput();
		public:
			/**
			 * �ļ���λ
			 */
			void	seek(int64_t pos);

			/**
			 * ��ȡ�ļ�����
			 */
			int64_t length(){return m_file->m_length;}

			/**
			 * �ر��ļ��ͷ���Դ
			 */
			void	close();

			/**
			 * ���ļ�����д����һ��Output
			 * @param pOutput Ŀ��Output
			 */
			void	writeTo(CIndexOutput* pOutput);

		protected:
			/**
			 * ʵ��дbuffer�������ڵ�ǰλ��д����
			 * @param b ���ݵ�ַ
			 * @param len b�ĳ���
			 */
			void	flushBuffer(char* b, size_t len);
		protected:				
			CRAMFile*	m_file;
			bool		m_bDeleteFile;
			int64_t		m_pointer;

			friend class CRAMDirectory;
		};

		class CRAMDirectory :	public CDirectory
		{
		public:
			CRAMDirectory(const tchar* dir);
			CRAMDirectory(CDirectory* pDir);
			CRAMDirectory(void);
			virtual ~CRAMDirectory(void);
		public:
			/**
			 * �ж��ļ��Ƿ����
			 * @param name �ļ���
			 */
			bool			fileExists(const tstring& name) const;

			/**
			 * ��ȡ�ļ����޸�����
			 * @param name �ļ���
			 */
			int64_t			fileModified(const tstring& name) const;

			/**
			 * ��ȡ�ļ�����
			 * @param name �ļ���
			 */
			int64_t			fileLength(const tstring& name) const;

			/**
			 * �г�Ŀ¼�������ļ�
			 * @return �ļ��б�����
			 * @memory ���ڲ�����,�ⲿɾ��
			 */
			FileList*		list();

			/**
			 * ���ļ�
			 * @param name �ļ���
			 */
			CIndexInput*	openInput(const tstring& name);

			/**
			 * ʹ��ָ�����ļ���������ļ�
			 * @param name �ļ���
			 * @param buf �ṩ�����ص�CIndexInput����Ŀ����ڴ�
			 * @param bufSize buf�Ĵ�С
			 */
			CIndexInput*	openInput(const tstring& name,char* buf,size_t bufsize);

			/**
			 * ɾ���ļ�
			 * @param filename ȫ·���ļ���������չ��
			 * @param throwError ɾ���ļ������Ƿ��׳��쳣
			 */
			void			deleteFile(const tstring& filename,bool throwError = true);

			/**
			 * �޸��ļ���
			 * @param from ԭ�ļ���
			 * @param to Ŀ���ļ���
			 */
			void			renameFile(const tstring& from, const tstring& to);

			/**
			 * ����ɾ������ɾ����������Ϊfilename���ļ����൱��ɾ���ļ�filenam.*
			 * @param filename ����ڱ�Directory���ļ�����������չ��
			 * @param throwError ɾ���ļ������Ƿ��׳��쳣
			 */
			void			batDeleteFiles(const tstring& filename,bool throwError = true);

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
			 * �����޸��ļ�������from.* ������to.*
			 * @param from ����ڱ�Directory��Դ�ļ�����������չ��
			 * @param to ����ڱ�Directory��Ŀ���ļ�����������չ��
			 */
			void			batRenameFiles(const tstring& from, const tstring& to);
			
			/**
			 * �����ļ�
			 * @param name �ļ���
			 */
			CIndexOutput*	createOutput(const tstring& name);

			/**
			 * ���ļ����޸�ʱ�����óɵ�ǰʱ��
			 * @param name �ļ���
			 */
			void			touchFile(const tstring& name);

			/**
			 * �رմ洢����
			 */
			void			close();

		protected:
			void	fromDirectory(CDirectory* pDir,bool bCloseDir);
		private:			
			map<tstring,CRAMFile*>		m_files;		//�ļ�
		};
	}
}


#endif
