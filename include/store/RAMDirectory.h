//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
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
			int64_t		m_length;			//文件长度
			int64_t		m_lastModified;		//最后修改时间

			vector<byte*>	m_buffers;		//文件内容buffer
			int32_t			m_streamBufSize;//m_buffers中每个buffer的大小

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
			 * 克隆一个CIndexInput对象,克隆对象使用buffer作为缓冲	 
			 * @param buffer 克隆对象使用的buffer
			 * @param buffsize buffer的大小
			 */					
			CIndexInput*	clone(char* buffer,size_t buffsize);			

			/**
			 * 克隆一个CIndexInput对象,克隆对象使用自己私有的内存作为缓冲	 
			 */
			CIndexInput*	clone();

			/**
			 * 关闭文件，释放资源
			 */
			void			close();
		protected:
			/**
			 * 实现文件定位操作，readInternal的读操作从这个位置开始，需子类实现
			 * @param pos 定位位置
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
			 * 文件定位
			 */
			void	seek(int64_t pos);

			/**
			 * 获取文件长度
			 */
			int64_t length(){return m_file->m_length;}

			/**
			 * 关闭文件释放资源
			 */
			void	close();

			/**
			 * 将文件内容写到另一个Output
			 * @param pOutput 目标Output
			 */
			void	writeTo(CIndexOutput* pOutput);

		protected:
			/**
			 * 实现写buffer操作，在当前位置写数据
			 * @param b 数据地址
			 * @param len b的长度
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
			 * 判断文件是否存在
			 * @param name 文件名
			 */
			bool			fileExists(const tstring& name) const;

			/**
			 * 获取文件的修改日期
			 * @param name 文件名
			 */
			int64_t			fileModified(const tstring& name) const;

			/**
			 * 获取文件长度
			 * @param name 文件名
			 */
			int64_t			fileLength(const tstring& name) const;

			/**
			 * 列出目录下所有文件
			 * @return 文件列表数组
			 * @memory 非内部对象,外部删除
			 */
			FileList*		list();

			/**
			 * 打开文件
			 * @param name 文件名
			 */
			CIndexInput*	openInput(const tstring& name);

			/**
			 * 使用指定的文件读缓存打开文件
			 * @param name 文件名
			 * @param buf 提供给返回的CIndexInput对象的可用内存
			 * @param bufSize buf的大小
			 */
			CIndexInput*	openInput(const tstring& name,char* buf,size_t bufsize);

			/**
			 * 删除文件
			 * @param filename 全路径文件名，带扩展名
			 * @param throwError 删除文件出错是否抛出异常
			 */
			void			deleteFile(const tstring& filename,bool throwError = true);

			/**
			 * 修改文件名
			 * @param from 原文件名
			 * @param to 目标文件名
			 */
			void			renameFile(const tstring& from, const tstring& to);

			/**
			 * 批量删除，将删除所有名字为filename的文件，相当于删除文件filenam.*
			 * @param filename 相对于本Directory的文件名，不带扩展名
			 * @param throwError 删除文件出错是否抛出异常
			 */
			void			batDeleteFiles(const tstring& filename,bool throwError = true);

			/**
			 * 文件拷贝，将文件的文件从pSrcFile拷贝到filename
			 * @param filename  相对于本Directory的径文件名，不带扩展名
			 * @param throwError 拷贝文件出错是否抛出异常
			 */
			void			copyFile(const tstring& filename,CIndexInput* pSrcFile,bool throwError = true);

			/** 
			 * 批量拷贝文件，实现copy from.* to.*
			 * @param pFromDir 源目录
			 * @param from 相对于pFromDir的文件名,不带扩展名
			 * @param to 相对于自身目录的文件名，不带扩展名
			 * @param throwError 拷贝文件出错时是否抛出异常
			 */
			void			batCopyFiles(CDirectory*pFromDir,const tstring& from,const tstring& to,bool throwError = true);

			/**
			 * 批量修改文件名，将from.* 改名成to.*
			 * @param from 相对于本Directory的源文件名，不带扩展名
			 * @param to 相对于本Directory的目标文件名，不带扩展名
			 */
			void			batRenameFiles(const tstring& from, const tstring& to);
			
			/**
			 * 创建文件
			 * @param name 文件名
			 */
			CIndexOutput*	createOutput(const tstring& name);

			/**
			 * 将文件得修改时间设置成当前时间
			 * @param name 文件名
			 */
			void			touchFile(const tstring& name);

			/**
			 * 关闭存储对象
			 */
			void			close();

		protected:
			void	fromDirectory(CDirectory* pDir,bool bCloseDir);
		private:			
			map<tstring,CRAMFile*>		m_files;		//文件
		};
	}
}


#endif
