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
// Created	: 2007/1/9
//

#ifndef _GZTWCOLLECTION_H
#define _GZTWCCOLLECTION_H

#include "TWCollection.h"
#include "../../src/utility/zlib/zlib.h"

namespace firtex
{
	namespace collection
	{
		class CGZTWCollection :	public CTWCollection
		{
		public:		
			class Creator : public CTWCollection::Creator
			{
			public:
				Creator()
				{
					m_identifier += _T(".gz");
				}
				~Creator(){}
			public:
				CCollection* create(const tchar* path){return new CGZTWCollection(path);}

			public:
				static Creator* getInstance()
				{
					static CGZTWCollection::Creator INSTANCE;
					return &INSTANCE;
				}
			};
		public:
			/**
			* 构造函数
			* @param dir 文档目录			 
			*/
			CGZTWCollection(const tchar* dir);

			/**
			* 构造函数
			* @param dir 文档目录
			* @param pIndexWriter Index Writer对象
			*/
			CGZTWCollection(const tchar* dir,CIndexWriter* pWriter);			
			virtual ~CGZTWCollection(void);
		protected:
			/**
			* enumerate directory and index files in it
			* @param dir directory path
			*/
			virtual void	indexDocs(const tchar* dir);
		protected:
			/**
			* parse a .gz document
			* @param filename path of document
			*/
			inline void		parseGZFile(const tchar* filename);
		};

		//////////////////////////////////////////////////////////////////////////
		///
#define TW_BLOCKSIZE     1048575//32767

		inline void CGZTWCollection::parseGZFile(const tchar* filename)
		{
			if(!filename || !(*filename))
				return ;           
			gzFile f = NULL;
			int32_t fileHandle = -1;
			try
			{
				fileHandle = ::fileOpen(filename, O_BINARY | O_RDONLY | O_RANDOM, _S_IREAD );
				if (fileHandle < 0)
				{
					FIRTEX_CLOG(level::warn) << _T("Open file error:") << filename << FIRTEX_ENDL;
					return ;
				}			
				f = gzdopen(fileHandle,"rb");
				if(f == NULL)
				{
					FIRTEX_CLOG(level::warn) << _T("Open .gz file error:") << filename << FIRTEX_ENDL;
					return ;
				}			
				size_t length,fileLength;
				size_t guess = 6;
				if(m_nBufSize == 0)
				{
					fileLength = (size_t)fileSize(fileHandle);
					m_nBufSize = fileLength*guess;
					if(m_nBufSize <= TW_BLOCKSIZE)
						m_nBufSize = TW_BLOCKSIZE + 1;
					m_pContentBuffer = new char[m_nBufSize];
				}
				length = 0;
				size_t nRead = gzread(f,m_pContentBuffer + length,TW_BLOCKSIZE);
				while (nRead > 0)
				{
					length += nRead;
					if((length + TW_BLOCKSIZE) >= m_nBufSize)
					{	
						fileLength = (size_t)fileSize(fileHandle);
						guess++;						
						size_t newSize = fileLength*guess;
						if(newSize < length + TW_BLOCKSIZE + 1)
							newSize = length + TW_BLOCKSIZE + 1;
						char* newBuff = new char[newSize];
						if(m_pContentBuffer)
						{						
							memcpy(newBuff,m_pContentBuffer,length);
							delete[] m_pContentBuffer;
						}
						m_pContentBuffer = newBuff;
						m_nBufSize = newSize;
					}
					nRead = gzread(f,m_pContentBuffer + length,TW_BLOCKSIZE);
				}			
				if(nRead == -1)
				{
					FIRTEX_CLOG(level::warn) << _T("Decompress .gz file error:") << filename << FIRTEX_ENDL;
				}
				///read content
				::fileClose(fileHandle);
				fileHandle = -1;
				gzclose(f);
				f = NULL;

				m_pContentBuffer[length] = 0;
				parseBuffer(m_pContentBuffer,length);
			}
			catch (CFirteXException& e)
			{
				if(fileHandle != -1)
					::fileClose(fileHandle);
				if(f)
					gzclose(f);
				FIRTEX_CLOG(level::warn) << _T("CGZTWCollection::parseGZFile() failed:") << e.what() << _T(". file:") << filename << FIRTEX_ENDL;
			}			
		}	
	}
}

#endif
