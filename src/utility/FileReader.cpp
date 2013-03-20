#include "utility/FileReader.h"
#include "utility/Config.h"

namespace firtex
{
	namespace utility
	{
		CFileReader::CFileReader(void):m_fileHandle(-1),CReader(0)
		{		
		}
		CFileReader::CFileReader(char* buffer,size_t buffsize):CReader(buffer,buffsize),m_fileHandle(-1)
		{
		}
		CFileReader::CFileReader(size_t buffsize):CReader(buffsize),m_fileHandle(-1)
		{
		}
        CFileReader::CFileReader(const tstring& filename)
		{			
			m_fileHandle = ::fileOpen(filename.c_str(), O_BINARY | O_RDONLY | O_RANDOM | O_LARGEFILE, _S_IREAD );
			if (m_fileHandle < 0)
			{
				int err = errno;
				if ( err == ENOENT )
					FIRTEX_THROW2(FILEIO_ERROR,"File does not exist: " + filename);
				else if ( err == EACCES )
					FIRTEX_THROW2(FILEIO_ERROR,"File Access denied: " + filename);
				else if ( err == EMFILE )
					FIRTEX_THROW2(FILEIO_ERROR,"Too many open files: " + filename);
			}				
			m_length = fileSize(m_fileHandle);			
			if(m_length < 0)
			{				
				FIRTEX_THROW3(FILEIO_ERROR,_T("get the length of file failed.") + filename);
			}
			m_filename = filename;
		}
		CFileReader::CFileReader(const tstring& filename,char* buffer,size_t buffsize):CReader(buffer,buffsize)
		{
			m_fileHandle = ::fileOpen(filename.c_str(), O_BINARY | O_RDONLY | O_RANDOM | O_LARGEFILE, _S_IREAD );
			if (m_fileHandle < 0)
			{
				int err = errno;
				if ( err == ENOENT )
					FIRTEX_THROW2(FILEIO_ERROR,"File does not exist: " + filename);
				else if ( err == EACCES )
					FIRTEX_THROW2(FILEIO_ERROR,"File Access denied: " + filename);
				else if ( err == EMFILE )
					FIRTEX_THROW2(FILEIO_ERROR,"Too many open files: " + filename);
			}				
			m_length = fileSize(m_fileHandle);			
			m_filename = filename;
		}
		CFileReader::CFileReader(const tstring& filename,size_t buffsize):CReader(buffsize)
		{
			m_fileHandle = ::fileOpen(filename.c_str(), O_BINARY | O_RDONLY | O_RANDOM | O_LARGEFILE, _S_IREAD );
			if (m_fileHandle < 0)
			{
				int err = errno;
				if ( err == ENOENT )
					FIRTEX_THROW2(FILEIO_ERROR,"File does not exist: " + filename);
				else if ( err == EACCES )
					FIRTEX_THROW2(FILEIO_ERROR,"File Access denied: " + filename);
				else if ( err == EMFILE )
					FIRTEX_THROW2(FILEIO_ERROR,"Too many open files: " + filename);
			}				
			m_length = fileSize(m_fileHandle);
			m_filename = filename;
		}

		CFileReader::~CFileReader()
		{
			close(); 
		}
	
		void CFileReader::readInternal(char* b, size_t offset, size_t len)
		{
			int64_t position = getFilePointer();
			if (position != fileTell(m_fileHandle))
			{
				if(fileSeek(m_fileHandle,position,SEEK_SET) != position)
					FIRTEX_THROW2(FILEIO_ERROR,"void CFileReader::readInternal():file IO seek error: " + m_filename);
			}
			int64_t ret = ::fileRead(m_fileHandle,b+offset,(unsigned int)len);
			if(ret == 0)
				FIRTEX_THROW2(FILEIO_ERROR,"CFileReader::readInternal():read past EOF:" + m_filename);
			else if (ret == -1)
				FIRTEX_THROW2(FILEIO_ERROR,"CFileReader::readInternal():file IO read error:" + m_filename);			
		}

		bool CFileReader::open(const tstring& filename,bool bThrowError)
		{			
			close();
			m_fileHandle = ::fileOpen(filename.c_str(), O_BINARY | O_RDONLY | O_RANDOM | O_LARGEFILE, _S_IREAD );
			if (m_fileHandle < 0)
			{
				int err = errno;
				if(bThrowError)
				{
					if ( err == ENOENT )
						FIRTEX_THROW2(FILEIO_ERROR,_T("File does not exist: ") + filename);
					else if ( err == EACCES )
						FIRTEX_THROW2(FILEIO_ERROR,_T("File Access denied: ") + filename);
					else if ( err == EMFILE )
						FIRTEX_THROW2(FILEIO_ERROR,_T("Too many open files: ") + filename);
				}
				else 
				{
					if ( err == ENOENT )
						FIRTEX_CLOG(level::err) << _T("File does not exist: ") << filename << FIRTEX_ENDL
					else if ( err == EACCES )
						FIRTEX_CLOG(level::err) << _T("File Access denied: ") << filename << FIRTEX_ENDL
					else if ( err == EMFILE )
						FIRTEX_CLOG(level::err) << _T("Too many open files: ") << filename << FIRTEX_ENDL				
				}
				return false;
			}				
			m_length = ::fileSize(m_fileHandle);
			m_filename = filename;
			if(m_length < 0)
			{				
				FIRTEX_THROW3(FILEIO_ERROR,_T("get the length of file failed.") + filename);
			}
			return true;
		}
		void  CFileReader::close()
		{
			if(m_fileHandle >= 0)
			{
				::fileClose(m_fileHandle);
				m_fileHandle = -1;
			}
			
			CReader::close();
		}

		void  CFileReader::seekInternal(int64_t position)
		{
			if(fileSeek(m_fileHandle,position,SEEK_SET) != position)
				FIRTEX_THROW2(FILEIO_ERROR,"CFileReader::seekInternal():file IO seek error: " + m_filename);
		}		


		CReader*	CFileReader::clone(char* buffer,size_t buffsize)
		{	
			if(m_filename.empty())
				return new CFileReader(buffer,buffsize);
			else 
				return new CFileReader(m_filename,buffer,buffsize);			
		}

		CReader*	CFileReader::clone()
		{
			if(m_filename.empty())
				return new CFileReader(m_bufferSize);
			else 
				return new CFileReader(m_filename,m_bufferSize);									
		}
	}
}

