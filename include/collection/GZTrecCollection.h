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

#ifndef _GZTRECCOLLECTION_H
#define _GZTRECCOLLECTION_H

//#define GUNZIP 1


#include "TrecCollection.h"
#include "../../src/utility/zlib/zlib.h"

#ifdef USE_MMAP
	#include "../utility/MMapFile.h"
	using namespace firtex::utility;
#endif


namespace firtex
{
	namespace collection
	{
		class CGZTrecCollection :	public CTrecCollection
		{
		public:		
			class Creator : public CTrecCollection::Creator
			{
			public:
				Creator()
				{
					m_identifier += _T(".gz");
				}
				~Creator()
				{
				}
			public:
				CCollection* create(const tchar* path){return new CGZTrecCollection(path);}

			public:
				static Creator* getInstance()
				{
					static Creator INSTANCE;
					return &INSTANCE;
				}
			};

		public:
			/**
			* 构造函数
			* @param dir 文档目录			 
			*/
			CGZTrecCollection(const tchar* dir);

			/**
			* 构造函数
			* @param dir 文档目录
			* @param pIndexWriter Index Writer对象
			*/
			CGZTrecCollection(const tchar* dir,CIndexWriter* pWriter);			
			virtual ~CGZTrecCollection(void);
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
#ifdef USE_MMAP
			inline void		parseGZFile_MMap(const tchar* filename);
			inline bool		check_header(z_stream& s);
			inline int		get_byte(z_stream& s);
#endif
		protected:
#ifdef USE_MMAP
			CMMapFile*		m_pMMapFile;
#endif
		};

		//////////////////////////////////////////////////////////////////////////
		///
#define BLOCKSIZE     1048575 //32768

		inline void CGZTrecCollection::parseGZFile(const tchar* filename)
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
				if(m_length == 0)
				{
					fileLength = (size_t)fileSize(fileHandle);
					m_length = fileLength*guess;
					if(m_length <= BLOCKSIZE)
						m_length = BLOCKSIZE + 1;
					m_contentBuffer = new char[m_length];
				}
				length = 0;
				size_t nRead = gzread(f,m_contentBuffer + length,BLOCKSIZE);
				while (nRead > 0)
				{
					length += nRead;
					if((length + BLOCKSIZE) >= m_length)
					{	
						fileLength = (size_t)fileSize(fileHandle);
						guess++;						
						size_t newSize = fileLength*guess;
						if(newSize < length + BLOCKSIZE + 1)
							newSize = length + BLOCKSIZE + 1;
						char* newBuff = new char[newSize];
						if(m_contentBuffer)
						{						
							memcpy(newBuff,m_contentBuffer,length);
							delete[] m_contentBuffer;
						}
						m_contentBuffer = newBuff;
						m_length = newSize;
					}
					nRead = gzread(f,m_contentBuffer + length,BLOCKSIZE);
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

				m_contentBuffer[length] = 0;
				parseBuffer(m_contentBuffer,length);
			}
			catch (CFirteXException& e)
			{
				if(fileHandle != -1)
					::fileClose(fileHandle);
				if(f)
					gzclose(f);
				FIRTEX_CLOG(level::warn) << _T("CGZTrecCollection::parseGZFile() failed:") << e.what() << _T(". file:") << filename << FIRTEX_ENDL;
			}			
		}

#ifdef USE_MMAP

		inline void CGZTrecCollection::parseGZFile_MMap(const tchar* filename)
		{
			if(!filename || !(*filename))
				return ;           			
			try
			{
				if(m_pMMapFile)
					m_pMMapFile->open(filename);
				else m_pMMapFile = new CMMapFile(filename);
				
				size_t length,fileLength;
				size_t guess = 6;
				if(m_length == 0)
				{
					fileLength = (size_t)m_pMMapFile->length();
					m_length = fileLength*guess;
					if(m_length <= BLOCKSIZE)
						m_length = BLOCKSIZE + 1;
					m_contentBuffer = new char[m_length];
				}
				unsigned char* in = (unsigned char*)m_pMMapFile->data();
				unsigned char* out = (unsigned char*)m_contentBuffer;
				size_t nInLen = (size_t)m_pMMapFile->length();
				size_t nOutLen = m_length;				
				length = 0;

				int ret;
				z_stream strm;				

				/* allocate inflate state */
				strm.zalloc = Z_NULL;
				strm.zfree = Z_NULL;
				strm.opaque = Z_NULL;
				strm.avail_in = 0;
				strm.next_in = Z_NULL;
				/*strm.avail_in = (uInt)nInLen;
				strm.next_in = in;*/
				//ret = inflateInit(&strm);
				ret = inflateInit2(&strm, -MAX_WBITS);
				if (ret != Z_OK)
				{		
					FIRTEX_CLOG(level::warn) << _T("Decompress .gz file error:") << filename << FIRTEX_ENDL;
					return;
				}

				/* decompress until deflate stream ends or end of file */				
				strm.avail_in = (uInt)nInLen;									
				strm.next_in = in;

				if(check_header(strm) == false)
				{
					FIRTEX_CLOG(level::warn) << _T("not a .gz file:") << filename << FIRTEX_ENDL;
					return;
				}

				/* run inflate() on input until output buffer not full */

				strm.avail_out = (uInt)nOutLen;
				strm.next_out = out;
				do 
				{
					length += (nOutLen - strm.avail_out);
					nOutLen = strm.avail_out;
					if((length + BLOCKSIZE) >= m_length)
					{
						guess++;						
						size_t newSize = fileLength*guess;
						if(newSize < length + BLOCKSIZE + 1)
							newSize = length + BLOCKSIZE + 1;
						char* newBuff = new char[newSize];
						if(m_contentBuffer)
						{						
							memcpy(newBuff,m_contentBuffer,length);
							delete[] m_contentBuffer;
						}
						m_contentBuffer = newBuff;
						m_length = newSize;
						out = (unsigned char*)(m_contentBuffer + length);
						nOutLen = m_length - length;
						strm.avail_out = (uInt)nOutLen;
						strm.next_out = out;
					}					
					ret = inflate(&strm, Z_NO_FLUSH);
				} while (ret == Z_OK);		

				if(ret == Z_STREAM_END)
				{
					length += (nOutLen - strm.avail_out);
				}

				/* clean up and return */
				ret = inflateEnd(&strm);				
				if(ret != Z_OK)
				{
					FIRTEX_CLOG(level::warn) << _T("Decompress .gz file error:") << filename << FIRTEX_ENDL;
				}				

				m_contentBuffer[length] = 0;
				parseBuffer(m_contentBuffer,length);
			}
			catch (CFirteXException& e)
			{				
				FIRTEX_CLOG(level::warn) << _T("CGZTrecCollection::parseGZFile() failed:") << e.what() << _T(". file:") << filename << FIRTEX_ENDL;
			}			
		}

		static int const gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */
		/* gzip flag byte */
		#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
		#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
		#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
		#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
		#define COMMENT      0x10 /* bit 4 set: file comment present */
		#define RESERVED     0xE0 /* bits 5..7: reserved */

		inline bool CGZTrecCollection::check_header(z_stream& s)
		{
			int method; /* method byte */
			int flags;  /* flags byte */
			uInt len;
			int c;

			/* Assure two bytes in the buffer so we can peek ahead -- handle case
			where first byte of header is at the end of the buffer after the last
			gzip segment */
			len = s.avail_in;
			if (len < 2) 
			{
				/*if (len)
					s->inbuf[0] = s.next_in[0];
				errno = 0;
				len = (uInt)fread(s->inbuf + len, 1, Z_BUFSIZE >> len, s->file);
				if (len == 0 && ferror(s->file)) s->z_err = Z_ERRNO;
				s->stream.avail_in += len;
				s->stream.next_in = s->inbuf;
				if (s->stream.avail_in < 2) {
					s->transparent = s->stream.avail_in;
					return;
				}*/
				return false;
			}

			/* Peek ahead to check the gzip magic header */
			if (s.next_in[0] != gz_magic[0] ||
				s.next_in[1] != gz_magic[1]) 
			{
				return false;
			}
			s.avail_in -= 2;
			s.next_in += 2;
            
			/* Check the rest of the gzip header */			
			method = get_byte(s);
			flags = get_byte(s);
			if (method != Z_DEFLATED || (flags & RESERVED) != 0) 
			{
				//s->z_err = Z_DATA_ERROR;
				return false;
			}
            
			/* Discard time, xflags and OS code: */
			for (len = 0; len < 6; len++)
				(void)get_byte(s);
            
			if ((flags & EXTRA_FIELD) != 0) 
			{ /* skip the extra field */
				len  =  (uInt)get_byte(s);
				len += ((uInt)get_byte(s))<<8;
				/* len is garbage if EOF but the loop below will quit anyway */
				while (len-- != 0 && get_byte(s) != EOF) ;
			}
			if ((flags & ORIG_NAME) != 0) 
			{ /* skip the original file name */
				while ((c = get_byte(s)) != 0 && c != EOF) ;
			}
			if ((flags & COMMENT) != 0) 
			{   /* skip the .gz file comment */
				while ((c = get_byte(s)) != 0 && c != EOF) ;
			}
			if ((flags & HEAD_CRC) != 0)
			{  /* skip the header crc */
				for (len = 0; len < 2; len++) (void)get_byte(s);
			}
			return (s.avail_in > 0)?true:false;
			//s->z_err = s->z_eof ? Z_DATA_ERROR : Z_OK;
		}
		
		inline int CGZTrecCollection::get_byte(z_stream& s)
		{			
			if (s.avail_in == 0) 
			{
				return EOF;
			}			
			s.avail_in--;
			return *(s.next_in)++;
		}
#endif
	}
}

#endif
