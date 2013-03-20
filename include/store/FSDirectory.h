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
			 * 列出目录下所有文件
			 */
			FileList*		list();

			/**
			 * 判断文件是否存在
			 */
			bool			fileExists(const tstring& name) const;
			
			/**
			 * 获取文件的修改日期
			 */
			int64_t			fileModified(const tstring& name) const;

			/**
			 * 获取文件长度
			 */
			int64_t			fileLength(const tstring& name) const;
			/**
			 * 打开文件
			 */
			CIndexInput*	openInput(const tstring& name);

			/**
			 * 使用指定的文件读缓存打开文件
			 */
			CIndexInput*	openInput(const tstring& name,char* buf,size_t bufsize);

			/**
			 * 删除文件
			 */
			void			deleteFile(const tstring& filename,bool throwError = true);
			/**
			 * 修改文件名
			 */
			void			renameFile(const tstring& from, const tstring& to);

			/**
			 * 批量删除，将删除所有名字为filename的文件，相当于删除文件filenam.*
			 * @param filename  相对于本Directory的径文件名，不带扩展名
			 * @param throwError 删除文件出错是否抛出异常
			 */
			void			batDeleteFiles(const tstring& filename,bool throwError = true);

			/**
			 * 批量改名，将文件的文件from.*改成to.*
			 * @param filename  相对于本Directory的径文件名，不带扩展名
			 * @param throwError 删除文件出错是否抛出异常
			 */
			void			batRenameFiles(const tstring& from, const tstring& to);

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
			 * 创建文件
			 */
			CIndexOutput*	createOutput(const tstring& name);

			/**
			 * 将文件得修改时间设置成当前时间
			 */
			void			touchFile(const tstring& name);

			///**
			// * 克隆一个新的对象
			// * @return 克隆对象
			// * @memory 非内部对象,外部删除
			// */
			//CDirectory*		clone();

			/**
			 * 关闭存储对象
			 */
			void			close();

		protected:
			void			create();
		protected:
			tstring		m_directory;
			int			m_nRefCount;//引用计数
		};
	}
}


#endif
