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
			 * 判断文件是否存在
			 * @param name 文件名
			 */
			virtual bool			fileExists(const tstring& name) const = 0;
		
			/**
			 * 获取文件的修改日期
			 * @param name 文件名
			 */
			virtual int64_t			fileModified(const tstring& name) const = 0;

			/**
			 * 获取文件长度
			 * @param name 文件名
			 */
			virtual int64_t			fileLength(const tstring& name) const = 0;

			/**
			 * 列出目录下所有文件
			 * @return 文件列表数组
			 * @memory 非内部对象,外部删除
			 */
			virtual FileList*		list() = 0;

			/**
			 * 打开文件
			 * @param name 文件名
			 */
			virtual CIndexInput*	openInput(const tstring& name) = 0;

			/**
			 * 使用指定的文件读缓存打开文件
			 * @param name 文件名
			 * @param buf 提供给返回的CIndexInput对象的可用内存
			 * @param bufSize buf的大小
			 */
			virtual CIndexInput*	openInput(const tstring& name,char* buf,size_t bufsize) = 0;

			/**
			 * 删除文件
			 * @param filename 相对路径文件名，带扩展名
			 * @param throwError 删除文件出错是否抛出异常
			 */
			virtual void			deleteFile(const tstring& filename,bool throwError = true) = 0;

			/**
			 * 修改文件名
			 * @param from 原文件名
			 * @param to 目标文件名
			 */
			virtual void			renameFile(const tstring& from, const tstring& to) = 0;

			/**
			 * 批量删除，将删除所有名字为filename的文件，相当于删除文件filenam.*
			 * @param filename 相对于本Directory的文件名，不带扩展名
			 * @param throwError 删除文件出错是否抛出异常
			 */
			virtual void			batDeleteFiles(const tstring& filename,bool throwError = true) = 0;

			/**
			 * 文件拷贝，将文件的文件从pSrcFile拷贝到filename
			 * @param filename  相对于本Directory的径文件名，不带扩展名
			 * @param throwError 拷贝文件出错是否抛出异常
			 */
			virtual void			copyFile(const tstring& filename,CIndexInput* pSrcFile,bool throwError = true) = 0;

			/** 
			 * 批量拷贝文件，实现copy from.* to.*
			 * @param pFromDir 源目录
			 * @param from 相对于pFromDir的文件名,不带扩展名
			 * @param to 相对于自身目录的文件名，不带扩展名
			 * @param throwError 拷贝文件出错时是否抛出异常
			 */
			virtual void			batCopyFiles(CDirectory*pFromDir,const tstring& from,const tstring& to,bool throwError = true) = 0;

			/**
			 * 批量修改文件名，将from.* 改名成to.*
			 * @param from 相对于本Directory的源文件名，不带扩展名
			 * @param to 相对于本Directory的目标文件名，不带扩展名
			 */
			virtual void			batRenameFiles(const tstring& from, const tstring& to) = 0;
			
			/**
			 * 创建文件
			 * @param name 文件名
			 */
			virtual CIndexOutput*	createOutput(const tstring& name) = 0;

			/**
			 * 将文件得修改时间设置成当前时间
			 * @param name 文件名
			 */
			virtual void			touchFile(const tstring& name) = 0;

			/**
			 * 关闭存储对象
			 */
			virtual void			close() = 0;		
		};
	}
}

#endif
