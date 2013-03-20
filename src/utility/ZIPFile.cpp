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
// Created	: 2007/1/9
//
#include "utility/ZIPFile.h"
#include "zlib/zlib.h"
#include "utility/Misc.h"

namespace firtex
{
	namespace utility
	{
		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile construction

		CZIPFile::CZIPFile(int hAttach)
			:m_bAttach(false)
			,m_fileHandle(-1)
			,m_pFile(NULL)
			,m_nFile(0)
		{		

			if ( hAttach != -1 )
				attach( hAttach );
		}

		CZIPFile::~CZIPFile()
		{
			close();
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile open

		bool CZIPFile::open(const tchar* pszFile)
		{	

			close();

			m_bAttach = false;
			m_fileHandle = ::fileOpen(pszFile, O_BINARY | O_RDONLY | O_RANDOM | O_LARGEFILE, _S_IREAD );

			if (m_fileHandle < 0)
			{
				return false;
			}							

			if ( locateCentralDirectory() )
			{
				return true;
			}
			else
			{
				close();
				return false;
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile attach

		bool CZIPFile::attach(int hFile)
		{
			FIRTEX_ASSERT(( hFile != -1 ),_T("Invalidate parameter"));

			close();

			m_bAttach		= true;
			m_fileHandle	= hFile;

			if ( locateCentralDirectory() )
			{
				return true;
			}
			else
			{
				close();
				return false;
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile open test

		bool CZIPFile::isOpen() const
		{
			return m_fileHandle != -1;
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile close

		void CZIPFile::close()
		{
			if ( m_fileHandle != -1 )
			{							
				if ( ! m_bAttach ) 
					::fileClose( m_fileHandle );
				m_fileHandle = -1;
			}

			if ( m_pFile != NULL ) 
				delete [] m_pFile;
			m_pFile = NULL;
			m_nFile = 0;		
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile get the file count

		int32_t CZIPFile::getCount() const
		{
			return m_nFile;
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile get a particular file

		CZIPFile::File* CZIPFile::getFile(int32_t nFile) const
		{
			return ( nFile < 0 || nFile >= m_nFile ) ? NULL : m_pFile + nFile;
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile lookup a file by name

		CZIPFile::File* CZIPFile::getFile(const tchar* pszFile, bool bPartial) const
		{
			File* pFile = m_pFile;

			for ( int nFile = m_nFile ; nFile ; nFile--, pFile++ )
			{
				if ( bPartial )
				{					
					const tchar* pszName = _tcsrchr( pFile->m_sName.c_str(), '/' );
					pszName = pszName ? pszName + 1 : (const tchar*)pFile->m_sName.c_str();
					//if ( _tcsicoll( pszName, pszFile ) == 0 ) return pFile;
					if ( _tcsicmp( pszName, pszFile ) == 0 ) return pFile;
				}
				else
				{
					//if ( _tcsicoll( pFile->m_sName.c_str(), pszFile ) == 0 ) return pFile;
					if ( _tcsicmp( pFile->m_sName.c_str(), pszFile ) == 0 ) return pFile;
				}
			}

			return NULL;
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile locate the central directory

#pragma pack(1)
		typedef struct
		{
			uint32_t	nSignature;			// 0x06054b50
			uint16_t	nThisDisk;
			uint16_t	nDirectoryDisk;
			uint16_t	nFilesThisDisk;
			uint16_t	nTotalFiles;
			uint32_t	nDirectorySize;
			uint32_t	nDirectoryOffset;
			uint16_t	nCommentLen;
		} ZIP_DIRECTORY_LOC;
#pragma pack()

		bool CZIPFile::locateCentralDirectory()
		{
			byte pBuffer[4096];
			int32_t nBuffer = 0;

			fileSeek(m_fileHandle,-4096,SEEK_END);
			//SetFilePointer( m_fileHandle, -4096, NULL, FILE_END );
			if ( (nBuffer = ::fileRead( m_fileHandle, pBuffer, 4096)) <= 0 )
				return false;
			if ( nBuffer < sizeof(ZIP_DIRECTORY_LOC) )
				return false;

			ZIP_DIRECTORY_LOC* pLoc = NULL;

			for ( int32_t nScan = 4 ; nScan < nBuffer ; nScan++ )
			{
				uint32_t* pnSignature = (uint32_t*)( pBuffer + nBuffer - nScan  );

				if ( *pnSignature == 0x06054b50 )
				{
					pLoc = (ZIP_DIRECTORY_LOC*)pnSignature;
					break;
				}
			}

			if ( pLoc == NULL )
				return false;
			FIRTEX_ASSERT(( pLoc->nSignature == 0x06054b50 ),_T("false signature."));

			if ( fileSize(m_fileHandle) < pLoc->nDirectorySize )
				return false;

			//if ( SetFilePointer( m_fileHandle, pLoc->nDirectoryOffset, NULL, FILE_BEGIN )
			if(fileSeek(m_fileHandle,pLoc->nDirectoryOffset,SEEK_SET) != pLoc->nDirectoryOffset )
				return false;

			byte* pDirectory = new byte[ pLoc->nDirectorySize ];
			//ReadFile( m_fileHandle, pDirectory, pLoc->nDirectorySize, &nBuffer, NULL );
			nBuffer = ::fileRead(m_fileHandle,pDirectory,pLoc->nDirectorySize);

			if ( nBuffer == pLoc->nDirectorySize )
			{
				m_nFile = (int)pLoc->nTotalFiles;
				m_pFile = new File[ m_nFile ];

				if ( ! parseCentralDirectory( pDirectory, pLoc->nDirectorySize ) )
				{
					delete [] m_pFile;
					m_pFile = NULL;
					m_nFile = 0;
				}
			}

			delete [] pDirectory;

			return ( m_nFile > 0 );
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile parse the central directory

#pragma pack(1)
		typedef struct
		{
			uint32_t	nSignature;		// 0x02014b50
			uint16_t	nWriteVersion;
			uint16_t	nReadVersion;
			uint16_t	nFlags;
			uint16_t	nCompression;
			uint16_t	nFileTime;
			uint16_t	nFileDate;
			uint32_t	nCRC;
			uint32_t	nCompressedSize;
			uint32_t	nActualSize;
			uint16_t	nNameLen;
			uint16_t	nExtraLen;
			uint16_t	nCommentLen;
			uint16_t	nStartDisk;
			uint16_t	nInternalAttr;
			uint32_t	nExternalAttr;
			uint32_t	nLocalOffset;
		} ZIP_CENTRAL_FILE;
#pragma pack()

		bool CZIPFile::parseCentralDirectory(byte* pDirectory, int32_t nDirectory)
		{
			for ( int nFile = 0 ; nFile < m_nFile ; nFile++ )
			{
				ZIP_CENTRAL_FILE* pRecord = (ZIP_CENTRAL_FILE*)pDirectory;

				if ( nDirectory < sizeof(*pRecord) ) return false;
				if ( pRecord->nSignature != 0x02014b50 ) return false;

				pDirectory += sizeof(*pRecord);
				nDirectory -= sizeof(*pRecord);

				int nTailLen = (int)pRecord->nNameLen + (int)pRecord->nExtraLen + (int)pRecord->nCommentLen;
				if ( nDirectory < (int32_t)nTailLen ) return false;

				m_pFile[ nFile ].m_pZIP				= this;
				m_pFile[ nFile ].m_nSize			= pRecord->nActualSize;
				m_pFile[ nFile ].m_nLocalOffset		= pRecord->nLocalOffset;
				m_pFile[ nFile ].m_nCompressedSize	= pRecord->nCompressedSize;
				m_pFile[ nFile ].m_nCompression		= pRecord->nCompression;

				m_pFile[ nFile ].m_sName.resize(pRecord->nNameLen);
				tchar* pszName = (tchar*)(m_pFile[ nFile ].m_sName.c_str());

				for ( uint16_t nChar = 0 ; nChar < pRecord->nNameLen ; nChar++ )
				{
					pszName[ nChar ] = (tchar)pDirectory[ nChar ];
					if ( pszName[ nChar ] == '\\' ) pszName[ nChar ] = '/';
				}				

				pDirectory += (int32_t)nTailLen;
				nDirectory -= (int32_t)nTailLen;//?
			}
			return true;
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile::File seek to a file

#pragma pack(1)
		typedef struct
		{
			uint32_t	nSignature;		// 0x04034b50
			uint16_t	nVersion;
			uint16_t	nFlags;
			uint16_t	nCompression;
			uint16_t	nFileTime;
			uint16_t	nFileDate;
			uint32_t	nCRC;
			uint32_t	nCompressedSize;
			uint32_t	nActualSize;
			uint16_t	nNameLen;
			uint16_t	nExtraLen;
		} ZIP_LOCAL_FILE;
#pragma pack()	

		bool CZIPFile::seekToFile(File* pFile)
		{			
			FIRTEX_ASSERT(( pFile != NULL ),_T("Invalidate parameter"));
			FIRTEX_ASSERT(( pFile->m_pZIP == this ),_T("Invalidate parameter"));

			if ( m_fileHandle == -1 ) return false;

			//if ( SetFilePointer( m_fileHandle, (DWORD)pFile->m_nLocalOffset, NULL, FILE_BEGIN )
			//	!= pFile->m_nLocalOffset ) return false;
			if(fileSeek(m_fileHandle,pFile->m_nLocalOffset,SEEK_SET) != pFile->m_nLocalOffset)
				return false;

			ZIP_LOCAL_FILE pLocal;
			int32_t nRead = 0;

			//ReadFile( m_fileHandle, &pLocal, sizeof(pLocal), &nRead, NULL );
			nRead = ::fileRead(m_fileHandle,&pLocal,sizeof(pLocal));
			if ( nRead != sizeof(pLocal) ) return false;

			if ( pLocal.nSignature != 0x04034b50 ) return false;
			if ( pLocal.nCompression != Z_DEFLATED && pLocal.nCompression != 0 ) return false;

			//SetFilePointer( m_fileHandle, pLocal.nNameLen + pLocal.nExtraLen, NULL, FILE_CURRENT );
			fileSeek(m_fileHandle,pLocal.nNameLen + pLocal.nExtraLen,SEEK_CUR);

			return true;
		}		

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile::File prepare to decompress

		bool CZIPFile::File::prepareToDecompress(void* pStream)
		{
			memset(pStream,0,sizeof(z_stream));			

			if ( ! m_pZIP->seekToFile( this ) ) return false;

			if ( m_nCompression == 0 )
			{
				return ( m_nSize == m_nCompressedSize );
			}
			else
			{
				FIRTEX_ASSERT(( m_nCompression == Z_DEFLATED ),_T("prepareToDecompress() failed."));
				return Z_OK == inflateInit2( (z_stream*)pStream, -MAX_WBITS );
			}
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile::File decompress to memory

		size_t CZIPFile::File::decompress(byte* pOutBuffer,size_t outLen)
		{
			/*z_stream pStream;

			if ( m_nSize > 32*1024*1024 ) return NULL;
			if ( ! prepareToDecompress( &pStream ) ) return NULL;

			if ( m_nCompression == 0 )
			{				
				CBuffer* pTarget = new CBuffer();
				pTarget->EnsureBuffer( (DWORD)m_nSize );
				ReadFile( m_pZIP->m_fileHandle, pTarget->m_pBuffer, (DWORD)m_nSize, &pTarget->m_nLength, NULL );
				if ( pTarget->m_nLength == (DWORD)m_nSize ) return pTarget;
				delete pTarget;
				return NULL;
			}

			DWORD nSource = (DWORD)m_nCompressedSize;
			byte* pSource = new byte[ nSource ];
			ReadFile( m_pZIP->m_fileHandle, pSource, nSource, &nSource, NULL );

			if ( nSource != (DWORD)m_nCompressedSize )
			{
				inflateEnd( &pStream );
				return NULL;
			}

			CBuffer* pTarget = new CBuffer();
			pTarget->EnsureBuffer( (DWORD)m_nSize );
			pTarget->m_nLength = (DWORD)m_nSize;

			pStream.next_in		= pSource;
			pStream.avail_in	= (DWORD)m_nCompressedSize;
			pStream.next_out	= pTarget->m_pBuffer;
			pStream.avail_out	= pTarget->m_nLength;

			inflate( &pStream, Z_FINISH );

			delete [] pSource;

			if ( pStream.avail_out != 0 )
			{
				delete pTarget;
				pTarget = NULL;
			}

			inflateEnd( &pStream );

			return pTarget;*/

			z_stream pStream;

			if ( m_nSize > 32*1024*1024 ) 
				return 0;
			if ( ! prepareToDecompress( &pStream ) )
				return 0;

			if ( m_nCompression == 0 )
			{				
				if(!pOutBuffer)
					return (size_t)m_nSize;
				int32_t len = ::fileRead(m_pZIP->m_fileHandle,pOutBuffer,(unsigned int)m_nSize);
				//ReadFile( m_pZIP->m_fileHandle, pTarget->m_pBuffer, (DWORD)m_nSize, &pTarget->m_nLength, NULL );
				if ( len == (uint32_t)m_nSize ) 
					return len;				
				return 0;
			}

			uint32_t nSource = (uint32_t)m_nCompressedSize;
			byte* pSource = new byte[ nSource ];
			//ReadFile( m_pZIP->m_fileHandle, pSource, nSource, &nSource, NULL );
			nSource = ::fileRead(m_pZIP->m_fileHandle,pSource,nSource);

			if ( nSource != (uint32_t)m_nCompressedSize )
			{				
				inflateEnd( &pStream );
				delete[] pSource;
				return 0;
			}		

			pStream.next_in		= pSource;
			pStream.avail_in	= (uint32_t)m_nCompressedSize;
			pStream.next_out	= pOutBuffer;
			pStream.avail_out	= (uint32_t)m_nSize;

			inflate( &pStream, Z_FINISH );

			delete [] pSource;

			if ( pStream.avail_out != 0 )
			{
				inflateEnd( &pStream );
				return 0;
			}

			inflateEnd( &pStream );

			return (size_t)m_nSize;
		}

		/////////////////////////////////////////////////////////////////////////////
		// CZIPFile::File decompress to disk

		const uint32_t BUFFER_IN_SIZE = 64 * 1024u;
		const uint32_t BUFFER_OUT_SIZE = 128 * 1024u;

		bool CZIPFile::File::extract(const tchar* pszFile)
		{
			z_stream pStream;
			int hFile;

			/*hFile = CreateFile( pszFile, GENERIC_WRITE, 0, NULL, CREATE_NEW,
				FILE_ATTRIBUTE_NORMAL, NULL );
			if ( hFile == -1 ) return false;*/

			if ( CMisc::dirExists(pszFile) )
				hFile = ::fileOpen( pszFile, O_BINARY | O_RDWR | O_RANDOM | O_TRUNC | O_LARGEFILE , _S_IREAD | _S_IWRITE);
			else 
				hFile = ::fileOpen( pszFile, O_BINARY | O_RDWR | O_RANDOM | O_CREAT | O_LARGEFILE, _S_IREAD | _S_IWRITE);
			if(hFile < 0)
				return false;

			if ( ! prepareToDecompress( &pStream ) ) return false;

			int64_t nCompressed = 0, nUncompressed = 0;

			if ( m_nCompression == Z_DEFLATED )
			{
				byte* pBufferIn		= new byte[BUFFER_IN_SIZE];
				byte* pBufferOut	= new byte[BUFFER_OUT_SIZE];

				while ( nCompressed < m_nCompressedSize || nUncompressed < m_nSize )
				{
					if ( pStream.avail_in == 0 )
					{
						pStream.avail_in	= (uint32_t)((int32_t)(m_nCompressedSize - nCompressed) > BUFFER_IN_SIZE )?BUFFER_IN_SIZE:(int32_t)(m_nCompressedSize - nCompressed);
						pStream.next_in		= pBufferIn;

						//DWORD nRead = 0;
						//ReadFile( m_pZIP->m_fileHandle, pBufferIn, pStream.avail_in, &nRead, NULL );
						uint32_t nRead = ::fileRead(m_pZIP->m_fileHandle,pBufferIn,pStream.avail_in);
						if ( nRead != pStream.avail_in ) break;
						nCompressed += nRead;
					}

					pStream.avail_out	= BUFFER_OUT_SIZE;
					pStream.next_out	= pBufferOut;

					/*int nInflate =*/ inflate( &pStream, Z_SYNC_FLUSH );

					if ( pStream.avail_out < BUFFER_OUT_SIZE )
					{
						uint32_t nWrite = BUFFER_OUT_SIZE - pStream.avail_out;
						//WriteFile( hFile, pBufferOut, nWrite, &nWrite, NULL );
						nWrite = ::fileWrite(hFile,pBufferOut,nWrite);
						if ( nWrite != BUFFER_OUT_SIZE - pStream.avail_out ) break;
						nUncompressed += nWrite;
					}
				}

				delete [] pBufferOut;
				delete [] pBufferIn;

				inflateEnd( &pStream );
			}
			else
			{
				byte* pBufferOut = new byte[BUFFER_OUT_SIZE];

				while ( nUncompressed < m_nSize )
				{
					uint32_t nChunk = (uint32_t)((uint32_t)( m_nSize - nUncompressed)> BUFFER_OUT_SIZE )?BUFFER_OUT_SIZE:(uint32_t)( m_nSize - nUncompressed);
					uint32_t nProcess = 0;

					//ReadFile( m_pZIP->m_fileHandle, pBufferOut, nChunk, &nProcess, NULL );
					nProcess = ::fileRead(m_pZIP->m_fileHandle,pBufferOut,nChunk);
					if ( nChunk != nProcess ) break;
					//WriteFile( hFile, pBufferOut, nChunk, &nProcess, NULL );
					nProcess = ::fileWrite(hFile,pBufferOut,nChunk);
					if ( nChunk != nProcess ) break;

					nCompressed += nChunk;
					nUncompressed += nChunk;
				}

				delete [] pBufferOut;
			}

			::fileClose( hFile );

			if ( nUncompressed >= m_nSize ) return true;

			//DeleteFile( pszFile );
			_tunlink(pszFile);

			return false;
		}
	}
}
