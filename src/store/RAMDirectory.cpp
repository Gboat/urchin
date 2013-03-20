//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/7/2
//
#include "store/RAMDirectory.h"
#include "store/FSDirectory.h"
#include "utility/Config.h"

namespace firtex
{
	namespace store
	{
		//////////////////////////////////////////////////////////////////////////
		//
		CRAMIndexInput::CRAMIndexInput(CRAMFile* file)
			:CIndexInput(0)
			,m_file(file)
			,m_pointer(0)
		{
			m_length = file->m_length;
		}
		CRAMIndexInput::CRAMIndexInput(CRAMFile* file,char* buffer,size_t bufSize) 
			:CIndexInput(buffer,bufSize)
			,m_file(file)			
			,m_pointer(0)
		{
			m_length = file->m_length;
		}
		CRAMIndexInput::CRAMIndexInput(const CRAMIndexInput& clone)
			:m_file(clone.m_file)			
			,m_pointer(clone.m_pointer)
		{
			 m_length = clone.m_length;
		}
		CRAMIndexInput::~CRAMIndexInput()
		{
			close();
		}
		void CRAMIndexInput::readInternal(char* b,size_t length,bool bCheck)
		{
			const int64_t bytesAvailable = m_file->m_length - m_pointer;
			int64_t remainder = (length <= bytesAvailable) ? length : bytesAvailable;
			int64_t start = m_pointer;
			size_t destOffset = 0;
			int32_t bufferNumber;
			int32_t bufferOffset;
			int32_t bytesInBuffer;
			int32_t bytesToCopy;
			byte* buffer ;
			while (remainder != 0) 
			{
				bufferNumber = (int32_t)(start / m_file->m_streamBufSize);
				bufferOffset = (int32_t)(start % m_file->m_streamBufSize);
				bytesInBuffer = m_file->m_streamBufSize- bufferOffset;
				
				bytesToCopy = bytesInBuffer >= remainder ? static_cast<int32_t>(remainder) : bytesInBuffer;
				buffer = m_file->m_buffers[bufferNumber];

				memcpy(b+destOffset,buffer+bufferOffset,bytesToCopy * sizeof(byte));

				destOffset += bytesToCopy;
				start += bytesToCopy;
				remainder -= bytesToCopy;
				m_pointer += bytesToCopy;
			}
		}
		CIndexInput* CRAMIndexInput::clone(char* buffer,size_t buffsize)
		{
			CRAMIndexInput* pInput = new CRAMIndexInput(this->m_file,buffer,buffsize);
			pInput->m_length = m_length;
			pInput->m_pointer = m_pointer;
			return pInput;
		}
		
		CIndexInput* CRAMIndexInput::clone()
		{
			return new CRAMIndexInput(*this);
		}		
		void CRAMIndexInput::close()
		{
			m_file = 0;
			m_length = 0;
			m_pointer = 0;
		}
		void CRAMIndexInput::seekInternal(int64_t position)
		{
			if(position < 0 || position > m_length)
				FIRTEX_THROW2(FILEIO_ERROR,"CRAMIndexInput::seekInternal():file IO seek error. ");
			m_pointer = position;
		}

		//////////////////////////////////////////////////////////////////////////
		//CRAMIndexOutput
		CRAMIndexOutput::CRAMIndexOutput()
		{
			m_file = new CRAMFile();
			m_bDeleteFile = true;
			m_pointer = 0;
		}
		CRAMIndexOutput::CRAMIndexOutput(CRAMFile* ramFile)
			:m_file(ramFile)
			,m_bDeleteFile(false)
			,m_pointer(0)
		{

		}
		CRAMIndexOutput::~CRAMIndexOutput()
		{
			if(m_bDeleteFile)
				delete m_file;
			m_file = NULL;
		}		

		void CRAMIndexOutput::flushBuffer(char* b, size_t len)
		{
			byte * buffer = NULL;
			size_t bufferPos = 0;
			int32_t bufferNumber;
			int32_t bufferOffset;
			int32_t bytesInBuffer;
			int32_t remainInSrcBuffer;
			int32_t bytesToCopy;
			while (bufferPos != len) 
			{
				bufferNumber = (int32_t)(m_pointer/m_file->m_streamBufSize);
				bufferOffset = (int32_t)(m_pointer%m_file->m_streamBufSize);
				bytesInBuffer = m_file->m_streamBufSize - bufferOffset;
				remainInSrcBuffer = (int32_t)(len - bufferPos);
				bytesToCopy = bytesInBuffer >= remainInSrcBuffer ? remainInSrcBuffer : bytesInBuffer;

				if (bufferNumber == m_file->m_buffers.size())
				{
					buffer = new byte[m_file->m_streamBufSize];
					m_file->m_buffers.push_back(buffer);
				}
				else
				{
					buffer = m_file->m_buffers[bufferNumber];	
				}
				memcpy(buffer+bufferOffset, b + bufferPos, bytesToCopy * sizeof(byte));
				bufferPos += bytesToCopy;
				m_pointer += bytesToCopy;
			}
			if (m_pointer > m_file->m_length)
				m_file->m_length = m_pointer;

			m_file->m_lastModified = firtex::utility::CMisc::currentTimeMillis();
		}
		void CRAMIndexOutput::seek(int64_t pos)
		{
			CIndexOutput::seek(pos);
			m_pointer = pos;
		}
		void CRAMIndexOutput::close()
		{
			CIndexOutput::close();
		}

		void CRAMIndexOutput::writeTo(CIndexOutput* pOutput)
		{
			flush();
			int64_t end = m_file->m_length;
			int64_t pos = 0;
			int32_t buffer = 0;
			int32_t length ;
			int64_t nextPos ;
			while (pos < end) 
			{
				length = m_file->m_streamBufSize;
				nextPos = pos + length;
				if (nextPos > end) 
				{                        // at the last buffer
					length = (int32_t)(end - pos);
				}
				pOutput->write((const char*)m_file->m_buffers[buffer++], length);
				pos = nextPos;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//CRAMDirectory
		CRAMDirectory::CRAMDirectory(void)
		{
		}

		CRAMDirectory::CRAMDirectory(const tchar* dir)
		{
			CDirectory* pDir = CFSDirectory::getDirectory(dir,false);
			try
			{
				fromDirectory(pDir,false);
			}
			catch(...)
			{
				pDir->close();
				throw;
			}
			pDir->close();
		}
		CRAMDirectory::CRAMDirectory(CDirectory* pDir)
		{
			fromDirectory(pDir,false);
		}

		CRAMDirectory::~CRAMDirectory(void)
		{
			close();
		}

		//CDirectory* CRAMDirectory::clone()
		//{
		//	CRAMDirectory* pRAMDir = new CRAMDirectory;
		//	
		//	//TODO:lock it
		//	map<tstring,CRAMFile*>::iterator iter = m_files.begin();
		//	while ( iter != m_files.end() )
		//	{
		//		pRAMDir->m_files.insert(make_pair(iter->first,iter->second->clone()));
		//		iter++;
		//	}			
		//	return pRAMDir;
		//}


		void CRAMDirectory::fromDirectory(CDirectory* pDir,bool bCloseDir)
		{
			FileList* files = pDir->list();

			tstring str;
			int64_t len;
			int32_t	streamBufSize;
			int64_t readCount;
			int32_t toRead ;
			CIndexOutput* pOutput;
			CIndexInput* pInput;
			FileList::iterator iter = files->begin();
			while (iter != files->end())
			{
				str = (*iter);
				
				pOutput = createOutput(str);
				pInput = pDir->openInput(str);
				len = pInput->length();
				streamBufSize = ((CRAMIndexOutput*)pOutput)->m_file->m_streamBufSize;
				while ( (len/streamBufSize) > RAMFILE_MAXSEGS)
				{
					streamBufSize *= 2;
				}
				((CRAMIndexOutput*)pOutput)->m_file->m_streamBufSize = streamBufSize;
				
				readCount = 0;
				while (readCount < len)
				{
					toRead = (int32_t)( ((readCount + streamBufSize) > len) ? len - readCount : streamBufSize);					
					pOutput->write(pInput,toRead);
					readCount += toRead;
				}

				iter++;
			}		
		}
		
		bool CRAMDirectory::fileExists(const tstring& name) const
		{
			return (m_files.find(name) != m_files.end());
		}
		
		int64_t CRAMDirectory::fileModified(const tstring& name) const
		{
			map<tstring,CRAMFile*>::const_iterator iter = m_files.find(name);
			if(iter != m_files.end())
			{
				return iter->second->m_lastModified;
			}
			return -1;
		}
		
		int64_t CRAMDirectory::fileLength(const tstring& name) const
		{
			map<tstring,CRAMFile*>::const_iterator iter = m_files.find(name);
			if(iter != m_files.end())
			{
				return iter->second->m_length;
			}
			return -1;
		}
		
		FileList* CRAMDirectory::list()
		{
			FileList* fileList = new FileList();
			map<tstring,CRAMFile*>::iterator iter = m_files.begin();
			while(iter != m_files.end())
			{
				fileList->push_back(iter->first);
				iter++;
			}
			return fileList;
		}
		
		CIndexInput* CRAMDirectory::openInput(const tstring& name)
		{
			map<tstring,CRAMFile*>::iterator iter = m_files.find(name);
			if(iter != m_files.end())
			{
				return new CRAMIndexInput(iter->second);
			}
			return NULL;
		}
		
		CIndexInput* CRAMDirectory::openInput(const tstring& name,char* buf,size_t bufsize)
		{
			map<tstring,CRAMFile*>::iterator iter = m_files.find(name);
			if(iter != m_files.end())
			{
				return new CRAMIndexInput(iter->second);
			}
			return NULL;
		}

		void CRAMDirectory::deleteFile(const tstring& filename,bool throwError)
		{
			//TODO:LOCK IT
			map<tstring,CRAMFile*>::iterator iter = m_files.find(filename);
			if(iter != m_files.end())
			{
				delete iter->second;
				m_files.erase(iter);
			}
		}		
		void CRAMDirectory::renameFile(const tstring& from, const tstring& to)
		{
			map<tstring,CRAMFile*>::iterator iter = m_files.find(from);
			if(iter != m_files.end())
			{
				m_files.insert(make_pair(to,iter->second));
				m_files.erase(iter);
			}
		}

		void CRAMDirectory::batDeleteFiles(const tstring& filename,bool throwError)
		{		
			try
			{
				map<tstring,CRAMFile*>::iterator iter = m_files.begin();
				tstring str;
				tstring::size_type npos = (size_t)-1;
				tstring::size_type pos ;
				while(iter != m_files.end())
				{
					str = iter->first;
					pos = str.rfind(_T('.'));
					if(pos != npos)
						str = str.substr(0,pos);
					if(str == filename)
					{
						delete iter->second;
						m_files.erase(iter++);//TODO:
						continue;
						//str.clear();
					}
					iter++;
				}		
			}
			catch (...)
			{
				if(throwError)
					FIRTEX_THROW2(FILEIO_ERROR,"void CRAMDirectory::batDeleteFiles()");
				else 
					FIRTEX_CLOG(level::warn) << " delete files error:" << filename << FIRTEX_ENDL;
			}			
		}
		
		void CRAMDirectory::copyFile(const tstring& filename,CIndexInput* pSrcFile,bool throwError)
		{
			CIndexOutput* pOutput = NULL;
			try
			{
				pOutput = createOutput(filename);
				pOutput->write(pSrcFile,pSrcFile->length());
				delete pOutput;
				pOutput = NULL;
			}
			catch (CFirteXException& e)
			{
				if(pOutput)
					delete pOutput;
				if(throwError)
					FIRTEX_RETHROW(e);
				else 
				{
					FIRTEX_CLOG(level::warn) << " copy file error:" << filename << FIRTEX_ENDL;
				}
			}
		}
		
		void CRAMDirectory::batCopyFiles(CDirectory*pFromDir,const tstring& from,const tstring& to,bool throwError)
		{
			CIndexInput* pInput = NULL;
			CIndexOutput* pOutput = NULL;
			const size_t buflen = 1048576;
			char* buf = NULL;
			tstring::size_type npos = (size_t)-1;
			tstring::size_type pos;
			tstring path,fname,fext,s;
			try
			{
				buf = new char[buflen];
				FileList* fileList = pFromDir->list();
				if(fileList == NULL)
					return;

				for(int i = 0;i<(int)fileList->size();i++)
				{
					s = (*fileList)[i];
					pos = s.rfind(_T('.'));
					if(pos != npos)
					{
						fname = s.substr(0,pos);
						fext = s.substr(pos,s.length()-pos);
					}	
					if(fname == from)
					{
						pInput = pFromDir->openInput(from+fext);
						pOutput = createOutput(to+fext);
						pOutput->setBuffer(buf,buflen);
						pOutput->write(pInput,pInput->length());
						delete pOutput;
						pOutput = NULL;
						delete pInput;
						pInput = NULL;
						fname.clear();
					}
				}

				delete fileList;				
				delete[] buf;
				buf = NULL;
			}
			catch (CFirteXException& fe)
			{				
				if(buf)
					delete[] buf;
				if(pInput)
					delete pInput;
				if(pOutput)
					delete pOutput;

				if(throwError)
					FIRTEX_RETHROW(fe);
				else FIRTEX_CLOG(level::warn) << _T("Copy files error:void CFSDirectory::batCopyFiles()") << FIRTEX_ENDL;
			}					
			catch (...) 
			{
				if(buf)
					delete[] buf;
				if(pInput)
					delete pInput;
				if(pOutput)
					delete pOutput;
				if(throwError)
					FIRTEX_THROW2(UNKNOWN_ERROR,_T("void CFSDirectory::batCopyFiles(CDirectory*pFromDir,const tstring& from,const tstring& to,bool throwError)"));
				else FIRTEX_CLOG(level::warn) << _T("Copy files error:void CFSDirectory::batCopyFiles()") << FIRTEX_ENDL;
			}
		}
		
		void CRAMDirectory::batRenameFiles(const tstring& from, const tstring& to)
		{
			tstring path,fname,fext,s;
			tstring::size_type npos = (size_t)-1;
			tstring::size_type pos ;
			map<tstring,CRAMFile*>::iterator iter = m_files.begin();
			while ( iter != m_files.end() )
			{
				s = iter->first;
				pos = s.rfind(_T('.'));
				if(pos != npos)
				{
					fname = s.substr(0,pos);
					fext = s.substr(pos,s.length()-pos);
				}	
				if(fname == from)
				{
					renameFile(s,to+fext);
					fname.clear();
				}
				iter++;
			}
		}
		
		CIndexOutput* CRAMDirectory::createOutput(const tstring& name)
		{
			//TODO:LOCK IT
			map<tstring,CRAMFile*>::iterator iter = m_files.find(name);

			if (iter != m_files.end())
			{
				CRAMFile* rf = iter->second;
				delete rf;
				m_files.erase(iter);
			} 			

			CRAMFile* file = new CRAMFile();
			m_files[name] = file;

			CIndexOutput* ret = new CRAMIndexOutput(file);
			return ret;
		}		
		void CRAMDirectory::touchFile(const tstring& name)
		{
			map<tstring,CRAMFile*>::iterator iter = m_files.find(name);
			if(iter != m_files.end())
			{
				iter->second->m_lastModified = CMisc::currentTimeMillis();				
			}
		}
		
		void CRAMDirectory::close()
		{
			//TODO:lock it
			map<tstring,CRAMFile*>::iterator iter = m_files.begin();
			while ( iter != m_files.end() )
			{
				delete iter->second;
				iter++;
			}
			m_files.clear();
		}		
	}
}

