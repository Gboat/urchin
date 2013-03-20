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
#include "store/FSDirectory.h"
#include "store/FSIndexInput.h"
#include "store/FSIndexOutput.h"
#include "utility/dirent.h"
#include "utility/Config.h"
#include "utility/Misc.h"
#include <map>
using namespace std;
using namespace firtex::utility;

namespace firtex
{
	namespace store
	{
		//同一个索引只允许存在一个Directory对象

		CFSDirectory::CFSDirectory(const tstring& path,bool bCreate)
			: m_nRefCount(0)
		{
			m_directory = path;
			if (bCreate) 
			{
				create();
			}

			if (!CMisc::dirExists(m_directory.c_str()))
			{
				tstring s = m_directory;
				s += _T(" is not a directory.");
				FIRTEX_THROW2(FILEIO_ERROR,s);
			}
		}
		
		CFSDirectory::~CFSDirectory(void)
		{
		}


		void CFSDirectory::create()
		{
			if ( !CMisc::dirExists(m_directory.c_str()) )
			{				
				if ( _mkdir(m_directory.c_str()) == -1 )
				{
					tstring s = _T("Couldn't create directory: ");
					s += m_directory;
					FIRTEX_THROW2(FILEIO_ERROR,s);
				}
				return;
			}

			//clear old files
			DIR* dir = opendir(m_directory.c_str());
			struct dirent* fl = readdir(dir);
			struct fileStat buf;

			tstring path;
			while ( fl != NULL )
			{
				path = m_directory;
				path += PATH_DELIMITERA;
				path += fl->d_name;				
				int32_t ret = fileStat(path.c_str(),&buf);
				if ( ret==0 && !(buf.st_mode & S_IFDIR) )
				{
					if ( (_tcscmp(fl->d_name, _T("."))) && (_tcscmp(fl->d_name, _T(".."))) )
					{
						if ( _tunlink( path.c_str() ) == -1 ) 
						{
							closedir(dir);
							tstring s;
							s = _T("Couldn't delete file:");
							s += path;
							FIRTEX_THROW2(FILEIO_ERROR,s);
						}
					}
				}
				fl = readdir(dir);
			}
			closedir(dir);			
		}
		
		CFSDirectory* CFSDirectory::getDirectory(const tstring& path,bool bCreate)
		{			
			CFSDirectory* pS = NULL;
			directory_map& dm = getDirectoryMap();
			directory_iterator iter = dm.find(path);
			if(iter == dm.end())
			{
				pS = new CFSDirectory(path,bCreate);
				dm.insert(pair<string,CFSDirectory*>(path,pS));				
			}
			else 
			{
				pS = iter->second;
			}
			pS->m_nRefCount++;
			return pS;
		}

		void CFSDirectory::deleteFile(const tstring& filename,bool throwError)
		{			
			tstring fullpath = m_directory + PATH_DELIMITERA + filename;
			if ( _tunlink(fullpath.c_str()) == -1 && throwError)
			{
				tstring str = _T("deleteFile error:");
				str += fullpath;
				throw firtex::utility::CFileIOException(str);
			}
		}
		void CFSDirectory::renameFile(const tstring& from, const tstring& to)
		{	
			tstring tofullpath = m_directory + PATH_DELIMITERA + to;
			tstring fromfullpath = m_directory + PATH_DELIMITERA + from;
			if ( fileExists(to.c_str()) )
			{
				tstring s = to;
				s += _T(" already exist");
				throw firtex::utility::CFileIOException(s);
				
			}
			else 
			{
				if ( _trename(fromfullpath.c_str(),tofullpath.c_str()) != 0 )
				{
					tstring s;
					s = _T("couldn't rename ");
					s += fromfullpath;
					s += _T(" to ");
					s += tofullpath;
					throw firtex::utility::CFileIOException(s);
				}
			}
		}	
		void CFSDirectory::batDeleteFiles(const tstring& filename,bool throwError)
		{
			DIR* dir = opendir(m_directory.c_str());
			struct dirent* fl = readdir(dir);
			struct fileStat buf;

			tstring path;
			tstring fname;
			tstring::size_type npos = (size_t)-1;
			while ( fl != NULL )
			{
				path = m_directory;
				path += PATH_DELIMITERA;
				path += fl->d_name;				
				int32_t ret = fileStat(path.c_str(),&buf);
				if ( ret==0 && !(buf.st_mode & S_IFDIR) )
				{
					if ( (_tcscmp(fl->d_name, _T("."))) && (_tcscmp(fl->d_name, _T(".."))) )
					{
						fname = fl->d_name;
						tstring::size_type pos = fname.rfind(_T('.'));
						if(pos != npos)
							fname = fname.substr(0,pos);
						if(fname == filename)
						{
							if ( ( _tunlink( path.c_str() ) == -1 ) && (throwError) )
							{
								closedir(dir);                                
								tstring s;
								s = _T("Couldn't delete file:");
								s += path;
								FIRTEX_THROW2(FILEIO_ERROR,s);
							}
							fname.clear();
						}
					}
				}
				fl = readdir(dir);
			}
			closedir(dir);			
		}
		void CFSDirectory::batRenameFiles(const tstring& from, const tstring& to)
		{
			DIR* dir = opendir(m_directory.c_str());
			struct dirent* fl = readdir(dir);
			struct fileStat buf;

			tstring path,fname,fext,s;
			tstring::size_type npos = (size_t)-1;
			while ( fl != NULL )
			{
				path = m_directory;
				path += PATH_DELIMITERA;
				path += fl->d_name;				
				int32_t ret = fileStat(path.c_str(),&buf);
				if ( ret==0 && !(buf.st_mode & S_IFDIR) )
				{
					if ( (_tcscmp(fl->d_name, _T("."))) && (_tcscmp(fl->d_name, _T(".."))) )
					{
						s = fl->d_name;
						tstring::size_type pos = s.rfind(_T('.'));
						if(pos != npos)
						{
							fname = s.substr(0,pos);
							fext = s.substr(pos,s.length()-pos);
						}	
						if(fname == from)
						{
							renameFile(fl->d_name,to+fext);
							fname.clear();
						}
					}
				}
				fl = readdir(dir);
			}
			closedir(dir);			
		}
		bool CFSDirectory::fileExists(const tstring& name) const
		{
			tstring s = m_directory + PATH_DELIMITERA + name;
			return CMisc::dirExists(s.c_str());			
		}		
		int64_t CFSDirectory::fileModified(const tstring& name) const
		{
			struct fileStat buf;
			tstring fullpath = m_directory + PATH_DELIMITERA + name;
			if ( fileStat(fullpath.c_str(),&buf) == -1 )
				return 0;
			else
				return buf.st_mtime;
		}
		int64_t CFSDirectory::fileLength(const tstring& name) const
		{
			struct fileStat buf;
			tstring fullpath = m_directory + PATH_DELIMITERA + name;
			if ( fileStat(fullpath.c_str(),&buf) == -1 )
				return 0;
			else
				return buf.st_size;
		}
		CIndexInput* CFSDirectory::openInput(const tstring& name)
		{
			tstring fullpath = m_directory + PATH_DELIMITERA + name;
			return new CFSIndexInput(fullpath.c_str());			
		}
		CIndexInput* CFSDirectory::openInput(const tstring& name,char* buf,size_t bufsize)
		{
			tstring fullpath = m_directory + PATH_DELIMITERA + name;
			return new CFSIndexInput(fullpath.c_str(),buf,bufsize);	
		}
		CIndexOutput* CFSDirectory::createOutput(const tstring& name)
		{
			tstring fullpath = m_directory + PATH_DELIMITERA + name;
			return new CFSIndexOutput(fullpath.c_str());			
		}
		void CFSDirectory::touchFile(const tstring& name)
		{
			tstring fullpath = m_directory + PATH_DELIMITERA + name;			
			
			int32_t r = _topen(fullpath.c_str(), O_RDWR, _S_IWRITE);
			if ( r < 0 )
				throw firtex::utility::CFileIOException(fullpath);
			_close(r);
		}
		void CFSDirectory::close()
		{			
			m_nRefCount--;
			if(m_nRefCount < 1)
			{
				getDirectoryMap().erase(m_directory);
				delete this;
			}
		}
		FileList* CFSDirectory::list()
		{			
			DIR* dir = opendir(m_directory.c_str());
			struct dirent* fl = readdir(dir);
			struct fileStat buf;
			
			FileList* names = new FileList;

			tstring path = m_directory;
			path += PATH_DELIMITERA;						

			while ( fl != NULL )
			{
				path += fl->d_name;				
				fileStat(path.c_str(),&buf);
				if ( buf.st_mode & S_IFDIR ) 
				{
					names->push_back(fl->d_name);					
				}
				fl = readdir(dir);
			}
			closedir(dir);
			
			return names;
		}
		void CFSDirectory::copyFile(const tstring& filename,CIndexInput* pSrcFile,bool throwError)
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
					FIRTEX_CLOG(level::warn) << _T(" copy file error:") << filename << FIRTEX_ENDL;
				}
			}			
		}
		void CFSDirectory::batCopyFiles(CDirectory*pFromDir,const tstring& from,const tstring& to,bool throwError)
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
		CFSDirectory::directory_map& CFSDirectory::getDirectoryMap()
		{
			static directory_map FS_DIRECTORIES;
			return FS_DIRECTORIES;
		}
	}
}
